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

#include "file-utils.h"
#include "favorite-vfs-file.h"
#include "favorite-vfs-file-monitor.h"
#include "favorite-vfs-file-enumerator.h"

#include <syslog.h>

#include <QUrl>
#include <QDebug>
#include <bookmark-manager.h>

static void vfs_favorite_file_g_file_iface_init(GFileIface *iface);

GFile*              vfs_favorite_file_dup(GFile *file);
char*               vfs_favorite_file_get_uri(GFile *file);
char*               vfs_favorite_file_get_path(GFile *file);
gboolean            vfs_favorite_file_is_native(GFile *file);
GFile*              vfs_favorite_file_get_parent(GFile *file);
char*               vfs_favorite_file_get_schema (GFile* file);
void                vfs_favorite_file_dispose(GObject *object);
char*               vfs_favorite_file_get_basename (GFile* file);
GFile*              vfs_favorite_file_new_for_uri(const char *uri);
gboolean            vfs_favorite_file_is_equal(GFile *file1, GFile *file2);
char*               vfs_favorite_file_get_relative_path (GFile* parent, GFile* descendant);
GFile*              vfs_favorite_file_resolve_relative_path(GFile *file, const char *relative_path);
GFileInputStream*   vfs_favorite_file_read_fn(GFile* file, GCancellable* cancellable, GError** error);
gboolean            vfs_favorite_file_delete (GFile* file, GCancellable* cancellable, GError** error);
GFileIOStream *     vfs_favorite_file_open_readwrite(GFile* file, GCancellable* cancellable, GError** error);
gboolean            vfs_favorite_file_make_directory(GFile* file, GCancellable* cancellable, GError** error);
GMount *            vfs_favorite_file_find_enclosing_mount(GFile* file, GCancellable* cancellable, GError** error);
GFileOutputStream*  vfs_favorite_file_create(GFile* file, GFileCreateFlags flags, GCancellable* cancellable, GError** error);
gboolean            vfs_favorite_file_make_symbolic_link(GFile* file, const char* svalue, GCancellable* cancellable, GError** error);
GFileMonitor*       vfs_favorite_file_monitor_directory (GFile* file, GFileMonitorFlags flags, GCancellable* cancellable, GError** error);
GFile*              vfs_favorite_file_set_display_name (GFile* file, const gchar* display_name, GCancellable* cancellable, GError** error);
GFileInfo*          vfs_favorite_file_query_filesystem_info(GFile* file, const char* attributes, GCancellable* cancellable, GError** error);
GFileInfo*          vfs_favorite_file_query_info(GFile *file, const char *attributes, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error);
GFileEnumerator*    vfs_favorite_file_enumerate_children(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error);
GFileOutputStream*  vfs_favorite_file_replace(GFile* file, const char* etag, gboolean make_backup, GFileCreateFlags flags, GCancellable* cancellable, GError** error);
GFileEnumerator*    vfs_favorite_file_enumerate_children_internal(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error);
gboolean            vfs_favorite_file_copy(GFile* source, GFile* destination, GFileCopyFlags flags, GCancellable* cancellable, GFileProgressCallback pcallback, gpointer pcallbackdata, GError** error);
gboolean            vfs_favorite_file_move(GFile* source, GFile* destination, GFileCopyFlags flags, GCancellable* cancellable, GFileProgressCallback progress_callback, gpointer progress, GError** error);

static char*        vfs_favorite_file_get_target_file (GFile* file);

G_DEFINE_TYPE_EXTENDED(FavoriteVFSFile, vfs_favorite_file, G_TYPE_OBJECT, 0, G_ADD_PRIVATE(FavoriteVFSFile) G_IMPLEMENT_INTERFACE(G_TYPE_FILE, vfs_favorite_file_g_file_iface_init));

static void vfs_favorite_file_init(FavoriteVFSFile* self)
{
    g_return_if_fail(VFS_IS_FAVORITES_FILE(self));
    FavoriteVFSFilePrivate *priv = (FavoriteVFSFilePrivate*)vfs_favorite_file_get_instance_private(self);
    self->priv = priv;

    self->priv->uri = nullptr;
}

