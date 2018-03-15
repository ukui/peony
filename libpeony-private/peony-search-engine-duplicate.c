/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Red Hat, Inc
 * Copyright (C) 2018, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * Peony is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 * Modified by: liupeng <liupeng@kylinos.cn>
 *
 */

#include <config.h>
#include "peony-search-engine-duplicate.h"

#include <string.h>
#include <glib.h>

#include <eel/eel-gtk-macros.h>
#include <gio/gio.h>

#define BATCH_SIZE 500

typedef struct
{
    PeonySearchEngineDuplicate *engine;
    GCancellable *cancellable;

    GList *mime_types;
    char **words;
    GList *found_list;

    GQueue *directories; /* GFiles */

    GHashTable *visited;
    GHashTable *pFileList;
    GHashTable *pFileRes;
    gint n_processed_files;
    GList *uri_hits;
	
    GList *pListRes;
} SearchThreadData;


struct PeonySearchEngineDuplicateDetails
{
    PeonyQuery *query;

    SearchThreadData *active_search;

    gboolean query_finished;
    GList *pListRes;
};


static void  peony_search_engine_duplicate_class_init       (PeonySearchEngineDuplicateClass *class);
static void  peony_search_engine_duplicate_init             (PeonySearchEngineDuplicate      *engine);

G_DEFINE_TYPE (PeonySearchEngineDuplicate,
               peony_search_engine_duplicate,
               PEONY_TYPE_SEARCH_ENGINE);

static PeonySearchEngineClass *parent_class = NULL;

static void
finalize (GObject *object)
{
    PeonySearchEngineDuplicate *simple;

    simple = PEONY_SEARCH_ENGINE_DUPLICATE (object);

    if (simple->details->query)
    {
        g_object_unref (simple->details->query);
        simple->details->query = NULL;
    }

    g_free (simple->details);

    EEL_CALL_PARENT (G_OBJECT_CLASS, finalize, (object));
}

static SearchThreadData *
search_thread_data_new (PeonySearchEngineDuplicate *engine,
                        PeonyQuery *query)
{
    SearchThreadData *data;
    char *text, *lower, *normalized, *uri;
    GFile *location;

    data = g_new0 (SearchThreadData, 1);

    data->engine = engine;
    data->directories = g_queue_new ();
    data->visited = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    data->pFileList = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    data->pFileRes = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    uri = peony_query_get_location (query);
    location = NULL;
    if (uri != NULL)
    {
        location = g_file_new_for_uri (uri);
        g_free (uri);
    }
    if (location == NULL)
    {
        location = g_file_new_for_path ("/");
    }
    g_queue_push_tail (data->directories, location);

    text = peony_query_get_text (query);
    normalized = g_utf8_normalize (text, -1, G_NORMALIZE_NFD);
    lower = g_utf8_strdown (normalized, -1);
    data->words = g_strsplit (lower, " ", -1);
    g_free (text);
    g_free (lower);
    g_free (normalized);

    data->mime_types = peony_query_get_mime_types (query);

    data->cancellable = g_cancellable_new ();

    return data;
}

static void
search_thread_data_free (SearchThreadData *data)
{
    g_queue_foreach (data->directories,
                     (GFunc)g_object_unref, NULL);
    g_queue_free (data->directories);
    g_hash_table_destroy (data->visited);
    g_hash_table_destroy (data->pFileList);
    g_hash_table_destroy (data->pFileRes);
    g_object_unref (data->cancellable);
    g_strfreev (data->words);
    g_list_free_full (data->mime_types, g_free);
    g_list_free_full (data->uri_hits, g_free);
    g_free (data);
}

static gboolean
search_thread_done_idle (gpointer user_data)
{
    SearchThreadData *data;

    data = user_data;

    if (!g_cancellable_is_cancelled (data->cancellable))
    {
        peony_search_engine_finished (PEONY_SEARCH_ENGINE (data->engine));
        data->engine->details->active_search = NULL;
    }

    search_thread_data_free (data);

    return FALSE;
}

