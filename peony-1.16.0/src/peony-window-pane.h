/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-window-pane.h: Peony window pane

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

#ifndef PEONY_WINDOW_PANE_H
#define PEONY_WINDOW_PANE_H

#include "peony-window.h"

#define PEONY_TYPE_WINDOW_PANE	 (peony_window_pane_get_type())
#define PEONY_WINDOW_PANE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_WINDOW_PANE, PeonyWindowPaneClass))
#define PEONY_WINDOW_PANE(obj)	 (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_WINDOW_PANE, PeonyWindowPane))
#define PEONY_IS_WINDOW_PANE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_WINDOW_PANE))
#define PEONY_IS_WINDOW_PANE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_WINDOW_PANE))
#define PEONY_WINDOW_PANE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_WINDOW_PANE, PeonyWindowPaneClass))

typedef struct _PeonyWindowPaneClass PeonyWindowPaneClass;

struct _PeonyWindowPaneClass
{
    GObjectClass parent_class;

    void (*show) (PeonyWindowPane *pane);
    void (*set_active) (PeonyWindowPane *pane,
                        gboolean is_active);
    void (*sync_search_widgets) (PeonyWindowPane *pane);
    void (*sync_location_widgets) (PeonyWindowPane *pane);
};

/* A PeonyWindowPane is a layer between a slot and a window.
 * Each slot is contained in one pane, and each pane can contain
 * one or more slots. It also supports the notion of an "active slot".
 * On the other hand, each pane is contained in a window, while each
 * window can contain one or multiple panes. Likewise, the window has
 * the notion of an "active pane".
 *
 * A spatial window has only one pane, which contains a single slot.
 * A navigation window may have one or more panes.
 */
struct _PeonyWindowPane
{
    GObject parent;

    /* hosting window */
    PeonyWindow *window;
    gboolean visible;

    /* available slots, and active slot.
     * Both of them may never be NULL. */
    GList *slots;
    GList *active_slots;
    PeonyWindowSlot *active_slot;

    /* whether or not this pane is active */
    gboolean is_active;
};

GType peony_window_pane_get_type (void);
PeonyWindowPane *peony_window_pane_new (PeonyWindow *window);


void peony_window_pane_show (PeonyWindowPane *pane);
void peony_window_pane_zoom_in (PeonyWindowPane *pane);
void peony_window_pane_zoom_to_level (PeonyWindowPane *pane, PeonyZoomLevel level);
void peony_window_pane_zoom_out (PeonyWindowPane *pane);
void peony_window_pane_zoom_to_default (PeonyWindowPane *pane);
void peony_window_pane_sync_location_widgets (PeonyWindowPane *pane);
void peony_window_pane_sync_search_widgets  (PeonyWindowPane *pane);
void peony_window_pane_set_active (PeonyWindowPane *pane, gboolean is_active);
void peony_window_pane_slot_close (PeonyWindowPane *pane, PeonyWindowSlot *slot);

PeonyWindowSlot* peony_window_pane_get_slot_for_content_box (PeonyWindowPane *pane, GtkWidget *content_box);
void peony_window_pane_switch_to (PeonyWindowPane *pane);
void peony_window_pane_grab_focus (PeonyWindowPane *pane);


#endif /* PEONY_WINDOW_PANE_H */
