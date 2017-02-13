/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-navigation-window-pane.h: Peony navigation window pane

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

#ifndef PEONY_NAVIGATION_WINDOW_PANE_H
#define PEONY_NAVIGATION_WINDOW_PANE_H

#include "peony-window-pane.h"
#include "peony-navigation-window-slot.h"

#define PEONY_TYPE_NAVIGATION_WINDOW_PANE     (peony_navigation_window_pane_get_type())
#define PEONY_NAVIGATION_WINDOW_PANE_CLASS(k) (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_NAVIGATION_WINDOW_PANE, PeonyNavigationWindowPaneClass))
#define PEONY_NAVIGATION_WINDOW_PANE(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_NAVIGATION_WINDOW_PANE, PeonyNavigationWindowPane))
#define PEONY_IS_NAVIGATION_WINDOW_PANE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_NAVIGATION_WINDOW_PANE))
#define PEONY_IS_NAVIGATION_WINDOW_PANE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_NAVIGATION_WINDOW_PANE))
#define PEONY_NAVIGATION_WINDOW_PANE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_NAVIGATION_WINDOW_PANE, PeonyNavigationWindowPaneClass))

typedef struct _PeonyNavigationWindowPaneClass PeonyNavigationWindowPaneClass;
typedef struct _PeonyNavigationWindowPane      PeonyNavigationWindowPane;

struct _PeonyNavigationWindowPaneClass
{
    PeonyWindowPaneClass parent_class;
};

struct _PeonyNavigationWindowPane
{
    PeonyWindowPane parent;

    GtkWidget *widget;

    /* location bar */
    GtkWidget *location_bar;
    GtkWidget *location_button;
    GtkWidget *navigation_bar;
    GtkWidget *path_bar;
    GtkWidget *search_bar;

    gboolean temporary_navigation_bar;
    gboolean temporary_location_bar;
    gboolean temporary_search_bar;

    /* notebook */
    GtkWidget *notebook;

    /* split view */
    GtkWidget *split_view_hpane;
};

GType    peony_navigation_window_pane_get_type (void);

PeonyNavigationWindowPane* peony_navigation_window_pane_new (PeonyWindow *window);

/* location bar */
void     peony_navigation_window_pane_setup             (PeonyNavigationWindowPane *pane);

void     peony_navigation_window_pane_hide_location_bar (PeonyNavigationWindowPane *pane, gboolean save_preference);
void     peony_navigation_window_pane_show_location_bar (PeonyNavigationWindowPane *pane, gboolean save_preference);
gboolean peony_navigation_window_pane_location_bar_showing (PeonyNavigationWindowPane *pane);
void     peony_navigation_window_pane_hide_path_bar (PeonyNavigationWindowPane *pane);
void     peony_navigation_window_pane_show_path_bar (PeonyNavigationWindowPane *pane);
gboolean peony_navigation_window_pane_path_bar_showing (PeonyNavigationWindowPane *pane);
gboolean peony_navigation_window_pane_search_bar_showing (PeonyNavigationWindowPane *pane);
void     peony_navigation_window_pane_set_bar_mode  (PeonyNavigationWindowPane *pane, PeonyBarMode mode);
void     peony_navigation_window_pane_show_location_bar_temporarily (PeonyNavigationWindowPane *pane);
void     peony_navigation_window_pane_show_navigation_bar_temporarily (PeonyNavigationWindowPane *pane);
void     peony_navigation_window_pane_always_use_location_entry (PeonyNavigationWindowPane *pane, gboolean use_entry);
gboolean peony_navigation_window_pane_hide_temporary_bars (PeonyNavigationWindowPane *pane);
/* notebook */
void     peony_navigation_window_pane_add_slot_in_tab (PeonyNavigationWindowPane *pane, PeonyWindowSlot *slot, PeonyWindowOpenSlotFlags flags);
void     peony_navigation_window_pane_remove_page (PeonyNavigationWindowPane *pane, int page_num);

#endif /* PEONY_NAVIGATION_WINDOW_PANE_H */
