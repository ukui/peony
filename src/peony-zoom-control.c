/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
 * Copyright (C) 2004 Red Hat, Inc.
 *
 * Peony is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 *         Alexander Larsson <alexl@redhat.com>
 *
 * This is the zoom control for the location bar
 *
 */

#include <config.h>
#include "peony-zoom-control.h"

#include <atk/atkaction.h>
#include <glib/gi18n.h>
#include <eel/eel-accessibility.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-graphic-effects.h>
#include <eel/eel-gtk-extensions.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libpeony-private/peony-file-utilities.h>
#include <libpeony-private/peony-global-preferences.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if !GTK_CHECK_VERSION(3,0,0)
#define gtk_widget_get_preferred_size(x,y,z) gtk_widget_size_request(x,y)
#endif

enum
{
    ZOOM_IN,
    ZOOM_OUT,
    ZOOM_TO_LEVEL,
    ZOOM_TO_DEFAULT,
    CHANGE_VALUE,
    LAST_SIGNAL
};

struct PeonyZoomControlDetails
{
    GtkWidget *zoom_in;
    GtkWidget *zoom_out;
    GtkWidget *zoom_label;
    GtkWidget *zoom_button;

    PeonyZoomLevel zoom_level;
    PeonyZoomLevel min_zoom_level;
    PeonyZoomLevel max_zoom_level;
    gboolean has_min_zoom_level;
    gboolean has_max_zoom_level;
    GList *preferred_zoom_levels;

    gboolean marking_menu_items;
};


static guint signals[LAST_SIGNAL] = { 0 };

static gpointer accessible_parent_class;

static const char * const peony_zoom_control_accessible_action_names[] =
{
    N_("Zoom In"),
    N_("Zoom Out"),
    N_("Zoom to Default"),
};

static const int peony_zoom_control_accessible_action_signals[] =
{
    ZOOM_IN,
    ZOOM_OUT,
    ZOOM_TO_DEFAULT,
};

static const char * const peony_zoom_control_accessible_action_descriptions[] =
{
    N_("Increase the view size"),
    N_("Decrease the view size"),
    N_("Use the normal view size")
};

static GtkMenu *create_zoom_menu (PeonyZoomControl *zoom_control);

static GType peony_zoom_control_accessible_get_type (void);

/* button assignments */
#define CONTEXTUAL_MENU_BUTTON 3

#define NUM_ACTIONS ((int)G_N_ELEMENTS (peony_zoom_control_accessible_action_names))

#if GTK_CHECK_VERSION (3, 0, 0)
G_DEFINE_TYPE (PeonyZoomControl, peony_zoom_control, GTK_TYPE_BOX);
#else
G_DEFINE_TYPE (PeonyZoomControl, peony_zoom_control, GTK_TYPE_HBOX);
#endif

static void
peony_zoom_control_finalize (GObject *object)
{
    g_list_free (PEONY_ZOOM_CONTROL (object)->details->preferred_zoom_levels);

    G_OBJECT_CLASS (peony_zoom_control_parent_class)->finalize (object);
}

static void
zoom_button_clicked (GtkButton *button, PeonyZoomControl *zoom_control)
{
    g_signal_emit (zoom_control, signals[ZOOM_TO_DEFAULT], 0);
}

static void
zoom_popup_menu_show (GdkEventButton *event, PeonyZoomControl *zoom_control)
{
    eel_pop_up_context_menu (create_zoom_menu (zoom_control),
                             EEL_DEFAULT_POPUP_MENU_DISPLACEMENT,
                             EEL_DEFAULT_POPUP_MENU_DISPLACEMENT,
                             event);
}

