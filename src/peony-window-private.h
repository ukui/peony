/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Peony is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Peony is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *           Darin Adler <darin@bentspoon.com>
 *
 */

#ifndef PEONY_WINDOW_PRIVATE_H
#define PEONY_WINDOW_PRIVATE_H

#include "peony-window.h"
#include "peony-window-slot.h"
#include "peony-window-pane.h"
#include "peony-spatial-window.h"
#include "peony-navigation-window.h"
#include "peony-bookmark-list.h"

#include <libpeony-private/peony-directory.h>

#include "file-manager/test-widget.h"

struct _PeonyNavigationWindowPane;

/* FIXME bugzilla.gnome.org 42575: Migrate more fields into here. */
struct PeonyWindowDetails
{
    GtkWidget *grid;

    GtkWidget *statusbar;
    GtkWidget *menubar;

    GtkUIManager *ui_manager;
    GtkActionGroup *main_action_group; /* owned by ui_manager */
    guint help_message_cid;

    /* Menus. */
    guint extensions_menu_merge_id;
    GtkActionGroup *extensions_menu_action_group;

    GtkActionGroup *bookmarks_action_group;
    guint bookmarks_merge_id;
    PeonyBookmarkList *bookmark_list;

    PeonyWindowShowHiddenFilesMode show_hidden_files_mode;

    /* View As menu */
    GList *short_list_viewers;
    char *extra_viewer;

    /* View As choices */
    GtkActionGroup *view_as_action_group; /* owned by ui_manager */
    GtkRadioAction *view_as_radio_action;
    GtkRadioAction *extra_viewer_radio_action;
    guint short_list_merge_id;
    guint extra_viewer_merge_id;

    /* Ensures that we do not react on signals of a
     * view that is re-used as new view when its loading
     * is cancelled
     */
    gboolean temporarily_ignore_view_signals;

    /* available panes, and active pane.
     * Both of them may never be NULL.
     */
    GList *panes;
    PeonyWindowPane *active_pane;

    /* So we can tell which window initiated
     * an unmount operation.
     */
    gboolean initiated_unmount;
};

struct _PeonyNavigationWindowDetails
{
    GtkWidget *content_paned;
    GtkWidget *content_box;
    GtkActionGroup *navigation_action_group; /* owned by ui_manager */

    GtkSizeGroup *header_size_group;

    /* Side Pane */
    int side_pane_width;
    PeonySidebar *current_side_panel;

    /* Menus */
    GtkActionGroup *go_menu_action_group;
    guint refresh_go_menu_idle_id;
    guint go_menu_merge_id;

    /* Toolbar */
    GtkWidget *toolbar;

    guint extensions_toolbar_merge_id;
    GtkActionGroup *extensions_toolbar_action_group;

    /* spinner */
    gboolean    spinner_active;
    GtkWidget  *spinner;

    /* focus widget before the location bar has been shown temporarily */
    GtkWidget *last_focus_widget;

    /* split view */
    GtkWidget *split_view_hpane;
    gboolean is_split_view_showing;

    /* hbox for kinds of preview views */
    GtkBox *preview_hbox;

    /* gtk_source_view */
    GtkWidget *test_widget;
    TestWidget *gtk_source_widget;

    /* pdf view */
    GtkWidget *pdf_swindow;
    GtkWidget *pdf_view;

    /* web view  for exel*/
    GtkWidget *web_swindow;
    GtkWidget *web_view;

    /* empty view */
    GtkWidget *empty_window;
    GtkWidget *hint_view;

    /* preview file name */
    char *current_preview_filename;
    char *pending_preview_filename;
};

#define PEONY_MENU_PATH_BACK_ITEM			"/menu/Go/Back"
#define PEONY_MENU_PATH_FORWARD_ITEM			"/menu/Go/Forward"
#define PEONY_MENU_PATH_UP_ITEM			"/menu/Go/Up"

#define PEONY_MENU_PATH_RELOAD_ITEM			"/menu/View/Reload"
#define PEONY_MENU_PATH_ZOOM_IN_ITEM			"/menu/View/Zoom Items Placeholder/Zoom In"
#define PEONY_MENU_PATH_ZOOM_OUT_ITEM		"/menu/View/Zoom Items Placeholder/Zoom Out"
#define PEONY_MENU_PATH_ZOOM_NORMAL_ITEM		"/menu/View/Zoom Items Placeholder/Zoom Normal"

#define PEONY_COMMAND_BACK				"/commands/Back"
#define PEONY_COMMAND_FORWARD			"/commands/Forward"
#define PEONY_COMMAND_UP				"/commands/Up"

#define PEONY_COMMAND_RELOAD				"/commands/Reload"
#define PEONY_COMMAND_BURN_CD			"/commands/Burn CD"
#define PEONY_COMMAND_STOP				"/commands/Stop"
#define PEONY_COMMAND_ZOOM_IN			"/commands/Zoom In"
#define PEONY_COMMAND_ZOOM_OUT			"/commands/Zoom Out"
#define PEONY_COMMAND_ZOOM_NORMAL			"/commands/Zoom Normal"

/* window geometry */
/* Min values are very small, and a Peony window at this tiny size is *almost*
 * completely unusable. However, if all the extra bits (sidebar, location bar, etc)
 * are turned off, you can see an icon or two at this size. See bug 5946.
 */

