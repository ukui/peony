/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
   Copyright (C) 2005 Novell, Inc

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Author: Anders Carlsson <andersca@imendio.com>
*/

#include <config.h>
#include "peony-search-directory.h"
#include "peony-search-directory-file.h"

#include "peony-directory-private.h"
#include "peony-file.h"
#include "peony-file-private.h"
#include "peony-file-utilities.h"
#include "peony-search-engine.h"
#include <eel/eel-glib-extensions.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <string.h>
#include <sys/time.h>

struct PeonySearchDirectoryDetails
{
    PeonyQuery *query;
    char *saved_search_uri;
    gboolean modified;

    PeonySearchEngine *engine;

    gboolean search_running;
    gboolean search_finished;

    GList *files;
    GHashTable *file_hash;

    GList *monitor_list;
    GList *callback_list;
    GList *pending_callback_list;
};

typedef struct
{
    gboolean monitor_hidden_files;
    PeonyFileAttributes monitor_attributes;

    gconstpointer client;
} SearchMonitor;

typedef struct
{
    PeonySearchDirectory *search_directory;

    PeonyDirectoryCallback callback;
    gpointer callback_data;

    PeonyFileAttributes wait_for_attributes;
    gboolean wait_for_file_list;
    GList *file_list;
    GHashTable *non_ready_hash;
} SearchCallback;

G_DEFINE_TYPE (PeonySearchDirectory, peony_search_directory,
               PEONY_TYPE_DIRECTORY);

static void search_engine_hits_added (PeonySearchEngine *engine, GList *hits, PeonySearchDirectory *search);
static void search_engine_hits_subtracted (PeonySearchEngine *engine, GList *hits, PeonySearchDirectory *search);
static void search_engine_finished (PeonySearchEngine *engine, PeonySearchDirectory *search);
static void search_engine_error (PeonySearchEngine *engine, const char *error, PeonySearchDirectory *search);
static void search_callback_file_ready_callback (PeonyFile *file, gpointer data);
static void file_changed (PeonyFile *file, PeonySearchDirectory *search);

static void
ensure_search_engine (PeonySearchDirectory *search)
{
    if (!search->details->engine)
    {
        search->details->engine = peony_search_engine_new ();
        g_signal_connect (search->details->engine, "hits-added",
                          G_CALLBACK (search_engine_hits_added),
                          search);
        g_signal_connect (search->details->engine, "hits-subtracted",
                          G_CALLBACK (search_engine_hits_subtracted),
                          search);
        g_signal_connect (search->details->engine, "finished",
                          G_CALLBACK (search_engine_finished),
                          search);
        g_signal_connect (search->details->engine, "error",
                          G_CALLBACK (search_engine_error),
                          search);
    }
}

static void
reset_file_list (PeonySearchDirectory *search)
{
    GList *list, *monitor_list;
    PeonyFile *file;
    SearchMonitor *monitor;

    /* Remove file connections */
    for (list = search->details->files; list != NULL; list = list->next)
    {
        file = list->data;

        /* Disconnect change handler */
        g_signal_handlers_disconnect_by_func (file, file_changed, search);

        /* Remove monitors */
        for (monitor_list = search->details->monitor_list; monitor_list;
                monitor_list = monitor_list->next)
        {
            monitor = monitor_list->data;
            peony_file_monitor_remove (file, monitor);
        }
    }

    peony_file_list_free (search->details->files);
    search->details->files = NULL;
}

static void
start_or_stop_search_engine (PeonySearchDirectory *search, gboolean adding)
{
    if (adding && (search->details->monitor_list ||
                   search->details->pending_callback_list) &&
            search->details->query &&
            !search->details->search_running)
    {
        /* We need to start the search engine */
        search->details->search_running = TRUE;
        search->details->search_finished = FALSE;
        ensure_search_engine (search);
        peony_search_engine_set_query (search->details->engine, search->details->query);

        reset_file_list (search);

        peony_search_engine_start (search->details->engine);
    }
    else if (!adding && !search->details->monitor_list &&
             !search->details->pending_callback_list &&
             search->details->engine &&
             search->details->search_running)
    {
        search->details->search_running = FALSE;
        peony_search_engine_stop (search->details->engine);

        reset_file_list (search);
    }

}

