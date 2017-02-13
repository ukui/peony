/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-window-info.h: Interface for peony windows

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

#ifndef PEONY_WINDOW_INFO_H
#define PEONY_WINDOW_INFO_H

#include <glib-object.h>
#include <libpeony-private/peony-view.h>
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum
    {
        PEONY_WINDOW_SHOW_HIDDEN_FILES_DEFAULT,
        PEONY_WINDOW_SHOW_HIDDEN_FILES_ENABLE,
        PEONY_WINDOW_SHOW_HIDDEN_FILES_DISABLE
    }
    PeonyWindowShowHiddenFilesMode;


    typedef enum
    {
        PEONY_WINDOW_OPEN_ACCORDING_TO_MODE,
        PEONY_WINDOW_OPEN_IN_SPATIAL,
        PEONY_WINDOW_OPEN_IN_NAVIGATION
    } PeonyWindowOpenMode;

    typedef enum
    {
        /* used in spatial mode */
        PEONY_WINDOW_OPEN_FLAG_CLOSE_BEHIND = 1<<0,
        /* used in navigation mode */
        PEONY_WINDOW_OPEN_FLAG_NEW_WINDOW = 1<<1,
        PEONY_WINDOW_OPEN_FLAG_NEW_TAB = 1<<2
    } PeonyWindowOpenFlags;

    typedef	enum
    {
        PEONY_WINDOW_SPATIAL,
        PEONY_WINDOW_NAVIGATION,
        PEONY_WINDOW_DESKTOP
    } PeonyWindowType;

#define PEONY_TYPE_WINDOW_INFO           (peony_window_info_get_type ())
#define PEONY_WINDOW_INFO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_WINDOW_INFO, PeonyWindowInfo))
#define PEONY_IS_WINDOW_INFO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_WINDOW_INFO))
#define PEONY_WINDOW_INFO_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), PEONY_TYPE_WINDOW_INFO, PeonyWindowInfoIface))

#ifndef PEONY_WINDOW_DEFINED
#define PEONY_WINDOW_DEFINED
    /* Using PeonyWindow for the vtable to make implementing this in
     * PeonyWindow easier */
    typedef struct PeonyWindow          PeonyWindow;
#endif

#ifndef PEONY_WINDOW_SLOT_DEFINED
#define PEONY_WINDOW_SLOT_DEFINED
    typedef struct PeonyWindowSlot      PeonyWindowSlot;