static void
menu_position_under_widget (GtkMenu   *menu,
                            gint      *x,
                            gint      *y,
                            gboolean  *push_in,
                            gpointer   user_data)
{
    GtkWidget *widget;
    GtkWidget *container;
    GtkRequisition req;
    GtkRequisition menu_req;
    GdkRectangle monitor;
    int monitor_num;
    GdkScreen *screen;
    GtkAllocation allocation;

    widget = GTK_WIDGET (user_data);
    g_assert (GTK_IS_WIDGET (widget));

    container = gtk_widget_get_ancestor (widget, GTK_TYPE_CONTAINER);
    g_assert (container != NULL);

    gtk_widget_get_preferred_size (GTK_WIDGET (menu), &menu_req, NULL);
    gtk_widget_get_preferred_size (widget, &req, NULL);
    gtk_widget_get_allocation (widget, &allocation);

    screen = gtk_widget_get_screen (GTK_WIDGET (menu));
    monitor_num = gdk_screen_get_monitor_at_window (screen, gtk_widget_get_window (widget));
    if (monitor_num < 0)
    {
        monitor_num = 0;
    }
    gdk_screen_get_monitor_geometry (screen, monitor_num, &monitor);

    gdk_window_get_origin (gtk_widget_get_window (widget), x, y);
    if (!gtk_widget_get_has_window (widget))
    {
        *x += allocation.x;
        *y += allocation.y;
    }

    if (gtk_widget_get_direction (container) == GTK_TEXT_DIR_LTR)
    {
        *x += allocation.width - req.width;
    }
    else
    {
        *x += req.width - menu_req.width;
    }

    if ((*y + allocation.height + menu_req.height) <= monitor.y + monitor.height)
    {
        *y += allocation.height;
    }
    else if ((*y - menu_req.height) >= monitor.y)
    {
        *y -= menu_req.height;
    }
    else if (monitor.y + monitor.height - (*y + allocation.height) > *y)
    {
        *y += allocation.height;
    }
    else
    {
        *y -= menu_req.height;
    }

    *push_in = FALSE;
}


static void
zoom_popup_menu (GtkWidget *widget, PeonyZoomControl *zoom_control)
{
    GtkMenu *menu;

    menu = create_zoom_menu (zoom_control);
    gtk_menu_popup (menu, NULL, NULL,
                    menu_position_under_widget, widget,
                    0, gtk_get_current_event_time ());
    gtk_menu_shell_select_first (GTK_MENU_SHELL (menu), FALSE);
}

/* handle button presses */
static gboolean
peony_zoom_control_button_press_event (GtkWidget *widget,
                                      GdkEventButton *event,
                                      PeonyZoomControl *zoom_control)
{
    if (event->type != GDK_BUTTON_PRESS)
    {
        return FALSE;
    }

    /* check for the context menu button and show the menu */
    if (event->button == CONTEXTUAL_MENU_BUTTON)
    {
        zoom_popup_menu_show (event, zoom_control);
        return TRUE;
    }
    /* We don't change our state (to reflect the new zoom) here.
       The zoomable will call back with the new level.
       Actually, the callback goes to the viewframe containing the
       zoomable which, in turn, emits zoom_level_changed,
       which someone (e.g. peony_window) picks up and handles by
       calling into is - peony_zoom_control_set_zoom_level.
    */

    return FALSE;
}

static void
zoom_out_clicked (GtkButton *button,
                  PeonyZoomControl *zoom_control)
{
    if (peony_zoom_control_can_zoom_out (zoom_control))
    {
        g_signal_emit (G_OBJECT (zoom_control), signals[ZOOM_OUT], 0);
    }
}

static void
zoom_in_clicked (GtkButton *button,
                 PeonyZoomControl *zoom_control)
{
    if (peony_zoom_control_can_zoom_in (zoom_control))
    {
        g_signal_emit (G_OBJECT (zoom_control), signals[ZOOM_IN], 0);
    }
}

static void
set_label_size (PeonyZoomControl *zoom_control)
{
    const char *text;
    PangoLayout *layout;
    int width;
    int height;

    text = gtk_label_get_text (GTK_LABEL (zoom_control->details->zoom_label));
    layout = gtk_label_get_layout (GTK_LABEL (zoom_control->details->zoom_label));
    pango_layout_set_text (layout, "100%", -1);
    pango_layout_get_pixel_size (layout, &width, &height);
    gtk_widget_set_size_request (zoom_control->details->zoom_label, width, height);
    gtk_label_set_text (GTK_LABEL (zoom_control->details->zoom_label),
                        text);
}

