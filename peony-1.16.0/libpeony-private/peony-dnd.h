/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-dnd.h - Common Drag & drop handling code shared by the icon container
   and the list view.

   Copyright (C) 2000 Eazel, Inc.

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

   Authors: Pavel Cisler <pavel@eazel.com>,
	    Ettore Perazzoli <ettore@gnu.org>
*/

#ifndef PEONY_DND_H
#define PEONY_DND_H

#include <gtk/gtk.h>
#include <libpeony-private/peony-window-slot-info.h>

/* Drag & Drop target names. */
#define PEONY_ICON_DND_UKUI_ICON_LIST_TYPE	"x-special/ukui-icon-list"
#define PEONY_ICON_DND_URI_LIST_TYPE		"text/uri-list"
#define PEONY_ICON_DND_NETSCAPE_URL_TYPE	"_NETSCAPE_URL"
#define PEONY_ICON_DND_COLOR_TYPE		"application/x-color"
#define PEONY_ICON_DND_BGIMAGE_TYPE		"property/bgimage"
#define PEONY_ICON_DND_KEYWORD_TYPE		"property/keyword"
#define PEONY_ICON_DND_RESET_BACKGROUND_TYPE "x-special/ukui-reset-background"
#define PEONY_ICON_DND_ROOTWINDOW_DROP_TYPE	"application/x-rootwindow-drop"
#define PEONY_ICON_DND_XDNDDIRECTSAVE_TYPE	"XdndDirectSave0" /* XDS Protocol Type */
#define PEONY_ICON_DND_RAW_TYPE	"application/octet-stream"

/* Item of the drag selection list */
typedef struct
{
    char *uri;
    gboolean got_icon_position;
    int icon_x, icon_y;
    int icon_width, icon_height;
} PeonyDragSelectionItem;

/* Standard Drag & Drop types. */
typedef enum
{
    PEONY_ICON_DND_UKUI_ICON_LIST,
    PEONY_ICON_DND_URI_LIST,
    PEONY_ICON_DND_NETSCAPE_URL,
    PEONY_ICON_DND_COLOR,
    PEONY_ICON_DND_BGIMAGE,
    PEONY_ICON_DND_KEYWORD,
    PEONY_ICON_DND_TEXT,
    PEONY_ICON_DND_RESET_BACKGROUND,
    PEONY_ICON_DND_XDNDDIRECTSAVE,
    PEONY_ICON_DND_RAW,
    PEONY_ICON_DND_ROOTWINDOW_DROP
} PeonyIconDndTargetType;

typedef enum
{
    PEONY_DND_ACTION_FIRST = GDK_ACTION_ASK << 1,
    PEONY_DND_ACTION_SET_AS_BACKGROUND = PEONY_DND_ACTION_FIRST << 0,
    PEONY_DND_ACTION_SET_AS_FOLDER_BACKGROUND = PEONY_DND_ACTION_FIRST << 1,
    PEONY_DND_ACTION_SET_AS_GLOBAL_BACKGROUND = PEONY_DND_ACTION_FIRST << 2
} PeonyDndAction;

/* drag&drop-related information. */
typedef struct
{
    GtkTargetList *target_list;

    /* Stuff saved at "receive data" time needed later in the drag. */
    gboolean got_drop_data_type;
    PeonyIconDndTargetType data_type;
    GtkSelectionData *selection_data;
    char *direct_save_uri;

    /* Start of the drag, in window coordinates. */
    int start_x, start_y;

    /* List of PeonyDragSelectionItems, representing items being dragged, or NULL
     * if data about them has not been received from the source yet.
     */
    GList *selection_list;

    /* has the drop occured ? */
    gboolean drop_occured;

    /* whether or not need to clean up the previous dnd data */
    gboolean need_to_destroy;

    /* autoscrolling during dragging */
    int auto_scroll_timeout_id;
    gboolean waiting_to_autoscroll;
    gint64 start_auto_scroll_in;

} PeonyDragInfo;

typedef struct
{
    /* NB: the following elements are managed by us */
    gboolean have_data;
    gboolean have_valid_data;

    gboolean drop_occured;

    unsigned int info;
    union
    {
        GList *selection_list;
        GList *uri_list;
        char *netscape_url;
    } data;

    /* NB: the following elements are managed by the caller of
     *   peony_drag_slot_proxy_init() */

    /* a fixed location, or NULL to use slot's location */
    GFile *target_location;
    /* a fixed slot, or NULL to use the window's active slot */
    PeonyWindowSlotInfo *target_slot;
} PeonyDragSlotProxyInfo;

typedef void		(* PeonyDragEachSelectedItemDataGet)	(const char *url,
        int x, int y, int w, int h,
        gpointer data);
typedef void		(* PeonyDragEachSelectedItemIterator)	(PeonyDragEachSelectedItemDataGet iteratee,
        gpointer iterator_context,
        gpointer data);

void			    peony_drag_init				(PeonyDragInfo		      *drag_info,
        const GtkTargetEntry		      *drag_types,
        int				       drag_type_count,
        gboolean			       add_text_targets);
void			    peony_drag_finalize			(PeonyDragInfo		      *drag_info);
PeonyDragSelectionItem  *peony_drag_selection_item_new		(void);
void			    peony_drag_destroy_selection_list	(GList				      *selection_list);
GList			   *peony_drag_build_selection_list		(GtkSelectionData		      *data);

GList *			    peony_drag_uri_list_from_selection_list	(const GList			      *selection_list);

GList *			    peony_drag_uri_list_from_array		(const char			     **uris);

gboolean		    peony_drag_items_local			(const char			      *target_uri,
        const GList			      *selection_list);
gboolean		    peony_drag_uris_local			(const char			      *target_uri,
        const GList			      *source_uri_list);
gboolean		    peony_drag_items_on_desktop		(const GList			      *selection_list);
void			    peony_drag_default_drop_action_for_icons (GdkDragContext			      *context,
        const char			      *target_uri,
        const GList			      *items,
        int				      *action);
GdkDragAction		    peony_drag_default_drop_action_for_netscape_url (GdkDragContext			     *context);
GdkDragAction		    peony_drag_default_drop_action_for_uri_list     (GdkDragContext			     *context,
        const char			     *target_uri_string);
gboolean		    peony_drag_drag_data_get			(GtkWidget			      *widget,
        GdkDragContext			      *context,
        GtkSelectionData		      *selection_data,
        guint				       info,
        guint32			       time,
        gpointer			       container_context,
        PeonyDragEachSelectedItemIterator  each_selected_item_iterator);
int			    peony_drag_modifier_based_action		(int				       default_action,
        int				       non_default_action);

GdkDragAction		    peony_drag_drop_action_ask		(GtkWidget			      *widget,
        GdkDragAction			       possible_actions);
GdkDragAction		    peony_drag_drop_background_ask		(GtkWidget			      *widget,
        GdkDragAction			       possible_actions);

gboolean		    peony_drag_autoscroll_in_scroll_region	(GtkWidget			      *widget);
void			    peony_drag_autoscroll_calculate_delta	(GtkWidget			      *widget,
        float				      *x_scroll_delta,
        float				      *y_scroll_delta);
void			    peony_drag_autoscroll_start		(PeonyDragInfo		      *drag_info,
        GtkWidget			      *widget,
        GSourceFunc			       callback,
        gpointer			       user_data);
void			    peony_drag_autoscroll_stop		(PeonyDragInfo		      *drag_info);

gboolean		    peony_drag_selection_includes_special_link (GList			      *selection_list);

void                        peony_drag_slot_proxy_init               (GtkWidget *widget,
        PeonyDragSlotProxyInfo *drag_info);

#endif
