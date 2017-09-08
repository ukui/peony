/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2002 James Willcox
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "peony-recent.h"

#include <eel/eel-vfs-extensions.h>

#define DEFAULT_APP_EXEC "gvfs-open %u"

static GtkRecentManager *
peony_recent_get_manager (void)
{
    static GtkRecentManager *manager = NULL;

    if (manager == NULL)
    {
        manager = gtk_recent_manager_get_default ();
    }

    return manager;
}

void
peony_recent_add_file (PeonyFile *file,
                      GAppInfo *application)
{
    GtkRecentData recent_data;
    char *uri;

    uri = peony_file_get_uri (file);

    /* do not add trash:// etc */
    if (eel_uri_is_trash (uri)  ||
            eel_uri_is_search (uri) ||
            eel_uri_is_desktop (uri))
    {
        g_free (uri);
        return;
    }

    recent_data.display_name = NULL;
    recent_data.description = NULL;

    recent_data.mime_type = peony_file_get_mime_type (file);
    recent_data.app_name = g_strdup (g_get_application_name ());

    if (application != NULL)
        recent_data.app_exec = g_strdup (g_app_info_get_commandline (application));
    else
        recent_data.app_exec = g_strdup (DEFAULT_APP_EXEC);

    recent_data.groups = NULL;
    recent_data.is_private = FALSE;

    gtk_recent_manager_add_full (peony_recent_get_manager (),
                                 uri, &recent_data);

    g_free (recent_data.mime_type);
    g_free (recent_data.app_name);
    g_free (recent_data.app_exec);

    g_free (uri);
}

void recent_file_remove (gpointer data,gpointer user_data)
{
	char          *pFileUri    = NULL;
	GtkRecentInfo *pRecentInfo = NULL;
	if(NULL == data)
	{
		return;
	}

	pRecentInfo = (GtkRecentInfo *)data;
	pFileUri = gtk_recent_info_get_uri(pRecentInfo);
	if(NULL != pFileUri)
	{
		gtk_recent_manager_remove_item(peony_recent_get_manager(),pFileUri,NULL);
	}
	
}

void empty_recent_file()
{
	GList *listRecentFile = NULL;

	listRecentFile = gtk_recent_manager_get_items(peony_recent_get_manager());
	if(NULL != listRecentFile)
	{ 
		g_list_foreach(listRecentFile,recent_file_remove,NULL);
		g_list_free(listRecentFile);
	}
	
}

void delete_recent_file(char *pUri)
{
	gboolean bRet = FALSE;
	GError   *error	   = NULL;


	if(NULL == pUri)
	{
		return;
	}

	bRet = gtk_recent_manager_remove_item(peony_recent_get_manager(),pUri,&error);
	if(TRUE != bRet)
	{
		peony_debug_log(TRUE,"recent","gtk_recent_manager_remove_item failed [%s][%s]",pUri,error->message);
		g_error_free (error);
	}
}