static void
label_style_set_callback (GtkWidget *label,
#if GTK_CHECK_VERSION (3, 0, 0)
                          GtkStyleContext *style,
#else
                          GtkStyle *style,
#endif
                          gpointer user_data)
{
    set_label_size (PEONY_ZOOM_CONTROL (user_data));
}

static void
peony_zoom_control_init (PeonyZoomControl *zoom_control)
{
    GtkWidget *image;
    int i;

    zoom_control->details = G_TYPE_INSTANCE_GET_PRIVATE (zoom_control, PEONY_TYPE_ZOOM_CONTROL, PeonyZoomControlDetails);

    zoom_control->details->zoom_level = PEONY_ZOOM_LEVEL_STANDARD;
    zoom_control->details->min_zoom_level = PEONY_ZOOM_LEVEL_SMALLEST;
    zoom_control->details->max_zoom_level = PEONY_ZOOM_LEVEL_LARGEST;
    zoom_control->details->has_min_zoom_level = TRUE;
    zoom_control->details->has_max_zoom_level = TRUE;

    for (i = PEONY_ZOOM_LEVEL_LARGEST; i >= PEONY_ZOOM_LEVEL_SMALLEST; i--)
    {
        zoom_control->details->preferred_zoom_levels = g_list_prepend (
                    zoom_control->details->preferred_zoom_levels,
                    GINT_TO_POINTER (i));
    }

    image = gtk_image_new_from_icon_name ("zoom-out", GTK_ICON_SIZE_MENU);
    zoom_control->details->zoom_out = gtk_button_new ();
#if GTK_CHECK_VERSION(3,20,0)
    gtk_widget_set_focus_on_click (zoom_control->details->zoom_out, FALSE);
#else
    gtk_button_set_focus_on_click (GTK_BUTTON (zoom_control->details->zoom_out), FALSE);
#endif
    gtk_button_set_relief (GTK_BUTTON (zoom_control->details->zoom_out),
                           GTK_RELIEF_NONE);
    gtk_widget_set_tooltip_text (zoom_control->details->zoom_out,
                                 _("Decrease the view size"));
    g_signal_connect (G_OBJECT (zoom_control->details->zoom_out),
                      "clicked", G_CALLBACK (zoom_out_clicked),
                      zoom_control);
#if GTK_CHECK_VERSION (3, 0, 0)
    gtk_orientable_set_orientation (GTK_ORIENTABLE (zoom_control), GTK_ORIENTATION_HORIZONTAL);
#endif
    gtk_container_add (GTK_CONTAINER (zoom_control->details->zoom_out), image);
    gtk_box_pack_start (GTK_BOX (zoom_control),
                        zoom_control->details->zoom_out, FALSE, FALSE, 0);

    zoom_control->details->zoom_button = gtk_button_new ();
#if GTK_CHECK_VERSION(3,20,0)
    gtk_widget_set_focus_on_click (zoom_control->details->zoom_button, FALSE);
#else
    gtk_button_set_focus_on_click (GTK_BUTTON (zoom_control->details->zoom_button), FALSE);
#endif
    gtk_button_set_relief (GTK_BUTTON (zoom_control->details->zoom_button),
                           GTK_RELIEF_NONE);
    gtk_widget_set_tooltip_text (zoom_control->details->zoom_button,
                                 _("Use the normal view size"));

    gtk_widget_add_events (GTK_WIDGET (zoom_control->details->zoom_button),
                           GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_POINTER_MOTION_MASK);

    g_signal_connect (G_OBJECT (zoom_control->details->zoom_button),
                      "button-press-event",
                      G_CALLBACK (peony_zoom_control_button_press_event),
                      zoom_control);

    g_signal_connect (G_OBJECT (zoom_control->details->zoom_button),
                      "clicked", G_CALLBACK (zoom_button_clicked),
                      zoom_control);

    g_signal_connect (G_OBJECT (zoom_control->details->zoom_button),
                      "popup-menu", G_CALLBACK (zoom_popup_menu),
                      zoom_control);

    zoom_control->details->zoom_label = gtk_label_new ("100%");
    g_signal_connect (zoom_control->details->zoom_label,
                      "style_set",
                      G_CALLBACK (label_style_set_callback),
                      zoom_control);
    set_label_size (zoom_control);

    gtk_container_add (GTK_CONTAINER (zoom_control->details->zoom_button), zoom_control->details->zoom_label);

    gtk_box_pack_start (GTK_BOX (zoom_control),
                        zoom_control->details->zoom_button, TRUE, TRUE, 0);

    image = gtk_image_new_from_icon_name ("zoom-in", GTK_ICON_SIZE_MENU);
    zoom_control->details->zoom_in = gtk_button_new ();
#if GTK_CHECK_VERSION(3,20,0)
    gtk_widget_set_focus_on_click (zoom_control->details->zoom_in, FALSE);
#else
    gtk_button_set_focus_on_click (GTK_BUTTON (zoom_control->details->zoom_in), FALSE);
#endif
    gtk_button_set_relief (GTK_BUTTON (zoom_control->details->zoom_in),
                           GTK_RELIEF_NONE);
    gtk_widget_set_tooltip_text (zoom_control->details->zoom_in,
                                 _("Increase the view size"));
    g_signal_connect (G_OBJECT (zoom_control->details->zoom_in),
                      "clicked", G_CALLBACK (zoom_in_clicked),
                      zoom_control);

    gtk_container_add (GTK_CONTAINER (zoom_control->details->zoom_in), image);
    gtk_box_pack_start (GTK_BOX (zoom_control),
                        zoom_control->details->zoom_in, FALSE, FALSE, 0);

    gtk_widget_show_all (zoom_control->details->zoom_out);
    gtk_widget_show_all (zoom_control->details->zoom_button);
    gtk_widget_show_all (zoom_control->details->zoom_in);
}