typedef struct
{
    GList *uris;
    SearchThreadData *thread_data;
} SearchHits;


static gboolean
search_thread_add_hits_idle (gpointer user_data)
{
    SearchHits *hits;

    hits = user_data;

    if (!g_cancellable_is_cancelled (hits->thread_data->cancellable))
    {
        peony_search_engine_hits_added (PEONY_SEARCH_ENGINE (hits->thread_data->engine),
                                       hits->uris);
    }

    g_list_free_full (hits->uris, g_free);
    g_free (hits);

    return FALSE;
}

static void
send_batch (SearchThreadData *data)
{
    SearchHits *hits;

    data->n_processed_files = 0;

    if (data->uri_hits)
    {
        hits = g_new (SearchHits, 1);
        hits->uris = data->uri_hits;
        hits->thread_data = data;
        g_idle_add (search_thread_add_hits_idle, hits);
    }
    data->uri_hits = NULL;
}

#define STD_ATTRIBUTES \
	G_FILE_ATTRIBUTE_STANDARD_NAME "," \
	G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME "," \
	G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN "," \
	G_FILE_ATTRIBUTE_STANDARD_TYPE "," \
	G_FILE_ATTRIBUTE_ID_FILE
	
static void
visit_directory_duplicate (GFile *dir, SearchThreadData *data)
{
	GFileEnumerator *enumerator;
	GFileInfo *info;
	GFile *child;
	const char *mime_type, *display_name;
	char *lower_name, *normalized;
	gboolean hit;
	int i;
	GList *l;
	const char *id;
	gboolean visited;
	char *pUri = NULL;
	char *pOldKey = NULL;
	char *pOldUri = NULL;
	enumerator = g_file_enumerate_children (dir,
											data->mime_types != NULL ?
											STD_ATTRIBUTES ","
											G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE
											:
											STD_ATTRIBUTES
											,
											0, data->cancellable, NULL);

	if (enumerator == NULL)
	{
		return;
	}

	while ((info = g_file_enumerator_next_file (enumerator, data->cancellable, NULL)) != NULL)
	{
		if (g_file_info_get_is_hidden (info))
		{
			goto next;
		}

		child = g_file_get_child (dir, g_file_info_get_name (info));
		if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY)
		{
			id = g_file_info_get_attribute_string (info, G_FILE_ATTRIBUTE_ID_FILE);
			visited = FALSE;
			if (id)
			{
				if (g_hash_table_lookup_extended (data->visited,
												  id, NULL, NULL))
				{
					visited = TRUE;
				}
				else
				{
					g_hash_table_insert (data->visited, g_strdup (id), NULL);
				}
			}

			if (!visited)
			{
				g_queue_push_tail (data->directories, g_object_ref (child));
			}
		}
		else
		{
			display_name = g_file_info_get_display_name (info);
			if (display_name == NULL)
			{
				goto next;
			}

			hit = FALSE;
			pUri = g_file_get_uri (child);
			if(NULL != pUri)
			{
				if (g_hash_table_lookup_extended (data->pFileList,
												  display_name, (gpointer *)&pOldKey, (gpointer *)&pOldUri))
				{
					hit = TRUE;
				}
				else
				{
					g_hash_table_insert (data->pFileList, g_strdup (display_name), g_strdup (pUri));
				}
				g_free(pUri);
			}
						
			if (hit)
			{
				if (!g_hash_table_lookup_extended (data->pFileRes,
												  pOldUri, NULL, NULL))
				{
					g_hash_table_insert (data->pFileRes, g_strdup (pOldUri), NULL);
					data->uri_hits = g_list_prepend (data->uri_hits, g_strdup (pOldUri));
				}
				data->uri_hits = g_list_prepend (data->uri_hits, g_file_get_uri (child));
			}

			data->n_processed_files++;
			
			if (data->n_processed_files > BATCH_SIZE)
			{
				send_batch (data);
			}
		}
		g_object_unref (child);
next:
		g_object_unref (info);
	}

	g_object_unref (enumerator);
}

