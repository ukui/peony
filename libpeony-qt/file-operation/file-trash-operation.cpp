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

#include <sys/stat.h>
#include <QMessageBox>
#include <unistd.h>

#include "file-trash-operation.h"
#include "file-operation-manager.h"

using namespace Peony;

FileTrashOperation::FileTrashOperation(QStringList srcUris, QObject *parent) : FileOperation (parent)
{
    m_src_uris = srcUris;
    m_info = std::make_shared<FileOperationInfo>(srcUris, "trash:///", FileOperationInfo::Trash);
}

void FileTrashOperation::run()
{
    Q_EMIT operationStarted();

    ResponseType response = Invalid;
    for (auto src : m_src_uris) {
        if (isCancelled())
            break;
retry:
        auto srcFile = wrapGFile(g_file_new_for_uri(src.toUtf8().constData()));
        GError *err = nullptr;
        // check file is readonly
        auto fileInfo = g_file_query_info(srcFile.get()->get(), "*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
        if (nullptr != fileInfo) {
            guint32 uid = g_file_info_get_attribute_uint32(fileInfo, G_FILE_ATTRIBUTE_UNIX_UID);
            if (uid == getuid()) {
                guint mode = g_file_info_get_attribute_uint32(fileInfo, G_FILE_ATTRIBUTE_UNIX_MODE);
                if (((S_IWUSR & mode) == 0) && (S_IRUSR & mode) && ((S_IXUSR & mode) == 0)) {
                    int ret = QMessageBox::warning(nullptr, tr("File delete warning"), tr("Whether to delete normal files with write protection"), QMessageBox::Ok, QMessageBox::Cancel);
                    if (ret == QMessageBox::Cancel) {
                        break;
                    }
                }
            }
        }
        g_file_trash(srcFile.get()->get(), getCancellable().get()->get(), &err);
        if (err) {
            if (response == IgnoreAll) {
                g_error_free(err);
                continue;
            }

            auto responseData = Q_EMIT errored(src, tr("trash:///"), GErrorWrapper::wrapFrom(err), true);
            switch (responseData) {
            case Retry:
                goto retry;
            case Cancel:
                cancel();
                break;
            case IgnoreAll:
                response = IgnoreAll;
                break;
            default:
                break;
            }
        }
    }
    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}
