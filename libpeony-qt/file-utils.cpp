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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "file-utils.h"
#include <QUrl>
#include <QFileInfo>

#include <QStandardPaths>
#include <QDir>

using namespace Peony;

FileUtils::FileUtils()
{

}

QString FileUtils::getQStringFromCString(char *c_string, bool free)
{
    QString value = c_string;
    if (free)
        g_free(c_string);
    return value;
}

QString FileUtils::getFileUri(const GFileWrapperPtr &file)
{
    if (!G_IS_FILE (file.get()->get())) {
        return nullptr;
    }
    char *uri = g_file_get_uri(file.get()->get());
    QString urlString = QString(uri);
    QUrl url = urlString;
    g_free(uri);
    char *path = g_file_get_path(file.get()->get());
    if (path && !url.isLocalFile()) {
        QString urlString = QString("file://%1").arg(path);
        g_free(path);
        return urlString;
    }

    return urlString;
}

QString FileUtils::getFileBaseName(const GFileWrapperPtr &file)
{
    char *basename = g_file_get_basename(file.get()->get());
    return getQStringFromCString(basename);
}

QString FileUtils::getUriBaseName(const QString &uri)
{
    QUrl url = uri;
    return url.fileName();
}

GFileWrapperPtr FileUtils::getFileParent(const GFileWrapperPtr &file)
{
    return wrapGFile(g_file_get_parent(file.get()->get()));
}

QString FileUtils::getRelativePath(const GFileWrapperPtr &dir, const GFileWrapperPtr &file)
{
    char *relative_path = g_file_get_relative_path(dir.get()->get(), file.get()->get());
    return getQStringFromCString(relative_path);
}

GFileWrapperPtr FileUtils::resolveRelativePath(const GFileWrapperPtr &dir, const QString &relativePath)
{
    return wrapGFile(g_file_resolve_relative_path(dir.get()->get(),
                     relativePath.toUtf8().constData()));
}

bool FileUtils::getFileHasChildren(const GFileWrapperPtr &file)
{
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY || type == G_FILE_TYPE_MOUNTABLE;
}

bool FileUtils::getFileIsFolder(const GFileWrapperPtr &file)
{
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY;
}

bool FileUtils::getFileIsFolder(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_DIRECTORY;
}

bool FileUtils::getFileIsSymbolicLink(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    GFileType type = g_file_query_file_type(file.get()->get(),
                                            G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                            nullptr);
    return type == G_FILE_TYPE_SYMBOLIC_LINK;
}

QStringList FileUtils::getChildrenUris(const QString &directoryUri)
{
    QStringList uris;

    GError *err = nullptr;
    GFile *top = g_file_new_for_uri(directoryUri.toUtf8().constData());
    GFileEnumerator *e = g_file_enumerate_children(top, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                   nullptr, &err);

    if (err) {
        g_error_free(err);
    }
    g_object_unref(top);
    if (!e)
        return uris;

    auto child_info = g_file_enumerator_next_file(e, nullptr, nullptr);
    while (child_info) {
        auto child = g_file_enumerator_get_child(e, child_info);

        auto uri = g_file_get_uri(child);
        auto path = g_file_get_path(child);
        QString urlString = uri;
        QUrl url = urlString;
        if (path && !url.isLocalFile()) {
            urlString = QString("file://%1").arg(path);
            g_free(path);
        } else {
            urlString = uri;
        }

        uris<<urlString;
        g_free(uri);
        g_object_unref(child);
        g_object_unref(child_info);
        child_info = g_file_enumerator_next_file(e, nullptr, nullptr);
    }

    g_file_enumerator_close(e, nullptr, nullptr);
    g_object_unref(e);

    return uris;
}

QString FileUtils::getNonSuffixedBaseNameFromUri(const QString &uri)
{
    QUrl url = uri;
    if (url.isLocalFile()) {
        QFileInfo info(url.path());
        return info.baseName();
    } else {
        QString suffixedBaseName = url.fileName();
        int index = suffixedBaseName.lastIndexOf(".");
        if (index != -1) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            QString suffix = suffixedBaseName.chopped(suffixedBaseName.size() - index);
            if (suffix == ".gz" || suffix == ".xz" || suffix == ".bz"
                    || suffix == ".bz2" || suffix == ".Z" ||
                    suffix == ".sit") {
                int secondIndex = suffixedBaseName.lastIndexOf('.');
                suffixedBaseName.chop(suffixedBaseName.size() - secondIndex);
            }
#else
            suffixedBaseName.chop(suffixedBaseName.size() - index);
#endif
        }
        return suffixedBaseName;
    }
}

QString FileUtils::getFileDisplayName(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    if (!info.get()->get())
        return nullptr;
    return g_file_info_get_display_name(info.get()->get());
}

QString FileUtils::getFileIconName(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_ICON,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    GIcon *g_icon = g_file_info_get_icon (info.get()->get());
    QString icon_name;
    //do not unref the GIcon from info.
    if (G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        if (icon_names)
            icon_name = QString (*icon_names);
    }
    return icon_name;
}

