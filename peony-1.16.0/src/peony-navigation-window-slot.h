/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-navigation-window-slot.h: Peony navigation window slot

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

#ifndef PEONY_NAVIGATION_WINDOW_SLOT_H
#define PEONY_NAVIGATION_WINDOW_SLOT_H

#include "peony-window-slot.h"

typedef struct PeonyNavigationWindowSlot PeonyNavigationWindowSlot;
typedef struct PeonyNavigationWindowSlotClass PeonyNavigationWindowSlotClass;


#define PEONY_TYPE_NAVIGATION_WINDOW_SLOT         (peony_navigation_window_slot_get_type())
#define PEONY_NAVIGATION_WINDOW_SLOT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_NAVIGATION_WINDOW_SLOT_CLASS, PeonyNavigationWindowSlotClass))
#define PEONY_NAVIGATION_WINDOW_SLOT(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_NAVIGATION_WINDOW_SLOT, PeonyNavigationWindowSlot))
#define PEONY_IS_NAVIGATION_WINDOW_SLOT(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_NAVIGATION_WINDOW_SLOT))
#define PEONY_IS_NAVIGATION_WINDOW_SLOT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_NAVIGATION_WINDOW_SLOT))
#define PEONY_NAVIGATION_WINDOW_SLOT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_NAVIGATION_WINDOW_SLOT, PeonyNavigationWindowSlotClass))

typedef enum
{
    PEONY_BAR_PATH,
    PEONY_BAR_NAVIGATION,
    PEONY_BAR_SEARCH
} PeonyBarMode;

struct PeonyNavigationWindowSlot
{
    PeonyWindowSlot parent;

    PeonyBarMode bar_mode;
    GtkTreeModel *viewer_model;
    int num_viewers;

    /* Back/Forward chain, and history list.
     * The data in these lists are PeonyBookmark pointers.
     */
    GList *back_list, *forward_list;

    /* Current views stuff */
    GList *sidebar_panels;
};

struct PeonyNavigationWindowSlotClass
{
    PeonyWindowSlotClass parent;
};

GType peony_navigation_window_slot_get_type (void);

gboolean peony_navigation_window_slot_should_close_with_mount (PeonyNavigationWindowSlot *slot,
        GMount *mount);

void peony_navigation_window_slot_clear_forward_list (PeonyNavigationWindowSlot *slot);
void peony_navigation_window_slot_clear_back_list    (PeonyNavigationWindowSlot *slot);

#endif /* PEONY_NAVIGATION_WINDOW_SLOT_H */
