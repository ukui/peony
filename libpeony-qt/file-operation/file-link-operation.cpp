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

#include "file-link-operation.h"
#include "file-operation-manager.h"

#include "gerror-wrapper.h"

#include <QUrl>

using namespace Peony;

FileLinkOperation::FileLinkOperation(QString srcUri, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    m_src_uri = srcUri;
    QUrl url = srcUri;
    if (!url.fileName().contains(".")) {
        m_dest_uri = destDirUri + "/" + url.fileName() + tr(" - Symbolic Link");
    } else {
        m_dest_uri = destDirUri + "/" + url.fileName();
        m_dest_uri = m_dest_uri.insert(m_dest_uri.lastIndexOf('.'), tr(" - Symbolic Link"));
    }

    QStringList fake_uris;
    fake_uris<<srcUri;
    m_info = std::make_shared<FileOperationInfo>(fake_uris, destDirUri, FileOperationInfo::Link);
}

FileLinkOperation::~FileLinkOperation()
{

}

void FileLinkOperation::run()
{
    operationStarted();
    auto destFile = wrapGFile(g_file_new_for_uri(m_dest_uri.toUtf8().constData()));
    GError *err = nullptr;
retry:
    QUrl url = m_src_uri;
    g_file_make_symbolic_link(destFile.get()->get(),
                              url.path().toUtf8().constData(),
                              nullptr,
                              &err);
    if (err) {
        setHasError(true);
        //forbid response actions except retry and cancel.
        auto responseType = errored(m_src_uri, m_dest_uri, GErrorWrapper::wrapFrom(err), true);
        if (responseType.value<FileOperation::ResponseType>() == FileOperation::Retry) {
            goto retry;
        }
    }
    operationFinished();
}
