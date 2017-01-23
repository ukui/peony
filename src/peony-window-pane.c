/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-window-pane.c: Peony window pane

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

   Author: Holger Berndt <berndth@gmx.de>
*/

#include "peony-window-pane.h"
#include "peony-window-private.h"
#include "peony-navigation-window-pane.h"
#include "peony-window-manage-views.h"
#include <eel/eel-gtk-macros.h>

static void peony_window_pane_init       (PeonyWindowPane *pane);
static void peony_window_pane_class_init (PeonyWindowPaneClass *class);
static void peony_window_pane_dispose    (GObject *object);

G_DEFINE_TYPE (PeonyWindowPane,
               peony_window_pane,
               G_TYPE_OBJECT)
#define parent_class peony_window_pane_parent_class


static inline PeonyWindowSlot *
get_first_inactive_slot (PeonyWindowPane *pane)
{
    GList *l;
    PeonyWindowSlot *slot;

    for (l = pane->slots; l != NULL; l = l->next)
    {
        slot = PEONY_WINDOW_SLOT (l->data);
        if (slot != pane->active_slot)
        {
            return slot;
        }
    }

    return NULL;
}

void
peony_window_pane_show (PeonyWindowPane *pane)
{
    pane->visible = TRUE;
    EEL_CALL_METHOD (PEONY_WINDOW_PANE_CLASS, pane,
                     show, (pane));
}

void
peony_window_pane_zoom_in (PeonyWindowPane *pane)
{
    PeonyWindowSlot *slot;

    g_assert (pane != NULL);

    peony_window_set_active_pane (pane->window, pane);

    slot = pane->active_slot;
    if (slot->content_view != NULL)
    {
        peony_view_bump_zoom_level (slot->content_view, 1);
    }
}

void
peony_window_pane_zoom_to_level (PeonyWindowPane *pane,
                                PeonyZoomLevel level)
{
    PeonyWindowSlot *slot;

    g_assert (pane != NULL);

    peony_window_set_active_pane (pane->window, pane);

    slot = pane->active_slot;
    if (slot->content_view != NULL)
    {
        peony_view_zoom_to_level (slot->content_view, level);
    }
}

void
peony_window_pane_zoom_out (PeonyWindowPane *pane)
{
    PeonyWindowSlot *slot;

    g_assert (pane != NULL);

    peony_window_set_active_pane (pane->window, pane);

    slot = pane->active_slot;
    if (slot->content_view != NULL)
    {
        peony_view_bump_zoom_level (slot->content_view, -1);
    }
}

void
peony_window_pane_zoom_to_default (PeonyWindowPane *pane)
{
    PeonyWindowSlot *slot;

    g_assert (pane != NULL);

    peony_window_set_active_pane (pane->window, pane);

    slot = pane->active_slot;
    if (slot->content_view != NULL)
    {
        peony_view_restore_default_zoom_level (slot->content_view);
    }
}

void
peony_window_pane_slot_close (PeonyWindowPane *pane, PeonyWindowSlot *slot)
{
    PeonyWindowSlot *next_slot;

    if (pane->window)
    {
        PeonyWindow *window;
        window = pane->window;
        if (pane->active_slot == slot)
        {
            g_assert (pane->active_slots != NULL);
            g_assert (pane->active_slots->data == slot);

            next_slot = NULL;
            if (pane->active_slots->next != NULL)
            {
                next_slot = PEONY_WINDOW_SLOT (pane->active_slots->next->data);
            }

            if (next_slot == NULL)
            {
                next_slot = get_first_inactive_slot (PEONY_WINDOW_PANE (pane));
            }

            peony_window_set_active_slot (window, next_slot);
        }
        peony_window_close_slot (slot);

        /* If that was the last slot in the active pane, close the pane or even the whole window. */
        if (window->details->active_pane->slots == NULL)
        {
            PeonyWindowPane *next_pane;
            next_pane = peony_window_get_next_pane (window);

            /* If next_pane is non-NULL, we have more than one pane available. In this
             * case, close the current pane and switch to the next one. If there is
             * no next pane, close the window. */
            if(next_pane)
            {
                peony_window_close_pane (pane);
                peony_window_pane_switch_to (next_pane);
                if (PEONY_IS_NAVIGATION_WINDOW (window))
                {
                    peony_navigation_window_update_show_hide_menu_items (PEONY_NAVIGATION_WINDOW (window));
                }
            }
            else
            {
                peony_window_close (window);
            }
        }
    }
}

