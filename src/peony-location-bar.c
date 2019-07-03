/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
 * Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Author: Maciej Stachowiak <mjs@eazel.com>
 *         Ettore Perazzoli <ettore@gnu.org>
 *         Michael Meeks <michael@nuclecu.unam.mx>
 *	   Andy Hertzfeld <andy@eazel.com>
 *         Zuxun Yang <yangzuxun@kylinos.cn>
 *
 */

/* peony-location-bar.c - Location bar for Peony
 */

#include <config.h>
#include "peony-location-bar.h"

#include "peony-location-entry.h"
#include "peony-window-private.h"
#include "peony-window.h"
#include "peony-navigation-window-pane.h"
#include <eel/eel-accessibility.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-gtk-macros.h>
#include <eel/eel-stock-dialogs.h>
#include <eel/eel-string.h>
#include <eel/eel-vfs-extensions.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libpeony-private/peony-icon-dnd.h>
#include <libpeony-private/peony-clipboard.h>
#include <stdio.h>
#include <string.h>

#include "peony-icon-info.h"
#include <libpeony-private/peony-file.h>

#define PEONY_DND_URI_LIST_TYPE 	  "text/uri-list"
#define PEONY_DND_TEXT_PLAIN_TYPE 	  "text/plain"

static const char untranslated_location_label[] = N_("Location:");
static const char untranslated_go_to_label[] = N_("Go To:");


 	static const gchar css[] =
	".go_down_image { "
	"padding-right: 1px;"
	"padding-left: 5px;"
        "border: 1px solid #b6b6b3;"
	"border-right: 0px;"
	"}"
	".folder_image_new { "
	"padding-right: 1px;"
	"padding-left: 5px;"
        "border: 1px solid #b6b6b3;"
	"border-right: 0px;"
	"border-left: 0px;"
	"}"
	".folder_image { "
	"padding-right: 1px;"
	"padding-left: 5px;"
        "border: 1px solid #b6b6b3;"
	"border-right: 0px;"
	"}"
	".location_button_list { "
        "border: 1px solid #b6b6b3;"
	"border-left: 0px"
	"}"
	".event_box3 { "
        "border: 1px solid #b6b6b3;"
	"border-left: 0px"
	"}"
	;

#define LOCATION_LABEL _(untranslated_location_label)
#define GO_TO_LABEL _(untranslated_go_to_label)
static void
peony_location_frame_allocate_callback (GtkWidget    *widget,
                                                                                                GdkRectangle *allocation,
                                                                                gpointer      user_data);

struct PeonyLocationBarDetails
{
    GtkLabel *label;
    PeonyEntry *entry;
	GtkWidget *hbox;
	GtkWidget *framehbox;

    char *last_location;

    guint idle_id;
	gboolean bActive;
	gboolean bNeedDes;
	gboolean bANeedemit;
	GList     *pChildList;
	GtkWidget *backbutton;
	GtkWidget *backseparator;
	GtkWidget *folder_image;
	GtkWidget *menu;
	GtkWidget *aspectframesec;
	GtkWidget *aspectframe;
	GtkWidget *interbox;
	int        iFrameWidthBack;
	int        iSecFrameWidthBack;
};

typedef struct
{
	GtkWidget *widget;
	int        iWidth;
}WIDGET_PARAM;

enum
{
    PEONY_DND_MC_DESKTOP_ICON,
    PEONY_DND_URI_LIST,
    PEONY_DND_TEXT_PLAIN,
    PEONY_DND_NTARGETS
};

