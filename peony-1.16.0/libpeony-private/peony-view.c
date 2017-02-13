/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-view.c: Interface for peony views

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
#include "peony-view.h"

enum
{
    TITLE_CHANGED,
    ZOOM_LEVEL_CHANGED,
    LAST_SIGNAL
};

static guint peony_view_signals[LAST_SIGNAL] = { 0 };

static void
peony_view_base_init (gpointer g_class)
{
    static gboolean initialized = FALSE;

    if (! initialized)
    {
        peony_view_signals[TITLE_CHANGED] =
            g_signal_new ("title_changed",
                          PEONY_TYPE_VIEW,
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (PeonyViewIface, title_changed),
                          NULL, NULL,
                          g_cclosure_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);

        peony_view_signals[ZOOM_LEVEL_CHANGED] =
            g_signal_new ("zoom_level_changed",
                          PEONY_TYPE_VIEW,
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (PeonyViewIface, zoom_level_changed),
                          NULL, NULL,
                          g_cclosure_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);

        initialized = TRUE;
    }
}

GType
peony_view_get_type (void)
{
    static GType type = 0;

    if (!type)
    {
        const GTypeInfo info =
        {
            sizeof (PeonyViewIface),
            peony_view_base_init,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE,
                                       "PeonyView",
                                       &info, 0);
        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

const char *
peony_view_get_view_id (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), NULL);

    return (* PEONY_VIEW_GET_IFACE (view)->get_view_id) (view);
}

GtkWidget *
peony_view_get_widget (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), NULL);

    return (* PEONY_VIEW_GET_IFACE (view)->get_widget) (view);
}

void
peony_view_load_location (PeonyView *view,
                         const char   *location_uri)
{
    g_return_if_fail (PEONY_IS_VIEW (view));
    g_return_if_fail (location_uri != NULL);

    (* PEONY_VIEW_GET_IFACE (view)->load_location) (view,
            location_uri);
}

void
peony_view_stop_loading (PeonyView *view)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->stop_loading) (view);
}

int
peony_view_get_selection_count (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), 0);

    return (* PEONY_VIEW_GET_IFACE (view)->get_selection_count) (view);
}

GList *
peony_view_get_selection (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), NULL);

    return (* PEONY_VIEW_GET_IFACE (view)->get_selection) (view);
}

void
peony_view_set_selection (PeonyView *view,
                         GList        *list)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->set_selection) (view,
            list);
}

void
peony_view_set_is_active (PeonyView *view,
                         gboolean is_active)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->set_is_active) (view,
            is_active);
}

void
peony_view_invert_selection (PeonyView *view)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->invert_selection) (view);
}

char *
peony_view_get_first_visible_file (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), NULL);

    return (* PEONY_VIEW_GET_IFACE (view)->get_first_visible_file) (view);
}

void
peony_view_scroll_to_file (PeonyView *view,
                          const char   *uri)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->scroll_to_file) (view, uri);
}

char *
peony_view_get_title (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), NULL);

    if (PEONY_VIEW_GET_IFACE (view)->get_title != NULL)
    {
        return (* PEONY_VIEW_GET_IFACE (view)->get_title) (view);
    }
    else
    {
        return NULL;
    }
}


gboolean
peony_view_supports_zooming (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), FALSE);

    return (* PEONY_VIEW_GET_IFACE (view)->supports_zooming) (view);
}

void
peony_view_bump_zoom_level (PeonyView *view,
                           int zoom_increment)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->bump_zoom_level) (view,
            zoom_increment);
}

void
peony_view_zoom_to_level (PeonyView      *view,
                         PeonyZoomLevel  level)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->zoom_to_level) (view,
            level);
}

void
peony_view_restore_default_zoom_level (PeonyView *view)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    (* PEONY_VIEW_GET_IFACE (view)->restore_default_zoom_level) (view);
}

gboolean
peony_view_can_zoom_in (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), FALSE);

    return (* PEONY_VIEW_GET_IFACE (view)->can_zoom_in) (view);
}

gboolean
peony_view_can_zoom_out (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), FALSE);

    return (* PEONY_VIEW_GET_IFACE (view)->can_zoom_out) (view);
}

PeonyZoomLevel
peony_view_get_zoom_level (PeonyView *view)
{
    g_return_val_if_fail (PEONY_IS_VIEW (view), PEONY_ZOOM_LEVEL_STANDARD);

    return (* PEONY_VIEW_GET_IFACE (view)->get_zoom_level) (view);
}

void
peony_view_grab_focus (PeonyView   *view)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    if (PEONY_VIEW_GET_IFACE (view)->grab_focus != NULL)
    {
        (* PEONY_VIEW_GET_IFACE (view)->grab_focus) (view);
    }
}

void
peony_view_update_menus (PeonyView *view)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    if (PEONY_VIEW_GET_IFACE (view)->update_menus != NULL)
    {
        (* PEONY_VIEW_GET_IFACE (view)->update_menus) (view);
    }
}

void
peony_view_pop_up_location_context_menu (PeonyView   *view,
                                        GdkEventButton *event,
                                        const char     *location)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    if (PEONY_VIEW_GET_IFACE (view)->pop_up_location_context_menu != NULL)
    {
        (* PEONY_VIEW_GET_IFACE (view)->pop_up_location_context_menu) (view, event, location);
    }
}

void
peony_view_drop_proxy_received_uris   (PeonyView         *view,
                                      GList                *uris,
                                      const char           *target_location,
                                      GdkDragAction         action)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    if (PEONY_VIEW_GET_IFACE (view)->drop_proxy_received_uris != NULL)
    {
        (* PEONY_VIEW_GET_IFACE (view)->drop_proxy_received_uris) (view, uris, target_location, action);
    }
}

void
peony_view_drop_proxy_received_netscape_url (PeonyView         *view,
        const char           *source_url,
        const char           *target_location,
        GdkDragAction         action)
{
    g_return_if_fail (PEONY_IS_VIEW (view));

    if (PEONY_VIEW_GET_IFACE (view)->drop_proxy_received_netscape_url != NULL)
    {
        (* PEONY_VIEW_GET_IFACE (view)->drop_proxy_received_netscape_url) (view, source_url, target_location, action);
    }
}