/* Allocate a new zoom control */
GtkWidget *
peony_zoom_control_new (void)
{
    return gtk_widget_new (peony_zoom_control_get_type (), NULL);
}

static void
peony_zoom_control_redraw (PeonyZoomControl *zoom_control)
{
    int percent;
    char *num_str;

    gtk_widget_set_sensitive (zoom_control->details->zoom_in,
                              peony_zoom_control_can_zoom_in (zoom_control));
    gtk_widget_set_sensitive (zoom_control->details->zoom_out,
                              peony_zoom_control_can_zoom_out (zoom_control));

    percent = floor ((100.0 * peony_get_relative_icon_size_for_zoom_level (zoom_control->details->zoom_level)) + .2);
    num_str = g_strdup_printf ("%d%%", percent);
    gtk_label_set_text (GTK_LABEL (zoom_control->details->zoom_label), num_str);
    g_free (num_str);
}

/* routines to create and handle the zoom menu */

static void
zoom_menu_callback (GtkMenuItem *item, gpointer callback_data)
{
    PeonyZoomLevel zoom_level;
    PeonyZoomControl *zoom_control;
    gboolean can_zoom;

    zoom_control = PEONY_ZOOM_CONTROL (callback_data);

    /* Don't do anything if we're just setting the toggle state of menu items. */
    if (zoom_control->details->marking_menu_items)
    {
        return;
    }

    /* Don't send the signal if the menuitem was toggled off */
    if (!gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (item)))
    {
        return;
    }

    zoom_level = (PeonyZoomLevel) GPOINTER_TO_INT (g_object_get_data (G_OBJECT (item), "zoom_level"));

    /* Assume we can zoom and then check whether we're right. */
    can_zoom = TRUE;
    if (zoom_control->details->has_min_zoom_level &&
            zoom_level < zoom_control->details->min_zoom_level)
        can_zoom = FALSE; /* no, we're below the minimum zoom level. */
    if (zoom_control->details->has_max_zoom_level &&
            zoom_level > zoom_control->details->max_zoom_level)
        can_zoom = FALSE; /* no, we're beyond the upper zoom level. */

    /* if we can zoom */
    if (can_zoom)
    {
        g_signal_emit (zoom_control, signals[ZOOM_TO_LEVEL], 0, zoom_level);
    }
}