enum {
	CANCEL,
	LOCATION_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

static const GtkTargetEntry drag_types [] =
{
    { PEONY_DND_URI_LIST_TYPE,   0, PEONY_DND_URI_LIST },
    { PEONY_DND_TEXT_PLAIN_TYPE, 0, PEONY_DND_TEXT_PLAIN },
};

static const GtkTargetEntry drop_types [] =
{
    { PEONY_DND_URI_LIST_TYPE,   0, PEONY_DND_URI_LIST },
    { PEONY_DND_TEXT_PLAIN_TYPE, 0, PEONY_DND_TEXT_PLAIN },
};
#define SEC_FRAME_SIZE 64
G_DEFINE_TYPE (PeonyLocationBar, peony_location_bar, GTK_TYPE_BOX);

static PeonyNavigationWindow *
peony_location_bar_get_window (GtkWidget *bar)
{
    return PEONY_NAVIGATION_WINDOW (gtk_widget_get_ancestor (bar, PEONY_TYPE_WINDOW));
}

/**
 * peony_location_bar_get_location
 *
 * Get the "URI" represented by the text in the location bar.
 *
 * @bar: A PeonyLocationBar.
 *
 * returns a newly allocated "string" containing the mangled
 * (by g_file_parse_name) text that the user typed in...maybe a URI
 * but not guaranteed.
 *
 **/
static char *
peony_location_bar_get_location (PeonyLocationBar *bar)
{
    char *user_location, *uri;
    GFile *location;

//	#if 0 //_LOCATION_BAR_CHANGE_
	if(TRUE == bar->details->bActive)
	{
	    user_location = gtk_editable_get_chars (GTK_EDITABLE (bar->details->entry), 0, -1);
	    location = g_file_parse_name (user_location);
	    g_free (user_location);
	    uri = g_file_get_uri (location);
	    g_object_unref (location);
	}
	else
	{
		uri = g_strdup(bar->details->last_location);
	}
//	#else
/*	
	user_location = gtk_editable_get_chars (GTK_EDITABLE (bar->details->entry), 0, -1);
    location = g_file_parse_name (user_location);
    g_free (user_location);
    uri = g_file_get_uri (location);
    g_object_unref (location);
//	#endif
*/	
    return uri;
}

static void
emit_location_changed (PeonyLocationBar *bar)
{
    char *location;

    location = peony_location_bar_get_location (bar);
    g_signal_emit (bar,
                   signals[LOCATION_CHANGED], 0,
                   location);
    g_free (location);
}

static void
drag_data_received_callback (GtkWidget *widget,
                             GdkDragContext *context,
                             int x,
                             int y,
                             GtkSelectionData *data,
                             guint info,
                             guint32 time,
                             gpointer callback_data)
{
    char **names;
    PeonyApplication *application;
    int name_count;
    PeonyWindow *new_window;
    PeonyNavigationWindow *window;
    GdkScreen      *screen;
    gboolean new_windows_for_extras;
    char *prompt;
    char *detail;
    GFile *location;
    PeonyLocationBar *self = PEONY_LOCATION_BAR (widget);

    g_assert (data != NULL);
    g_assert (callback_data == NULL);

    names = g_uri_list_extract_uris (gtk_selection_data_get_data (data));

    if (names == NULL || *names == NULL)
    {
        g_warning ("No D&D URI's");
        g_strfreev (names);
        gtk_drag_finish (context, FALSE, FALSE, time);
        return;
    }

    window = peony_location_bar_get_window (widget);
    new_windows_for_extras = FALSE;
    /* Ask user if they really want to open multiple windows
     * for multiple dropped URIs. This is likely to have been
     * a mistake.
     */
    name_count = g_strv_length (names);
    if (name_count > 1)
    {
        prompt = g_strdup_printf (ngettext("Do you want to view %d location?",
                                           "Do you want to view %d locations?",
                                           name_count),
                                  name_count);
        detail = g_strdup_printf (ngettext("This will open %d separate window.",
                                           "This will open %d separate windows.",
                                           name_count),
                                  name_count);
        /* eel_run_simple_dialog should really take in pairs
         * like gtk_dialog_new_with_buttons() does. */
        new_windows_for_extras = eel_run_simple_dialog
                                 (GTK_WIDGET (window),
                                  TRUE,
                                  GTK_MESSAGE_QUESTION,
                                  prompt,
                                  detail,
                                  GTK_STOCK_CANCEL, GTK_STOCK_OK,
                                  NULL) != 0 /* UKUI_OK */;

        g_free (prompt);
        g_free (detail);

        if (!new_windows_for_extras)
        {
            g_strfreev (names);
            gtk_drag_finish (context, FALSE, FALSE, time);
            return;
        }
    }
	PEONY_LOCATION_BAR (widget)->details->bANeedemit = FALSE;
    peony_location_bar_set_location (self, names[0]);
    emit_location_changed (self);

    if (new_windows_for_extras)
    {
        int i;

        application = PEONY_WINDOW (window)->application;
        screen = gtk_window_get_screen (GTK_WINDOW (window));

        for (i = 1; names[i] != NULL; ++i)
        {
            new_window = peony_application_create_navigation_window (application, screen);

            location = g_file_new_for_uri (names[i]);
            peony_window_go_to (new_window, location);
            g_object_unref (location);
        }
    }

    g_strfreev (names);

    gtk_drag_finish (context, TRUE, FALSE, time);
}

static void
drag_data_get_callback (GtkWidget *widget,
                        GdkDragContext *context,
                        GtkSelectionData *selection_data,
                        guint info,
                        guint32 time,
                        gpointer callback_data)
{
    PeonyLocationBar *self;
    char *entry_text;

    g_assert (selection_data != NULL);
    self = callback_data;

    entry_text = peony_location_bar_get_location (self);

    switch (info)
    {
    case PEONY_DND_URI_LIST:
    case PEONY_DND_TEXT_PLAIN:
        gtk_selection_data_set (selection_data,
                                gtk_selection_data_get_target (selection_data),
                                8, (guchar *) entry_text,
                                eel_strlen (entry_text));
        break;
    default:
        g_assert_not_reached ();
    }
    g_free (entry_text);
}

/* routine that determines the usize for the label widget as larger
   then the size of the largest string and then sets it to that so
   that we don't have localization problems. see
   gtk_label_finalize_lines in gtklabel.c (line 618) for the code that
   we are imitating here. */

static void
style_set_handler (GtkWidget *widget, GtkStyleContext *previous_style)
{
    PangoLayout *layout;
    int width, width2;
    int xpad;
    gint margin_start, margin_end;

    layout = gtk_label_get_layout (GTK_LABEL(widget));

    layout = pango_layout_copy (layout);

    pango_layout_set_text (layout, LOCATION_LABEL, -1);
    pango_layout_get_pixel_size (layout, &width, NULL);

    pango_layout_set_text (layout, GO_TO_LABEL, -1);
    pango_layout_get_pixel_size (layout, &width2, NULL);
    width = MAX (width, width2);

    margin_start = gtk_widget_get_margin_start (widget);
    margin_end = gtk_widget_get_margin_end (widget);
    xpad = margin_start + margin_end;

    width += 2 * xpad;

    gtk_widget_set_size_request (widget, width, -1);

    g_object_unref (layout);
}

static gboolean
label_button_pressed_callback (GtkWidget             *widget,
                               GdkEventButton        *event)
{
    PeonyNavigationWindow *window;
    PeonyWindowSlot       *slot;
    PeonyView             *view;
    GtkWidget                *label;

    if (event->button != 3)
    {
        return FALSE;
    }

    window = peony_location_bar_get_window (gtk_widget_get_parent (widget));
    slot = PEONY_WINDOW (window)->details->active_pane->active_slot;
    view = slot->content_view;
    label = gtk_bin_get_child (GTK_BIN (widget));
    /* only pop-up if the URI in the entry matches the displayed location */
    if (view == NULL ||
            strcmp (gtk_label_get_text (GTK_LABEL (label)), LOCATION_LABEL))
    {
        return FALSE;
    }

    peony_view_pop_up_location_context_menu (view, event, NULL);

    return FALSE;
}

static void
editable_activate_callback (GtkEntry *entry,
                            gpointer user_data)
{
    PeonyLocationBar *self = user_data;
    const char *entry_text;

    entry_text = gtk_entry_get_text (entry);
    if (entry_text != NULL && *entry_text != '\0')
    {
            emit_location_changed (self);
    }
}

/**
 * peony_location_bar_update_label
 *
 * if the text in the entry matches the uri, set the label to "location", otherwise use "goto"
 *
 **/
static void
peony_location_bar_update_label (PeonyLocationBar *bar)
{
    const char *current_text;
    GFile *location;
    GFile *last_location;

    if (bar->details->last_location == NULL){
        gtk_label_set_text (GTK_LABEL (bar->details->label), GO_TO_LABEL);
        peony_location_entry_set_secondary_action (PEONY_LOCATION_ENTRY (bar->details->entry),
                                                  PEONY_LOCATION_ENTRY_ACTION_GOTO);
        return;
    }

    current_text = gtk_entry_get_text (GTK_ENTRY (bar->details->entry));
    location = g_file_parse_name (current_text);
    last_location = g_file_parse_name (bar->details->last_location);

    if (g_file_equal (last_location, location)) {
        gtk_label_set_text (GTK_LABEL (bar->details->label), LOCATION_LABEL);
        peony_location_entry_set_secondary_action (PEONY_LOCATION_ENTRY (bar->details->entry),
                                                  PEONY_LOCATION_ENTRY_ACTION_CLEAR);
    } else {
        gtk_label_set_text (GTK_LABEL (bar->details->label), GO_TO_LABEL);
        peony_location_entry_set_secondary_action (PEONY_LOCATION_ENTRY (bar->details->entry),
                                                  PEONY_LOCATION_ENTRY_ACTION_GOTO);
    }

    g_object_unref (location);
    g_object_unref (last_location);
}

static void
editable_changed_callback (GtkEntry *entry,
                           gpointer user_data)
{
    peony_location_bar_update_label (PEONY_LOCATION_BAR (user_data));
}

void
peony_location_bar_activate (PeonyLocationBar *bar)
{
    /* Put the keyboard focus in the text field when switching to this mode,
     * and select all text for easy overtyping
     */
    gtk_widget_grab_focus (GTK_WIDGET (bar->details->entry));
    peony_entry_select_all (bar->details->entry);
}

static void
peony_location_bar_cancel (PeonyLocationBar *bar)
{
    char *last_location;

    last_location = bar->details->last_location;
	bar->details->bANeedemit = FALSE;
    peony_location_bar_set_location (bar, last_location);
}

static void
finalize (GObject *object)
{
    PeonyLocationBar *bar;

    bar = PEONY_LOCATION_BAR (object);

    /* cancel the pending idle call, if any */
    if (bar->details->idle_id != 0)
    {
        g_source_remove (bar->details->idle_id);
        bar->details->idle_id = 0;
    }

    g_free (bar->details->last_location);
    bar->details->last_location = NULL;

    G_OBJECT_CLASS (peony_location_bar_parent_class)->finalize (object);
}

static void
peony_location_bar_class_init (PeonyLocationBarClass *klass)
 {
    GObjectClass *gobject_class;
    GtkBindingSet *binding_set;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = finalize;

    klass->cancel = peony_location_bar_cancel;

    signals[CANCEL] = g_signal_new
            ("cancel",
            G_TYPE_FROM_CLASS (klass),
            G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
            G_STRUCT_OFFSET (PeonyLocationBarClass,
                             cancel),
            NULL, NULL,
            g_cclosure_marshal_VOID__VOID,
            G_TYPE_NONE, 0);

    signals[LOCATION_CHANGED] = g_signal_new
            ("location-changed",
            G_TYPE_FROM_CLASS (klass),
            G_SIGNAL_RUN_LAST, 0,
            NULL, NULL,
            g_cclosure_marshal_VOID__STRING,
            G_TYPE_NONE, 1, G_TYPE_STRING);

    binding_set = gtk_binding_set_by_class (klass);
    gtk_binding_entry_add_signal (binding_set, GDK_KEY_Escape, 0, "cancel", 0);

    g_type_class_add_private (klass, sizeof (PeonyLocationBarDetails));
}

void peony_set_location_bar_emit_flag(GtkWidget *widget,gboolean bEmit)
{
	if(NULL == widget)
	{
		return;
	}
	
	PEONY_LOCATION_BAR (widget)->details->bANeedemit = bEmit;
}
static gboolean
peony_location_bar_focus_in (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	PeonyLocationBar *bar = NULL;

	if(NULL == user_data)
	{
		return;
	}
	bar = PEONY_LOCATION_BAR (user_data);
	bar->details->bActive = TRUE;
	bar->details->bANeedemit = TRUE;
	peony_location_bar_set_location (PEONY_LOCATION_BAR (bar),bar->details->last_location);
}

static gboolean
peony_location_bar_focus_out (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	PeonyLocationBar *bar = NULL;
	GtkAllocation allocation;
	GList *node = NULL;
	WIDGET_PARAM *pWidget = NULL;

	if(NULL == user_data)
	{
		return;
	}
	bar = PEONY_LOCATION_BAR (user_data);
	bar->details->bActive = FALSE;
	bar->details->bANeedemit = FALSE;
	
	gtk_widget_get_allocation(bar,&allocation);
	peony_location_bar_set_location (PEONY_LOCATION_BAR (bar),bar->details->last_location);
	peony_location_frame_allocate_callback (bar,&allocation,bar);
}

static void
peony_location_bar_init (PeonyLocationBar *bar)
{
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *event_box;

    bar->details = G_TYPE_INSTANCE_GET_PRIVATE (bar, PEONY_TYPE_LOCATION_BAR,
                                                PeonyLocationBarDetails);

    gtk_orientable_set_orientation (GTK_ORIENTABLE (bar),
                                    GTK_ORIENTATION_HORIZONTAL);
    event_box = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (event_box), FALSE);

