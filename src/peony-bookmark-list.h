/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
 * Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: John Sullivan <sullivan@eazel.com>
 *          Zuxun Yang <yangzuxun@kylinos.cn>
 */

/* peony-bookmark-list.h - interface for centralized list of bookmarks.
 */

#ifndef PEONY_BOOKMARK_LIST_H
#define PEONY_BOOKMARK_LIST_H

#include <libpeony-private/peony-bookmark.h>
#include <gio/gio.h>

typedef struct PeonyBookmarkList PeonyBookmarkList;
typedef struct PeonyBookmarkListClass PeonyBookmarkListClass;

#define PEONY_TYPE_BOOKMARK_LIST peony_bookmark_list_get_type()
#define PEONY_BOOKMARK_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_BOOKMARK_LIST, PeonyBookmarkList))
#define PEONY_BOOKMARK_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_BOOKMARK_LIST, PeonyBookmarkListClass))
#define PEONY_IS_BOOKMARK_LIST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_BOOKMARK_LIST))
#define PEONY_IS_BOOKMARK_LIST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_BOOKMARK_LIST))
#define PEONY_BOOKMARK_LIST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_BOOKMARK_LIST, PeonyBookmarkListClass))

struct PeonyBookmarkList
{
    GObject object;

    GList *list;
    GFileMonitor *monitor;
    GQueue *pending_ops;
};

struct PeonyBookmarkListClass
{
    GObjectClass parent_class;
    void (* contents_changed) (PeonyBookmarkList *bookmarks);
};

GType                   peony_bookmark_list_get_type            (void);
PeonyBookmarkList *  peony_bookmark_list_new                 (void);
void                    peony_bookmark_list_append              (PeonyBookmarkList   *bookmarks,
        PeonyBookmark *bookmark);
gboolean                peony_bookmark_list_contains            (PeonyBookmarkList   *bookmarks,
        PeonyBookmark *bookmark);
void                    peony_bookmark_list_delete_item_at      (PeonyBookmarkList   *bookmarks,
        guint                   index);
void                    peony_bookmark_list_delete_items_with_uri (PeonyBookmarkList *bookmarks,
        const char		   *uri);
void                    peony_bookmark_list_insert_item         (PeonyBookmarkList   *bookmarks,
        PeonyBookmark *bookmark,
        guint                   index);
guint                   peony_bookmark_list_length              (PeonyBookmarkList   *bookmarks);
PeonyBookmark *      peony_bookmark_list_item_at             (PeonyBookmarkList   *bookmarks,
        guint                   index);
void                    peony_bookmark_list_move_item           (PeonyBookmarkList *bookmarks,
        guint                 index,
        guint                 destination);
void                    peony_bookmark_list_set_window_geometry (PeonyBookmarkList   *bookmarks,
        const char             *geometry);
const char *            peony_bookmark_list_get_window_geometry (PeonyBookmarkList   *bookmarks);

#endif /* PEONY_BOOKMARK_LIST_H */
