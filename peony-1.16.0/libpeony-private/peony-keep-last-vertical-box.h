/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-keep-last-vertical-box.h: Subclass of GtkBox that clips off
 				      items that don't fit, except the last one.

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

   Author: John Sullivan <sullivan@eazel.com>,
 */

#ifndef PEONY_KEEP_LAST_VERTICAL_BOX_H
#define PEONY_KEEP_LAST_VERTICAL_BOX_H

#include <gtk/gtk.h>

#define PEONY_TYPE_KEEP_LAST_VERTICAL_BOX peony_keep_last_vertical_box_get_type()
#define PEONY_KEEP_LAST_VERTICAL_BOX(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_KEEP_LAST_VERTICAL_BOX, PeonyKeepLastVerticalBox))
#define PEONY_KEEP_LAST_VERTICAL_BOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_KEEP_LAST_VERTICAL_BOX, PeonyKeepLastVerticalBoxClass))
#define PEONY_IS_KEEP_LAST_VERTICAL_BOX(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_KEEP_LAST_VERTICAL_BOX))
#define PEONY_IS_KEEP_LAST_VERTICAL_BOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_KEEP_LAST_VERTICAL_BOX))
#define PEONY_KEEP_LAST_VERTICAL_BOX_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_KEEP_LAST_VERTICAL_BOX, PeonyKeepLastVerticalBoxClass))

typedef struct PeonyKeepLastVerticalBox PeonyKeepLastVerticalBox;
typedef struct PeonyKeepLastVerticalBoxClass PeonyKeepLastVerticalBoxClass;

struct PeonyKeepLastVerticalBox
{
    GtkBox parent;
};

struct PeonyKeepLastVerticalBoxClass
{
    GtkBoxClass parent_class;
};

GType      peony_keep_last_vertical_box_get_type  (void);
GtkWidget *peony_keep_last_vertical_box_new       (gint spacing);

#endif /* PEONY_KEEP_LAST_VERTICAL_BOX_H */