    gtk_container_set_border_width (GTK_CONTAINER (event_box), 4);
    label = gtk_label_new (LOCATION_LABEL);
    gtk_container_add   (GTK_CONTAINER (event_box), label);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
#if GTK_CHECK_VERSION (3, 16, 0)
    gtk_label_set_xalign (GTK_LABEL (label), 1.0);
    gtk_label_set_yalign (GTK_LABEL (label), 0.5);
#else
    gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);
#endif
    g_signal_connect (label, "style_set",
                      G_CALLBACK (style_set_handler), NULL);

   // gtk_box_pack_start (GTK_BOX (bar), event_box, FALSE, TRUE, 4);

    entry = peony_location_entry_new ();

    g_signal_connect_object (entry, "activate",
                             G_CALLBACK (editable_activate_callback), bar, G_CONNECT_AFTER);
    g_signal_connect_object (entry, "changed",
                             G_CALLBACK (editable_changed_callback), bar, 0);
//	#if 0 //_LOCATION_BAR_CHANGE_
	g_signal_connect_object (GTK_WIDGET (entry), "focus-out-event",
							 G_CALLBACK (peony_location_bar_focus_out), bar, G_CONNECT_AFTER);
//	#endif

    gtk_box_pack_start (GTK_BOX (bar), entry, TRUE, TRUE, 0);

    eel_accessibility_set_up_label_widget_relation (label, entry);


    /* Label context menu */
    g_signal_connect (event_box, "button-press-event",
                      G_CALLBACK (label_button_pressed_callback), NULL);

    /* Drag source */
    gtk_drag_source_set (GTK_WIDGET (event_box),
                         GDK_BUTTON1_MASK | GDK_BUTTON3_MASK,
                         drag_types, G_N_ELEMENTS (drag_types),
                         GDK_ACTION_COPY | GDK_ACTION_LINK);
    g_signal_connect_object (event_box, "drag_data_get",
                             G_CALLBACK (drag_data_get_callback), bar, 0);

    /* Drag dest. */
    gtk_drag_dest_set (GTK_WIDGET (bar),
                       GTK_DEST_DEFAULT_ALL,
                       drop_types, G_N_ELEMENTS (drop_types),
                       GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK);
    g_signal_connect (bar, "drag_data_received",
                      G_CALLBACK (drag_data_received_callback), NULL);

	bar->details->hbox = entry;
    bar->details->label = GTK_LABEL (label);
    bar->details->entry = PEONY_ENTRY (entry);
	bar->details->framehbox = NULL;
	bar->details->backbutton = NULL;
	bar->details->pChildList = NULL;

	bar->details->bActive = FALSE;
	bar->details->bNeedDes = FALSE;
	bar->details->iFrameWidthBack = 0;
	bar->details->iSecFrameWidthBack = 0;
	
	bar->details->interbox = NULL;
    //gtk_widget_show_all (GTK_WIDGET (bar));
	//gtk_widget_hide(hbox);
    gtk_widget_hide (bar->details->label);
}

