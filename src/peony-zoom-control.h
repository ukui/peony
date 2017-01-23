/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Peony is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 *
 * This is the header file for the zoom control on the location bar
 *
 */

#ifndef PEONY_ZOOM_CONTROL_H
#define PEONY_ZOOM_CONTROL_H

#include <gtk/gtk.h>
#include <libpeony-private/peony-icon-info.h> /* For PeonyZoomLevel */

#define PEONY_TYPE_ZOOM_CONTROL peony_zoom_control_get_type()
#define PEONY_ZOOM_CONTROL(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_ZOOM_CONTROL, PeonyZoomControl))
#define PEONY_ZOOM_CONTROL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_ZOOM_CONTROL, PeonyZoomControlClass))
#define PEONY_IS_ZOOM_CONTROL(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_ZOOM_CONTROL))
#define PEONY_IS_ZOOM_CONTROL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_ZOOM_CONTROL))
#define PEONY_ZOOM_CONTROL_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_ZOOM_CONTROL, PeonyZoomControlClass))

typedef struct PeonyZoomControl PeonyZoomControl;
typedef struct PeonyZoomControlClass PeonyZoomControlClass;
typedef struct PeonyZoomControlDetails PeonyZoomControlDetails;

struct PeonyZoomControl
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBox parent;
#else
    GtkHBox parent;
#endif
    PeonyZoomControlDetails *details;
};

struct PeonyZoomControlClass
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBoxClass parent_class;
#else
    GtkHBoxClass parent_class;
#endif

    void (*zoom_in)		(PeonyZoomControl *control);
    void (*zoom_out) 	(PeonyZoomControl *control);
    void (*zoom_to_level) 	(PeonyZoomControl *control,
                             PeonyZoomLevel zoom_level);
    void (*zoom_to_default)	(PeonyZoomControl *control);

    /* Action signal for keybindings, do not connect to this */
    void (*change_value)    (PeonyZoomControl *control,
                             GtkScrollType scroll);
};

GType             peony_zoom_control_get_type           (void);
GtkWidget *       peony_zoom_control_new                (void);
void              peony_zoom_control_set_zoom_level     (PeonyZoomControl *zoom_control,
        PeonyZoomLevel    zoom_level);
void              peony_zoom_control_set_parameters     (PeonyZoomControl *zoom_control,
        PeonyZoomLevel    min_zoom_level,
        PeonyZoomLevel    max_zoom_level,
        gboolean             has_min_zoom_level,
        gboolean             has_max_zoom_level,
        GList               *zoom_levels);
PeonyZoomLevel peony_zoom_control_get_zoom_level     (PeonyZoomControl *zoom_control);
PeonyZoomLevel peony_zoom_control_get_min_zoom_level (PeonyZoomControl *zoom_control);
PeonyZoomLevel peony_zoom_control_get_max_zoom_level (PeonyZoomControl *zoom_control);
gboolean          peony_zoom_control_has_min_zoom_level (PeonyZoomControl *zoom_control);
gboolean          peony_zoom_control_has_max_zoom_level (PeonyZoomControl *zoom_control);
gboolean          peony_zoom_control_can_zoom_in        (PeonyZoomControl *zoom_control);
gboolean          peony_zoom_control_can_zoom_out       (PeonyZoomControl *zoom_control);

void              peony_zoom_control_set_active_appearance (PeonyZoomControl *zoom_control, gboolean is_active);

#endif /* PEONY_ZOOM_CONTROL_H */