static void
file_changed (PeonyFile *file, PeonySearchDirectory *search)
{
    GList list;

    list.data = file;
    list.next = NULL;

    peony_directory_emit_files_changed (PEONY_DIRECTORY (search), &list);
}

static void
search_monitor_add (PeonyDirectory *directory,
                    gconstpointer client,
                    gboolean monitor_hidden_files,
                    PeonyFileAttributes file_attributes,
                    PeonyDirectoryCallback callback,
                    gpointer callback_data)
{
    GList *list;
    SearchMonitor *monitor;
    PeonySearchDirectory *search;
    PeonyFile *file;

    search = PEONY_SEARCH_DIRECTORY (directory);

    monitor = g_new0 (SearchMonitor, 1);
    monitor->monitor_hidden_files = monitor_hidden_files;
    monitor->monitor_attributes = file_attributes;
    monitor->client = client;

    search->details->monitor_list = g_list_prepend (search->details->monitor_list, monitor);

    if (callback != NULL)
    {
        (* callback) (directory, search->details->files, callback_data);
    }

    for (list = search->details->files; list != NULL; list = list->next)
    {
        file = list->data;

        /* Add monitors */
        peony_file_monitor_add (file, monitor, file_attributes);
    }

    start_or_stop_search_engine (search, TRUE);
}

static void
search_monitor_remove_file_monitors (SearchMonitor *monitor, PeonySearchDirectory *search)
{
    GList *list;
    PeonyFile *file;

    for (list = search->details->files; list != NULL; list = list->next)
    {
        file = list->data;

        peony_file_monitor_remove (file, monitor);
    }
}

static void
search_monitor_destroy (SearchMonitor *monitor, PeonySearchDirectory *search)
{
    search_monitor_remove_file_monitors (monitor, search);

    g_free (monitor);
}

static void
search_monitor_remove (PeonyDirectory *directory,
                       gconstpointer client)
{
    PeonySearchDirectory *search;
    SearchMonitor *monitor;
    GList *list;

    search = PEONY_SEARCH_DIRECTORY (directory);

    for (list = search->details->monitor_list; list != NULL; list = list->next)
    {
        monitor = list->data;

        if (monitor->client == client)
        {
            search->details->monitor_list = g_list_delete_link (search->details->monitor_list, list);

            search_monitor_destroy (monitor, search);

            break;
        }
    }

    start_or_stop_search_engine (search, FALSE);
}

static void
cancel_call_when_ready (gpointer key, gpointer value, gpointer user_data)
{
    SearchCallback *search_callback;
    PeonyFile *file;

    file = key;
    search_callback = user_data;

    peony_file_cancel_call_when_ready (file, search_callback_file_ready_callback,
                                      search_callback);
}

static void
search_callback_destroy (SearchCallback *search_callback)
{
    if (search_callback->non_ready_hash)
    {
        g_hash_table_foreach (search_callback->non_ready_hash, cancel_call_when_ready, search_callback);
        g_hash_table_destroy (search_callback->non_ready_hash);
    }

    peony_file_list_free (search_callback->file_list);

    g_free (search_callback);
}

static void
search_callback_invoke_and_destroy (SearchCallback *search_callback)
{
    search_callback->callback (PEONY_DIRECTORY (search_callback->search_directory),
                               search_callback->file_list,
                               search_callback->callback_data);

    search_callback->search_directory->details->callback_list =
        g_list_remove (search_callback->search_directory->details->callback_list, search_callback);

    search_callback_destroy (search_callback);
}

