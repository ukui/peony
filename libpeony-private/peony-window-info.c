/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-window-info.c: Interface for peony window

   Copyright (C) 2004 Red Hat Inc.

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

   Author: Alexander Larsson <alexl@redhat.com>
*/

#include <config.h>
#include "peony-window-info.h"

//#include "peony-file.h"
//#include <libpeony-private/peony-file-info.h>

enum
{
    LOADING_URI,
    SELECTION_CHANGED,
    TITLE_CHANGED,
    HIDDEN_FILES_MODE_CHANGED,
    PREVIEW_FILE,
    LAST_SIGNAL
};

static guint peony_window_info_signals[LAST_SIGNAL] = { 0 };

static void
peony_window_info_base_init (gpointer g_class)
{
    static gboolean initialized = FALSE;

    if (! initialized)
    {
        peony_window_info_signals[LOADING_URI] =
            g_signal_new ("loading_uri",
                          PEONY_TYPE_WINDOW_INFO,
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (PeonyWindowInfoIface, loading_uri),
                          NULL, NULL,
                          g_cclosure_marshal_VOID__STRING,
                          G_TYPE_NONE, 1,
                          G_TYPE_STRING);

        peony_window_info_signals[SELECTION_CHANGED] =
            g_signal_new ("selection_changed",
                          PEONY_TYPE_WINDOW_INFO,
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (PeonyWindowInfoIface, selection_changed),
                          NULL, NULL,
                          g_cclosure_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);

        peony_window_info_signals[TITLE_CHANGED] =
            g_signal_new ("title_changed",
                          PEONY_TYPE_WINDOW_INFO,
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (PeonyWindowInfoIface, title_changed),
                          NULL, NULL,
                          g_cclosure_marshal_VOID__STRING,
                          G_TYPE_NONE, 1,
                          G_TYPE_STRING);

        peony_window_info_signals[HIDDEN_FILES_MODE_CHANGED] =
            g_signal_new ("hidden_files_mode_changed",
                          PEONY_TYPE_WINDOW_INFO,
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (PeonyWindowInfoIface, hidden_files_mode_changed),
                          NULL, NULL,
                          g_cclosure_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);

        peony_window_info_signals[PREVIEW_FILE] =
            g_signal_new ("preview_file",
                          PEONY_TYPE_WINDOW_INFO,
                          G_SIGNAL_RUN_FIRST,
                          0,
                          NULL, NULL,
                          g_cclosure_marshal_VOID__POINTER,
                          G_TYPE_NONE, 1, G_TYPE_POINTER);

        peony_window_info_signals[PREVIEW_FILE] =
            g_signal_new ("office2pdf_ready",
                          PEONY_TYPE_WINDOW_INFO,
                          G_SIGNAL_RUN_FIRST,
                          0,
                          NULL, NULL,
                          g_cclosure_marshal_VOID__VOID,
                          G_TYPE_NONE,  0);

        initialized = TRUE;
    }
}

GType
peony_window_info_get_type (void)
{
    static GType type = 0;

    if (!type)
    {
        const GTypeInfo info =
        {
            sizeof (PeonyWindowInfoIface),
            peony_window_info_base_init,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE,
                                       "PeonyWindowInfo",
                                       &info, 0);
        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

void
peony_window_info_report_load_underway (PeonyWindowInfo      *window,
                                       PeonyView            *view)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->report_load_underway) (window,
            view);
}

void
peony_window_info_report_load_complete (PeonyWindowInfo      *window,
                                       PeonyView            *view)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->report_load_complete) (window,
            view);
}

void
peony_window_info_report_view_failed (PeonyWindowInfo      *window,
                                     PeonyView            *view)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->report_view_failed) (window,
            view);
}

void
peony_window_info_report_selection_changed (PeonyWindowInfo      *window)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));

/*
    GList *selection = peony_window_info_get_selection(window);
    if(selection){
        PeonyFile *file = selection->data;
        char* uri = peony_file_info_get_uri(file);
        printf("window info: preview file changed!: %s\n",uri);
    }

    g_list_free(selection);
*/

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->report_selection_changed) (window);
}

void
peony_window_info_view_visible (PeonyWindowInfo      *window,
                               PeonyView            *view)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->view_visible) (window, view);
}

void
peony_window_info_close (PeonyWindowInfo      *window)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->close_window) (window);
}

void
peony_window_info_push_status (PeonyWindowInfo      *window,
                              const char              *status)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->push_status) (window,
            status);
}

PeonyWindowType
peony_window_info_get_window_type (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), PEONY_WINDOW_SPATIAL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_window_type) (window);
}

char *
peony_window_info_get_title (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), NULL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_title) (window);
}

GList *
peony_window_info_get_history (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), NULL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_history) (window);
}

char *
peony_window_info_get_current_location (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), NULL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_current_location) (window);
}

int
peony_window_info_get_selection_count (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), 0);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_selection_count) (window);
}

GList *
peony_window_info_get_selection (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), NULL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_selection) (window);
}

PeonyWindowShowHiddenFilesMode
peony_window_info_get_hidden_files_mode (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), PEONY_WINDOW_SHOW_HIDDEN_FILES_DEFAULT);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_hidden_files_mode) (window);
}

void
peony_window_info_set_hidden_files_mode (PeonyWindowInfo *window,
                                        PeonyWindowShowHiddenFilesMode  mode)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->set_hidden_files_mode) (window,
            mode);
}

GtkUIManager *
peony_window_info_get_ui_manager (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), NULL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_ui_manager) (window);
}

PeonyWindowSlotInfo *
peony_window_info_get_active_slot (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), NULL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_active_slot) (window);
}

PeonyWindowSlotInfo *
peony_window_info_get_extra_slot (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), NULL);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_extra_slot) (window);
}

gboolean
peony_window_info_get_initiated_unmount (PeonyWindowInfo *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW_INFO (window), FALSE);

    return (* PEONY_WINDOW_INFO_GET_IFACE (window)->get_initiated_unmount) (window);
}

void
peony_window_info_set_initiated_unmount (PeonyWindowInfo *window, gboolean initiated_unmount)
{
    g_return_if_fail (PEONY_IS_WINDOW_INFO (window));

    (* PEONY_WINDOW_INFO_GET_IFACE (window)->set_initiated_unmount) (window,
            initiated_unmount);

}
