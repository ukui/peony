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

#include "file-info-job.h"

#include "file-info.h"
#include "file-meta-info.h"

#include "file-info-manager.h"
#include "file-label-model.h"

#include <gio/gdesktopappinfo.h>

#include <QDebug>
#include <QDateTime>
#include <QIcon>
#include <QUrl>
#include <QLocale>

using namespace Peony;

FileInfoJob::FileInfoJob(std::shared_ptr<FileInfo> info, QObject *parent) : QObject(parent)
{
    m_info = info;
    //connect(m_info.get(), &FileInfo::updated, this, &FileInfoJob::infoUpdated);

    m_cancellable = g_cancellable_new();
}

FileInfoJob::FileInfoJob(const QString &uri, QObject *parent) : QObject (parent)
{
    auto info = FileInfo::fromUri(uri);
    m_info = info;
    //connect(m_info.get(), &FileInfo::updated, this, &FileInfoJob::infoUpdated);

    m_cancellable = g_cancellable_new();
}

FileInfoJob::~FileInfoJob()
{
    g_object_unref(m_cancellable);
}

void FileInfoJob::cancel()
{
    //NOTE: do not use same cancellble for cancelling, otherwise all job might be cancelled.
    g_cancellable_cancel(m_cancellable);
    g_object_unref(m_cancellable);
    m_cancellable = g_cancellable_new();
}

bool FileInfoJob::querySync()
{
    FileInfo *info = nullptr;
    if (auto data = m_info.get()) {
        info = data;
    } else {
        if (m_auto_delete)
            deleteLater();
        return false;
    }
    GError *err = nullptr;

    auto _info = g_file_query_info(info->m_file,
                                   "standard::*," "time::*," "access::*," "mountable::*," "metadata::*," G_FILE_ATTRIBUTE_ID_FILE,
                                   G_FILE_QUERY_INFO_NONE,
                                   nullptr,
                                   &err);

    if (err) {
        qDebug()<<err->code<<err->message;
        g_error_free(err);
        if (m_auto_delete)
            deleteLater();
        return false;
    }

    refreshInfoContents(_info);
    g_object_unref(_info);
    if (m_auto_delete)
        deleteLater();

    infoUpdated();

    return true;
}

GAsyncReadyCallback FileInfoJob::query_info_async_callback(GFile *file, GAsyncResult *res, FileInfoJob *thisJob)
{
    //qDebug()<<"query_info_async_callback"<<thisJob->m_info->uri();

    GError *err = nullptr;

    GFileInfo *_info = g_file_query_info_finish(file,
                       res,
                       &err);

    if (_info != nullptr) {
        thisJob->refreshInfoContents(_info);
        g_object_unref(_info);
        Q_EMIT thisJob->queryAsyncFinished(true);
        Q_EMIT thisJob->infoUpdated();
    }
    else {
        if (err) {
            qDebug()<<err->code<<err->message;
            g_error_free(err);
        }
        Q_EMIT thisJob->queryAsyncFinished(false);
        return nullptr;
    }

    return nullptr;
}

void FileInfoJob::queryAsync()
{
    FileInfo *info = nullptr;
    if (auto data = m_info) {
        info = data.get();
        cancel();
    } else {
        Q_EMIT queryAsyncFinished(false);
        return;
    }
    g_file_query_info_async(info->m_file,
                            "standard::*," "time::*," "access::*," "mountable::*," "metadata::*," G_FILE_ATTRIBUTE_ID_FILE,
                            G_FILE_QUERY_INFO_NONE,
                            G_PRIORITY_DEFAULT,
                            m_cancellable,
                            GAsyncReadyCallback(query_info_async_callback),
                            this);

    if (m_auto_delete)
        connect(this, &FileInfoJob::queryAsyncFinished, this, &FileInfoJob::deleteLater, Qt::QueuedConnection);
}

