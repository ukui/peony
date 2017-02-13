/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 2009 Red Hat, Inc.
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
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *
 */

#ifndef PEONY_ZOOM_ACTION_H
#define PEONY_ZOOM_ACTION_H

#include <gtk/gtk.h>

#define PEONY_TYPE_ZOOM_ACTION            (peony_zoom_action_get_type ())
#define PEONY_ZOOM_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_ZOOM_ACTION, PeonyZoomAction))
#define PEONY_ZOOM_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_ZOOM_ACTION, PeonyZoomActionClass))
#define PEONY_IS_ZOOM_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_ZOOM_ACTION))
#define PEONY_IS_ZOOM_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), PEONY_TYPE_ZOOM_ACTION))
#define PEONY_ZOOM_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PEONY_TYPE_ZOOM_ACTION, PeonyZoomActionClass))

typedef struct _PeonyZoomAction       PeonyZoomAction;
typedef struct _PeonyZoomActionClass  PeonyZoomActionClass;
typedef struct PeonyZoomActionPrivate PeonyZoomActionPrivate;

struct _PeonyZoomAction
{
    GtkAction parent;

    /*< private >*/
    PeonyZoomActionPrivate *priv;
};

struct _PeonyZoomActionClass
{
    GtkActionClass parent_class;
};

GType    peony_zoom_action_get_type   (void);

#endif
