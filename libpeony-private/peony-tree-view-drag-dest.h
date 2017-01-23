/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2002 Sun Microsystems, Inc.
 *
 * Peony is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Dave Camp <dave@ximian.com>
 */

/* peony-tree-view-drag-dest.h: Handles drag and drop for treeviews which
 *                                 contain a hierarchy of files
 */

#ifndef PEONY_TREE_VIEW_DRAG_DEST_H
#define PEONY_TREE_VIEW_DRAG_DEST_H

#include <gtk/gtk.h>

#include "peony-file.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PEONY_TYPE_TREE_VIEW_DRAG_DEST	(peony_tree_view_drag_dest_get_type ())
#define PEONY_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_TREE_VIEW_DRAG_DEST, PeonyTreeViewDragDest))
#define PEONY_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_TREE_VIEW_DRAG_DEST, PeonyTreeViewDragDestClass))
#define PEONY_IS_TREE_VIEW_DRAG_DEST(obj)		(G_TYPE_INSTANCE_CHECK_TYPE ((obj), PEONY_TYPE_TREE_VIEW_DRAG_DEST))
#define PEONY_IS_TREE_VIEW_DRAG_DEST_CLASS(klass)	(G_TYPE_CLASS_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_TREE_VIEW_DRAG_DEST))

    typedef struct _PeonyTreeViewDragDest        PeonyTreeViewDragDest;
    typedef struct _PeonyTreeViewDragDestClass   PeonyTreeViewDragDestClass;
    typedef struct _PeonyTreeViewDragDestDetails PeonyTreeViewDragDestDetails;

    struct _PeonyTreeViewDragDest
    {
        GObject parent;

        PeonyTreeViewDragDestDetails *details;
    };

    struct _PeonyTreeViewDragDestClass
    {
        GObjectClass parent;

        char *(*get_root_uri) (PeonyTreeViewDragDest *dest);
        PeonyFile *(*get_file_for_path) (PeonyTreeViewDragDest *dest,
                                        GtkTreePath *path);
        void (*move_copy_items) (PeonyTreeViewDragDest *dest,
                                 const GList *item_uris,
                                 const char *target_uri,
                                 GdkDragAction action,
                                 int x,
                                 int y);
        void (* handle_netscape_url) (PeonyTreeViewDragDest *dest,
                                      const char *url,
                                      const char *target_uri,
                                      GdkDragAction action,
                                      int x,
                                      int y);
        void (* handle_uri_list) (PeonyTreeViewDragDest *dest,
                                  const char *uri_list,
                                  const char *target_uri,
                                  GdkDragAction action,
                                  int x,
                                  int y);
        void (* handle_text)    (PeonyTreeViewDragDest *dest,
                                 const char *text,
                                 const char *target_uri,
                                 GdkDragAction action,
                                 int x,
                                 int y);
        void (* handle_raw)    (PeonyTreeViewDragDest *dest,
                                char *raw_data,
                                int length,
                                const char *target_uri,
                                const char *direct_save_uri,
                                GdkDragAction action,
                                int x,
                                int y);
    };

    GType                     peony_tree_view_drag_dest_get_type (void);
    PeonyTreeViewDragDest *peony_tree_view_drag_dest_new      (GtkTreeView *tree_view);

#ifdef __cplusplus
}
#endif

#endif
