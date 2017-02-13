/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *  Copyright (C) 2003 Ximian, Inc.
 *
 *  Peony is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Peony is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
/* peony-window.h: Interface of the main window object */

#ifndef PEONY_SPATIAL_WINDOW_H
#define PEONY_SPATIAL_WINDOW_H

#include "peony-window.h"
#include "peony-window-private.h"

#define PEONY_TYPE_SPATIAL_WINDOW peony_spatial_window_get_type()
#define PEONY_SPATIAL_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SPATIAL_WINDOW, PeonySpatialWindow))
#define PEONY_SPATIAL_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SPATIAL_WINDOW, PeonySpatialWindowClass))
#define PEONY_IS_SPATIAL_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SPATIAL_WINDOW))
#define PEONY_IS_SPATIAL_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SPATIAL_WINDOW))
#define PEONY_SPATIAL_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SPATIAL_WINDOW, PeonySpatialWindowClass))

#ifndef PEONY_SPATIAL_WINDOW_DEFINED
#define PEONY_SPATIAL_WINDOW_DEFINED
typedef struct _PeonySpatialWindow        PeonySpatialWindow;
#endif
typedef struct _PeonySpatialWindowClass   PeonySpatialWindowClass;
typedef struct _PeonySpatialWindowDetails PeonySpatialWindowDetails;

struct _PeonySpatialWindow
{
    PeonyWindow parent_object;

    PeonySpatialWindowDetails *details;
};

struct _PeonySpatialWindowClass
{
    PeonyWindowClass parent_spot;
};


GType            peony_spatial_window_get_type			(void);
void             peony_spatial_window_set_location_button		(PeonySpatialWindow *window,
        GFile                 *location);

#endif