static void vfs_favorite_file_class_init (FavoriteVFSFileClass* kclass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS (kclass);

    gobject_class->dispose = vfs_favorite_file_dispose;
}

void vfs_favorite_file_dispose(GObject *object)
{
    g_return_if_fail(VFS_IS_FAVORITES_FILE(object));

    auto vfsfile = VFS_FAVORITES_FILE (object);

    if (G_IS_FILE_MONITOR(vfsfile->priv->fileMonitor)) {
        g_file_monitor_cancel (vfsfile->priv->fileMonitor);
        vfsfile->priv->fileMonitor = nullptr;
    }

    if (vfsfile->priv->uri) {
        g_free(vfsfile->priv->uri);
        vfsfile->priv->uri = nullptr;
    }
}

static void vfs_favorite_file_g_file_iface_init(GFileIface *iface)
{
    iface->dup                      = vfs_favorite_file_dup;
    iface->move                     = vfs_favorite_file_move;
    iface->copy                     = vfs_favorite_file_copy;
    iface->trash                    = vfs_favorite_file_delete;
    iface->read_fn                  = vfs_favorite_file_read_fn;
    iface->equal                    = vfs_favorite_file_is_equal;
    iface->create                   = vfs_favorite_file_create;
    iface->get_uri                  = vfs_favorite_file_get_uri;
    iface->get_path                 = vfs_favorite_file_get_path;
    iface->is_native                = vfs_favorite_file_is_native;
    iface->get_parent               = vfs_favorite_file_get_parent;
    iface->query_info               = vfs_favorite_file_query_info;
    iface->delete_file              = vfs_favorite_file_delete;
    iface->get_uri_scheme           = vfs_favorite_file_get_schema;
    iface->get_basename             = vfs_favorite_file_get_basename;
    iface->open_readwrite           = vfs_favorite_file_open_readwrite;
    iface->make_directory           = vfs_favorite_file_make_directory;
    iface->set_display_name         = vfs_favorite_file_set_display_name;
    iface->get_relative_path        = vfs_favorite_file_get_relative_path;
    iface->monitor_dir              = vfs_favorite_file_monitor_directory;
    iface->make_symbolic_link       = vfs_favorite_file_make_symbolic_link;
    iface->enumerate_children       = vfs_favorite_file_enumerate_children;
    iface->find_enclosing_mount     = vfs_favorite_file_find_enclosing_mount;
    iface->query_filesystem_info    = vfs_favorite_file_query_filesystem_info;
    iface->resolve_relative_path    = vfs_favorite_file_resolve_relative_path;

}

char* vfs_favorite_file_get_uri(GFile *file)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), g_strdup("favorite:///"));

    auto vfsfile = VFS_FAVORITES_FILE (file);

    return g_strdup(vfsfile->priv->uri);
}

gboolean vfs_favorite_file_is_native(GFile *file)
{
    Q_UNUSED(file);

    return FALSE;
}

GFile* vfs_favorite_file_dup (GFile *file)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), g_file_new_for_uri("favorite:///"));
    auto vfs_file = VFS_FAVORITES_FILE(file);
    auto dup = VFS_FAVORITES_FILE(g_object_new(VFS_TYPE_FAVORITE_FILE, nullptr));
    dup->priv->uri = g_strdup(vfs_file->priv->uri);

    return G_FILE(dup);
}

GFileEnumerator* vfs_favorite_file_enumerate_children_internal(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), nullptr);
    Q_UNUSED(flags)
    Q_UNUSED(error)
    Q_UNUSED(attribute)
    Q_UNUSED(cancellable)

    QString uri = g_file_get_uri(file);
    if ("favorite:///" == uri) {
        return G_FILE_ENUMERATOR(g_object_new(VFS_TYPE_FAVORITES_FILE_ENUMERATOR, "container", file, nullptr));
    }

    return nullptr;
}

