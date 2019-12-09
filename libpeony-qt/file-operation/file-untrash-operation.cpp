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

#include "file-untrash-operation.h"
#include "file-utils.h"
#include "file-operation-manager.h"

#include <QUrl>

using namespace Peony;

FileUntrashOperation::FileUntrashOperation(QStringList uris, QObject *parent) : FileOperation (parent)
{
    m_uris = uris;
    //FIXME: should i put this into prepare process?
    cacheOriginalUri();
    QStringList oppositeSrcUris;
    for (auto value : m_restore_hash) {
        oppositeSrcUris<<value;
    }
    m_info = std::make_shared<FileOperationInfo>(oppositeSrcUris, "trash:///", FileOperationInfo::Trash);
}

void FileUntrashOperation::cacheOriginalUri()
{
    for (auto uri : m_uris) {
        if (isCancelled())
            break;

        auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
        auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                                    G_FILE_ATTRIBUTE_TRASH_ORIG_PATH,
                                                    G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                    getCancellable().get()->get(),
                                                    nullptr));
        auto origin_path = g_file_info_get_attribute_byte_string(info.get()->get(), G_FILE_ATTRIBUTE_TRASH_ORIG_PATH);
        qDebug()<<"orig-path"<<origin_path;

        auto destFile = wrapGFile(g_file_new_for_path(origin_path));
        auto originUri = FileUtils::getFileUri(destFile);

        m_restore_hash.insert(uri, originUri);
    }
}

void FileUntrashOperation::run()
{
    /*!
      \bug
      can not restore the files in desktop.
      it caused by the parent uri string has chinese.
      */
    for (auto uri : m_uris) {
        if (isCancelled())
            break;

        //cacheOriginalUri();
        auto originUri = m_restore_hash.value(uri);

        auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
        auto destFile = wrapGFile(g_file_new_for_uri(originUri.toUtf8().constData()));

retry:
        GError *err = nullptr;
        g_file_move(file.get()->get(),
                    destFile.get()->get(),
                    GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                    getCancellable().get()->get(),
                    nullptr,
                    nullptr,
                    &err);

        if (err) {
            qDebug()<<"untrash err"<<uri<<originUri<<err->message;
            auto responseData = Q_EMIT errored(uri, originUri, GErrorWrapper::wrapFrom(err), false);
            switch (responseData.value<ResponseType>()) {
            case Retry:
                goto retry;
            case Cancel:
                cancel();
                break;
            case OverWriteOne:
                g_file_move(file.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            nullptr,
                            nullptr,
                            &err);
                break;
            case OverWriteAll:
                g_file_move(file.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            nullptr,
                            nullptr,
                            &err);
                break;
            default:
                break;
            }
        }
    }
}
