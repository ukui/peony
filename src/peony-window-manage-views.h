/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *
 *  Peony is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Peony is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author: Darin Adler <darin@bentspoon.com>
 *
 */

#ifndef PEONY_WINDOW_MANAGE_VIEWS_H
#define PEONY_WINDOW_MANAGE_VIEWS_H

#include "peony-window.h"
#include "peony-window-pane.h"
#include "peony-navigation-window.h"

void peony_window_manage_views_close_slot (PeonyWindowPane *pane,
        PeonyWindowSlot *slot);


/* PeonyWindowInfo implementation: */
void peony_window_report_load_underway     (PeonyWindow     *window,
        PeonyView       *view);
void peony_window_report_selection_changed (PeonyWindowInfo *window);
void peony_window_report_view_failed       (PeonyWindow     *window,
        PeonyView       *view);
void peony_window_report_load_complete     (PeonyWindow     *window,
        PeonyView       *view);
void peony_window_report_location_change   (PeonyWindow     *window);
void peony_window_update_up_button         (PeonyWindow     *window);

#endif /* PEONY_WINDOW_MANAGE_VIEWS_H */