GtkWidget *
peony_location_bar_new (PeonyNavigationWindowPane *pane)
{
    GtkWidget *bar;
    PeonyLocationBar *location_bar;

    bar = gtk_widget_new (PEONY_TYPE_LOCATION_BAR, NULL);
    location_bar = PEONY_LOCATION_BAR (bar);

    /* Clipboard */
    peony_clipboard_set_up_editable
    (GTK_EDITABLE (location_bar->details->entry),
     peony_window_get_ui_manager (PEONY_WINDOW (PEONY_WINDOW_PANE(pane)->window)),
     TRUE);

    return bar;
}

static gboolean
peony_location_button_pressed_callback (GtkWidget             *widget,
                               GdkEventButton        *event)
{
    PeonyNavigationWindow *window = NULL;
    PeonyWindowSlot       *slot = NULL;
    PeonyView             *view = NULL;
    GtkWidget                *label = NULL;
	GFile *location = NULL;
	char *local_uri = NULL;
	char *pWinLocalUri = NULL;

	if (NULL == widget || NULL == event)
    {
		return FALSE;
	}	

	local_uri = (char *)g_object_get_data (G_OBJECT (widget),"location-addr");
	
    window = peony_location_bar_get_window (gtk_widget_get_parent (widget));
    slot = PEONY_WINDOW (window)->details->active_pane->active_slot;
    if (NULL != slot)
    {
    	pWinLocalUri = peony_window_slot_get_location_uri(slot);
    	if(NULL != pWinLocalUri)
    	{
			if(0 != strcmp(pWinLocalUri,local_uri))
			{
				location = g_file_new_for_uri (local_uri);
				if(NULL != location)
				{
					peony_window_slot_go_to (slot, location, FALSE);
					g_object_unref (location);
				}
			}
			g_free(pWinLocalUri);
		}
    }

    return FALSE;
}

static gboolean
peony_location_menu_pressed_callback (GtkMenuItem *menu_item, PeonyNavigationWindow *window)
{
    PeonyWindowSlot       *slot = NULL;
    PeonyView             *view = NULL;
    GtkWidget                *label = NULL;
	GFile *location = NULL;
	char *local_uri = NULL;
	char *pWinLocalUri = NULL;
	
	if (NULL == menu_item || NULL == window)
    {
		return FALSE;
	}	

	local_uri = (char *)g_object_get_data (G_OBJECT (menu_item),"location-addr");
    slot = PEONY_WINDOW (window)->details->active_pane->active_slot;
    if (NULL != slot)
    {
    	pWinLocalUri = peony_window_slot_get_location_uri(slot);
    	if(NULL != pWinLocalUri)
    	{
			if(0 != strcmp(pWinLocalUri,local_uri))
			{
				location = g_file_new_for_uri (local_uri);
				if(NULL != location)
				{
					peony_window_slot_go_to (slot, location, FALSE);
					g_object_unref (location);
				}
			}
			g_free(pWinLocalUri);
		}
    }

    return FALSE;
}

static void
menu_position_callback (GtkMenu *menu,
                     int *x,
                     int *y,
                     gboolean *push_in,
                     gpointer user_data)
{
    GtkWidget *widget;
    GtkAllocation allocation;

    g_return_if_fail (GTK_IS_BUTTON (user_data));
    g_return_if_fail (!gtk_widget_get_has_window (GTK_WIDGET (user_data)));

    widget = GTK_WIDGET (user_data);

    gdk_window_get_origin (gtk_widget_get_window (widget), x, y);
    gtk_widget_get_allocation (widget, &allocation);

    *x += allocation.x;
    *y += allocation.y + allocation.height;

    *push_in = FALSE;
}

static gboolean
peony_location_backbutton_pressed_callback (GtkWidget             *widget,
                               						GdkEventButton        *event,
                               						gpointer      user_data)
{
	PeonyNavigationWindow *window;
	PeonyLocationBar *bar = NULL;
	GList *node = NULL;
	GList *next = NULL;
	WIDGET_PARAM *pWidget = NULL;
	GtkWidget *menu = NULL;
	GList *children = NULL;
	GList *li = NULL;
		
	if (NULL == widget || NULL == user_data)
	{
		return;
	}	

	bar = PEONY_LOCATION_BAR (user_data);
	window = peony_location_bar_get_window (gtk_widget_get_parent (widget));
	menu = bar->details->menu;
    gtk_menu_set_screen (GTK_MENU (menu), gtk_widget_get_screen (widget));

	children = gtk_container_get_children (GTK_CONTAINER (menu));
	for (li = children; li; li = li->next)
	{
		gtk_container_remove (GTK_CONTAINER (menu), li->data);
	}
	g_list_free (children);

	for (node = g_list_first(bar->details->pChildList); node != NULL;  node = next)
	{
		GtkWidget *widget = NULL;
		GtkWidget *menu_item = NULL;
		next = node->next;

		pWidget = (WIDGET_PARAM *)node->data;
		widget = pWidget->widget;
		
		if(FALSE == gtk_widget_get_visible(widget))
		{
			GFile *location = NULL;
			char *local_uri = NULL;
			char *pBaseName = NULL;
			GIcon *icon = NULL;
			PeonyBookmark *bookmark = NULL;

			local_uri = (char *)g_object_get_data (G_OBJECT (widget),"location-addr");
			if (NULL != local_uri)
			{
				location = g_file_new_for_uri (local_uri);
				if(NULL != location)
				{
					pBaseName = g_file_get_basename(location);
					icon = g_file_icon_new (location);
					bookmark = peony_bookmark_new (location, pBaseName, TRUE, icon);//???
					menu_item = peony_bookmark_menu_item_new (bookmark);
					if(menu_item != NULL)
					{							
						g_object_set_data_full (G_OBJECT (menu_item),
												"location-addr",
												g_strdup(local_uri), g_free);
						gtk_widget_show (GTK_WIDGET (menu_item));
						g_signal_connect_object (menu_item, "activate",
											 G_CALLBACK (peony_location_menu_pressed_callback),
											 window, 0);
					
						gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
					}
					g_object_unref (location);
					if(NULL != pBaseName)
					{
						g_free(pBaseName);
					}
					if (icon)
					{
						g_object_unref (icon);
					}
				}
			}
		}
	}
	
	gtk_menu_popup (GTK_MENU (menu),
					NULL, NULL, menu_position_callback, widget,
					1, event->time);
	gtk_widget_show_all(menu);
	
	return;
}

