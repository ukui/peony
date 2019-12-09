/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "file-rename-operation.h"
#include "file-operation-manager.h"
#include "file-utils.h"
#include <gio/gdesktopappinfo.h>
#include <glib/gprintf.h>
#include <QUrl>

using namespace Peony;

FileRenameOperation::FileRenameOperation(QString uri, QString newName)
{
    m_uri = uri;
    m_new_name = newName;
    QStringList srcUris;
    srcUris<<uri;
    QString destUri = FileUtils::getParentUri(uri);
    if (destUri != nullptr) {
        destUri = destUri + "/" + newName;
    }

    m_info = std::make_shared<FileOperationInfo>(srcUris, destUri, FileOperationInfo::Rename);
}

/*!
 * \brief FileRenameOperation::run
 * \bug
 * how to keep the permission?
 */
void FileRenameOperation::run()
{
    Q_EMIT operationStarted();
    auto file = wrapGFile(g_file_new_for_uri(m_uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                                "*",
                                                G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                getCancellable().get()->get(),
                                                nullptr));

    bool is_local_desktop_file = false;
    QUrl url = m_uri;
    //change the content of .desktop file;
    if (url.isLocalFile()) {
        GDesktopAppInfo *desktop_info = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
        if (G_IS_DESKTOP_APP_INFO(desktop_info)) {
            bool is_executable = g_file_test (url.path().toUtf8().constData(), G_FILE_TEST_IS_EXECUTABLE);
            is_local_desktop_file = is_executable;
            if (is_executable) {
                //rename the generic name
                GKeyFile *key_file = g_key_file_new();
                g_key_file_load_from_file(key_file,
                                          url.path().toUtf8().constData(),
                                          G_KEY_FILE_KEEP_COMMENTS,
                                          nullptr);
                QString locale_name = QLocale::system().name();
                QString local_generic_name_key = QString("Name[%1]").arg(locale_name);
                if (g_key_file_has_key(key_file,
                                       G_KEY_FILE_DESKTOP_GROUP,
                                       local_generic_name_key.toUtf8().constData(),
                                       nullptr)) {
                    g_key_file_set_value(key_file,
                                         G_KEY_FILE_DESKTOP_GROUP,
                                         local_generic_name_key.toUtf8().constData(),
                                         m_new_name.toUtf8().constData());
                } else {
                    g_key_file_set_value(key_file,
                                         G_KEY_FILE_DESKTOP_GROUP,
                                         G_KEY_FILE_DESKTOP_KEY_NAME,
                                         m_new_name.toUtf8().constData());
                }

                g_key_file_save_to_file(key_file,
                                        url.path().toUtf8().constData(),
                                        nullptr);

                //set attributes again.
                GError *set_err = nullptr;
                g_file_set_attributes_from_info(file.get()->get(),
                                                info.get()->get(),
                                                G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                nullptr,
                                                &set_err);
                if (set_err) {
                    qDebug()<<set_err->message;
                    g_error_free(set_err);
                }
                g_key_file_free(key_file);
            }
            g_object_unref(desktop_info);
        }
    }

    if (is_local_desktop_file) {
        m_new_name = m_new_name+".desktop";
    }

    //move the file. normally means 'rename'.
    auto parent = FileUtils::getFileParent(file);
    auto newFile = FileUtils::resolveRelativePath(parent, m_new_name);
    if (is_local_desktop_file) {
fallback_retry:
    GError *err = nullptr;
    g_file_move(file.get()->get(),
                newFile.get()->get(),
                G_FILE_COPY_ALL_METADATA,
                nullptr,
                nullptr,
                nullptr,
                &err);
    if (err) {
        qDebug()<<err->message;
        auto responseType = errored(m_uri,
                                    FileUtils::getFileUri(newFile),
                                    GErrorWrapper::wrapFrom(err),
                                    true);
        switch (responseType.value<ResponseType>()) {
        case Retry:
            goto fallback_retry;
        case Cancel:
            cancel();
            break;
        default:
            break;
        }
    }
    } else {
retry:
    GError *err = nullptr;
    g_file_move(file.get()->get(),
                newFile.get()->get(),
                m_default_copy_flag,
                getCancellable().get()->get(),
                nullptr,
                nullptr,
                &err);
    if (err) {
        auto responseType = errored(m_uri,
                                    FileUtils::getFileUri(newFile),
                                    GErrorWrapper::wrapFrom(err),
                                    true);
        switch (responseType.value<ResponseType>()) {
        case Retry:
            goto retry;
        case Cancel:
            cancel();
            break;
        default:
            break;
        }
    }
    }
    Q_EMIT operationFinished();
}
