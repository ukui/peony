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

#include "file-move-operation.h"
#include "file-node-reporter.h"
#include "file-node.h"
#include "file-enumerator.h"
#include "file-info.h"

#include "file-operation-manager.h"

using namespace Peony;

static void handleDuplicate(FileNode *node) {
    QString name = node->destBaseName();
    QRegExp regExp("\\(\\d+\\)");
    if (name.contains(regExp)) {
        int pos = 0;
        int num = 0;
        QString tmp;
        while ((pos = regExp.indexIn(name, pos)) != -1) {
            tmp = regExp.cap(0).toUtf8();
            pos += regExp.matchedLength();
            qDebug()<<"pos"<<pos;
        }
        tmp.remove(0,1);
        tmp.chop(1);
        num = tmp.toInt();

        num++;
        name = name.replace(regExp, QString("(%1)").arg(num));
        node->setDestFileName(name);
    } else {
        if (name.contains(".")) {
            auto list = name.split(".");
            if (list.count() <= 1) {
                node->setDestFileName(name+"(1)");
            } else {
                list.insert(1, "(1)");
                name = list.join(".");
                if (name.endsWith("."))
                    name.chop(1);
                node->setDestFileName(name);
            }
        } else {
            name = name + "(1)";
            node->setDestFileName(name);
        }
    }
}

FileMoveOperation::FileMoveOperation(QStringList sourceUris, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    m_source_uris = sourceUris;
    m_dest_dir_uri = destDirUri;
    m_info = std::make_shared<FileOperationInfo>(sourceUris, destDirUri, FileOperationInfo::Move);
}

FileMoveOperation::~FileMoveOperation()
{
    if (m_reporter)
        delete m_reporter;
}

void FileMoveOperation::progress_callback(goffset current_num_bytes,
                                          goffset total_num_bytes,
                                          FileMoveOperation *p_this)
{
    Q_EMIT p_this->FileProgressCallback(p_this->m_current_src_uri,
                                        p_this->m_current_dest_dir_uri,
                                        current_num_bytes,
                                        total_num_bytes);
    //format: move srcUri to destDirUri: curent_bytes(count) of total_bytes(count).
}

FileOperation::ResponseType FileMoveOperation::prehandle(GError *err)
{
    setHasError(true);
    if (m_prehandle_hash.contains(err->code))
        return m_prehandle_hash.value(err->code);

    return Other;
}