GFile* vfs_favorite_file_new_for_uri(const char *uri)
{
    auto vfsfile = VFS_FAVORITES_FILE(g_object_new(VFS_TYPE_FAVORITE_FILE, nullptr));

    // fix favorite:///xxx/xxx/xxx?schema=file/sss.txt
    QString quri = uri;
    QStringList path1 = quri.split("/");
    QStringList path2;
    QString schemaInfo;

    for (auto i : path1) {
        if (!i.contains("?schema=")) {
            path2 << i;
            continue;
        }
        QStringList tmp = i.split("?schema=");
        if (tmp.size() >= 2) {
            path2 << tmp[0];
            schemaInfo = "?schema=" + tmp[1];
        }
    }

    quri = path2.join("/") + schemaInfo;
    vfsfile->priv->uri = g_strdup(quri.toUtf8().constData());

    return G_FILE(vfsfile);
}

char* vfs_favorite_file_get_path(GFile *file)
{
    return nullptr;
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), nullptr);

    QUrl url = QString(vfs_favorite_file_get_uri (file));

    return g_strdup (url.path().toUtf8().constData());
}

GFile* vfs_favorite_file_resolve_relative_path(GFile *file, const char *relativepath)
{
    Q_UNUSED(file);
    Q_UNUSED(relativepath);

    if (relativepath) {
        return g_file_new_for_uri(relativepath);
    }

    return g_file_new_for_uri("favorite:///");;
}

GFileInfo* vfs_favorite_file_query_info(GFile *file, const char *attributes, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), nullptr);

    auto vfsfile = VFS_FAVORITES_FILE(file);

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
            if (!info)
                info = g_file_info_new();
            g_file_info_set_attribute_string(info, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI, trueUri.toUtf8().constData());
        } else {
            info = g_file_info_new();
        }
        g_object_unref(file);
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
        g_file_info_set_display_name(info, name.toUtf8().constData());
    }

    g_file_info_set_name (info, vfsfile->priv->uri);
    g_file_info_set_attribute_boolean(info, G_FILE_ATTRIBUTE_STANDARD_IS_VIRTUAL, TRUE);
    g_file_info_set_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_DELETE, TRUE);
    g_file_info_set_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_TRASH, FALSE);
    g_file_info_set_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE, FALSE);
    g_file_info_set_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_RENAME, FALSE);


    Q_UNUSED(flags)
    Q_UNUSED(error)
    Q_UNUSED(attributes)
    Q_UNUSED(cancellable)

    return info;
}

GFileEnumerator* vfs_favorite_file_enumerate_children(GFile *file, const char *attribute, GFileQueryInfoFlags flags, GCancellable *cancellable, GError **error)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), nullptr);

    return vfs_favorite_file_enumerate_children_internal(file, attribute, flags, cancellable, error);
}

GFileMonitor* vfs_favorite_file_monitor_directory (GFile* file, GFileMonitorFlags flags, GCancellable* cancellable, GError** error)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), nullptr);

    FavoriteVFSFilePrivate* priv = VFS_FAVORITE_FILE((FavoriteVFSFile*)file)->priv;

    priv->fileMonitor = (GFileMonitor*) g_object_new (VFS_TYPE_FAVORITE_FILE_MONITOR, nullptr);

    vfs_favorite_file_monitor_dir (VFS_FAVORITE_FILE_MONITOR(priv->fileMonitor));

    Q_UNUSED(file)
    Q_UNUSED(flags)
    Q_UNUSED(error)
    Q_UNUSED(cancellable)

    return priv->fileMonitor;
}

gboolean vfs_favorite_file_delete (GFile* file, GCancellable* cancellable, GError** error)
{
    QString uri = nullptr;
    GFileIface *iface = nullptr;

    g_return_val_if_fail (VFS_IS_FAVORITES_FILE(file), FALSE);

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

char* vfs_favorite_file_get_schema (GFile* file)
{
    Q_UNUSED(file);
    return g_strdup("favorite");
}

GFile* vfs_favorite_file_get_parent (GFile* file)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), nullptr);

    Q_UNUSED(file);

    QString uri = g_file_get_uri(file);

    if ("favorite:///" == uri) {
        return nullptr;
    }

    return g_file_new_for_uri("favorite:///");
}