static void
show_location_menu_callback (GtkMenuToolButton *button,
                    				gpointer      user_data)
{
    PeonyNavigationWindow *window;
 	PeonyLocationBar *bar = NULL;
    GList *node = NULL;
	GList *next = NULL;
	WIDGET_PARAM *pWidget = NULL;
    GtkWidget *menu = NULL;
    GList *children = NULL;
    GList *li = NULL;

	if (NULL == button || NULL == user_data)
    {
		return;
	}	

	bar = PEONY_LOCATION_BAR (user_data);
    window = peony_location_bar_get_window (gtk_widget_get_parent (button));
	menu = gtk_menu_tool_button_get_menu(button);

    children = gtk_container_get_children (GTK_CONTAINER (menu));
    for (li = children; li; li = li->next)
    {
        gtk_container_remove (GTK_CONTAINER (menu), li->data);
    }
    g_list_free (children);

    for (node = g_list_first(bar->details->pChildList); node != NULL;  node = next)
    {
		GtkWidget *widget = NULL;
		GtkWidget *menu_item = NULL;
		next = node->next;

		pWidget = (WIDGET_PARAM *)node->data;
		widget = pWidget->widget;
		
		if(FALSE == gtk_widget_get_visible(widget))
		{
			GFile *location = NULL;
			char *local_uri = NULL;
			char *pBaseName = NULL;
			GIcon *icon = NULL;
			PeonyBookmark *bookmark = NULL;

			local_uri = (char *)g_object_get_data (G_OBJECT (widget),"location-addr");
		    if (NULL != local_uri)
		    {
				location = g_file_new_for_uri (local_uri);
				if(NULL != location)
				{
					pBaseName = g_file_get_basename(location);
					icon = g_file_icon_new (location);
					bookmark = peony_bookmark_new (location, pBaseName, TRUE, icon);//???
					menu_item = peony_bookmark_menu_item_new (bookmark);
					if(menu_item != NULL)
					{							
						g_object_set_data_full (G_OBJECT (menu_item),
												"location-addr",
												g_strdup(local_uri), g_free);
						gtk_widget_show (GTK_WIDGET (menu_item));
						g_signal_connect_object (menu_item, "activate",
											 G_CALLBACK (peony_location_menu_pressed_callback),
											 window, 0);
					
						gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
					}
					g_object_unref (location);
					if(NULL != pBaseName)
					{
						g_free(pBaseName);
					}
					if (icon)
					{
						g_object_unref (icon);
					}
				}
		    }
		}
    }

    return;
}