void FileInfoJob::refreshInfoContents(GFileInfo *new_info)
{
//    if (!m_info->m_mutex.tryLock(300))
//        return;

    FileInfo *info = nullptr;
    if (auto data = m_info) {
        info = data.get();
    } else {
        return;
    }
    GFileType type = g_file_info_get_file_type (new_info);
    switch (type) {
    case G_FILE_TYPE_DIRECTORY:
        //qDebug()<<"dir";
        info->m_is_dir = true;
        break;
    case G_FILE_TYPE_MOUNTABLE:
        //qDebug()<<"mountable";
        info->m_is_volume = true;
        break;
    default:
        break;
    }

    info->m_is_symbol_link = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK);
    if (g_file_info_has_attribute(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ)) {
        info->m_can_read = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
    } else {
        // we assume an unknow access file is readable.
        info->m_can_read = true;
    }
    info->m_can_write = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
    info->m_can_excute = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE);
    info->m_can_delete = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_DELETE);
    info->m_can_trash = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_TRASH);
    info->m_can_rename = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_RENAME);

    info->m_can_mount = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_MOUNT);
    info->m_can_unmount = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT);
    info->m_can_eject = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_EJECT);
    info->m_can_start = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_START);
    info->m_can_stop = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_STOP);

    info->m_is_virtual = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_STANDARD_IS_VIRTUAL);
    if(g_file_info_has_attribute(new_info,G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE_FILE))
        info->m_unix_device_file = g_file_info_get_attribute_string(new_info,G_FILE_ATTRIBUTE_MOUNTABLE_UNIX_DEVICE_FILE);

    info->m_display_name = QString (g_file_info_get_display_name(new_info));
    GIcon *g_icon = g_file_info_get_icon (new_info);
    if (G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        if (icon_names) {
            auto p = icon_names;
            while (*p) {
                QIcon icon = QIcon::fromTheme(*p);
                if (!icon.isNull()) {
                    info->m_icon_name = QString (*p);
                    break;
                } else {
                    p++;
                }
            }
        }
        //g_object_unref(g_icon);
    }

    //qDebug()<<m_display_name<<m_icon_name;
    GIcon *g_symbolic_icon = g_file_info_get_symbolic_icon (new_info);
    if (G_IS_ICON(g_symbolic_icon)) {
        const gchar* const* symbolic_icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_symbolic_icon));
        if (symbolic_icon_names)
            info->m_symbolic_icon_name = QString (*symbolic_icon_names);
        //g_object_unref(g_symbolic_icon);
    }

    char* name = g_file_get_path(info->m_file);
    info->m_path = name;
    if (NULL != name) {
        g_free(name);
    }
    info->m_file_id = g_file_info_get_attribute_string(new_info, G_FILE_ATTRIBUTE_ID_FILE);

    info->m_content_type = g_file_info_get_content_type (new_info);
    if (info->m_content_type == nullptr) {
        if (g_file_info_has_attribute(new_info, "standard::fast-content-type")) {
            info->m_content_type = g_file_info_get_attribute_string(new_info, "standard::fast-content-type");
        }
    }

    info->m_size = g_file_info_get_attribute_uint64(new_info, G_FILE_ATTRIBUTE_STANDARD_SIZE);
    info->m_modified_time = g_file_info_get_attribute_uint64(new_info, G_FILE_ATTRIBUTE_TIME_MODIFIED);
    info->m_access_time = g_file_info_get_attribute_uint64(new_info, G_FILE_ATTRIBUTE_TIME_ACCESS);

    info->m_mime_type_string = info->m_content_type;
    if (!info->m_mime_type_string.isEmpty()) {
        char *content_type = g_content_type_get_description (info->m_mime_type_string.toUtf8().constData());
        info->m_file_type = content_type;
        g_free (content_type);
        content_type = nullptr;
    }

    char *size_full = g_format_size_full(info->m_size, G_FORMAT_SIZE_DEFAULT);
    info->m_file_size = size_full;
    g_free(size_full);

    QDateTime date = QDateTime::fromMSecsSinceEpoch(info->m_modified_time*1000);
    info->m_modified_date = date.toString(Qt::SystemLocaleShortDate);

    date = QDateTime::fromMSecsSinceEpoch(info->m_access_time*1000);
    info->m_access_date = date.toString(Qt::SystemLocaleShortDate);

    m_info->m_meta_info = FileMetaInfo::fromGFileInfo(m_info->uri(), new_info);
    // update peony qt color list after meta info updated.
    m_info->m_colors = FileLabelModel::getGlobalModel()->getFileColors(m_info->uri());

    if (info->isDesktopFile()) {
        info->m_desktop_name = info->displayName();
        QUrl url = info->uri();
        GDesktopAppInfo *desktop_info = g_desktop_app_info_new_from_filename(url.path().toUtf8());
        if (!desktop_info) {
            m_info->m_mutex.unlock();
            info->updated();
            return;
        }
#if GLIB_CHECK_VERSION(2, 56, 0)
        auto string = g_desktop_app_info_get_locale_string(desktop_info, "Name");
#else
        //FIXME: should handle locale?
        //change "Name" to QLocale::system().name(),
        //try to fix Qt5.6 untranslated desktop file issue
        auto key = "Name[" +  QLocale::system().name() + "]";
        auto string = g_desktop_app_info_get_string(desktop_info, key.toUtf8().constData());
#endif
        qDebug() << "get name string:"<<string <<info->uri()<<info->displayName();
        if (string) {
            info->m_display_name = string;
            g_free(string);
        } else {
            QString path = "/usr/share/applications/" + info->displayName();
            auto name = getAppName(path);
            if (name.length() > 0)
                info->m_display_name = name;
            else
            {
                string = g_desktop_app_info_get_string(desktop_info, "Name");
                if (string) {
                    info->m_display_name = string;
                    g_free(string);
                }
            }
        }
        g_object_unref(desktop_info);
    }

    info->m_target_uri = g_file_info_get_attribute_string(new_info, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
    info->m_symlink_target = g_file_info_get_symlink_target(new_info);

    Q_EMIT info->updated();
//    m_info->m_mutex.unlock();
}

QString FileInfoJob::getAppName(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);

    char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Name", nullptr, nullptr);
    QString namestr=QString::fromLocal8Bit(name);

    g_key_file_free(keyfile);
    return namestr;
}