static void
search_callback_file_ready_callback (PeonyFile *file, gpointer data)
{
    SearchCallback *search_callback = data;

    g_hash_table_remove (search_callback->non_ready_hash, file);

    if (g_hash_table_size (search_callback->non_ready_hash) == 0)
    {
        search_callback_invoke_and_destroy (search_callback);
    }
}

static void
search_callback_add_file_callbacks (SearchCallback *callback)
{
    GList *file_list_copy, *list;
    PeonyFile *file;

    file_list_copy = g_list_copy (callback->file_list);

    for (list = file_list_copy; list != NULL; list = list->next)
    {
        file = list->data;

        peony_file_call_when_ready (file,
                                   callback->wait_for_attributes,
                                   search_callback_file_ready_callback,
                                   callback);
    }
    g_list_free (file_list_copy);
}

static SearchCallback *
search_callback_find (PeonySearchDirectory *search, PeonyDirectoryCallback callback, gpointer callback_data)
{
    SearchCallback *search_callback;
    GList *list;

    for (list = search->details->callback_list; list != NULL; list = list->next)
    {
        search_callback = list->data;

        if (search_callback->callback == callback &&
                search_callback->callback_data == callback_data)
        {
            return search_callback;
        }
    }

    return NULL;
}

static SearchCallback *
search_callback_find_pending (PeonySearchDirectory *search, PeonyDirectoryCallback callback, gpointer callback_data)
{
    SearchCallback *search_callback;
    GList *list;

    for (list = search->details->pending_callback_list; list != NULL; list = list->next)
    {
        search_callback = list->data;

        if (search_callback->callback == callback &&
                search_callback->callback_data == callback_data)
        {
            return search_callback;
        }
    }

    return NULL;
}

static GHashTable *
file_list_to_hash_table (GList *file_list)
{
    GList *list;
    GHashTable *table;

    if (!file_list)
        return NULL;

    table = g_hash_table_new (NULL, NULL);

    for (list = file_list; list != NULL; list = list->next)
    {
        g_hash_table_insert (table, list->data, list->data);
    }

    return table;
}

static void
search_call_when_ready (PeonyDirectory *directory,
                        PeonyFileAttributes file_attributes,
                        gboolean wait_for_file_list,
                        PeonyDirectoryCallback callback,
                        gpointer callback_data)
{
    PeonySearchDirectory *search;
    SearchCallback *search_callback;

    search = PEONY_SEARCH_DIRECTORY (directory);

    search_callback = search_callback_find (search, callback, callback_data);
    if (search_callback == NULL)
    {
        search_callback = search_callback_find_pending (search, callback, callback_data);
    }

    if (search_callback)
    {
        g_warning ("tried to add a new callback while an old one was pending");
        return;
    }

    search_callback = g_new0 (SearchCallback, 1);
    search_callback->search_directory = search;
    search_callback->callback = callback;
    search_callback->callback_data = callback_data;
    search_callback->wait_for_attributes = file_attributes;
    search_callback->wait_for_file_list = wait_for_file_list;

    if (wait_for_file_list && !search->details->search_finished)
    {
        /* Add it to the pending callback list, which will be
         * processed when the directory has finished loading
         */
        search->details->pending_callback_list =
            g_list_prepend (search->details->pending_callback_list, search_callback);

        /* We might need to start the search engine */
        start_or_stop_search_engine (search, TRUE);
    }
    else
    {
        search_callback->file_list = peony_file_list_copy (search->details->files);
        search_callback->non_ready_hash = file_list_to_hash_table (search->details->files);

        if (!search_callback->non_ready_hash)
        {
            /* If there are no ready files, we invoke the callback
               with an empty list.
            */
            search_callback_invoke_and_destroy (search_callback);
        }
        else
        {
            search->details->callback_list = g_list_prepend (search->details->callback_list, search_callback);
            search_callback_add_file_callbacks (search_callback);
        }
    }
}