char* vfs_favorite_file_get_basename (GFile* file)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file), nullptr);

    QString url = QString(vfs_favorite_file_get_uri (file));
    QString baseName = url.split("/").takeLast();

    return g_strdup (baseName.toUtf8().constData());
}

gboolean vfs_favorite_file_is_equal(GFile *file1, GFile* file2)
{
    g_return_val_if_fail(VFS_IS_FAVORITES_FILE(file1) || VFS_IS_FAVORITES_FILE(file2), false);

    char* f1 = g_file_get_uri(file1);
    char* f2 = g_file_get_uri(file2);
    int ret = g_strcmp0(f1, f2);

    if (nullptr != f1) {
        g_free(f1);
    }

    if (nullptr != f2) {
        g_free(f2);
    }

    return ret == 0;
}

GFileOutputStream* vfs_favorite_file_create(GFile* file, GFileCreateFlags flags, GCancellable* cancellable, GError** error)
{
    if (nullptr == file) {
        QString str = QObject::tr("Incorrect source file");
        *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_INVALID_FILENAME, "%s\n", str.toUtf8().constData());
    }
    GFileOutputStream*      ret = nullptr;

    gchar*                  sourceUri = vfs_favorite_file_get_target_file (file);

    if (nullptr != sourceUri && 0 == strncmp("file://", sourceUri, 7)) {
        GFile* srcFile = g_file_new_for_uri(sourceUri);
        ret = g_file_create(srcFile, flags, cancellable, error);
    } else {
        QString str = QObject::tr("Unable to create virtual file");
        *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());
    }

    return ret;
}

gboolean vfs_favorite_file_make_directory(GFile* file, GCancellable* cancellable, GError** error)
{
    Q_UNUSED(file);
    Q_UNUSED(error);
    Q_UNUSED(cancellable);

    QString str = QObject::tr("The virtual file system does not support folder creation");

    *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());

    return FALSE;
}

gboolean vfs_favorite_file_make_symbolic_link(GFile* file, const char* svalue, GCancellable* cancellable, GError** error)
{
    Q_UNUSED(file);
    Q_UNUSED(error);
    Q_UNUSED(svalue);
    Q_UNUSED(cancellable);

    // fixme:// Do not implement
    QString str = QObject::tr("Unable to create linker for virtual file");

    *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());

    return FALSE;
}

gboolean vfs_favorite_file_move(GFile* source, GFile* destination, GFileCopyFlags flags, GCancellable* cancellable, GFileProgressCallback progress_callback, gpointer progress, GError** error)
{
    if (nullptr == source || nullptr == destination) {
        QString str = QObject::tr("Incorrect source or destination file");
        *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_INVALID_FILENAME, "%s\n", str.toUtf8().constData());
    }

    gboolean                ret = FALSE;
    gchar*                  destSUri = g_file_get_uri(destination);
    gchar*                  sourceUri = vfs_favorite_file_get_target_file (source);
    gchar*                  destUri = vfs_favorite_file_get_target_file (destination);

    if (nullptr != sourceUri && nullptr != destUri && 0 == strncmp("favorite://", destSUri, 11) && 0 == strncmp("file://", sourceUri, 7)) {
        GFile* destFile = g_file_new_for_uri(destUri);
        GFile* srcFile = g_file_new_for_uri(sourceUri);

        ret = g_file_move(srcFile, destFile, flags, cancellable, progress_callback, progress, error);

        if (nullptr != destFile) g_object_unref(destFile);
        if (nullptr != srcFile)  g_object_unref(srcFile);
    } else {
        QString str = QObject::tr("Unable to move the virtual file to the file system");
        *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());
    }

    if (nullptr != destSUri)    g_free(destSUri);
    if (nullptr != destUri)     g_free (destUri);
    if (nullptr != sourceUri)   g_free (sourceUri);

    return ret;
}

