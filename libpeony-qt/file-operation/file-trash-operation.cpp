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

    Peony::ExceptionResponse response = Invalid;
    for (auto src : m_src_uris) {
        if (isCancelled())
            break;
retry:
        auto srcFile = wrapGFile(g_file_new_for_uri(src.toUtf8().constData()));
        GError *err = nullptr;
        g_file_trash(srcFile.get()->get(),
                     getCancellable().get()->get(),
                     &err);
        if (err) {
            if (response == Peony::IgnoreAll) {
                g_error_free(err);
                continue;
            }
#if HANDLE_ERR_NEW
            FileOperationError except;
            except.srcUri = src;
            except.destDirUri = tr("trash:///");
            except.isCritical = true;
            except.title = tr("Trash file");
            except.errorCode = err->code;
            except.errorType = ET_GIO;
            except.dlgType = ED_CONFLICT;
            Q_EMIT errored(except);
            auto responseType = except.respCode;
            auto responseData = responseType;
#else
            auto responseData = Q_EMIT errored(src, tr("trash:///"), GErrorWrapper::wrapFrom(err), true);
#endif
            switch (responseData) {
            case Peony::Retry:
                goto retry;
            case Peony::Cancel:
                cancel();
                break;
            case Peony::IgnoreAll:
                response = Peony::IgnoreAll;
                break;
            default:
                break;
            }
        }
    }
    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}
