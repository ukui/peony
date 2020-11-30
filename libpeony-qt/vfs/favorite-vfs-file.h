/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Ding Jing <dingjing@kylinos.cn>
 *
 */

#ifndef FAVORITEVFSFILE_H
#define FAVORITEVFSFILE_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define VFS_TYPE_FAVORITES_FILE vfs_favorites_file_get_type()

G_DECLARE_FINAL_TYPE(FavoritesVFSFile, vfs_favorites_file, VFS, FAVORITES_FILE, GObject)

FavoritesVFSFile* vfs_favorites_file_new(void);

typedef struct _FavoritesVFSFilePrivate FavoritesVFSFilePrivate;

struct _FavoritesVFSFilePrivate
{
    gchar*              uri;
    GList*              fileList;
    GFileMonitor*       fileMonitor;
};

struct _FavoritesVFSFile
{
    GObject parent_instance;
    FavoritesVFSFilePrivate* priv;
};

G_END_DECLS

extern "C"
{
    GFile* vfs_favorites_file_new_for_uri (const char* uri);
    static GFileEnumerator* vfs_favorites_file_enumerate_children_internal (GFile* file, const char* attribute, GFileQueryInfoFlags flags, GCancellable* cancellable, GError** error);
}


#endif // FAVORITEVFSFILE_H
