/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 2009 Red Hat, Inc.
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
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *
 */

#include <config.h>

#include "peony-zoom-action.h"
#include "peony-zoom-control.h"
#include "peony-navigation-window.h"
#include "peony-window-private.h"
#include "peony-navigation-window-slot.h"
#include <gtk/gtk.h>
#include <eel/eel-gtk-extensions.h>

G_DEFINE_TYPE (PeonyZoomAction, peony_zoom_action, GTK_TYPE_ACTION)

static void peony_zoom_action_init       (PeonyZoomAction *action);
static void peony_zoom_action_class_init (PeonyZoomActionClass *class);

static GObjectClass *parent_class = NULL;

#define PEONY_ZOOM_ACTION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), PEONY_TYPE_ZOOM_ACTION, PeonyZoomActionPrivate))

struct PeonyZoomActionPrivate
{
    PeonyNavigationWindow *window;
};

enum
{
    PROP_0,
    PROP_WINDOW
};

static void
zoom_changed_callback (PeonyWindow *window,
                       PeonyZoomLevel zoom_level,
                       gboolean supports_zooming,
                       gboolean can_zoom,
                       gboolean can_zoom_in,
                       gboolean can_zoom_out,
                       GtkWidget *zoom_control)
{
    if (supports_zooming)
    {
        gtk_widget_set_sensitive (zoom_control, can_zoom);
        gtk_widget_show (zoom_control);
        if (can_zoom)
        {
            peony_zoom_control_set_zoom_level (PEONY_ZOOM_CONTROL (zoom_control),
                                              zoom_level);
        }
    }
    else
    {
        gtk_widget_hide (zoom_control);
    }
}

static void
connect_proxy (GtkAction *action,
               GtkWidget *proxy)
{
    if (GTK_IS_TOOL_ITEM (proxy))
    {
        GtkToolItem *item = GTK_TOOL_ITEM (proxy);
        PeonyZoomAction *zaction = PEONY_ZOOM_ACTION (action);
        PeonyNavigationWindow *window = zaction->priv->window;
        GtkWidget *zoom_control;

        zoom_control = peony_zoom_control_new ();
        gtk_container_add (GTK_CONTAINER (item),  zoom_control);
        gtk_widget_show (zoom_control);

        g_signal_connect_object (zoom_control, "zoom_in",
                                 G_CALLBACK (peony_window_zoom_in),
                                 window, G_CONNECT_SWAPPED);
        g_signal_connect_object (zoom_control, "zoom_out",
                                 G_CALLBACK (peony_window_zoom_out),
                                 window, G_CONNECT_SWAPPED);
        g_signal_connect_object (zoom_control, "zoom_to_level",
                                 G_CALLBACK (peony_window_zoom_to_level),
                                 window, G_CONNECT_SWAPPED);
        g_signal_connect_object (zoom_control, "zoom_to_default",
                                 G_CALLBACK (peony_window_zoom_to_default),
                                 window, G_CONNECT_SWAPPED);

        g_signal_connect (window, "zoom-changed",
                          G_CALLBACK (zoom_changed_callback),
                          zoom_control);
    }

    (* GTK_ACTION_CLASS (parent_class)->connect_proxy) (action, proxy);
}

static void
disconnect_proxy (GtkAction *action,
                  GtkWidget *proxy)
{
    if (GTK_IS_TOOL_ITEM (proxy))
    {
        GtkToolItem *item = GTK_TOOL_ITEM (proxy);
        PeonyZoomAction *zaction = PEONY_ZOOM_ACTION (action);
        PeonyNavigationWindow *window = zaction->priv->window;
        GtkWidget *child;

        child = gtk_bin_get_child (GTK_BIN (item));

        g_signal_handlers_disconnect_by_func (window, G_CALLBACK (zoom_changed_callback), child);

    }

    (* GTK_ACTION_CLASS (parent_class)->disconnect_proxy) (action, proxy);
}

static void
peony_zoom_action_finalize (GObject *object)
{
    (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
peony_zoom_action_set_property (GObject *object,
                               guint prop_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
    PeonyZoomAction *zoom;

    zoom = PEONY_ZOOM_ACTION (object);

    switch (prop_id)
    {
    case PROP_WINDOW:
        zoom->priv->window = PEONY_NAVIGATION_WINDOW (g_value_get_object (value));
        break;
    }
}

static void
peony_zoom_action_get_property (GObject *object,
                               guint prop_id,
                               GValue *value,
                               GParamSpec *pspec)
{
    PeonyZoomAction *zoom;

    zoom = PEONY_ZOOM_ACTION (object);

    switch (prop_id)
    {
    case PROP_WINDOW:
        g_value_set_object (value, zoom->priv->window);
        break;
    }
}

static void
peony_zoom_action_class_init (PeonyZoomActionClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);
    GtkActionClass *action_class = GTK_ACTION_CLASS (class);

    object_class->finalize = peony_zoom_action_finalize;
    object_class->set_property = peony_zoom_action_set_property;
    object_class->get_property = peony_zoom_action_get_property;

    parent_class = g_type_class_peek_parent (class);

    action_class->toolbar_item_type = GTK_TYPE_TOOL_ITEM;
    action_class->connect_proxy = connect_proxy;
    action_class->disconnect_proxy = disconnect_proxy;

    g_object_class_install_property (object_class,
                                     PROP_WINDOW,
                                     g_param_spec_object ("window",
                                             "Window",
                                             "The navigation window",
                                             G_TYPE_OBJECT,
                                             G_PARAM_READWRITE));

    g_type_class_add_private (object_class, sizeof(PeonyZoomActionPrivate));
}

static void
peony_zoom_action_init (PeonyZoomAction *action)
{
    action->priv = PEONY_ZOOM_ACTION_GET_PRIVATE (action);
}
