/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*  peony-side-pane.c
 *
 *  Copyright (C) 2002 Ximian, Inc.
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
 *  Author: Dave Camp <dave@ximian.com>
 */

#ifndef PEONY_SIDE_PANE_H
#define PEONY_SIDE_PANE_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEONY_TYPE_SIDE_PANE peony_side_pane_get_type()
#define PEONY_SIDE_PANE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SIDE_PANE, PeonySidePane))
#define PEONY_SIDE_PANE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SIDE_PANE, PeonySidePaneClass))
#define PEONY_IS_SIDE_PANE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SIDE_PANE))
#define PEONY_IS_SIDE_PANE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SIDE_PANE))
#define PEONY_SIDE_PANE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SIDE_PANE, PeonySidePaneClass))

    typedef struct _PeonySidePaneDetails PeonySidePaneDetails;

    typedef struct
    {
#if GTK_CHECK_VERSION (3, 0, 0)
        GtkBox parent;
#else
        GtkVBox parent;
#endif
        PeonySidePaneDetails *details;
    } PeonySidePane;

    typedef struct
    {
#if GTK_CHECK_VERSION (3, 0, 0)
        GtkBoxClass parent_slot;
#else
        GtkVBoxClass parent_slot;
#endif

        void (*close_requested) (PeonySidePane *side_pane);
        void (*switch_page) (PeonySidePane *side_pane,
                             GtkWidget *child);
    } PeonySidePaneClass;

    GType                  peony_side_pane_get_type        (void);
    PeonySidePane      *peony_side_pane_new             (void);
    void                   peony_side_pane_add_panel       (PeonySidePane *side_pane,
            GtkWidget        *widget,
            const char       *title,
            const char       *tooltip);
    void                   peony_side_pane_remove_panel    (PeonySidePane *side_pane,
            GtkWidget        *widget);
    void                   peony_side_pane_show_panel      (PeonySidePane *side_pane,
            GtkWidget        *widget);
    void                   peony_side_pane_set_panel_image (PeonySidePane *side_pane,
            GtkWidget        *widget,
            GdkPixbuf        *pixbuf);
    GtkWidget             *peony_side_pane_get_current_panel (PeonySidePane *side_pane);
    GtkWidget             *peony_side_pane_get_title        (PeonySidePane *side_pane);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_SIDE_PANE_H */
