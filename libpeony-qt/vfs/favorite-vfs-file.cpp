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

#include "favorite-vfs-file.h"
#include "favorite-vfs-file-monitor.h"
#include "favorite-vfs-file-enumerator.h"

#include <QDebug>
#include <QUrl>
#include <bookmark-manager.h>

static void vfs_favorites_file_g_file_iface_init(GFileIface *iface);

char* vfs_favorites_file_get_uri(GFile *file);
char* vfs_favorites_file_get_path(GFile *file);
char* vfs_favorites_file_get_schema (GFile* file);
GFile* vfs_favorites_file_get_parent(GFile *file);
static GFile* vfs_favorites_file_dup(GFile *file);
char* vfs_favorites_file_get_schema (GFile* file);
char* vfs_favorites_file_get_basename (GFile* file);
GFile* vfs_favorites_file_new_for_uri(const char *uri);
static void vfs_favorites_file_dispose(GObject *object);
static gboolean vfs_favorites_file_is_native(GFile *file);
GFile* vfs_favorites_file_resolve_relative_path(GFile *file, const char *relative_path);
gboolean vfs_favorites_file_delete (GFile* file, GCancellable* cancellable, GError** error);
GFileMonitor* vfs_favorites_file_monitor_directory (GFile* file, GFileMonitorFlags flags, GCancellable* cancellable, GError** error);
GFileInfo* vfs_favorites_file_query_info(GFile *file, const char *attributes, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error);
GFileEnumerator* vfs_favorites_file_enumerate_children(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error);
GFileEnumerator *vfs_favorites_file_enumerate_children_internal(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error);

G_DEFINE_TYPE_EXTENDED(FavoritesVFSFile, vfs_favorites_file, G_TYPE_OBJECT, 0, G_ADD_PRIVATE(FavoritesVFSFile) G_IMPLEMENT_INTERFACE(G_TYPE_FILE, vfs_favorites_file_g_file_iface_init));

static void vfs_favorites_file_init(FavoritesVFSFile* self)
{
    FavoritesVFSFilePrivate *priv = (FavoritesVFSFilePrivate*)vfs_favorites_file_get_instance_private(self);
    self->priv = priv;

    self->priv->uri = nullptr;
}

static void vfs_favorites_file_class_init (FavoritesVFSFileClass* kclass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS (kclass);

    gobject_class->dispose = vfs_favorites_file_dispose;
}

static void vfs_favorites_file_dispose(GObject *object)
{
    auto vfsfile = VFS_FAVORITES_FILE (object);

    if (nullptr != vfsfile->priv->fileMonitor) {
        g_object_unref (vfsfile->priv->fileMonitor);
        vfsfile->priv->fileMonitor = nullptr;
    }

    if (vfsfile->priv->uri) {
        g_free(vfsfile->priv->uri);
        vfsfile->priv->uri = nullptr;
    }
}

static void vfs_favorites_file_g_file_iface_init(GFileIface *iface)
{
    iface->dup = vfs_favorites_file_dup;
    iface->trash = vfs_favorites_file_delete;
    iface->get_uri = vfs_favorites_file_get_uri;
    iface->get_path = vfs_favorites_file_get_path;
    iface->delete_file = vfs_favorites_file_delete;
    iface->is_native = vfs_favorites_file_is_native;
    iface->get_parent = vfs_favorites_file_get_parent;
    iface->query_info = vfs_favorites_file_query_info;
    iface->get_basename = vfs_favorites_file_get_basename;
    iface->get_uri_scheme = vfs_favorites_file_get_schema;
    iface->monitor_dir = vfs_favorites_file_monitor_directory;
    iface->enumerate_children = vfs_favorites_file_enumerate_children;
    iface->resolve_relative_path = vfs_favorites_file_resolve_relative_path;
}

char* vfs_favorites_file_get_uri(GFile *file)
{
    auto vfs_file = VFS_FAVORITES_FILE (file);

    return g_strdup(vfs_file->priv->uri);
}

gboolean vfs_favorites_file_is_native(GFile *file)
{
    Q_UNUSED(file);

    return FALSE;
}

GFile* vfs_favorites_file_dup (GFile *file)
{
    if (!VFS_FAVORITES_FILE(file)) {
        return g_file_new_for_uri("favorite:///");
    }
    auto vfs_file = VFS_FAVORITES_FILE(file);
    auto dup = VFS_FAVORITES_FILE(g_object_new(VFS_TYPE_FAVORITES_FILE, nullptr));
    dup->priv->uri = g_strdup(vfs_file->priv->uri);

    return G_FILE(dup);
}

GFileEnumerator* vfs_favorites_file_enumerate_children_internal(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error)
{
    auto enumerator = VFS_FAVORITES_FILE_ENUMERATOR(g_object_new(VFS_TYPE_FAVORITES_FILE_ENUMERATOR, "container", file, nullptr));
    Q_UNUSED(flags)
    Q_UNUSED(error)
    Q_UNUSED(attribute)
    Q_UNUSED(cancellable)

    return G_FILE_ENUMERATOR(enumerator);
}