static gpointer
search_duplicate_thread_func_new (gpointer user_data)
{
    SearchThreadData *data;
    GFile *dir;
    GFileInfo *info;
    const char *id;

    data = user_data;

    /* Insert id for toplevel directory into visited */
    dir = g_queue_peek_head (data->directories);
    info = g_file_query_info (dir, G_FILE_ATTRIBUTE_ID_FILE, 0, data->cancellable, NULL);
    if (info)
    {
        id = g_file_info_get_attribute_string (info, G_FILE_ATTRIBUTE_ID_FILE);
        if (id)
        {
            g_hash_table_insert (data->visited, g_strdup (id), NULL);
        }
        g_object_unref (info);
    }

    while (!g_cancellable_is_cancelled (data->cancellable) &&
            (dir = g_queue_pop_head (data->directories)) != NULL)
    {
        visit_directory_duplicate (dir, data);
        g_object_unref (dir);
    }
    send_batch (data);

    g_idle_add (search_thread_done_idle, data);

    return NULL;
}

static gpointer
find_duplicate_thread (SearchThreadData *data)
{
    GList *pListRes = NULL;
    gboolean bFind = FALSE;
	GList *listnode = NULL;
	GList *listnodenext = NULL;
	GList *listnodenext2 = NULL;
	char *pUri = NULL;
	char *pUriNext = NULL;

	
    pListRes = (GList *)data->pListRes;
	if(NULL == pListRes)
	{
		return NULL;
	}

	listnode = g_list_first(pListRes);
	for(;NULL != listnode;listnode = g_list_first(pListRes))
	{
		bFind = FALSE;
		pUri = (char*)(listnode->data);
		if(NULL != pUri)
		{		
			listnodenext = g_list_next(listnode);
			while(NULL != listnodenext)
			{
				GFile *pFile = NULL;
				GFile *pFileNext = NULL;
				gchar *pInsertUri = NULL;
				char  *pFileUri = NULL;
				char  *pFileNextUri = NULL;
				listnodenext2 = g_list_next(listnodenext);
				pUriNext = (char*)(listnodenext->data);
				if(NULL == pUriNext)
				{
					listnodenext = g_list_next(listnodenext);
					continue;
				}
				
				pFile = g_file_new_for_uri (pUri);
				pFileNext = g_file_new_for_uri (pUriNext);
				if(NULL != pFile)
				{
					pFileUri = g_file_get_basename(pFile);
				}
				else
				{
					continue;
				}
				if(NULL != pFileNext)
				{
					pFileNextUri = g_file_get_basename(pFileNext);
				}
				else
				{
					continue;
				}
				if(0 == strcmp(pFileUri,pFileNextUri))
				{
					data->uri_hits = g_list_prepend (data->uri_hits, pUriNext);
					bFind = TRUE;
					data->n_processed_files++;
			        if (data->n_processed_files > BATCH_SIZE)
			        {
			            send_batch (data);
			        }
					pListRes = g_list_delete_link(pListRes,listnodenext);
					if(NULL == pListRes)
					{
						g_free(pFileUri);
						g_free(pFileNextUri);
						g_object_unref(pFile);
						g_object_unref(pFileNext);
						break;
					}
				}
				g_free(pFileUri);
				g_free(pFileNextUri);
				g_object_unref(pFile);
				g_object_unref(pFileNext);
				listnodenext = listnodenext2;
			}
			if(TRUE == bFind)
			{
				data->uri_hits = g_list_prepend (data->uri_hits, pUri);
			}
		}
		
		pListRes = g_list_delete_link(pListRes,listnode);
		if(NULL == pListRes)
		{
			break;
		}
	}
	
    return NULL;
}

