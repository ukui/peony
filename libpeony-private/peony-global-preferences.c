/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-global-preferences.c - Peony specific preference keys and
                                   functions.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Ramiro Estrugo <ramiro@eazel.com>
*/

#include <config.h>
#include "peony-global-preferences.h"

#include "peony-file-utilities.h"
#include "peony-file.h"
#include <eel/eel-glib-extensions.h>
#include <eel/eel-gtk-extensions.h>
#include <eel/eel-stock-dialogs.h>
#include <eel/eel-string.h>
#include <glib/gi18n.h>

/*
 * Public functions
 */
char *
peony_global_preferences_get_default_folder_viewer_preference_as_iid (void)
{
    int preference_value;
    const char *viewer_iid;

    preference_value =
        g_settings_get_enum (peony_preferences, PEONY_PREFERENCES_DEFAULT_FOLDER_VIEWER);

    if (preference_value == PEONY_DEFAULT_FOLDER_VIEWER_LIST_VIEW)
    {
        viewer_iid = PEONY_LIST_VIEW_IID;
    }
    else if (preference_value == PEONY_DEFAULT_FOLDER_VIEWER_COMPACT_VIEW)
    {
        viewer_iid = PEONY_COMPACT_VIEW_IID;
    }
    else
    {
        viewer_iid = PEONY_ICON_VIEW_IID;
    }

    return g_strdup (viewer_iid);
}

void
peony_global_preferences_init (void)
{
    static gboolean initialized = FALSE;
    int i;

    if (initialized)
    {
        return;
    }

    initialized = TRUE;
    
    peony_preferences = g_settings_new("org.ukui.peony.preferences");
    peony_media_preferences = g_settings_new("org.ukui.media-handling");
    peony_window_state = g_settings_new("org.ukui.peony.window-state");
    peony_icon_view_preferences = g_settings_new("org.ukui.peony.icon-view");
    peony_compact_view_preferences = g_settings_new("org.ukui.peony.compact-view");
    peony_computer_view_preferences = g_settings_new("org.ukui.peony.computer-view");
    peony_desktop_preferences = g_settings_new("org.ukui.peony.desktop");
    peony_tree_sidebar_preferences = g_settings_new("org.ukui.peony.sidebar-panels.tree");
    peony_list_view_preferences = g_settings_new("org.ukui.peony.list-view");
    peony_extension_preferences = g_settings_new("org.ukui.peony.extensions");

    ukui_background_preferences = g_settings_new("org.ukui.background");
    ukui_lockdown_preferences = g_settings_new("org.ukui.lockdown");
}
