/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-window-slot.h: Peony window slot

   Copyright (C) 2008 Free Software Foundation, Inc.

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

   Author: Christian Neumair <cneumair@gnome.org>
*/

#ifndef PEONY_WINDOW_SLOT_H
#define PEONY_WINDOW_SLOT_H

#include "peony-window-pane.h"
#include "peony-query-editor.h"
#include <glib/gi18n.h>

#define PEONY_TYPE_WINDOW_SLOT	 (peony_window_slot_get_type())
#define PEONY_WINDOW_SLOT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_WINDOW_SLOT, PeonyWindowSlotClass))
#define PEONY_WINDOW_SLOT(obj)	 (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_WINDOW_SLOT, PeonyWindowSlot))
#define PEONY_IS_WINDOW_SLOT(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_WINDOW_SLOT))
#define PEONY_IS_WINDOW_SLOT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_WINDOW_SLOT))
#define PEONY_WINDOW_SLOT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_WINDOW_SLOT, PeonyWindowSlotClass))

typedef enum
{
    PEONY_LOCATION_CHANGE_STANDARD,
    PEONY_LOCATION_CHANGE_BACK,
    PEONY_LOCATION_CHANGE_FORWARD,
    PEONY_LOCATION_CHANGE_RELOAD,
    PEONY_LOCATION_CHANGE_REDIRECT,
    PEONY_LOCATION_CHANGE_FALLBACK
} PeonyLocationChangeType;

struct PeonyWindowSlotClass
{
    GObjectClass parent_class;

    /* wrapped PeonyWindowInfo signals, for overloading */
    void (* active)   (PeonyWindowSlot *slot);
    void (* inactive) (PeonyWindowSlot *slot);

    void (* update_query_editor) (PeonyWindowSlot *slot);
};

/* Each PeonyWindowSlot corresponds to
 * a location in the window for displaying
 * a PeonyView.
 *
 * For navigation windows, this would be a
 * tab, while spatial windows only have one slot.
 */
struct PeonyWindowSlot
{
    GObject parent;

    PeonyWindowPane *pane;

    /* content_box contains
     *  1) an event box containing extra_location_widgets
     *  2) the view box for the content view
     */
    GtkWidget *content_box;
    GtkWidget *extra_location_frame;
    GtkWidget *extra_location_widgets;
    GtkWidget *view_box;

    PeonyView *content_view;
    PeonyView *new_content_view;

    /* Information about bookmarks */
    PeonyBookmark *current_location_bookmark;
    PeonyBookmark *last_location_bookmark;

    /* Current location. */
    GFile *location;
    char *title;
    char *status_text;

    PeonyFile *viewed_file;
    gboolean viewed_file_seen;
    gboolean viewed_file_in_trash;

    gboolean allow_stop;

    PeonyQueryEditor *query_editor;

    /* New location. */
    PeonyLocationChangeType location_change_type;
    guint location_change_distance;
    GFile *pending_location;
    char *pending_scroll_to;
    GList *pending_selection;
    PeonyFile *determine_view_file;
    GCancellable *mount_cancellable;
    GError *mount_error;
    gboolean tried_mount;
    PeonyWindowGoToCallback open_callback;
    gpointer open_callback_user_data;

    GCancellable *find_mount_cancellable;

    gboolean visible;
};

GType   peony_window_slot_get_type (void);

char *  peony_window_slot_get_title			   (PeonyWindowSlot *slot);
void    peony_window_slot_update_title		   (PeonyWindowSlot *slot);
void    peony_window_slot_update_icon		   (PeonyWindowSlot *slot);
void    peony_window_slot_update_query_editor	   (PeonyWindowSlot *slot);

GFile * peony_window_slot_get_location		   (PeonyWindowSlot *slot);
char *  peony_window_slot_get_location_uri		   (PeonyWindowSlot *slot);

void    peony_window_slot_close			   (PeonyWindowSlot *slot);
void    peony_window_slot_reload			   (PeonyWindowSlot *slot);

void			peony_window_slot_open_location	      (PeonyWindowSlot	*slot,
        GFile			*location,
        gboolean			 close_behind);
void			peony_window_slot_open_location_with_selection (PeonyWindowSlot	    *slot,
        GFile		    *location,
        GList		    *selection,
        gboolean		     close_behind);
void			peony_window_slot_open_location_full       (PeonyWindowSlot	*slot,
        GFile			*location,
        PeonyWindowOpenMode	 mode,
        PeonyWindowOpenFlags	 flags,
        GList			*new_selection,
        PeonyWindowGoToCallback   callback,
        gpointer		 user_data);
void			peony_window_slot_stop_loading	      (PeonyWindowSlot	*slot);

void			peony_window_slot_set_content_view	      (PeonyWindowSlot	*slot,
        const char		*id);
const char	       *peony_window_slot_get_content_view_id      (PeonyWindowSlot	*slot);
gboolean		peony_window_slot_content_view_matches_iid (PeonyWindowSlot	*slot,
        const char		*iid);

void                    peony_window_slot_connect_content_view     (PeonyWindowSlot       *slot,
        PeonyView             *view);
void                    peony_window_slot_disconnect_content_view  (PeonyWindowSlot       *slot,
        PeonyView             *view);

#define peony_window_slot_go_to(slot,location, new_tab) \
	peony_window_slot_open_location_full(slot, location, PEONY_WINDOW_OPEN_ACCORDING_TO_MODE, \
						(new_tab ? PEONY_WINDOW_OPEN_FLAG_NEW_TAB : 0), \
						NULL, NULL, NULL)

#define peony_window_slot_go_to_full(slot, location, new_tab, callback, user_data) \
	peony_window_slot_open_location_full(slot, location, PEONY_WINDOW_OPEN_ACCORDING_TO_MODE, \
						(new_tab ? PEONY_WINDOW_OPEN_FLAG_NEW_TAB : 0), \
						NULL, callback, user_data)

#define peony_window_slot_go_to_with_selection(slot,location,new_selection) \
	peony_window_slot_open_location_with_selection(slot, location, new_selection, FALSE)

void    peony_window_slot_go_home			   (PeonyWindowSlot *slot,
        gboolean            new_tab);
void    peony_window_slot_go_up			   (PeonyWindowSlot *slot,
        gboolean           close_behind);

void    peony_window_slot_set_content_view_widget	   (PeonyWindowSlot *slot,
        PeonyView       *content_view);
void    peony_window_slot_set_viewed_file		   (PeonyWindowSlot *slot,
        PeonyFile      *file);
void    peony_window_slot_set_allow_stop		   (PeonyWindowSlot *slot,
        gboolean	    allow_stop);
void    peony_window_slot_set_status			   (PeonyWindowSlot *slot,
        const char	 *status);

void    peony_window_slot_add_extra_location_widget     (PeonyWindowSlot *slot,
        GtkWidget       *widget);
void    peony_window_slot_remove_extra_location_widgets (PeonyWindowSlot *slot);

void    peony_window_slot_add_current_location_to_history_list (PeonyWindowSlot *slot);

void    peony_window_slot_is_in_active_pane (PeonyWindowSlot *slot, gboolean is_active);

#endif /* PEONY_WINDOW_SLOT_H */
