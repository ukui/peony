/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Peony - Icon canvas item class for icon container.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PEONY_ICON_CANVAS_ITEM_H
#define PEONY_ICON_CANVAS_ITEM_H

#include <eel/eel-canvas.h>
#include <eel/eel-art-extensions.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEONY_TYPE_ICON_CANVAS_ITEM peony_icon_canvas_item_get_type()
#define PEONY_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_ICON_CANVAS_ITEM, PeonyIconCanvasItem))
#define PEONY_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_ICON_CANVAS_ITEM, PeonyIconCanvasItemClass))
#define PEONY_IS_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_ICON_CANVAS_ITEM))
#define PEONY_IS_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_ICON_CANVAS_ITEM))
#define PEONY_ICON_CANVAS_ITEM_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_ICON_CANVAS_ITEM, PeonyIconCanvasItemClass))

    typedef struct PeonyIconCanvasItem PeonyIconCanvasItem;
    typedef struct PeonyIconCanvasItemClass PeonyIconCanvasItemClass;
    typedef struct PeonyIconCanvasItemDetails PeonyIconCanvasItemDetails;

    struct PeonyIconCanvasItem
    {
        EelCanvasItem item;
        PeonyIconCanvasItemDetails *details;
        gpointer user_data;
    };

    struct PeonyIconCanvasItemClass
    {
        EelCanvasItemClass parent_class;
    };

    /* not namespaced due to their length */
    typedef enum
    {
        BOUNDS_USAGE_FOR_LAYOUT,
        BOUNDS_USAGE_FOR_ENTIRE_ITEM,
        BOUNDS_USAGE_FOR_DISPLAY
    } PeonyIconCanvasItemBoundsUsage;

    /* GObject */
    GType       peony_icon_canvas_item_get_type                 (void);

    /* attributes */
    void        peony_icon_canvas_item_set_image                (PeonyIconCanvasItem       *item,
            GdkPixbuf                    *image);
#if GTK_CHECK_VERSION(3,0,0)
    cairo_surface_t* peony_icon_canvas_item_get_drag_surface    (PeonyIconCanvasItem       *item);
#else
    GdkPixmap * peony_icon_canvas_item_get_image                (PeonyIconCanvasItem       *item,
    								GdkBitmap                **mask,
    								GdkColormap		 *colormap);
#endif
    void        peony_icon_canvas_item_set_emblems              (PeonyIconCanvasItem       *item,
            GList                        *emblem_pixbufs);
    void        peony_icon_canvas_item_set_show_stretch_handles (PeonyIconCanvasItem       *item,
            gboolean                      show_stretch_handles);
    void        peony_icon_canvas_item_set_attach_points        (PeonyIconCanvasItem       *item,
            GdkPoint                     *attach_points,
            int                           n_attach_points);
    void        peony_icon_canvas_item_set_embedded_text_rect   (PeonyIconCanvasItem       *item,
            const GdkRectangle           *text_rect);
    void        peony_icon_canvas_item_set_embedded_text        (PeonyIconCanvasItem       *item,
            const char                   *text);
    double      peony_icon_canvas_item_get_max_text_width       (PeonyIconCanvasItem       *item);
    const char *peony_icon_canvas_item_get_editable_text        (PeonyIconCanvasItem       *icon_item);
    void        peony_icon_canvas_item_set_renaming             (PeonyIconCanvasItem       *icon_item,
            gboolean                      state);

    /* geometry and hit testing */
    gboolean    peony_icon_canvas_item_hit_test_rectangle       (PeonyIconCanvasItem       *item,
            EelIRect                      canvas_rect);
    gboolean    peony_icon_canvas_item_hit_test_stretch_handles (PeonyIconCanvasItem       *item,
            EelDPoint                     world_point,
            GtkCornerType                *corner);
    void        peony_icon_canvas_item_invalidate_label         (PeonyIconCanvasItem       *item);
    void        peony_icon_canvas_item_invalidate_label_size    (PeonyIconCanvasItem       *item);
    EelDRect    peony_icon_canvas_item_get_icon_rectangle       (const PeonyIconCanvasItem *item);
    EelDRect    peony_icon_canvas_item_get_text_rectangle       (PeonyIconCanvasItem       *item,
            gboolean                      for_layout);
    void        peony_icon_canvas_item_get_bounds_for_layout    (PeonyIconCanvasItem       *item,
            double *x1, double *y1, double *x2, double *y2);
    void        peony_icon_canvas_item_get_bounds_for_entire_item (PeonyIconCanvasItem       *item,
            double *x1, double *y1, double *x2, double *y2);
    void        peony_icon_canvas_item_update_bounds            (PeonyIconCanvasItem       *item,
            double i2w_dx, double i2w_dy);
    void        peony_icon_canvas_item_set_is_visible           (PeonyIconCanvasItem       *item,
            gboolean                      visible);
    /* whether the entire label text must be visible at all times */
    void        peony_icon_canvas_item_set_entire_text          (PeonyIconCanvasItem       *icon_item,
            gboolean                      entire_text);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_ICON_CANVAS_ITEM_H */