static gpointer
search_duplicate_thread_func (gpointer user_data)
{
    SearchThreadData *data;
    GFile *dir;
	char *pUri = NULL;
	gboolean bFRead = FALSE;
    
	do
	{
		if(NULL == user_data)
		{
			peony_debug_log(TRUE,"_find_","search_duplicate_thread_func param error.");
			break;
		}
		data = user_data;
		dir = g_queue_peek_head (data->directories);
		pUri = g_file_get_uri(dir);
		if(NULL == pUri)
		{
			peony_debug_log(TRUE,"_find_","search_duplicate_thread_func g_file_get_uri failed.");
			break;
		}
		
		bFRead  = read_all_children_for_find (pUri,
										G_FILE_ATTRIBUTE_STANDARD_NAME,
										&(data->pListRes));
		if(FALSE == bFRead || NULL == data->pListRes)
		{
			peony_debug_log(TRUE,"_find_","read_all_children_for_find is failed.");
			break;
		}
		
		find_duplicate_thread(data);
		
	    send_batch (data);

	    g_idle_add (search_thread_done_idle, data);
	}while(0);
    return NULL;
}


static void
peony_search_engine_duplicate_start (PeonySearchEngine *engine)
{
    PeonySearchEngineDuplicate *simple;
    SearchThreadData *data;

    simple = PEONY_SEARCH_ENGINE_DUPLICATE (engine);

    if (simple->details->active_search != NULL)
    {
        return;
    }

    if (simple->details->query == NULL)
    {
        return;
    }

    data = search_thread_data_new (simple, simple->details->query);

    //g_thread_create (search_duplicate_thread_func, data, FALSE, NULL);
    g_thread_create (search_duplicate_thread_func_new, data, FALSE, NULL);

    simple->details->active_search = data;
}

static void
peony_search_engine_duplicate_stop (PeonySearchEngine *engine)
{
    PeonySearchEngineDuplicate *simple;

    simple = PEONY_SEARCH_ENGINE_DUPLICATE (engine);

    if (simple->details->active_search != NULL)
    {
        g_cancellable_cancel (simple->details->active_search->cancellable);
        simple->details->active_search = NULL;
    }
}

static gboolean
peony_search_engine_duplicate_is_indexed (PeonySearchEngine *engine)
{
    return FALSE;
}

static void
peony_search_engine_duplicate_set_query (PeonySearchEngine *engine, PeonyQuery *query)
{
    PeonySearchEngineDuplicate *simple;

    simple = PEONY_SEARCH_ENGINE_DUPLICATE (engine);

    if (query)
    {
        g_object_ref (query);
    }

    if (simple->details->query)
    {
        g_object_unref (simple->details->query);
    }

    simple->details->query = query;
}

static void
peony_search_engine_duplicate_class_init (PeonySearchEngineDuplicateClass *class)
{
    GObjectClass *gobject_class;
    PeonySearchEngineClass *engine_class;

    parent_class = g_type_class_peek_parent (class);

    gobject_class = G_OBJECT_CLASS (class);
    gobject_class->finalize = finalize;

    engine_class = PEONY_SEARCH_ENGINE_CLASS (class);
    engine_class->set_query = peony_search_engine_duplicate_set_query;
    engine_class->start = peony_search_engine_duplicate_start;
    engine_class->stop = peony_search_engine_duplicate_stop;
    engine_class->is_indexed = peony_search_engine_duplicate_is_indexed;
}

static void
peony_search_engine_duplicate_init (PeonySearchEngineDuplicate *engine)
{
    engine->details = g_new0 (PeonySearchEngineDuplicateDetails, 1);
}


PeonySearchEngine *
peony_search_engine_duplicate_new (void)
{
    PeonySearchEngine *engine;

    engine = g_object_new (PEONY_TYPE_SEARCH_ENGINE_DUPLICATE, NULL);

    return engine;
}