#define PEONY_SPATIAL_WINDOW_MIN_WIDTH			100
#define PEONY_SPATIAL_WINDOW_MIN_HEIGHT			100
#define PEONY_SPATIAL_WINDOW_DEFAULT_WIDTH			500
#define PEONY_SPATIAL_WINDOW_DEFAULT_HEIGHT			300

#define PEONY_NAVIGATION_WINDOW_MIN_WIDTH			200
#define PEONY_NAVIGATION_WINDOW_MIN_HEIGHT			200
#define PEONY_NAVIGATION_WINDOW_DEFAULT_WIDTH		800
#define PEONY_NAVIGATION_WINDOW_DEFAULT_HEIGHT		550

typedef void (*PeonyBookmarkFailedCallback) (PeonyWindow *window,
        PeonyBookmark *bookmark);

void               peony_window_set_status                            (PeonyWindow    *window,
        PeonyWindowSlot *slot,
        const char        *status);
void               peony_window_load_view_as_menus                    (PeonyWindow    *window);
void               peony_window_load_extension_menus                  (PeonyWindow    *window);
void               peony_window_initialize_menus                      (PeonyWindow    *window);
void               peony_window_finalize_menus                        (PeonyWindow    *window);
PeonyWindowPane *peony_window_get_next_pane                        (PeonyWindow *window);
void               peony_menus_append_bookmark_to_menu                (PeonyWindow    *window,
        PeonyBookmark  *bookmark,
        const char        *parent_path,
        const char        *parent_id,
        guint              index_in_parent,
        GtkActionGroup    *action_group,
        guint              merge_id,
        GCallback          refresh_callback,
        PeonyBookmarkFailedCallback failed_callback);
void               peony_window_update_find_menu_item                 (PeonyWindow    *window);
void               peony_window_zoom_in                               (PeonyWindow    *window);
void               peony_window_zoom_out                              (PeonyWindow    *window);
void               peony_window_zoom_to_level                         (PeonyWindow    *window,
        PeonyZoomLevel  level);
void               peony_window_zoom_to_default                       (PeonyWindow    *window);

PeonyWindowSlot *peony_window_open_slot                            (PeonyWindowPane *pane,
        PeonyWindowOpenSlotFlags flags);
void                peony_window_close_slot                           (PeonyWindowSlot *slot);

PeonyWindowSlot *peony_window_get_slot_for_view                    (PeonyWindow *window,
        PeonyView   *view);

GList *              peony_window_get_slots                           (PeonyWindow    *window);
PeonyWindowSlot * peony_window_get_active_slot                     (PeonyWindow    *window);
PeonyWindowSlot * peony_window_get_extra_slot                      (PeonyWindow    *window);
void                 peony_window_set_active_slot                     (PeonyWindow    *window,
        PeonyWindowSlot *slot);
void                 peony_window_set_active_pane                     (PeonyWindow *window,
        PeonyWindowPane *new_pane);
PeonyWindowPane * peony_window_get_active_pane                     (PeonyWindow *window);

void               peony_send_history_list_changed                    (void);
void               peony_remove_from_history_list_no_notify           (GFile             *location);
gboolean           peony_add_bookmark_to_history_list                 (PeonyBookmark  *bookmark);
gboolean           peony_add_to_history_list_no_notify                (GFile             *location,
        const char        *name,
        gboolean           has_custom_name,
        GIcon            *icon);
GList *            peony_get_history_list                             (void);
void               peony_window_bookmarks_preference_changed_callback (gpointer           user_data);


/* sync window GUI with current slot. Used when changing slots,
 * and when updating the slot state.
 */
void peony_window_sync_status           (PeonyWindow *window);
void peony_window_sync_allow_stop       (PeonyWindow *window,
                                        PeonyWindowSlot *slot);
void peony_window_sync_title            (PeonyWindow *window,
                                        PeonyWindowSlot *slot);
void peony_window_sync_zoom_widgets     (PeonyWindow *window);

/* Navigation window menus */
void               peony_navigation_window_initialize_actions                    (PeonyNavigationWindow    *window);
void               peony_navigation_window_initialize_menus                      (PeonyNavigationWindow    *window);
void               peony_navigation_window_remove_bookmarks_menu_callback        (PeonyNavigationWindow    *window);

void               peony_navigation_window_remove_bookmarks_menu_items           (PeonyNavigationWindow    *window);
void               peony_navigation_window_update_show_hide_menu_items           (PeonyNavigationWindow     *window);
void               peony_navigation_window_update_spatial_menu_item              (PeonyNavigationWindow     *window);
void               peony_navigation_window_remove_go_menu_callback    (PeonyNavigationWindow    *window);
void               peony_navigation_window_remove_go_menu_items       (PeonyNavigationWindow    *window);

/* Navigation window toolbar */
void               peony_navigation_window_activate_spinner                     (PeonyNavigationWindow    *window);
void               peony_navigation_window_initialize_toolbars                   (PeonyNavigationWindow    *window);
void               peony_navigation_window_load_extension_toolbar_items          (PeonyNavigationWindow    *window);
void               peony_navigation_window_set_spinner_active                   (PeonyNavigationWindow    *window,
        gboolean                     active);
void               peony_navigation_window_go_back                               (PeonyNavigationWindow    *window);
void               peony_navigation_window_go_forward                            (PeonyNavigationWindow    *window);
void               peony_window_close_pane                                       (PeonyWindowPane *pane);
void               peony_navigation_window_update_split_view_actions_sensitivity (PeonyNavigationWindow    *window);

#endif /* PEONY_WINDOW_PRIVATE_H */