GFile* vfs_favorites_file_new_for_uri(const char *uri)
{
    auto search_vfs_file = VFS_FAVORITES_FILE(g_object_new(VFS_TYPE_FAVORITES_FILE, nullptr));
    search_vfs_file->priv->uri = g_strdup(uri);

    return G_FILE(search_vfs_file);
}

char* vfs_favorites_file_get_path(GFile *file)
{
    QUrl url = QString(vfs_favorites_file_get_uri (file));

    return g_strdup (url.path().toUtf8().constData());
}

GFile* vfs_favorites_file_resolve_relative_path(GFile *file, const char *relative_path)
{
    Q_UNUSED(file);

    QString tmp = relative_path;
    if (tmp.contains("real-uri:")) {
        tmp = tmp.remove("real-uri:");
        return g_file_new_for_uri(tmp.toUtf8().constData());
    }
    return g_file_new_for_uri("favorite:///");
}

GFileInfo* vfs_favorites_file_query_info(GFile *file, const char *attributes, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error)
{
    auto vfsfile = VFS_FAVORITES_FILE(file);

    g_return_val_if_fail(nullptr != file, nullptr);

    GFileInfo* info = nullptr;
    QString trueUri = nullptr;
    QUrl url(vfsfile->priv->uri);

    if ("favorite:///" != url.toString()) {
        QStringList querys = url.query().split("&");
        for (int i = 0; i < querys.count(); ++i) {
            if (querys.at(i).contains("schema=")) {
                QString scheam = querys.at(i).split("=").last();
                if ("favorite" == scheam) {
                    trueUri = QString("%1://%2").arg("file").arg(url.path());
                } else {
                    trueUri = QString("%1://%2").arg(scheam).arg(url.path());
                }
                break;
            }
        }
    }

    if (nullptr != trueUri) {
        GFile* file = g_file_new_for_uri (trueUri.toUtf8().constData());
        if (nullptr != file) {
            info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
        }
        g_file_info_set_attribute_string(info, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI, trueUri.toUtf8().constData());
    } else {
        info = g_file_info_new ();
        QString name = QObject::tr("favorite");
        auto icon = g_themed_icon_new("favorite");
        g_file_info_set_icon(info, icon);
        g_object_unref(icon);
        g_file_info_set_size(info, 0);
        g_file_info_set_is_hidden(info, FALSE);
        g_file_info_set_is_symlink(info, FALSE);
        g_file_info_set_file_type(info, G_FILE_TYPE_DIRECTORY);
        g_file_info_set_name(info, name.toUtf8().constData());
        g_file_info_set_display_name(info, name.toUtf8().constData());
        g_file_info_set_attribute_boolean(info, G_FILE_ATTRIBUTE_STANDARD_IS_VIRTUAL, TRUE);
    }

    Q_UNUSED(flags)
    Q_UNUSED(error)
    Q_UNUSED(attributes)
    Q_UNUSED(cancellable)

    return info;
}

GFileEnumerator* vfs_favorites_file_enumerate_children(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error)
{
    auto vf = VFS_FAVORITES_FILE(file);

    return vfs_favorites_file_enumerate_children_internal(file, attribute, flags, cancellable, error);
}

GFileMonitor* vfs_favorites_file_monitor_directory (GFile* file, GFileMonitorFlags flags, GCancellable* cancellable, GError** error)
{
    GFileMonitor* fmonitor = (GFileMonitor*) g_object_new (VFS_TYPE_FAVORITE_FILE_MONITOR, nullptr);

    vfs_favorite_file_monitor_dir (VFS_FAVORITE_FILE_MONITOR(fmonitor));

    Q_UNUSED(file)
    Q_UNUSED(flags)
    Q_UNUSED(error)
    Q_UNUSED(cancellable)

    return fmonitor;
}

gboolean vfs_favorites_file_delete (GFile* file, GCancellable* cancellable, GError** error)
{
    QString uri = nullptr;
    GFileIface *iface = nullptr;

    g_return_val_if_fail (G_IS_FILE (file), FALSE);

    if (g_cancellable_set_error_if_cancelled (cancellable, error)) {
        return FALSE;
    }

    iface = G_FILE_GET_IFACE (file);

    uri = g_file_get_uri (file);

    QString errorStr = QObject::tr("Operation not supported");

    if (iface->delete_file == NULL) {
        g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED, errorStr.toUtf8().constData());
        return FALSE;
    }

    Peony::BookMarkManager::getInstance()->removeBookMark(uri);

    return TRUE;
}

char* vfs_favorites_file_get_schema (GFile* file)
{
    Q_UNUSED(file);
    return g_strdup("favorite");
}

GFile* vfs_favorites_file_get_parent (GFile* file)
{
    Q_UNUSED(file);
    return g_file_new_for_uri ("favorite:///");
}

char* vfs_favorites_file_get_basename (GFile* file)
{
    QUrl url = QString(vfs_favorites_file_get_uri (file));

    return g_strdup (url.path().toUtf8().constData());
}