static void
search_cancel_callback (PeonyDirectory *directory,
                        PeonyDirectoryCallback callback,
                        gpointer callback_data)
{
    PeonySearchDirectory *search;
    SearchCallback *search_callback;

    search = PEONY_SEARCH_DIRECTORY (directory);
    search_callback = search_callback_find (search, callback, callback_data);

    if (search_callback)
    {
        search->details->callback_list = g_list_remove (search->details->callback_list, search_callback);

        search_callback_destroy (search_callback);

        return;
    }

    /* Check for a pending callback */
    search_callback = search_callback_find_pending (search, callback, callback_data);

    if (search_callback)
    {
        search->details->pending_callback_list = g_list_remove (search->details->pending_callback_list, search_callback);

        search_callback_destroy (search_callback);

        /* We might need to stop the search engine now */
        start_or_stop_search_engine (search, FALSE);
    }
}


static void
search_engine_hits_added (PeonySearchEngine *engine, GList *hits,
                          PeonySearchDirectory *search)
{
    GList *hit_list;
    GList *file_list;
    PeonyFile *file;
    char *uri;
    SearchMonitor *monitor;
    GList *monitor_list;

    file_list = NULL;

    for (hit_list = hits; hit_list != NULL; hit_list = hit_list->next)
    {
        uri = hit_list->data;

        if (g_str_has_suffix (uri, PEONY_SAVED_SEARCH_EXTENSION))
        {
            /* Never return saved searches themselves as hits */
            continue;
        }

        file = peony_file_get_by_uri (uri);

        for (monitor_list = search->details->monitor_list; monitor_list; monitor_list = monitor_list->next)
        {
            monitor = monitor_list->data;

            /* Add monitors */
            peony_file_monitor_add (file, monitor, monitor->monitor_attributes);
        }

        g_signal_connect (file, "changed", G_CALLBACK (file_changed), search),

                         file_list = g_list_prepend (file_list, file);
    }

    search->details->files = g_list_concat (search->details->files, file_list);

    peony_directory_emit_files_added (PEONY_DIRECTORY (search), file_list);

    file = peony_directory_get_corresponding_file (PEONY_DIRECTORY (search));
    peony_file_emit_changed (file);
    peony_file_unref (file);
}

static void
search_engine_hits_subtracted (PeonySearchEngine *engine, GList *hits,
                               PeonySearchDirectory *search)
{
    GList *hit_list;
    GList *monitor_list;
    SearchMonitor *monitor;
    GList *file_list;
    char *uri;
    PeonyFile *file;

    file_list = NULL;

    for (hit_list = hits; hit_list != NULL; hit_list = hit_list->next)
    {
        uri = hit_list->data;
        file = peony_file_get_by_uri (uri);

        for (monitor_list = search->details->monitor_list; monitor_list;
                monitor_list = monitor_list->next)
        {
            monitor = monitor_list->data;
            /* Remove monitors */
            peony_file_monitor_remove (file, monitor);
        }

        g_signal_handlers_disconnect_by_func (file, file_changed, search);

        search->details->files = g_list_remove (search->details->files, file);

        file_list = g_list_prepend (file_list, file);
    }

    peony_directory_emit_files_changed (PEONY_DIRECTORY (search), file_list);

    peony_file_list_free (file_list);

    file = peony_directory_get_corresponding_file (PEONY_DIRECTORY (search));
    peony_file_emit_changed (file);
    peony_file_unref (file);
}

static void
search_callback_add_pending_file_callbacks (SearchCallback *callback)
{
    callback->file_list = peony_file_list_copy (callback->search_directory->details->files);
    callback->non_ready_hash = file_list_to_hash_table (callback->search_directory->details->files);

    search_callback_add_file_callbacks (callback);
}

static void
search_engine_error (PeonySearchEngine *engine, const char *error_message, PeonySearchDirectory *search)
{
    GError *error;

    error = g_error_new_literal (G_IO_ERROR, G_IO_ERROR_FAILED,
                                 error_message);
    peony_directory_emit_load_error (PEONY_DIRECTORY (search),
                                    error);
    g_error_free (error);
}