static GtkRadioMenuItem *
create_zoom_menu_item (PeonyZoomControl *zoom_control, GtkMenu *menu,
                       PeonyZoomLevel zoom_level,
                       GtkRadioMenuItem *previous_radio_item)
{
    GtkWidget *menu_item;
    char *item_text;
    GSList *radio_item_group;
    int percent;

    /* Set flag so that callback isn't activated when set_active called
     * to set toggle state of other radio items.
     */
    zoom_control->details->marking_menu_items = TRUE;

    percent = floor ((100.0 * peony_get_relative_icon_size_for_zoom_level (zoom_level)) + .5);
    item_text = g_strdup_printf ("%d%%", percent);

    radio_item_group = previous_radio_item == NULL
                       ? NULL
                       : gtk_radio_menu_item_get_group (previous_radio_item);
    menu_item = gtk_radio_menu_item_new_with_label (radio_item_group, item_text);
    g_free (item_text);

    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menu_item),
                                    zoom_level == zoom_control->details->zoom_level);

    g_object_set_data (G_OBJECT (menu_item), "zoom_level", GINT_TO_POINTER (zoom_level));
    g_signal_connect_object (menu_item, "activate",
                             G_CALLBACK (zoom_menu_callback), zoom_control, 0);

    gtk_widget_show (menu_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);

    zoom_control->details->marking_menu_items = FALSE;

    return GTK_RADIO_MENU_ITEM (menu_item);
}

static GtkMenu *
create_zoom_menu (PeonyZoomControl *zoom_control)
{
    GtkMenu *menu;
    GtkRadioMenuItem *previous_item;
    GList *node;

    menu = GTK_MENU (gtk_menu_new ());

    previous_item = NULL;
    for (node = zoom_control->details->preferred_zoom_levels; node != NULL; node = node->next)
    {
        previous_item = create_zoom_menu_item
                        (zoom_control, menu, GPOINTER_TO_INT (node->data), previous_item);
    }

    return menu;
}

static AtkObject *
peony_zoom_control_get_accessible (GtkWidget *widget)
{
    AtkObject *accessible;

    accessible = eel_accessibility_get_atk_object (widget);

    if (accessible)
    {
        return accessible;
    }

    accessible = g_object_new
                 (peony_zoom_control_accessible_get_type (), NULL);

    return eel_accessibility_set_atk_object_return (widget, accessible);
}

static void
peony_zoom_control_change_value (PeonyZoomControl *zoom_control,
                                GtkScrollType scroll)
{
    switch (scroll)
    {
    case GTK_SCROLL_STEP_DOWN :
        if (peony_zoom_control_can_zoom_out (zoom_control))
        {
            g_signal_emit (zoom_control, signals[ZOOM_OUT], 0);
        }
        break;
    case GTK_SCROLL_STEP_UP :
        if (peony_zoom_control_can_zoom_in (zoom_control))
        {
            g_signal_emit (zoom_control, signals[ZOOM_IN], 0);
        }
        break;
    default :
        g_warning ("Invalid scroll type %d for PeonyZoomControl:change_value", scroll);
    }
}

void
peony_zoom_control_set_zoom_level (PeonyZoomControl *zoom_control,
                                  PeonyZoomLevel zoom_level)
{
    zoom_control->details->zoom_level = zoom_level;
    peony_zoom_control_redraw (zoom_control);
}

void
peony_zoom_control_set_parameters (PeonyZoomControl *zoom_control,
                                  PeonyZoomLevel min_zoom_level,
                                  PeonyZoomLevel max_zoom_level,
                                  gboolean has_min_zoom_level,
                                  gboolean has_max_zoom_level,
                                  GList *zoom_levels)
{
    g_return_if_fail (PEONY_IS_ZOOM_CONTROL (zoom_control));

    zoom_control->details->min_zoom_level = min_zoom_level;
    zoom_control->details->max_zoom_level = max_zoom_level;
    zoom_control->details->has_min_zoom_level = has_min_zoom_level;
    zoom_control->details->has_max_zoom_level = has_max_zoom_level;

    g_list_free (zoom_control->details->preferred_zoom_levels);
    zoom_control->details->preferred_zoom_levels = zoom_levels;

    peony_zoom_control_redraw (zoom_control);
}

PeonyZoomLevel
peony_zoom_control_get_zoom_level (PeonyZoomControl *zoom_control)
{
    return zoom_control->details->zoom_level;
}