static void
real_sync_location_widgets (PeonyWindowPane *pane)
{
    PeonyWindowSlot *slot;

    /* TODO: Would be nice with a real subclass for spatial panes */
    g_assert (PEONY_IS_SPATIAL_WINDOW (pane->window));

    slot = pane->active_slot;

    /* Change the location button to match the current location. */
    peony_spatial_window_set_location_button (PEONY_SPATIAL_WINDOW (pane->window),
            slot->location);
}


void
peony_window_pane_sync_location_widgets (PeonyWindowPane *pane)
{
    EEL_CALL_METHOD (PEONY_WINDOW_PANE_CLASS, pane,
                     sync_location_widgets, (pane));
}

void
peony_window_pane_sync_search_widgets (PeonyWindowPane *pane)
{
    g_assert (PEONY_IS_WINDOW_PANE (pane));

    EEL_CALL_METHOD (PEONY_WINDOW_PANE_CLASS, pane,
                     sync_search_widgets, (pane));
}

void
peony_window_pane_grab_focus (PeonyWindowPane *pane)
{
    if (PEONY_IS_WINDOW_PANE (pane) && pane->active_slot)
    {
        peony_view_grab_focus (pane->active_slot->content_view);
    }
}

void
peony_window_pane_switch_to (PeonyWindowPane *pane)
{
    peony_window_pane_grab_focus (pane);
}

static void
peony_window_pane_init (PeonyWindowPane *pane)
{
    pane->slots = NULL;
    pane->active_slots = NULL;
    pane->active_slot = NULL;
    pane->is_active = FALSE;
}

void
peony_window_pane_set_active (PeonyWindowPane *pane, gboolean is_active)
{
    if (is_active == pane->is_active)
    {
        return;
    }

    pane->is_active = is_active;

    /* notify the current slot about its activity state (so that it can e.g. modify the bg color) */
    peony_window_slot_is_in_active_pane (pane->active_slot, is_active);

    EEL_CALL_METHOD (PEONY_WINDOW_PANE_CLASS, pane,
                     set_active, (pane, is_active));
}

static void
peony_window_pane_class_init (PeonyWindowPaneClass *class)
{
    G_OBJECT_CLASS (class)->dispose = peony_window_pane_dispose;
    PEONY_WINDOW_PANE_CLASS (class)->sync_location_widgets = real_sync_location_widgets;
}

static void
peony_window_pane_dispose (GObject *object)
{
    PeonyWindowPane *pane = PEONY_WINDOW_PANE (object);

    g_assert (pane->slots == NULL);

    pane->window = NULL;
    G_OBJECT_CLASS (parent_class)->dispose (object);
}

PeonyWindowPane *
peony_window_pane_new (PeonyWindow *window)
{
    PeonyWindowPane *pane;

    pane = g_object_new (PEONY_TYPE_WINDOW_PANE, NULL);
    pane->window = window;
    return pane;
}

PeonyWindowSlot *
peony_window_pane_get_slot_for_content_box (PeonyWindowPane *pane,
        GtkWidget *content_box)
{
    PeonyWindowSlot *slot;
    GList *l;

    for (l = pane->slots; l != NULL; l = l->next)
    {
        slot = PEONY_WINDOW_SLOT (l->data);

        if (slot->content_box == content_box)
        {
            return slot;
        }
    }
    return NULL;
}
