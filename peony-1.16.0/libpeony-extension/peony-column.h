/*
 *  peony-column.h - Info columns exported by
 *                      PeonyColumnProvider objects.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

#ifndef PEONY_COLUMN_H
#define PEONY_COLUMN_H

#include <glib-object.h>
#include "peony-extension-types.h"

G_BEGIN_DECLS

#define PEONY_TYPE_COLUMN            (peony_column_get_type())
#define PEONY_COLUMN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_COLUMN, PeonyColumn))
#define PEONY_COLUMN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_COLUMN, PeonyColumnClass))
#define PEONY_INFO_IS_COLUMN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_COLUMN))
#define PEONY_INFO_IS_COLUMN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), PEONY_TYPE_COLUMN))
#define PEONY_COLUMN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PEONY_TYPE_COLUMN, PeonyColumnClass))

typedef struct _PeonyColumn        PeonyColumn;
typedef struct _PeonyColumnDetails PeonyColumnDetails;
typedef struct _PeonyColumnClass   PeonyColumnClass;

struct _PeonyColumn {
    GObject parent;

    PeonyColumnDetails *details;
};

struct _PeonyColumnClass {
    GObjectClass parent;
};

GType       peony_column_get_type  (void);
PeonyColumn *peony_column_new       (const char *name,
                                   const char *attribute,
                                   const char *label,
                                   const char *description);

/* PeonyColumn has the following properties:
 *   name (string)        - the identifier for the column
 *   attribute (string)   - the file attribute to be displayed in the
 *                          column
 *   label (string)       - the user-visible label for the column
 *   description (string) - a user-visible description of the column
 *   xalign (float)       - x-alignment of the column
 */

G_END_DECLS

#endif