void FileMoveOperation::move()
{
    if (isCancelled())
        return;

    QList<FileNode*> nodes;
    for (auto srcUri : m_source_uris) {
        //FIXME: ignore the total size when using native move.
        operationPreparedOne(srcUri, 0);
        auto node = new FileNode(srcUri, nullptr, nullptr);
        nodes<<node;
    }
    operationPrepared();

    auto destDir = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));
    m_total_count = m_source_uris.count();
    for (auto file : nodes) {
        if (isCancelled())
            return;

        QString srcUri = file->uri();
        m_current_count = nodes.indexOf(file) + 1;
        m_current_src_uri = srcUri;
        m_current_dest_dir_uri = m_dest_dir_uri;

        auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
        char *base_name = g_file_get_basename(srcFile.get()->get());
        auto destFile = wrapGFile(g_file_resolve_relative_path(destDir.get()->get(),
                                                               base_name));

        char *dest_uri = g_file_get_uri(destFile.get()->get());
        file->setDestUri(dest_uri);

        g_free(dest_uri);
        g_free(base_name);

retry:
        GError *err = nullptr;
        g_file_move(srcFile.get()->get(),
                    destFile.get()->get(),
                    m_default_copy_flag,
                    getCancellable().get()->get(),
                    GFileProgressCallback(progress_callback),
                    this,
                    &err);

        if (err) {
            auto errWrapper = GErrorWrapper::wrapFrom(err);
            switch (errWrapper.get()->code()) {
            case G_IO_ERROR_CANCELLED:
                return;
            case G_IO_ERROR_NOT_SUPPORTED:
            case G_IO_ERROR_WOULD_RECURSE: {
                m_force_use_fallback = true;
                for (auto node : nodes) {
                    delete node;
                }
                nodes.clear();
                return;
            }
            default:
                break;
            }
            ResponseType handle_type = prehandle(err);
            if (handle_type == Other) {
                qDebug()<<"send error";
                auto responseTypeWrapper = Q_EMIT errored(srcUri, m_dest_dir_uri, errWrapper);
                qDebug()<<"get return";
                handle_type = responseTypeWrapper.value<ResponseType>();
                //block until error has been handled.
            }
            switch (handle_type) {
            case IgnoreOne: {
                file->setState(FileNode::Unhandled);
                file->setErrorResponse(FileOperation::IgnoreOne);
                //skip to next loop.
                break;
            }
            case IgnoreAll: {
                file->setState(FileNode::Unhandled);
                file->setErrorResponse(FileOperation::IgnoreOne);
                m_prehandle_hash.insert(err->code, IgnoreOne);
                break;
            }
            case OverWriteOne: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(FileOperation::OverWriteOne);
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                break;
            }
            case OverWriteAll: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(FileOperation::OverWriteOne);
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                m_prehandle_hash.insert(err->code, OverWriteOne);
                break;
            }
            case BackupOne: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(FileOperation::BackupOne);
                handleDuplicate(file);
                auto handledDestFileUri = file->resoveDestFileUri(m_dest_dir_uri);
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                g_file_move(srcFile.get()->get(),
                            handledDestFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                break;
            }
            case BackupAll: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(FileOperation::BackupOne);
                auto handledDestFileUri = file->resoveDestFileUri(m_dest_dir_uri);
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                g_file_move(srcFile.get()->get(),
                            handledDestFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                m_prehandle_hash.insert(err->code, BackupOne);
                break;
            }
            case Retry: {
                goto retry;
            }
            case Cancel: {
                cancel();
                break;
            }
            default:
                break;
            }
        } else {
            file->setState(FileNode::Handled);
        }
        //FIXME: ignore the total size when using native move.
        operationProgressedOne(file->uri(), file->destUri(), 0);
    }
    //native move has not clear operation.
    operationProgressed();

    //rollback if cancelled
    //FIXME: if native move function get into error,
    //such as the target is existed, the rollback might
    //get into error too.
    if (isCancelled()) {
        for (auto file : nodes) {
            if (!file->destUri().isEmpty()) {
                GFileWrapperPtr destFile = wrapGFile(g_file_new_for_uri(file->destUri().toUtf8().constData()));
                GFileWrapperPtr srcFile = wrapGFile(g_file_new_for_uri(file->uri().toUtf8().constData()));
                //try rollbacking
                switch (file->responseType()) {
                case Other: {
                    //no error, move dest back to src
                    g_file_move(destFile.get()->get(),
                                srcFile.get()->get(),
                                m_default_copy_flag,
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr);
                    break;
                }
                case IgnoreOne: {
                    break;
                }
                case OverWriteOne: {
                    g_file_copy(destFile.get()->get(),
                                srcFile.get()->get(),
                                m_default_copy_flag,
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr);
                    break;
                }
                case BackupOne: {
                    g_file_copy(destFile.get()->get(),
                                srcFile.get()->get(),
                                m_default_copy_flag,
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr);
                    break;
                }
                default:
                    break;
                }
            }
        }
    }

    //release node
    for (auto file : nodes) {
        delete file;
    }
    nodes.clear();
}

void FileMoveOperation::rollbackNodeRecursively(FileNode *node)
{
    switch (node->state()) {
    case FileNode::Handled: {
        //do not clear the dest file if ignored or overwrite or backuped.
        if (node->responseType() != Other)
            break;

        if (node->isFolder()) {
            auto children = node->children();
            for (auto child : *children) {
                rollbackNodeRecursively(child);
            }
            GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
            g_file_delete(dest_file, nullptr, nullptr);
            g_object_unref(dest_file);
        } else {
            GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
            g_file_delete(dest_file, nullptr, nullptr);
            g_object_unref(dest_file);
        }
        operationRollbackedOne(node->destUri(), node->uri());
        break;
    }
    case FileNode::Cleared: {
        switch (node->responseType()) {
        case Other: {
            if (node->isFolder()) {
                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
                g_file_make_directory(src_file, nullptr, nullptr);
                g_object_unref(src_file);
                auto children = node->children();
                for (auto child : *children) {
                    rollbackNodeRecursively(child);
                }
                //try deleting the dest directory
                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
                g_file_delete(dest_file, nullptr, nullptr);
                g_object_unref(dest_file);
            } else {
                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
                //"rollback"
                GError *err = nullptr;
                g_file_move(dest_file,
                            src_file,
                            m_default_copy_flag,
                            nullptr,
                            nullptr,
                            nullptr,
                            &err);
                if (err) {
                    qDebug()<<node->destUri();
                    qDebug()<<node->uri();
                    qDebug()<<err->message;
                    g_error_free(err);
                }
                g_object_unref(dest_file);
                g_object_unref(src_file);
            }
            operationRollbackedOne(node->destUri(), node->uri());
            break;
        }
        default: {
            //copy if err handle response type is valid.
            if (node->isFolder()) {
                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
                g_file_make_directory(src_file, nullptr, nullptr);
                g_object_unref(src_file);
                auto children = node->children();
                for (auto child : *children) {
                    rollbackNodeRecursively(child);
                }
                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
                g_object_unref(dest_file);
            } else {
                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
                //"rollback"
                GError *err = nullptr;
                g_file_copy(dest_file,
                            src_file,
                            m_default_copy_flag,
                            nullptr,
                            nullptr,
                            nullptr,
                            &err);
                if (err) {
                    qDebug()<<node->destUri();
                    qDebug()<<node->uri();
                    qDebug()<<err->message;
                    g_error_free(err);
                }
                g_object_unref(dest_file);
                g_object_unref(src_file);
            }
            operationRollbackedOne(node->destUri(), node->uri());
            break;
        }
        }
        break;
    }
    default: {
        //make sure all nodes were rollbacked.
        if (node->isFolder()) {
            auto children = node->children();
            for (auto child : *children) {
                rollbackNodeRecursively(child);
            }
        }
        break;
    }
    }
}

