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

#include "file-delete-operation.h"
#include "file-operation-manager.h"
#include "file-node.h"
#include "file-node-reporter.h"

using namespace Peony;

FileDeleteOperation::FileDeleteOperation(QStringList sourceUris, QObject *parent) : FileOperation(parent)
{
    m_source_uris = sourceUris;
    m_reporter = new FileNodeReporter;
    m_info = std::make_shared<FileOperationInfo>(sourceUris, nullptr, FileOperationInfo::Delete);
    connect(m_reporter, &FileNodeReporter::nodeFound, this, &FileOperation::operationPreparedOne);
}

FileDeleteOperation::~FileDeleteOperation()
{
    delete m_reporter;
}

std::shared_ptr<FileOperationInfo> FileDeleteOperation::getOperationInfo()
{
    return m_info;
}

void FileDeleteOperation::deleteRecursively(FileNode *node)
{
    if (isCancelled())
        return;

    GFile *file = g_file_new_for_uri(node->uri().toUtf8().constData());
    if (node->isFolder()) {
        for (auto child : *(node->children())) {
            deleteRecursively(child);
        }
        GError *err = nullptr;
        g_file_delete(file,
                      getCancellable().get()->get(),
                      &err);
        if (err) {
            if (!m_prehandle_hash.isEmpty()) {
                g_error_free(err);
                return;
            }
            // if delete a file get into error, it might be a critical error.
            FileOperationError except;
            except.errorType = ET_GIO;
            except.dlgType = ED_WARNING;
            except.srcUri = node->uri();
            except.title = tr("File delete");
            except.errorStr = err->message;
            except.errorCode = err->code;
            Q_EMIT errored(except);
            auto response = except.respCode;
            auto responseType = response;
            if (responseType == Cancel) {
                cancel();
            }
            // Similar errors only remind the user once
            m_prehandle_hash.insert(err->code, IgnoreAll);
        }
    } else {
        GError *err = nullptr;
        g_file_delete(file,getCancellable().get()->get(),&err);
        if (err) {
            if (!m_prehandle_hash.isEmpty()) {
                g_error_free(err);
                return;
            }
            // if delete a file get into error, it might be a critical error.
            FileOperationError except;
            except.errorType = ET_GIO;
            except.dlgType = ED_WARNING;
            except.srcUri = node->uri();
            except.title = tr("File delete");
            except.errorCode = err->code;
            except.errorStr = err->message;
            Q_EMIT errored(except);
            auto response = except.respCode;
            qDebug()<<response;
            auto responseType = response;
            if (responseType == Cancel) {
                cancel();
            }
            // Similar errors only remind the user once
            m_prehandle_hash.insert(err->code, IgnoreAll);
        }
    }
    g_object_unref(file);
    qDebug()<<"deleted";
    //operationAfterProgressedOne(node->uri());
    m_current_offset += node->size();
    FileProgressCallback(node->uri(), node->uri(), m_current_offset, m_total_szie);
}

void FileDeleteOperation::run()
{
    if (isCancelled())
        return;

    Q_EMIT operationStarted();

    Q_EMIT operationRequestShowWizard();

    goffset *total_size = new goffset(0);

    QList<FileNode*> nodes;
    for (auto uri : m_source_uris) {
        FileNode *node = new FileNode(uri, nullptr, m_reporter);
        node->findChildrenRecursively();
        node->computeTotalSize(total_size);
        nodes<<node;
    }
    operationPrepared();

    m_total_szie = *total_size;
    m_current_offset = 0;
    delete total_size;

    //jump to the clearing stage.
    //operationProgressed();

    for (auto node : nodes) {
        deleteRecursively(node);
    }

    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}

void FileDeleteOperation::cancel()
{
    if (m_reporter)
        m_reporter->cancel();
    FileOperation::cancel();
}
