/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 * Authors: Darin Adler <darin@bentspoon.com>
 */

/* peony-desktop-window.h
 */

#ifndef PEONY_DESKTOP_WINDOW_H
#define PEONY_DESKTOP_WINDOW_H

#include "peony-window.h"
#include "peony-application.h"
#include "peony-spatial-window.h"

#define PEONY_TYPE_DESKTOP_WINDOW peony_desktop_window_get_type()
#define PEONY_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_DESKTOP_WINDOW, PeonyDesktopWindow))
#define PEONY_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_DESKTOP_WINDOW, PeonyDesktopWindowClass))
#define PEONY_IS_DESKTOP_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_DESKTOP_WINDOW))
#define PEONY_IS_DESKTOP_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_DESKTOP_WINDOW))
#define PEONY_DESKTOP_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_DESKTOP_WINDOW, PeonyDesktopWindowClass))

typedef struct PeonyDesktopWindowDetails PeonyDesktopWindowDetails;

typedef struct
{
    PeonySpatialWindow parent_spot;
    PeonyDesktopWindowDetails *details;
    gboolean affect_desktop_on_next_location_change;
} PeonyDesktopWindow;

typedef struct
{
    PeonySpatialWindowClass parent_spot;
} PeonyDesktopWindowClass;

GType                  peony_desktop_window_get_type            (void);
PeonyDesktopWindow *peony_desktop_window_new                 (PeonyApplication *application,
        GdkScreen           *screen);
void                   peony_desktop_window_update_directory    (PeonyDesktopWindow *window);
gboolean               peony_desktop_window_loaded              (PeonyDesktopWindow *window);

#endif /* PEONY_DESKTOP_WINDOW_H */
