/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2005 Red Hat, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Author:  Alexander Larsson <alexl@redhat.com>
 */

#ifndef PEONY_WINDOW_BOOKMARKS_H
#define PEONY_WINDOW_BOOKMARKS_H

#include <libpeony-private/peony-bookmark.h>
#include <peony-window.h>
#include "peony-bookmark-list.h"

void                  peony_bookmarks_exiting                        (void);
void                  peony_window_add_bookmark_for_current_location (PeonyWindow *window);
void                  peony_window_edit_bookmarks                    (PeonyWindow *window);
void                  peony_window_initialize_bookmarks_menu         (PeonyWindow *window);

#endif
