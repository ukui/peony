/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-column-utilities.h - Utilities related to column specifications

   Copyright (C) 2004 Novell, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the column COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Dave Camp <dave@ximian.com>
*/

#ifndef PEONY_COLUMN_UTILITIES_H
#define PEONY_COLUMN_UTILITIES_H

#include <libpeony-extension/peony-column.h>
#include <libpeony-private/peony-file.h>

GList *peony_get_all_columns       (void);
GList *peony_get_common_columns    (void);
GList *peony_get_columns_for_file (PeonyFile *file);
GList *peony_column_list_copy      (GList       *columns);
void   peony_column_list_free      (GList       *columns);

GList *peony_sort_columns          (GList       *columns,
                                   char       **column_order);


#endif /* PEONY_COLUMN_UTILITIES_H */
