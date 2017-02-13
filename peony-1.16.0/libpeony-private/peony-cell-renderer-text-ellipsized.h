/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-cell-renderer-text-ellipsized.c: Cell renderer for text which
   will use pango ellipsization but deactivate it temporarily for the size
   calculation to get the size based on the actual text length.

   Copyright (C) 2007 Martin Wehner

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

   Author: Martin Wehner <martin.wehner@gmail.com>
*/

#ifndef PEONY_CELL_RENDERER_TEXT_ELLIPSIZED_H
#define PEONY_CELL_RENDERER_TEXT_ELLIPSIZED_H

#include <gtk/gtk.h>

#define PEONY_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED peony_cell_renderer_text_ellipsized_get_type()
#define PEONY_CELL_RENDERER_TEXT_ELLIPSIZED(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED, PeonyCellRendererTextEllipsized))
#define PEONY_CELL_RENDERER_TEXT_ELLIPSIZED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED, PeonyCellRendererTextEllipsizedClass))
#define PEONY_IS_CELL_RENDERER_TEXT_ELLIPSIZED(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED))
#define PEONY_IS_CELL_RENDERER_TEXT_ELLIPSIZED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED))
#define PEONY_CELL_RENDERER_TEXT_ELLIPSIZED_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_CELL_RENDERER_TEXT_ELLIPSIZED, PeonyCellRendererTextEllipsizedClass))


typedef struct _PeonyCellRendererTextEllipsized PeonyCellRendererTextEllipsized;
typedef struct _PeonyCellRendererTextEllipsizedClass PeonyCellRendererTextEllipsizedClass;

struct _PeonyCellRendererTextEllipsized
{
    GtkCellRendererText parent;
};

struct _PeonyCellRendererTextEllipsizedClass
{
    GtkCellRendererTextClass parent_class;
};

GType		 peony_cell_renderer_text_ellipsized_get_type (void);
GtkCellRenderer *peony_cell_renderer_text_ellipsized_new      (void);

#endif /* PEONY_CELL_RENDERER_TEXT_ELLIPSIZED_H */