GErrorWrapperPtr FileUtils::getEnumerateError(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    GError *err = nullptr;
    auto enumerator = wrapGFileEnumerator(g_file_enumerate_children(file.get()->get(),
                                          G_FILE_ATTRIBUTE_STANDARD_NAME,
                                          G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                          nullptr,
                                          &err));
    if (err) {
        return GErrorWrapper::wrapFrom(err);
    }
    return nullptr;
}

QString FileUtils::getTargetUri(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_TARGET_URI,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    return g_file_info_get_attribute_string(info.get()->get(),
                                            G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
}

bool FileUtils::isMountPoint(const QString &uri)
{
    bool flag = false;                      // The uri is a mount point

    GFile* file = g_file_new_for_uri(uri.toUtf8().constData());

    GList* mounts = nullptr;
    GVolumeMonitor* vm = g_volume_monitor_get();
    if (nullptr != vm) {
        mounts = g_volume_monitor_get_mounts(vm);
        if (nullptr != mounts) {
            for (GList* l = mounts; nullptr != l; l = l->next) {
                GMount* m = (GMount*)l->data;
                GFile* f = g_mount_get_root(m);
                if (g_file_equal(file, f)) {
                    flag = true;
                    g_object_unref(f);
                    break;
                }
                g_object_unref(f);
            }
        }
    }

    if (nullptr != vm) {
        g_object_unref(vm);
    }

    if (nullptr != mounts) {
        g_list_free(mounts);
    }

    if (nullptr != file) {
        g_object_unref(file);
    }

    return flag;
}

bool FileUtils::stringStartWithChinese(const QString &string)
{
    if (string.isEmpty())
        return false;

    auto firstStrUnicode = string.at(0).unicode();
    return (firstStrUnicode <=0x9FA5 && firstStrUnicode >= 0x4E00);
}

bool FileUtils::stringLesserThan(const QString &left, const QString &right)
{
    bool leftStartWithChinese = stringStartWithChinese(left);
    bool rightStartWithChinese = stringStartWithChinese(right);
    if (!(!leftStartWithChinese && !rightStartWithChinese)) {
        return leftStartWithChinese;
    }
    return left.toLower() < right.toLower();
}

const QString FileUtils::getParentUri(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto parent = getFileParent(file);
    auto parentUri = getFileUri(parent);
    return parentUri == uri? nullptr: parentUri;
}

const QString FileUtils::getOriginalUri(const QString &uri)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto originalUri = getFileUri(file);
    return originalUri;
}

bool FileUtils::isFileExsit(const QString &uri)
{
    bool exist = false;
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    exist = g_file_query_exists(file, nullptr);
    g_object_unref(file);
    return exist;
}

const QStringList FileUtils::toDisplayUris(const QStringList &args)
{
    QStringList uris;
    for (QString path : args) {
        QUrl url = path;
        if (url.scheme().isEmpty()) {
            auto current_dir = g_get_current_dir();
            QDir currentDir = QDir(current_dir);
            g_free(current_dir);
            currentDir.cd(path);
            auto absPath = currentDir.absoluteFilePath(path);
            path = absPath;
            url = QUrl::fromLocalFile(absPath);
        }
        uris<<url.toDisplayString();
    }
    return uris;
}

bool FileUtils::isMountRoot(const QString &uri)
{
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        "unix::is-mountpoint",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);
    if (info) {
        bool isMount = g_file_info_get_attribute_boolean(info, "unix::is-mountpoint");
        g_object_unref(info);
        return isMount;
    }
    return false;
}

bool FileUtils::queryVolumeInfo(const QString &volumeUri, QString &volumeName, QString &unixDeviceName, const QString &volumeDisplayName)
{
    if (!volumeUri.startsWith("computer:///"))
        return false;

    GFile *file = g_file_new_for_uri(volumeUri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        "mountable::*",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);

    if (!info) {
        return false;
    }

    auto displayName = volumeDisplayName;
    if (displayName.isNull()) {
        displayName = getFileDisplayName(volumeUri);
    }

    char *unix_dev_file = g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE_FILE);
    unixDeviceName = unix_dev_file;
    if (unix_dev_file)
        g_free(unix_dev_file);

    auto list = displayName.split(":");
    if (list.count() > 1) {
        auto last = list.last();
        if (last.startsWith(" "))
            last.remove(0, 1);
        volumeName = last;
    } else {
        volumeName = displayName;
    }
    return true;
}

bool FileUtils::isFileDirectory(const QString &uri)
{
    bool isFolder = false;
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    isFolder = g_file_query_file_type(file,
                                      G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                      nullptr) == G_FILE_TYPE_DIRECTORY;
    g_object_unref(file);
    return isFolder;
}

bool FileUtils::isFileUnmountable(const QString &uri)
{
    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
    g_object_unref(file);
    if (info) {
        bool unmountable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT);
        g_object_unref(info);
        return unmountable;
    }
    return false;
}