static void
search_engine_finished (PeonySearchEngine *engine, PeonySearchDirectory *search)
{
    search->details->search_finished = TRUE;

    peony_directory_emit_done_loading (PEONY_DIRECTORY (search));

    /* Add all file callbacks */
    g_list_foreach (search->details->pending_callback_list,
                    (GFunc)search_callback_add_pending_file_callbacks, NULL);
    search->details->callback_list = g_list_concat (search->details->callback_list,
                                     search->details->pending_callback_list);

    g_list_free (search->details->pending_callback_list);
    search->details->pending_callback_list = NULL;
}

static void
search_force_reload (PeonyDirectory *directory)
{
    PeonySearchDirectory *search;

    search = PEONY_SEARCH_DIRECTORY (directory);

    if (!search->details->query)
    {
        return;
    }

    search->details->search_finished = FALSE;

    if (!search->details->engine)
    {
        return;
    }

    /* Remove file monitors */
    reset_file_list (search);

    if (search->details->search_running)
    {
        peony_search_engine_stop (search->details->engine);
        peony_search_engine_set_query (search->details->engine, search->details->query);
        peony_search_engine_start (search->details->engine);
    }
}

static gboolean
search_are_all_files_seen (PeonyDirectory *directory)
{
    PeonySearchDirectory *search;

    search = PEONY_SEARCH_DIRECTORY (directory);

    return (!search->details->query ||
            search->details->search_finished);
}

static gboolean
search_contains_file (PeonyDirectory *directory,
                      PeonyFile *file)
{
    PeonySearchDirectory *search;

    search = PEONY_SEARCH_DIRECTORY (directory);

    /* FIXME: Maybe put the files in a hash */
    return (g_list_find (search->details->files, file) != NULL);
}

static GList *
search_get_file_list (PeonyDirectory *directory)
{
    PeonySearchDirectory *search;

    search = PEONY_SEARCH_DIRECTORY (directory);

    return peony_file_list_copy (search->details->files);
}


static gboolean
search_is_editable (PeonyDirectory *directory)
{
    return FALSE;
}

static void
search_dispose (GObject *object)
{
    PeonySearchDirectory *search;
    GList *list;

    search = PEONY_SEARCH_DIRECTORY (object);

    /* Remove search monitors */
    if (search->details->monitor_list)
    {
        for (list = search->details->monitor_list; list != NULL; list = list->next)
        {
            search_monitor_destroy ((SearchMonitor *)list->data, search);
        }

        g_list_free (search->details->monitor_list);
        search->details->monitor_list = NULL;
    }

    reset_file_list (search);

    if (search->details->callback_list)
    {
        /* Remove callbacks */
        g_list_foreach (search->details->callback_list,
                        (GFunc)search_callback_destroy, NULL);
        g_list_free (search->details->callback_list);
        search->details->callback_list = NULL;
    }

    if (search->details->pending_callback_list)
    {
        g_list_foreach (search->details->pending_callback_list,
                        (GFunc)search_callback_destroy, NULL);
        g_list_free (search->details->pending_callback_list);
        search->details->pending_callback_list = NULL;
    }

    if (search->details->query)
    {
        g_object_unref (search->details->query);
        search->details->query = NULL;
    }

    if (search->details->engine)
    {
        if (search->details->search_running)
        {
            peony_search_engine_stop (search->details->engine);
        }

        g_object_unref (search->details->engine);
        search->details->engine = NULL;
    }

    G_OBJECT_CLASS (peony_search_directory_parent_class)->dispose (object);
}

static void
search_finalize (GObject *object)
{
    PeonySearchDirectory *search;

    search = PEONY_SEARCH_DIRECTORY (object);

    g_free (search->details->saved_search_uri);

    g_free (search->details);

    G_OBJECT_CLASS (peony_search_directory_parent_class)->finalize (object);
}