#endif


    typedef PeonyWindowSlot              PeonyWindowSlotInfo;
    typedef PeonyWindow                  PeonyWindowInfo;

    typedef struct _PeonyWindowInfoIface PeonyWindowInfoIface;

    typedef void (* PeonyWindowGoToCallback) (PeonyWindow *window,
    					     GError *error,
    					     gpointer user_data);

    struct _PeonyWindowInfoIface
    {
        GTypeInterface g_iface;

        /* signals: */

        void           (* loading_uri)              (PeonyWindowInfo *window,
                const char        *uri);
        /* Emitted when the view in the window changes the selection */
        void           (* selection_changed)        (PeonyWindowInfo *window);
        void           (* title_changed)            (PeonyWindowInfo *window,
                const char         *title);
        void           (* hidden_files_mode_changed)(PeonyWindowInfo *window);

        /* VTable: */
        /* A view calls this once after a load_location, once it starts loading the
         * directory. Might be called directly, or later on the mainloop.
         * This can also be called at any other time if the view needs to
         * re-load the location. But the view needs to call load_complete first if
         * its currently loading. */
        void (* report_load_underway) (PeonyWindowInfo *window,
                                       PeonyView *view);
        /* A view calls this once after reporting load_underway, when the location
           has been fully loaded, or when the load was stopped
           (by an error or by the user). */
        void (* report_load_complete) (PeonyWindowInfo *window,
                                       PeonyView *view);
        /* This can be called at any time when there has been a catastrophic failure of
           the view. It will result in the view being removed. */
        void (* report_view_failed)   (PeonyWindowInfo *window,
                                       PeonyView *view);
        void (* report_selection_changed) (PeonyWindowInfo *window);

        /* Returns the number of selected items in the view */
        int  (* get_selection_count)  (PeonyWindowInfo    *window);

        /* Returns a list of uris for th selected items in the view, caller frees it */
        GList *(* get_selection)      (PeonyWindowInfo    *window);

        char * (* get_current_location)  (PeonyWindowInfo *window);
        void   (* push_status)           (PeonyWindowInfo *window,
                                          const char *status);
        char * (* get_title)             (PeonyWindowInfo *window);
        GList *(* get_history)           (PeonyWindowInfo *window);
        PeonyWindowType
        (* get_window_type)       (PeonyWindowInfo *window);
        PeonyWindowShowHiddenFilesMode
        (* get_hidden_files_mode) (PeonyWindowInfo *window);
        void   (* set_hidden_files_mode) (PeonyWindowInfo *window,
                                          PeonyWindowShowHiddenFilesMode mode);

        PeonyWindowSlotInfo * (* get_active_slot) (PeonyWindowInfo *window);
        PeonyWindowSlotInfo * (* get_extra_slot)  (PeonyWindowInfo *window);

        gboolean (* get_initiated_unmount) (PeonyWindowInfo *window);
        void   (* set_initiated_unmount) (PeonyWindowInfo *window,
                                          gboolean initiated_unmount);

        void   (* view_visible)        (PeonyWindowInfo *window,
                                        PeonyView *view);
        void   (* close_window)       (PeonyWindowInfo *window);
        GtkUIManager *     (* get_ui_manager)   (PeonyWindowInfo *window);
    };

    GType                             peony_window_info_get_type                 (void);
    void                              peony_window_info_report_load_underway     (PeonyWindowInfo                *window,
            PeonyView                      *view);
    void                              peony_window_info_report_load_complete     (PeonyWindowInfo                *window,
            PeonyView                      *view);
    void                              peony_window_info_report_view_failed       (PeonyWindowInfo                *window,
            PeonyView                      *view);
    void                              peony_window_info_report_selection_changed (PeonyWindowInfo                *window);
    PeonyWindowSlotInfo *          peony_window_info_get_active_slot          (PeonyWindowInfo                *window);
    PeonyWindowSlotInfo *          peony_window_info_get_extra_slot           (PeonyWindowInfo                *window);
    void                              peony_window_info_view_visible             (PeonyWindowInfo                *window,
            PeonyView                      *view);
    void                              peony_window_info_close                    (PeonyWindowInfo                *window);
    void                              peony_window_info_push_status              (PeonyWindowInfo                *window,
            const char                        *status);
    PeonyWindowType                peony_window_info_get_window_type          (PeonyWindowInfo                *window);
    char *                            peony_window_info_get_title                (PeonyWindowInfo                *window);
    GList *                           peony_window_info_get_history              (PeonyWindowInfo                *window);
    char *                            peony_window_info_get_current_location     (PeonyWindowInfo                *window);
    int                               peony_window_info_get_selection_count      (PeonyWindowInfo                *window);
    GList *                           peony_window_info_get_selection            (PeonyWindowInfo                *window);
    PeonyWindowShowHiddenFilesMode peony_window_info_get_hidden_files_mode    (PeonyWindowInfo                *window);
    void                              peony_window_info_set_hidden_files_mode    (PeonyWindowInfo                *window,
            PeonyWindowShowHiddenFilesMode  mode);
    gboolean                          peony_window_info_get_initiated_unmount    (PeonyWindowInfo                *window);
    void                              peony_window_info_set_initiated_unmount    (PeonyWindowInfo                *window,
            gboolean initiated_unmount);
    GtkUIManager *                    peony_window_info_get_ui_manager           (PeonyWindowInfo                *window);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_WINDOW_INFO_H */