PeonyZoomLevel
peony_zoom_control_get_min_zoom_level (PeonyZoomControl *zoom_control)
{
    return zoom_control->details->min_zoom_level;
}

PeonyZoomLevel
peony_zoom_control_get_max_zoom_level (PeonyZoomControl *zoom_control)
{
    return zoom_control->details->max_zoom_level;
}

gboolean
peony_zoom_control_has_min_zoom_level (PeonyZoomControl *zoom_control)
{
    return zoom_control->details->has_min_zoom_level;
}

gboolean
peony_zoom_control_has_max_zoom_level (PeonyZoomControl *zoom_control)
{
    return zoom_control->details->has_max_zoom_level;
}

gboolean
peony_zoom_control_can_zoom_in (PeonyZoomControl *zoom_control)
{
    return !zoom_control->details->has_max_zoom_level ||
           (zoom_control->details->zoom_level
            < zoom_control->details->max_zoom_level);
}

gboolean
peony_zoom_control_can_zoom_out (PeonyZoomControl *zoom_control)
{
    return !zoom_control->details->has_min_zoom_level ||
           (zoom_control->details->zoom_level
            > zoom_control->details->min_zoom_level);
}

static gboolean
peony_zoom_control_scroll_event (GtkWidget *widget, GdkEventScroll *event)
{
    PeonyZoomControl *zoom_control;

    zoom_control = PEONY_ZOOM_CONTROL (widget);

    if (event->type != GDK_SCROLL)
    {
        return FALSE;
    }

    if (event->direction == GDK_SCROLL_DOWN &&
            peony_zoom_control_can_zoom_out (zoom_control))
    {
        g_signal_emit (widget, signals[ZOOM_OUT], 0);
    }
    else if (event->direction == GDK_SCROLL_UP &&
             peony_zoom_control_can_zoom_in (zoom_control))
    {
        g_signal_emit (widget, signals[ZOOM_IN], 0);
    }

    /* We don't change our state (to reflect the new zoom) here. The zoomable will
     * call back with the new level. Actually, the callback goes to the view-frame
     * containing the zoomable which, in turn, emits zoom_level_changed, which
     * someone (e.g. peony_window) picks up and handles by calling into us -
     * peony_zoom_control_set_zoom_level.
     */
    return TRUE;
}