void FileMoveOperation::copyRecursively(FileNode *node)
{
    if (isCancelled())
        return;

    QString relativePath = node->getRelativePath();
    //FIXME: the smart pointers' deconstruction spends too much time.
    GFileWrapperPtr destRoot = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));
    GFileWrapperPtr destFile = wrapGFile(g_file_resolve_relative_path(destRoot.get()->get(),
                                                                      relativePath.toUtf8().constData()));

    char *dest_file_uri = g_file_get_uri(destFile.get()->get());
    node->setDestUri(dest_file_uri);
    g_free(dest_file_uri);
    m_current_src_uri = node->uri();
    GFile *dest_parent = g_file_get_parent(destFile.get()->get());
    char *dest_dir_uri = g_file_get_uri(dest_parent);
    m_current_dest_dir_uri = dest_dir_uri;
    g_free(dest_dir_uri);
    g_object_unref(dest_parent);

fallback_retry:
    if (node->isFolder()) {
        GError *err = nullptr;

        //NOTE: mkdir doesn't have a progress callback.
        Q_EMIT FileProgressCallback(m_current_src_uri,
                                    m_current_dest_dir_uri,
                                    node->size(),
                                    node->size());
        g_file_make_directory(destFile.get()->get(),
                              getCancellable().get()->get(),
                              &err);
        if (err) {
            if (err->code == G_IO_ERROR_CANCELLED) {
                return;
            }
            auto errWrapperPtr = GErrorWrapper::wrapFrom(err);
            ResponseType handle_type = prehandle(err);
            if (handle_type == Other) {
                qDebug()<<"send error";
                auto typeData = errored(m_current_src_uri, m_current_dest_dir_uri, errWrapperPtr);
                qDebug()<<"get return";
                handle_type = typeData.value<ResponseType>();
            }
            //handle.
            switch (handle_type) {
            case IgnoreOne: {
                node->setState(FileNode::Unhandled);
                node->setErrorResponse(IgnoreOne);
                break;
            }
            case IgnoreAll: {
                node->setState(FileNode::Unhandled);
                node->setErrorResponse(IgnoreOne);
                m_prehandle_hash.insert(err->code, IgnoreOne);
                break;
            }
            case OverWriteOne: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                //make dir has no overwrite
                break;
            }
            case OverWriteAll: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                m_prehandle_hash.insert(err->code, OverWriteOne);
                break;
            }
            case BackupOne: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                //make dir has no backup
                break;
            }
            case BackupAll: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                //make dir has no backup
                m_prehandle_hash.insert(err->code, BackupOne);
                break;
            }
            case Retry: {
                goto fallback_retry;
            }
            case Cancel: {
                cancel();
                break;
            }
            default:
                break;
            }
        } else {
            node->setState(FileNode::Handled);
        }
        //assume that make dir finished anyway
        Q_EMIT FileProgressCallback(m_current_src_uri,
                                    m_current_dest_dir_uri,
                                    node->size(),
                                    node->size());
        Q_EMIT operationProgressedOne(node->uri(), node->destUri(), node->size());
        for (auto child : *(node->children())) {
            copyRecursively(child);
        }
    } else {
        GError *err = nullptr;
        GFileWrapperPtr sourceFile = wrapGFile(g_file_new_for_uri(node->uri().toUtf8().constData()));
        g_file_copy(sourceFile.get()->get(),
                    destFile.get()->get(),
                    m_default_copy_flag,
                    getCancellable().get()->get(),
                    GFileProgressCallback(progress_callback),
                    this,
                    &err);

        if (err) {
            if (err->code == G_IO_ERROR_CANCELLED) {
                return;
            }
            auto errWrapperPtr = GErrorWrapper::wrapFrom(err);
            ResponseType handle_type = prehandle(err);
            if (handle_type == Other) {
                qDebug()<<"send error";
                auto typeData = errored(m_current_src_uri, m_current_dest_dir_uri, errWrapperPtr);
                qDebug()<<"get return";
                handle_type = typeData.value<ResponseType>();
            }
            //handle.
            switch (handle_type) {
            case IgnoreOne: {
                node->setState(FileNode::Unhandled);
                node->setErrorResponse(IgnoreOne);
                break;
            }
            case IgnoreAll: {
                node->setState(FileNode::Unhandled);
                node->setErrorResponse(IgnoreOne);
                m_prehandle_hash.insert(err->code, IgnoreOne);
                break;
            }
            case OverWriteOne: {
                g_file_copy(sourceFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                break;
            }
            case OverWriteAll: {
                g_file_copy(sourceFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                m_prehandle_hash.insert(err->code, OverWriteOne);
                break;
            }
            case BackupOne: {
                g_file_copy(sourceFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                break;
            }
            case BackupAll: {
                g_file_copy(sourceFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                m_prehandle_hash.insert(err->code, BackupOne);
                break;
            }
            case Retry: {
                goto fallback_retry;
            }
            case Cancel: {
                cancel();
                break;
            }
            default:
                break;
            }
        } else {
            node->setState(FileNode::Handled);
        }
        Q_EMIT operationProgressedOne(node->uri(), node->destUri(), node->size());
    }
    destFile.reset();
    destRoot.reset();
}

void FileMoveOperation::deleteRecursively(FileNode *node)
{
    if (isCancelled())
        return;

    GFile *file = g_file_new_for_uri(node->uri().toUtf8().constData());
    if (node->isFolder()) {
        for (auto child : *(node->children())) {
            deleteRecursively(child);
        }
        g_file_delete(file,
                      getCancellable().get()->get(),
                      nullptr);
        node->setState(FileNode::Cleared);
    } else {
        g_file_delete(file,
                      getCancellable().get()->get(),
                      nullptr);
        node->setState(FileNode::Cleared);
    }
    g_object_unref(file);
    qDebug()<<"deleted";
    operationAfterProgressedOne(node->uri());
}

void FileMoveOperation::moveForceUseFallback()
{
    if (isCancelled())
        return;

    Q_EMIT operationRequestShowWizard();
    m_reporter = new FileNodeReporter;
    connect(m_reporter, &FileNodeReporter::nodeFound, this, &FileMoveOperation::operationPreparedOne);

    //FIXME: total size should not compute twice. I should get it from ui-thread.
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
    delete total_size;

    for (auto node : nodes) {
        copyRecursively(node);
    }
    operationProgressed();

    for (auto node : nodes) {
        deleteRecursively(node);
    }

    if (isCancelled())
        Q_EMIT operationStartRollbacked();

    for (auto file : nodes) {
        qDebug()<<file->uri();
        if (isCancelled()) {
            rollbackNodeRecursively(file);
        }
    }

    for (auto node : nodes) {
        delete node;
    }

    nodes.clear();
}

bool FileMoveOperation::isValid()
{
    bool isInvalid = false;
    for (auto srcUri : m_source_uris) {
        auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
        auto destFile = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));
        auto parentFile = wrapGFile(g_file_get_parent(srcFile.get()->get()));
        if (g_file_equal(destFile.get()->get(), parentFile.get()->get())) {
            isInvalid = true;
            invalidOperation(tr("Invalid move operation, cannot move a file itself."));
        }
        //BUG: some special basename like test and test2, will lead the operation invalid.
        /*
        if (m_dest_dir_uri.contains(srcUri)) {
            isInvalid = true;
            invalidOperation(tr("Invalid move operation, cannot move a file into its sub directories."));
        }
        */
        //FIXME: find if destUriDirFile is srcFile's child.
        //it will call G_IO_ERROR_INVALID_FILENAME
    }
    if (isInvalid)
        invalidExited(tr("Invalid Operation."));
    return !isInvalid;
}

void FileMoveOperation::run()
{
    Q_EMIT operationStarted();
start:
    if (!isValid()) {
        auto response = errored(nullptr,
                                nullptr,
                                GErrorWrapper::wrapFrom(g_error_new(G_IO_ERROR,
                                                                    G_IO_ERROR_INVAL,
                                                                    tr("Invalid Operation").toUtf8().constData(),
                                                                    nullptr)),
                                true);
        switch (response.value<ResponseType>()) {
        case Retry:
            goto start;
        case Cancel:
            cancel();
            break;
        default:
            break;
        }
        goto end;
    }

    if (isCancelled())
        return;

    //should block and wait for other object prepared.
    if (!m_force_use_fallback) {
        move();
    }

    //ensure again
    if (m_force_use_fallback) {
        moveForceUseFallback();
    }
    qDebug()<<"finished";
end:
    Q_EMIT operationFinished();
}

void FileMoveOperation::cancel()
{
    FileOperation::cancel();
    if (m_reporter)
        m_reporter->cancel();
}
