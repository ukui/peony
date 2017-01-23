/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-bookmark.h - interface for individual bookmarks.

   Copyright (C) 1999, 2000 Eazel, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: John Sullivan <sullivan@eazel.com>
*/

#ifndef PEONY_BOOKMARK_H
#define PEONY_BOOKMARK_H

#include <gtk/gtk.h>
#include <gio/gio.h>
typedef struct PeonyBookmark PeonyBookmark;

#define PEONY_TYPE_BOOKMARK peony_bookmark_get_type()
#define PEONY_BOOKMARK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_BOOKMARK, PeonyBookmark))
#define PEONY_BOOKMARK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_BOOKMARK, PeonyBookmarkClass))
#define PEONY_IS_BOOKMARK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_BOOKMARK))
#define PEONY_IS_BOOKMARK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_BOOKMARK))
#define PEONY_BOOKMARK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_BOOKMARK, PeonyBookmarkClass))

typedef struct PeonyBookmarkDetails PeonyBookmarkDetails;

struct PeonyBookmark
{
    GObject object;
    PeonyBookmarkDetails *details;
};

struct PeonyBookmarkClass
{
    GObjectClass parent_class;

    /* Signals that clients can connect to. */

    /* The appearance_changed signal is emitted when the bookmark's
     * name or icon has changed.
     */
    void	(* appearance_changed) (PeonyBookmark *bookmark);

    /* The contents_changed signal is emitted when the bookmark's
     * URI has changed.
     */
    void	(* contents_changed) (PeonyBookmark *bookmark);
};

typedef struct PeonyBookmarkClass PeonyBookmarkClass;

GType                 peony_bookmark_get_type               (void);
PeonyBookmark *    peony_bookmark_new                    (GFile *location,
        const char *name,
        gboolean has_custom_name,
        GIcon *icon);
PeonyBookmark *    peony_bookmark_copy                   (PeonyBookmark      *bookmark);
char *                peony_bookmark_get_name               (PeonyBookmark      *bookmark);
GFile *               peony_bookmark_get_location           (PeonyBookmark      *bookmark);
char *                peony_bookmark_get_uri                (PeonyBookmark      *bookmark);
GIcon *               peony_bookmark_get_icon               (PeonyBookmark      *bookmark);
gboolean	      peony_bookmark_get_has_custom_name    (PeonyBookmark      *bookmark);
gboolean              peony_bookmark_set_name               (PeonyBookmark      *bookmark,
        const char            *new_name);
gboolean              peony_bookmark_uri_known_not_to_exist (PeonyBookmark      *bookmark);
int                   peony_bookmark_compare_with           (gconstpointer          a,
        gconstpointer          b);
int                   peony_bookmark_compare_uris           (gconstpointer          a,
        gconstpointer          b);

void                  peony_bookmark_set_scroll_pos         (PeonyBookmark      *bookmark,
        const char            *uri);
char *                peony_bookmark_get_scroll_pos         (PeonyBookmark      *bookmark);


/* Helper functions for displaying bookmarks */
GdkPixbuf *           peony_bookmark_get_pixbuf             (PeonyBookmark      *bookmark,
        GtkIconSize            icon_size);
GtkWidget *           peony_bookmark_menu_item_new          (PeonyBookmark      *bookmark);

#endif /* PEONY_BOOKMARK_H */