static void
peony_zoom_control_class_init (PeonyZoomControlClass *class)
{
    GtkWidgetClass *widget_class;
    GtkBindingSet *binding_set;

    G_OBJECT_CLASS (class)->finalize = peony_zoom_control_finalize;

    widget_class = GTK_WIDGET_CLASS (class);

    widget_class->get_accessible = peony_zoom_control_get_accessible;
    widget_class->scroll_event = peony_zoom_control_scroll_event;

    class->change_value = peony_zoom_control_change_value;

    signals[ZOOM_IN] =
        g_signal_new ("zoom_in",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (PeonyZoomControlClass,
                                       zoom_in),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[ZOOM_OUT] =
        g_signal_new ("zoom_out",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (PeonyZoomControlClass,
                                       zoom_out),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[ZOOM_TO_LEVEL] =
        g_signal_new ("zoom_to_level",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (PeonyZoomControlClass,
                                       zoom_to_level),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__INT,
                      G_TYPE_NONE,
                      1,
                      G_TYPE_INT);

    signals[ZOOM_TO_DEFAULT] =
        g_signal_new ("zoom_to_default",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (PeonyZoomControlClass,
                                       zoom_to_default),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[CHANGE_VALUE] =
        g_signal_new ("change_value",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (PeonyZoomControlClass,
                                       change_value),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__ENUM,
                      G_TYPE_NONE, 1, GTK_TYPE_SCROLL_TYPE);

    binding_set = gtk_binding_set_by_class (class);

    gtk_binding_entry_add_signal (binding_set,
				      GDK_KEY_KP_Subtract, 0, 
                                  "change_value",
                                  1, GTK_TYPE_SCROLL_TYPE,
                                  GTK_SCROLL_STEP_DOWN);
    gtk_binding_entry_add_signal (binding_set,
				      GDK_KEY_minus, 0,
                                  "change_value",
                                  1, GTK_TYPE_SCROLL_TYPE,
                                  GTK_SCROLL_STEP_DOWN);

    gtk_binding_entry_add_signal (binding_set,
				      GDK_KEY_KP_Equal, 0, 
                                  "zoom_to_default",
                                  0);
    gtk_binding_entry_add_signal (binding_set,
				      GDK_KEY_KP_Equal, 0, 
                                  "zoom_to_default",
                                  0);

    gtk_binding_entry_add_signal (binding_set,
				      GDK_KEY_KP_Add, 0, 
                                  "change_value",
                                  1, GTK_TYPE_SCROLL_TYPE,
                                  GTK_SCROLL_STEP_UP);
    gtk_binding_entry_add_signal (binding_set,
				      GDK_KEY_plus, 0, 
                                  "change_value",
                                  1, GTK_TYPE_SCROLL_TYPE,
                                  GTK_SCROLL_STEP_UP);

    g_type_class_add_private (G_OBJECT_CLASS (class), sizeof (PeonyZoomControlDetails));
}

static gboolean
peony_zoom_control_accessible_do_action (AtkAction *accessible, int i)
{
    GtkWidget *widget;

    g_assert (i >= 0 && i < NUM_ACTIONS);

    widget = gtk_accessible_get_widget (GTK_ACCESSIBLE (accessible));
    if (!widget)
    {
        return FALSE;
    }

    g_signal_emit (widget,
                   signals[peony_zoom_control_accessible_action_signals [i]],
                   0);

    return TRUE;
}

static int
peony_zoom_control_accessible_get_n_actions (AtkAction *accessible)
{

    return NUM_ACTIONS;
}

static const char* peony_zoom_control_accessible_action_get_description(AtkAction* accessible, int i)
{
    g_assert(i >= 0 && i < NUM_ACTIONS);

    return _(peony_zoom_control_accessible_action_descriptions[i]);
}

static const char* peony_zoom_control_accessible_action_get_name(AtkAction* accessible, int i)
{
    g_assert (i >= 0 && i < NUM_ACTIONS);

    return _(peony_zoom_control_accessible_action_names[i]);
}

static void peony_zoom_control_accessible_action_interface_init(AtkActionIface* iface)
{
    iface->do_action = peony_zoom_control_accessible_do_action;
    iface->get_n_actions = peony_zoom_control_accessible_get_n_actions;
    iface->get_description = peony_zoom_control_accessible_action_get_description;
    iface->get_name = peony_zoom_control_accessible_action_get_name;
}

static void
peony_zoom_control_accessible_get_current_value (AtkValue *accessible,
        GValue *value)
{
    PeonyZoomControl *control;

    g_value_init (value, G_TYPE_INT);

    control = PEONY_ZOOM_CONTROL (gtk_accessible_get_widget (GTK_ACCESSIBLE (accessible)));
    if (!control)
    {
        g_value_set_int (value, PEONY_ZOOM_LEVEL_STANDARD);
        return;
    }

    g_value_set_int (value, control->details->zoom_level);
}

static void
peony_zoom_control_accessible_get_maximum_value (AtkValue *accessible,
        GValue *value)
{
    PeonyZoomControl *control;

    g_value_init (value, G_TYPE_INT);

    control = PEONY_ZOOM_CONTROL (gtk_accessible_get_widget (GTK_ACCESSIBLE (accessible)));
    if (!control)
    {
        g_value_set_int (value, PEONY_ZOOM_LEVEL_STANDARD);
        return;
    }

    g_value_set_int (value, control->details->max_zoom_level);
}

static void
peony_zoom_control_accessible_get_minimum_value (AtkValue *accessible,
        GValue *value)
{
    PeonyZoomControl *control;

    g_value_init (value, G_TYPE_INT);

    control = PEONY_ZOOM_CONTROL (gtk_accessible_get_widget (GTK_ACCESSIBLE (accessible)));
    if (!control)
    {
        g_value_set_int (value, PEONY_ZOOM_LEVEL_STANDARD);
        return;
    }

    g_value_set_int (value, control->details->min_zoom_level);
}

static PeonyZoomLevel
nearest_preferred (PeonyZoomControl *zoom_control, PeonyZoomLevel value)
{
    PeonyZoomLevel last_value;
    PeonyZoomLevel current_value;
    GList *l;

    if (!zoom_control->details->preferred_zoom_levels)
    {
        return value;
    }

    last_value = GPOINTER_TO_INT (zoom_control->details->preferred_zoom_levels->data);
    current_value = last_value;

    for (l = zoom_control->details->preferred_zoom_levels; l != NULL; l = l->next)
    {
        current_value = GPOINTER_TO_INT (l->data);

        if (current_value > value)
        {
            float center = (last_value + current_value) / 2;

            return (value < center) ? last_value : current_value;

        }

        last_value = current_value;
    }

    return current_value;
}

static gboolean
peony_zoom_control_accessible_set_current_value (AtkValue *accessible,
        const GValue *value)
{
    PeonyZoomControl *control;
    PeonyZoomLevel zoom;

    control = PEONY_ZOOM_CONTROL (gtk_accessible_get_widget (GTK_ACCESSIBLE (accessible)));
    if (!control)
    {
        return FALSE;
    }

    zoom = nearest_preferred (control, g_value_get_int (value));

    g_signal_emit (control, signals[ZOOM_TO_LEVEL], 0, zoom);

    return TRUE;
}

static void
peony_zoom_control_accessible_value_interface_init (AtkValueIface *iface)
{
    iface->get_current_value = peony_zoom_control_accessible_get_current_value;
    iface->get_maximum_value = peony_zoom_control_accessible_get_maximum_value;
    iface->get_minimum_value = peony_zoom_control_accessible_get_minimum_value;
    iface->set_current_value = peony_zoom_control_accessible_set_current_value;
}

static const char* peony_zoom_control_accessible_get_name(AtkObject* accessible)
{
    return _("Zoom");
}

static const char* peony_zoom_control_accessible_get_description(AtkObject* accessible)
{
    return _("Set the zoom level of the current view");
}

static void
peony_zoom_control_accessible_initialize (AtkObject *accessible,
        gpointer  data)
{
    if (ATK_OBJECT_CLASS (accessible_parent_class)->initialize != NULL)
    {
        ATK_OBJECT_CLASS (accessible_parent_class)->initialize (accessible, data);
    }
    atk_object_set_role (accessible, ATK_ROLE_DIAL);
}

static void
peony_zoom_control_accessible_class_init (AtkObjectClass *klass)
{
    accessible_parent_class = g_type_class_peek_parent (klass);

    klass->get_name = peony_zoom_control_accessible_get_name;
    klass->get_description = peony_zoom_control_accessible_get_description;
    klass->initialize = peony_zoom_control_accessible_initialize;
}

static GType
peony_zoom_control_accessible_get_type (void)
{
    static GType type = 0;

    if (!type)
    {
        static GInterfaceInfo atk_action_info =
        {
            (GInterfaceInitFunc)peony_zoom_control_accessible_action_interface_init,
            (GInterfaceFinalizeFunc)NULL,
            NULL
        };

        static GInterfaceInfo atk_value_info =
        {
            (GInterfaceInitFunc)peony_zoom_control_accessible_value_interface_init,
            (GInterfaceFinalizeFunc)NULL,
            NULL
        };

        type = eel_accessibility_create_derived_type
               ("PeonyZoomControlAccessible",
#if GTK_CHECK_VERSION (3, 0, 0)
                GTK_TYPE_BOX,
#else
                GTK_TYPE_HBOX,
#endif
                peony_zoom_control_accessible_class_init);

        g_type_add_interface_static (type, ATK_TYPE_ACTION,
                                     &atk_action_info);
        g_type_add_interface_static (type, ATK_TYPE_VALUE,
                                     &atk_value_info);
    }

    return type;
}

void
peony_zoom_control_set_active_appearance (PeonyZoomControl *zoom_control, gboolean is_active)
{
    gtk_widget_set_sensitive (gtk_bin_get_child (GTK_BIN (zoom_control->details->zoom_in)), is_active);
    gtk_widget_set_sensitive (gtk_bin_get_child (GTK_BIN (zoom_control->details->zoom_out)), is_active);
    gtk_widget_set_sensitive (zoom_control->details->zoom_label, is_active);
}
