/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* peony-file-drag.h - Drag & drop handling code that operated on
   PeonyFile objects.

   Copyright (C) 2000 Eazel, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Pavel Cisler <pavel@eazel.com>,
*/

#ifndef PEONY_FILE_DND_H
#define PEONY_FILE_DND_H

#include <libpeony-private/peony-dnd.h>
#include <libpeony-private/peony-file.h>

#define PEONY_FILE_DND_ERASE_KEYWORD "erase"

gboolean peony_drag_can_accept_item              (PeonyFile *drop_target_item,
        const char   *item_uri);
gboolean peony_drag_can_accept_items             (PeonyFile *drop_target_item,
        const GList  *items);
gboolean peony_drag_can_accept_info              (PeonyFile *drop_target_item,
        PeonyIconDndTargetType drag_type,
        const GList *items);
void     peony_drag_file_receive_dropped_keyword (PeonyFile *file,
        const char   *keyword);

#endif /* PEONY_FILE_DND_H */

