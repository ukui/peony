/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-column-choose.h - A column chooser widget

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

#ifndef PEONY_COLUMN_CHOOSER_H
#define PEONY_COLUMN_CHOOSER_H

#include <gtk/gtk.h>
#include <libpeony-private/peony-file.h>

#define PEONY_TYPE_COLUMN_CHOOSER peony_column_chooser_get_type()
#define PEONY_COLUMN_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_COLUMN_CHOOSER, PeonyColumnChooser))
#define PEONY_COLUMN_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_COLUMN_CHOOSER, PeonyColumnChooserClass))
#define PEONY_IS_COLUMN_CHOOSER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_COLUMN_CHOOSER))
#define PEONY_IS_COLUMN_CHOOSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_COLUMN_CHOOSER))
#define PEONY_COLUMN_CHOOSER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_COLUMN_CHOOSER, PeonyColumnChooserClass))

typedef struct _PeonyColumnChooserDetails PeonyColumnChooserDetails;

typedef struct
{
    GtkBox parent;

    PeonyColumnChooserDetails *details;
} PeonyColumnChooser;

typedef struct
{
    GtkBoxClass parent_slot;

    void (*changed) (PeonyColumnChooser *chooser);
    void (*use_default) (PeonyColumnChooser *chooser);
} PeonyColumnChooserClass;

GType      peony_column_chooser_get_type            (void);
GtkWidget *peony_column_chooser_new                 (PeonyFile *file);
void       peony_column_chooser_set_settings    (PeonyColumnChooser   *chooser,
        char                   **visible_columns,
        char                   **column_order);
void       peony_column_chooser_get_settings    (PeonyColumnChooser *chooser,
        char                  ***visible_columns,
        char                  ***column_order);

#endif /* PEONY_COLUMN_CHOOSER_H */