static void set_button_list (GtkWidget *hboxinter,PeonyLocationBar *bar)
{
	WIDGET_PARAM *pstrWidget = NULL;
	GList *pList = NULL;
    GList *node = NULL;
	GList *next = NULL;

	if(NULL == hboxinter || NULL == bar)
	{
		return;
	}

	pList = gtk_container_get_children(hboxinter);
	for (node = g_list_nth(pList,2); node != NULL;  node = next)
    {
		next = node->next;

		pstrWidget = g_new0 (WIDGET_PARAM, 1);		
		pstrWidget->widget = (GtkWidget *)node->data;
		pstrWidget->iWidth = 0;
		bar->details->pChildList = g_list_append(bar->details->pChildList,(gpointer)pstrWidget);
    }
	
	g_list_free(pList);
	return;
}
/*
static void
peony_sec_location_frame_allocate_callback (GtkWidget    *framewidget,
												GdkRectangle *allocation,
               									gpointer      user_data)
{
	GList *pNode = NULL;
 	PeonyLocationBar *bar = NULL;
	GdkRectangle strbarAllocation = {0};
	GdkRectangle strWidgetAllocation = {0};
    GList *node = NULL;
	GList *next = NULL;
	GList *prev = NULL;
	WIDGET_PARAM *pWidget = NULL;
	WIDGET_PARAM *pWidgetPre = NULL;

    if (NULL == framewidget || NULL == allocation || NULL == user_data)
    {
		return;
	}	

    bar = PEONY_LOCATION_BAR (user_data);
	gtk_widget_get_allocation (bar,&strbarAllocation);
	
	if(bar->details->iSecFrameWidthBack != allocation->width)
	{
		bar->details->iSecFrameWidthBack = allocation->width;
	}
	else if(allocation->width > SEC_FRAME_SIZE)
	{
		return;
	}

	if(allocation->width > SEC_FRAME_SIZE)
	{
		bar->details->iSecFrameWidthBack = allocation->width;
		for (node = g_list_last(bar->details->pChildList); node != NULL;  node = prev)
	    {
			GtkWidget *widget = NULL;
			prev = node->prev;

			pWidget = (WIDGET_PARAM *)node->data;
			widget = pWidget->widget;
			if(FALSE == gtk_widget_get_visible(widget)
			{
				//pWidgetPre = (WIDGET_PARAM *)prev->data;
//				if(pWidget->iWidth <= allocation->width - SEC_FRAME_SIZE)
				{
					gtk_widget_show(widget);
					//gtk_widget_show(pWidgetPre->widget);
					allocation->width -= (pWidget->iWidth);
				}
				else
				{
					break;
				}
			}
	    }

		if(NULL == node)
		{
			gtk_widget_hide(bar->details->backbutton);
			gtk_widget_hide(bar->details->backseparator);
		}
	}
	else if(allocation->width < SEC_FRAME_SIZE)
	{
	    for (node = g_list_first(bar->details->pChildList); node != NULL;  node = next)
	    {
			GtkWidget *widget = NULL;
			next = node->next;

			pWidget = (WIDGET_PARAM *)node->data;
			widget = pWidget->widget;
			
			if(TRUE == gtk_widget_get_visible(widget))
			{			
				gtk_widget_get_allocation (widget,&strWidgetAllocation);
				pWidget->iWidth = strWidgetAllocation.width;
				allocation->width += strWidgetAllocation.width;
				gtk_widget_hide(widget);
				if(NULL != next)
				{
					pWidget = (WIDGET_PARAM *)next->data;
					widget = pWidget->widget;
					gtk_widget_get_allocation (widget,&strWidgetAllocation);
					pWidget->iWidth = strWidgetAllocation.width;
					allocation->width += strWidgetAllocation.width;
					gtk_widget_hide(widget);
				}

				if(NULL != bar->details->backbutton && FALSE == gtk_widget_get_visible(bar->details->backbutton))
				{
					gtk_widget_show(bar->details->backbutton);
					gtk_widget_show(bar->details->backseparator);
				}
				if(allocation->width >= SEC_FRAME_SIZE)
				{
					break;
				}
			}
	    }
	}

    return;
} 
*/
int zz=0;
static void
peony_location_frame_allocate_callback (GtkWidget    *widget,
                                                                                                GdkRectangle *allocation,
                                                                                gpointer      user_data)
{
        GtkCssProvider *provider;
        GtkStyleContext *context;
	GtkAllocation strframehboxsize;
	PeonyNavigationWindow *local_window;
        GtkAllocation strbarsize;
        GtkUIManager *ui_manager;
	GdkWindow *window;
	GtkWidget *reload;
	GList *node = NULL;
        WIDGET_PARAM *pWidget = NULL;
        PeonyLocationBar *bar = NULL;

        provider = gtk_css_provider_new ();
        gtk_css_provider_load_from_data (provider, css, -1, NULL);
        gtk_style_context_add_provider_for_screen (gtk_widget_get_screen (widget),GTK_STYLE_PROVIDER (provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

        if (NULL == widget || NULL == allocation || NULL == user_data)
        {
                return;
        }
	
	bar = PEONY_LOCATION_BAR (user_data);	
	if(bar == NULL)return ;
	if(g_list_first(bar->details->pChildList)==NULL)return;
        gtk_widget_get_allocation(bar->details->interbox,&strbarsize);
	gtk_widget_get_allocation(bar,&strbarsize);
	if(strbarsize.width<SEC_FRAME_SIZE)return;
	
	int size = 0;	
	local_window = peony_location_bar_get_window(bar);
	if((local_window->details->spinner!=NULL) && gtk_widget_get_visible(local_window->details->spinner)==TRUE){
		GtkAllocation spinnersize;
		gtk_widget_get_allocation(local_window->details->spinner,&spinnersize);
		strbarsize.width+=spinnersize.width;
	}

        for (node = g_list_last(bar->details->pChildList); node != NULL;node=node->prev )
        {
                if(strbarsize.width-size<SEC_FRAME_SIZE)
                {
                        node = node->next;
                        GtkWidget *widget = NULL;
                        pWidget = (WIDGET_PARAM *)node->data;
                        widget = pWidget->widget;
                        gtk_widget_hide(widget);
			if(node->next  !=NULL)
                        {
                                node = node->next;
                                GtkWidget *widget = NULL;
                                pWidget = (WIDGET_PARAM *)node->data;
                                widget = pWidget->widget;
                                gtk_widget_hide(widget);
			}
                        break;
                }
                if(node == g_list_last(bar->details->pChildList))
                {
                        GtkWidget *widget = NULL;
                        GtkRequisition min_size;
                        pWidget = (WIDGET_PARAM *)node->data;
                        widget = pWidget->widget;
                        gtk_widget_show(widget);
                        gtk_widget_get_preferred_size(widget,&min_size,NULL);
                        size+=min_size.width;
                        continue;
                }
		GtkWidget *widget = NULL;
                GtkRequisition min_size;
                pWidget = (WIDGET_PARAM *)node->data;
                widget = pWidget->widget;
                gtk_widget_show(widget);
                gtk_widget_get_preferred_size(widget,&min_size,NULL);
                size+=min_size.width;
                if(node ->prev !=NULL)
                {
                        node = node->prev;
                        pWidget = (WIDGET_PARAM *)node->data;
                        widget = pWidget->widget;
                        gtk_widget_show(widget);
                        gtk_widget_get_preferred_size(widget,&min_size,NULL);
                        size+=min_size.width;
		}
		if(node == g_list_first(bar->details->pChildList) )
        	{
			if(strbarsize.width-size<SEC_FRAME_SIZE)
			{		
                		GtkWidget *widget = NULL;
                		pWidget = (WIDGET_PARAM *)node->data;
                		widget = pWidget->widget;
                		gtk_widget_hide(widget);
                		if(node->next  !=NULL)
                		{
                        		node = node->next;
                        		GtkWidget *widget = NULL;
                        		pWidget = (WIDGET_PARAM *)node->data;
                        		widget = pWidget->widget;
                        		gtk_widget_hide(widget);
                		}	
				break;	
			}
			else
			{
//				gtk_widget_hide(bar->details->backbutton);
  //              		gtk_widget_hide(bar->details->backseparator);
			}
        	}

        }

    	node = g_list_first(bar->details->pChildList);
    	pWidget = (WIDGET_PARAM *)node->data;
        GtkWidget *widget1;
	widget1 = pWidget->widget;
	if(gtk_widget_get_visible(widget1)==TRUE)
	{
		gtk_widget_hide(bar->details->backbutton);
                gtk_widget_hide(bar->details->backseparator);
                context = gtk_widget_get_style_context(bar->details->folder_image);
                gtk_style_context_remove_class (context,"folder_image_new");
                gtk_style_context_add_class(context,"folder_image");
                gtk_widget_show(bar->details->folder_image);
	}
	else
	{
                gtk_widget_show(bar->details->backbutton);
                context = gtk_widget_get_style_context(bar->details->folder_image);
                gtk_style_context_remove_class (context,"folder_image");
                gtk_style_context_add_class(context,"folder_image_new");
                gtk_widget_show(bar->details->folder_image);
//                gtk_widget_show(bar->details->backseparator);

	}
	gtk_widget_queue_draw(bar);
}
/*
static void
peony_location_frame_allocate_callback (GtkWidget    *widget,
												GdkRectangle *allocation,
               									gpointer      user_data)
{
	GList *pNode = NULL;
 	PeonyLocationBar *bar = NULL;
	GdkRectangle strbarAllocation = {0};
	GdkRectangle strWidgetAllocation = {0};
	GdkRectangle barwidth = {0};
    	GList *node = NULL;
	GList *next = NULL;
	GList *prev = NULL;
	WIDGET_PARAM *pWidget = NULL;
	WIDGET_PARAM *pWidgetPre = NULL;

    if (NULL == widget || NULL == allocation || NULL == user_data)
    {
		return;
	}	
	
    bar = PEONY_LOCATION_BAR (user_data);
	gtk_widget_get_allocation (bar,&strbarAllocation);
	
	if(strbarAllocation.width - allocation->width < SEC_FRAME_SIZE)
	{
		strWidgetAllocation.width = strbarAllocation.width - allocation->width;
		peony_sec_location_frame_allocate_callback(bar->details->aspectframesec,&strWidgetAllocation,bar);;
	}
	
    return;
}
*/
static void destory_location_button (gpointer data,gpointer user_data)
{
	WIDGET_PARAM *pstrWidget = NULL;
	if(NULL == data)
	{
		return;
	}

	pstrWidget = (WIDGET_PARAM *)data;
	gtk_widget_destroy(pstrWidget->widget);
	g_free(pstrWidget);
	
	return;
}

static void
remove_widget (GtkWidget *widget, GtkContainer *container)
{
	if (NULL == widget || NULL == container)
    {
		return;
	}
	
	gtk_widget_hide(widget);
    gtk_container_remove (container, widget);
	gtk_widget_destroy(widget);

	return;
}



void
peony_location_bar_set_location (PeonyLocationBar *bar,
                                const char *location)
{
    char *formatted_location = NULL;
    GFile *file = NULL;
    char* real_location = NULL;
	GtkWidget *LocationButton = NULL;
	gboolean bReturn = FALSE;
	char *pLocationTemp = NULL;
	gint iWidth = 0;
	gint iHeight = 0;
	WIDGET_PARAM *pWidget = NULL;

    if (NULL == location || NULL == bar)
    {
		return;
	}
//	#if 0 //_LOCATION_BAR_CHANGE_
	gtk_widget_hide(bar->details->hbox);
	if(NULL != bar->details->framehbox)
	{
		gtk_widget_hide(bar->details->framehbox);
		gtk_container_foreach (GTK_CONTAINER (bar->details->framehbox),
							   (GtkCallback)remove_widget,GTK_CONTAINER (bar));
		gtk_container_remove (GTK_CONTAINER (bar), bar->details->framehbox);
		gtk_widget_destroy(bar->details->framehbox);
		bar->details->framehbox = NULL;
	}

	if(NULL != bar->details->interbox)
	{
		gtk_container_foreach (GTK_CONTAINER (bar->details->interbox),
						   (GtkCallback)remove_widget,bar);
		gtk_widget_destroy(bar->details->interbox);
		bar->details->interbox = NULL;
	}
	
	if(FALSE == bar->details->bActive)
	{
		GtkWidget *hbox = NULL;
		GtkWidget *aspectframe = NULL;
		GtkWidget *aspectframesec = NULL;
		GtkWidget *hboxinter = NULL;
		GtkWidget *button = NULL;
		GtkWidget *separator = NULL;
		GtkWidget *event_box = NULL;
		char* pStart = NULL;
		char* pTemp = NULL;
		char* pFind = NULL;
		char* pStartBack = NULL;
		char* pStartBackPtr = NULL;
		gboolean bFirst = TRUE;

		if(NULL != bar->details->pChildList)
		{
        	g_list_foreach (bar->details->pChildList, (GFunc)destory_location_button, NULL);
        	g_list_free (bar->details->pChildList);
			bar->details->pChildList = NULL;
		}

//		aspectframe = gtk_frame_new(NULL);
		aspectframe = gtk_grid_new();
//		aspectframesec = gtk_frame_new(NULL);
		aspectframesec = gtk_grid_new();
		hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,FALSE);

		GdkColor color;
		gdk_color_parse ("white", &color);
		gtk_widget_modify_bg ( GTK_WIDGET(hbox), GTK_STATE_NORMAL, &color);

		hboxinter = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,FALSE);

		GtkCssProvider *provider;
		GtkStyleContext *context;
 		provider = gtk_css_provider_new ();
		gtk_css_provider_load_from_data (provider, css, -1, NULL);
		gtk_style_context_add_provider_for_screen (gtk_widget_get_screen (hboxinter),GTK_STYLE_PROVIDER (provider),GTK_STYLE_PROVIDER_PRIORITY_USER);

		event_box = gtk_event_box_new ();
	    gtk_event_box_set_visible_window (GTK_EVENT_BOX (event_box), FALSE);

		file = g_file_new_for_uri (location);
		if(NULL != file)
		{
			formatted_location = g_file_get_parse_name (file);
			if(NULL != formatted_location)
			{
				real_location = g_uri_unescape_string (formatted_location,"");
				if(NULL != real_location)
				{
					pStart = real_location;
				}
				else
				{
					pStart = formatted_location;
				}
				{
					GtkWidget *menu = NULL;
					GtkWidget *backbutton = NULL;
					GtkWidget *iconView_image = NULL;
					GtkWidget *child = NULL;
					GtkWidget *image = NULL;

					GtkWidget *folder_image = NULL;
					backbutton = gtk_button_new();
					gtk_button_set_relief(backbutton,GTK_RELIEF_NONE);
					image = gtk_image_new_from_icon_name("go-down",GTK_ICON_SIZE_MENU);
					//gtk_button_set_relief(image,GTK_RELIEF_NONE);
					gtk_button_set_image (GTK_BUTTON (backbutton),image);
					context = gtk_widget_get_style_context(backbutton);
					gtk_style_context_add_class(context,"go_down_image");
					
					PeonyFile *peony_file = peony_file_get_existing (file);
					PeonyIconInfo *icon_info = peony_file_get_icon (peony_file, PEONY_ICON_SIZE_SMALLEST, PEONY_FILE_ICON_FLAGS_NONE);
					const char* icon_name = peony_icon_info_get_used_name (icon_info);
					peony_file_unref (peony_file);
					g_object_unref (icon_info);
					folder_image = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_MENU);
					gtk_widget_set_size_request (GTK_WIDGET (folder_image), -1, 42);
					
					//folder_image = gtk_image_new_from_icon_name("folder",GTK_ICON_SIZE_MENU);
					context = gtk_widget_get_style_context(folder_image);
					gtk_style_context_remove_class (context,"folder_image_new");
					gtk_style_context_add_class(context,"folder_image");

					menu = gtk_menu_new ();
					bar->details->menu = menu;
					
					//gtk_menu_set_screen (GTK_MENU (menu), gtk_widget_get_screen (backbutton));
					g_signal_connect (backbutton, "button-press-event",
									  G_CALLBACK (peony_location_backbutton_pressed_callback), bar);

					gtk_box_pack_start (GTK_BOX (hboxinter), backbutton, FALSE, TRUE, 0);
					separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
					gtk_box_pack_start (GTK_BOX (hboxinter), folder_image, FALSE, TRUE, 0);
					gtk_box_pack_start (GTK_BOX (hboxinter), separator, FALSE, TRUE, 0);

					bar->details->backbutton = backbutton;
					bar->details->backseparator = separator;
					bar->details->folder_image = folder_image;
				}
				if (!strcmp(pStart,"/")){
					char *pFind_new = NULL;
					pFind_new=_("File System");
					button = gtk_button_new_with_label(pFind_new);
					context = gtk_widget_get_style_context(button);
					gtk_style_context_add_class(context,"location_button_list");
					gtk_button_set_relief(button,GTK_RELIEF_NONE);
					gtk_box_pack_start (GTK_BOX (hboxinter), button, FALSE, TRUE, 0);
				}
				else {
					pStartBack = g_strdup(pStart);
					pStartBackPtr = pStartBack;
					while(NULL != (pFind = strtok_r(pStart,"/",&pTemp)))
					{
						char *pUrl = NULL;
						char *uri = NULL;
						char *pFind_new = NULL;
						GFile *locationFile = NULL;

						if(FALSE == bFirst)
						{
							separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
//							gtk_box_pack_start (GTK_BOX (hboxinter), separator, FALSE, TRUE, 0);
						}
						if(!strcmp(pFind,"recent:")){
							pFind_new=_("recent");
						}
						if(!strcmp(pFind,"trash:")){
							pFind_new=_("trash");
						}
						if(!strcmp(pFind,"computer:")){
							pFind_new=_("computer");
						}
						if(!strcmp(pFind,"x-peony-search:")){
							pFind_new=_("search:");
						}
						if (pFind_new != NULL){
							button = gtk_button_new_with_label(pFind_new);
						} else{
							button = gtk_button_new_with_label(pFind);
						}
						context = gtk_widget_get_style_context(button);
						gtk_style_context_add_class(context,"location_button_list");
						gtk_button_set_relief(button,GTK_RELIEF_NONE);

						pLocationTemp = strstr(pStartBackPtr,pFind);
						if(NULL == pLocationTemp)
						{
							pLocationTemp = pStartBackPtr;
						}
						pLocationTemp += strlen(pFind);
						pUrl = g_strndup(pStartBack,pLocationTemp-pStartBack);
						locationFile = g_file_parse_name (pUrl);
						uri = g_file_get_uri (locationFile);
						g_object_unref (locationFile);
						g_free(pUrl);
						pStartBackPtr = pLocationTemp;
					
						if (!strcmp(uri,"x-peony-search:///")){
							gtk_widget_set_sensitive (button,FALSE);
						}
						g_object_set_data_full (G_OBJECT (button),
									"location-addr",
									uri, g_free);
						g_signal_connect (button, "button-press-event",
								  G_CALLBACK (peony_location_button_pressed_callback), NULL);
						gtk_box_pack_start (GTK_BOX (hboxinter), button, FALSE, TRUE, 0);

						bFirst = FALSE;
						pStart = NULL;
					}
					g_free (formatted_location);
					g_free (real_location);
					g_free (pStartBack);
				}
			}
			g_object_unref (file);
		}

		g_signal_connect (aspectframe, "size-allocate",
						  G_CALLBACK (peony_location_frame_allocate_callback), bar);
		g_signal_connect (aspectframesec, "size-allocate",
						  G_CALLBACK (peony_location_frame_allocate_callback), bar);

		gtk_container_add (GTK_CONTAINER (aspectframe), hboxinter);
		gtk_container_add   (GTK_CONTAINER (event_box), aspectframesec);
		gtk_box_pack_start (GTK_BOX (hbox), aspectframe, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (hbox), event_box, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_CONTAINER (bar), hbox, TRUE, TRUE, 0);
		bar->details->framehbox = hbox;
		bar->details->aspectframesec = aspectframesec;
		bar->details->aspectframe = aspectframe;

		context = gtk_widget_get_style_context(aspectframesec);
		gtk_style_context_add_class(context,"event_box3");

		g_signal_connect (event_box, "button_press_event",
						  G_CALLBACK (peony_location_bar_focus_in),
						  bar);
		
		gtk_widget_show (bar->details->framehbox);
		gtk_widget_hide(bar->details->backbutton);
		gtk_widget_hide(bar->details->backseparator);
		bar->details->bNeedDes = TRUE;
		bar->details->interbox = hboxinter;
		

		gtk_widget_show(aspectframe);
		gtk_widget_show(hboxinter);
		gtk_widget_show_all(event_box);
		set_button_list(hboxinter,bar);
		if (bar->details->last_location != location)
		{
			g_free (bar->details->last_location);
			bar->details->last_location = g_strdup (location);
		}
	}
	else
	{
	gtk_widget_show_all (bar->details->hbox);
	    if (eel_uri_is_search (location))
	    {
	        peony_location_entry_set_special_text (PEONY_LOCATION_ENTRY (bar->details->entry),
	                                              "");
	    }
	    else
	    {
	        file = g_file_new_for_uri (location);
			if(NULL != file)
			{
		        formatted_location = g_file_get_parse_name (file);
				if(NULL != formatted_location)
				{
			        real_location = g_uri_unescape_string (formatted_location,"");
					if(NULL != real_location)
					{
			        	peony_location_entry_update_current_location (PEONY_LOCATION_ENTRY (bar->details->entry),
			                real_location);
					}
					else
					{
						peony_location_entry_update_current_location (PEONY_LOCATION_ENTRY (bar->details->entry),
			                formatted_location);
					}
			        g_free (formatted_location);
			        g_free (real_location);
				}
				g_object_unref (file);
			}
	    }

	    /* remember the original location for later comparison */
	    if (bar->details->last_location != location)
	    {
	        g_free (bar->details->last_location);
	        bar->details->last_location = g_strdup (location);
	    }

	    peony_location_bar_update_label (bar);
		if(TRUE == bar->details->bANeedemit)
		{
			gtk_widget_grab_focus(bar->details->entry);			
			bar->details->bANeedemit = FALSE;
		}
	}

	return;
