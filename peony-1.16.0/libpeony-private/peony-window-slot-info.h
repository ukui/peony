/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-window-slot-info.h: Interface for peony window slots

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

#ifndef PEONY_WINDOW_SLOT_INFO_H
#define PEONY_WINDOW_SLOT_INFO_H

#include "peony-window-info.h"
#include "peony-view.h"


#define PEONY_TYPE_WINDOW_SLOT_INFO           (peony_window_slot_info_get_type ())
#define PEONY_WINDOW_SLOT_INFO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_WINDOW_SLOT_INFO, PeonyWindowSlotInfo))
#define PEONY_IS_WINDOW_SLOT_INFO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_WINDOW_SLOT_INFO))
#define PEONY_WINDOW_SLOT_INFO_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), PEONY_TYPE_WINDOW_SLOT_INFO, PeonyWindowSlotInfoIface))

typedef struct _PeonyWindowSlotInfoIface PeonyWindowSlotInfoIface;

struct _PeonyWindowSlotInfoIface
{
    GTypeInterface g_iface;

    /* signals */

    /* emitted right after this slot becomes active.
     * Views should connect to this signal and merge their UI
     * into the main window.
     */
    void  (* active)  (PeonyWindowSlotInfo *slot);
    /* emitted right before this slot becomes inactive.
     * Views should connect to this signal and unmerge their UI
     * from the main window.
     */
    void  (* inactive) (PeonyWindowSlotInfo *slot);

    /* returns the window info associated with this slot */
    PeonyWindowInfo * (* get_window) (PeonyWindowSlotInfo *slot);

    /* Returns the number of selected items in the view */
    int  (* get_selection_count)  (PeonyWindowSlotInfo    *slot);

    /* Returns a list of uris for th selected items in the view, caller frees it */
    GList *(* get_selection)      (PeonyWindowSlotInfo    *slot);

    char * (* get_current_location)  (PeonyWindowSlotInfo *slot);
    PeonyView * (* get_current_view) (PeonyWindowSlotInfo *slot);
    void   (* set_status)            (PeonyWindowSlotInfo *slot,
                                      const char *status);
    char * (* get_title)             (PeonyWindowSlotInfo *slot);

    void   (* open_location)      (PeonyWindowSlotInfo *slot,
                                   GFile *location,
                                   PeonyWindowOpenMode mode,
                                   PeonyWindowOpenFlags flags,
                                   GList *selection,
                                   PeonyWindowGoToCallback callback,
                                   gpointer user_data);
    void   (* make_hosting_pane_active) (PeonyWindowSlotInfo *slot);
};


GType                             peony_window_slot_info_get_type            (void);
PeonyWindowInfo *              peony_window_slot_info_get_window          (PeonyWindowSlotInfo            *slot);
#define peony_window_slot_info_open_location(slot, location, mode, flags, selection) \
	peony_window_slot_info_open_location_full(slot, location, mode, \
						 flags, selection, NULL, NULL)

void                              peony_window_slot_info_open_location_full
	(PeonyWindowSlotInfo *slot,
        GFile                             *location,
        PeonyWindowOpenMode                 mode,
        PeonyWindowOpenFlags                flags,
        GList                             *selection,
        PeonyWindowGoToCallback		   callback,
        gpointer			   user_data);
void                              peony_window_slot_info_set_status          (PeonyWindowSlotInfo            *slot,
        const char *status);
void                              peony_window_slot_info_make_hosting_pane_active (PeonyWindowSlotInfo       *slot);

char *                            peony_window_slot_info_get_current_location (PeonyWindowSlotInfo           *slot);
PeonyView *                    peony_window_slot_info_get_current_view     (PeonyWindowSlotInfo           *slot);
int                               peony_window_slot_info_get_selection_count  (PeonyWindowSlotInfo           *slot);
GList *                           peony_window_slot_info_get_selection        (PeonyWindowSlotInfo           *slot);
char *                            peony_window_slot_info_get_title            (PeonyWindowSlotInfo           *slot);

#endif /* PEONY_WINDOW_SLOT_INFO_H */
