/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
 * Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: John Sullivan <sullivan@eazel.com>
 *          Zuxun Yang <yangzuxun@kylinos.cn>
 */

/* peony-bookmarks-window.h - interface for bookmark-editing window.
 */

#ifndef PEONY_BOOKMARKS_WINDOW_H
#define PEONY_BOOKMARKS_WINDOW_H

#include <gtk/gtk.h>
#include "peony-bookmark-list.h"
#include "peony-window.h"

GtkWindow *create_bookmarks_window                 (PeonyBookmarkList *bookmarks,
                                                    PeonyWindow       *window_source);
void       peony_bookmarks_window_save_geometry     (GtkWindow        *window);
void	   edit_bookmarks_dialog_set_signals	   (PeonyWindow       *window);

#endif /* PEONY_BOOKMARKS_WINDOW_H */