static void
peony_search_directory_init (PeonySearchDirectory *search)
{
    search->details = g_new0 (PeonySearchDirectoryDetails, 1);
}

static void
peony_search_directory_class_init (PeonySearchDirectoryClass *class)
{
    PeonyDirectoryClass *directory_class;

    G_OBJECT_CLASS (class)->dispose = search_dispose;
    G_OBJECT_CLASS (class)->finalize = search_finalize;

    directory_class = PEONY_DIRECTORY_CLASS (class);

    directory_class->are_all_files_seen = search_are_all_files_seen;
    directory_class->contains_file = search_contains_file;
    directory_class->force_reload = search_force_reload;
    directory_class->call_when_ready = search_call_when_ready;
    directory_class->cancel_callback = search_cancel_callback;

    directory_class->file_monitor_add = search_monitor_add;
    directory_class->file_monitor_remove = search_monitor_remove;

    directory_class->get_file_list = search_get_file_list;
    directory_class->is_editable = search_is_editable;
}

char *
peony_search_directory_generate_new_uri (void)
{
    static int counter = 0;
    char *uri;

    uri = g_strdup_printf (EEL_SEARCH_URI"//%d/", counter++);

    return uri;
}


void
peony_search_directory_set_query (PeonySearchDirectory *search,
                                 PeonyQuery *query)
{
    PeonyDirectory *dir;
    PeonyFile *as_file;

    if (search->details->query != query)
    {
        search->details->modified = TRUE;
    }

    if (query)
    {
        g_object_ref (query);
    }

    if (search->details->query)
    {
        g_object_unref (search->details->query);
    }

    search->details->query = query;

    dir = PEONY_DIRECTORY (search);
    as_file = dir->details->as_file;
    if (as_file != NULL)
    {
        peony_search_directory_file_update_display_name (PEONY_SEARCH_DIRECTORY_FILE (as_file));
    }
}

PeonyQuery *
peony_search_directory_get_query (PeonySearchDirectory *search)
{
    if (search->details->query != NULL)
    {
        return g_object_ref (search->details->query);
    }

    return NULL;
}

PeonySearchDirectory *
peony_search_directory_new_from_saved_search (const char *uri)
{
    PeonySearchDirectory *search;
    PeonyQuery *query;
    char *file;

    search = PEONY_SEARCH_DIRECTORY (g_object_new (PEONY_TYPE_SEARCH_DIRECTORY, NULL));

    search->details->saved_search_uri = g_strdup (uri);

    file = g_filename_from_uri (uri, NULL, NULL);
    if (file != NULL)
    {
        query = peony_query_load (file);
        if (query != NULL)
        {
            peony_search_directory_set_query (search, query);
            g_object_unref (query);
        }
        g_free (file);
    }
    else
    {
        g_warning ("Non-local saved searches not supported");
    }

    search->details->modified = FALSE;
    return search;
}

gboolean
peony_search_directory_is_saved_search (PeonySearchDirectory *search)
{
    return search->details->saved_search_uri != NULL;
}

gboolean
peony_search_directory_is_modified (PeonySearchDirectory *search)
{
    return search->details->modified;
}

gboolean
peony_search_directory_is_indexed (PeonySearchDirectory *search)
{
    ensure_search_engine (search);
    return peony_search_engine_is_indexed (search->details->engine);
}


void
peony_search_directory_save_to_file (PeonySearchDirectory *search,
                                    const char              *save_file_uri)
{
    char *file;

    file = g_filename_from_uri (save_file_uri, NULL, NULL);
    if (file == NULL)
    {
        return;
    }

    if (search->details->query != NULL)
    {
        peony_query_save (search->details->query, file);
    }

    g_free (file);
}

void
peony_search_directory_save_search (PeonySearchDirectory *search)
{
    if (search->details->saved_search_uri == NULL)
    {
        return;
    }

    peony_search_directory_save_to_file (search,
                                        search->details->saved_search_uri);
    search->details->modified = FALSE;
}