/*	
//	#else
	if (eel_uri_is_search (location))
    {
        peony_location_entry_set_special_text (PEONY_LOCATION_ENTRY (bar->details->entry),
                                              "");
    }
    else
    {
        file = g_file_new_for_uri (location);
        formatted_location = g_file_get_parse_name (file);
        g_object_unref (file);
        peony_location_entry_update_current_location (PEONY_LOCATION_ENTRY (bar->details->entry),
                formatted_location);
        g_free (formatted_location);
    }

    if (bar->details->last_location != location)
    {
        g_free (bar->details->last_location);
        bar->details->last_location = g_strdup (location);
    }

    peony_location_bar_update_label (bar);
//	#endif
*/
}

/* change background color based on activity state */
void
peony_location_bar_set_active(PeonyLocationBar *location_bar, gboolean is_active)
{
    if (is_active)
    {
        /* reset style to default */
        gtk_widget_override_background_color (GTK_WIDGET (location_bar->details->entry), GTK_STATE_FLAG_NORMAL, NULL);
    }
    else
    {
        GtkStyleContext *style;
        GdkRGBA color;

        style = gtk_widget_get_style_context (GTK_WIDGET (location_bar->details->entry));
        gtk_style_context_get_background_color (style, GTK_STATE_FLAG_INSENSITIVE, &color);
        gtk_widget_override_background_color (GTK_WIDGET (location_bar->details->entry), GTK_STATE_FLAG_ACTIVE, &color);
    }
}

PeonyEntry *
peony_location_bar_get_entry (PeonyLocationBar *location_bar)
{
    return location_bar->details->entry;
}