gboolean vfs_favorite_file_copy(GFile* source, GFile* destination, GFileCopyFlags flags, GCancellable* cancellable, GFileProgressCallback pcallback, gpointer pcallbackdata, GError** error)
{
    if (nullptr == source || nullptr == destination) {
        QString str = QObject::tr("Incorrect source or destination file");
        *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_INVALID_FILENAME, "%s\n", str.toUtf8().constData());
    }

    gboolean                ret = FALSE;
    gchar*                  destSUri = g_file_get_uri(destination);
    gchar*                  sourceUri = vfs_favorite_file_get_target_file (source);
    gchar*                  destUri = vfs_favorite_file_get_target_file (destination);

    if (nullptr != sourceUri && nullptr != destUri && 0 == strncmp("favorite://", destSUri, 11) && 0 == strncmp("file://", sourceUri, 7)) {
        GFile* destFile = g_file_new_for_uri(destUri);
        GFile* srcFile = g_file_new_for_uri(sourceUri);

        ret = g_file_copy(srcFile, destFile, flags, cancellable, pcallback, pcallbackdata, error);

        if (nullptr != destFile) g_object_unref(destFile);
        if (nullptr != srcFile)  g_object_unref(srcFile);
    } else {
        QString str = QObject::tr("Unable to copy the virtual file to the file system");
        *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());
    }

    if (nullptr != destSUri)    g_free(destSUri);
    if (nullptr != destUri)     g_free (destUri);
    if (nullptr != sourceUri)   g_free (sourceUri);

    return ret;
}

GFile* vfs_favorite_file_set_display_name (GFile* file, const gchar* display_name, GCancellable* cancellable, GError** error)
{
    Q_UNUSED(error);
    Q_UNUSED(cancellable);
    Q_UNUSED(display_name);

    return file;
}

GFileOutputStream* vfs_favorite_file_replace(GFile* file, const char* etag, gboolean make_backup, GFileCreateFlags flags, GCancellable* cancellable, GError** error)
{
    QString str = QObject::tr("Virtual file directories do not support move and copy operations");

    *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());

    return FALSE;
}

GFileInfo* vfs_favorite_file_query_filesystem_info(GFile* file, const char* attributes, GCancellable* cancellable, GError** error)
{
    return vfs_favorite_file_query_info(file, attributes, G_FILE_QUERY_INFO_NONE, cancellable, error);
}

GMount* vfs_favorite_file_find_enclosing_mount(GFile* file, GCancellable* cancellable, GError** error)
{
    return nullptr;
}

char* vfs_favorite_file_get_relative_path (GFile* parent, GFile* descendant)
{
    return nullptr;
}

GFileInputStream* vfs_favorite_file_read_fn(GFile* file, GCancellable* cancellable, GError** error)
{
    QString str = QObject::tr("The virtual file system cannot be opened");

    *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());
    return nullptr;
}

GFileIOStream* vfs_favorite_file_open_readwrite(GFile* file, GCancellable* cancellable, GError** error)
{
    QString str = QObject::tr("The virtual file system cannot be opened");

    *error = g_error_new(G_FILE_ERROR_FAILED, G_IO_ERROR_NOT_SUPPORTED, "%s\n", str.toUtf8().constData());
    return nullptr;
}

gboolean vfs_favorite_file_is_exist(const char *uri)
{
    gboolean        ret = FALSE;
    GFile*          file = g_file_new_for_uri(uri);
    GFileInfo*      fileInfo = nullptr;

    if (nullptr != file) {
        fileInfo = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
        if (nullptr != fileInfo) {
            gchar* targetUri = g_file_info_get_attribute_as_string(fileInfo, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
            ret = Peony::FileUtils::isFileExsit(targetUri);
            g_free(targetUri);
            g_object_unref(fileInfo);
        }
        g_object_unref(file);
    }

    return ret;
}

static char* vfs_favorite_file_get_target_file (GFile* file)
{
    if (nullptr == file) {
        return nullptr;
    }

    gchar*          uri = nullptr;
    GFileInfo*      fileInfo = nullptr;
    gchar*          targetUri = nullptr;

    uri = g_file_get_uri(file);
    if (nullptr != uri && 0 != strncmp("favorite://", uri, 11)) {
        return uri;
    }

    if (nullptr != uri) g_free(uri);

    fileInfo = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
    if (nullptr != fileInfo) {
        targetUri = g_file_info_get_attribute_as_string(fileInfo, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
        g_object_unref(fileInfo);
    }

    return targetUri;
}
