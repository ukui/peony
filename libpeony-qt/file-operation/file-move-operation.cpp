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

#include "file-move-operation.h"
#include "file-node-reporter.h"
#include "file-node.h"
#include "file-enumerator.h"
#include "file-info.h"

#include "file-operation-manager.h"

#include <QProcess>

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
                int pos = list.count() - 1;
                if (list.last() == "gz" |
                        list.last() == "xz" |
                        list.last() == "Z" |
                        list.last() == "sit" |
                        list.last() == "bz" |
                        list.last() == "bz2") {
                    pos--;
                }
                if (pos < 0)
                    pos = 0;
                //list.insert(pos, "(1)");
                auto tmp = list;
                QStringList suffixList;
                for (int i = 0; i < list.count() - pos; i++) {
                    suffixList.prepend(tmp.takeLast());
                }
                auto suffix = suffixList.join(".");

                auto basename = tmp.join(".");
                name = basename + "(1)" + "." + suffix;
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
    if (total_num_bytes < current_num_bytes)
        return;

    QUrl url(p_this->m_current_src_uri);
    auto currnet = p_this->m_current_offset + current_num_bytes;
    auto total = p_this->m_total_szie;
    auto fileIconName = FileUtils::getFileIconName(p_this->m_current_src_uri, false);
    auto destFileName = FileUtils::isFileDirectory(p_this->m_current_dest_dir_uri) ?
                p_this->m_current_dest_dir_uri + "/" + url.fileName() : p_this->m_current_dest_dir_uri;

    Q_EMIT p_this->FileProgressCallback(p_this->m_current_src_uri, destFileName, fileIconName, currnet, total);
    //format: move srcUri to destDirUri: curent_bytes(count) of total_bytes(count).
}

ExceptionResponse FileMoveOperation::prehandle(GError *err)
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
            setHasError(true);
            auto errWrapper = GErrorWrapper::wrapFrom(err);
            switch (errWrapper.get()->code()) {
            case G_IO_ERROR_CANCELLED:
                return;
            case G_IO_ERROR_NOT_SUPPORTED:
            case G_IO_ERROR_WOULD_RECURSE:
            case G_IO_ERROR_EXISTS: {
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
            int handle_type = prehandle(err);
            FileOperationError except;
            except.srcUri = srcUri;
            except.destDirUri = m_dest_dir_uri;
            except.isCritical = false;
            except.op = FileOpMove;
            except.title = tr("Move file error");
            except.errorCode = err->code;
            except.errorStr = err->message;
            except.errorType = ET_GIO;
            if (handle_type == Other) {
                auto responseTypeWrapper = Invalid;
                if (G_IO_ERROR_EXISTS == err->code) {
                    except.dlgType = ED_CONFLICT;
                    Q_EMIT errored(except);
                    responseTypeWrapper = except.respCode;
                } else {
                    except.dlgType = ED_WARNING;
                    Q_EMIT errored(except);
                    responseTypeWrapper = except.respCode;
                }
                handle_type = responseTypeWrapper;
                //block until error has been handled.
            }

            GError *handled_err = nullptr;
            switch (handle_type) {
            case IgnoreOne: {
                file->setState(FileNode::Unhandled);
                file->setErrorResponse(IgnoreOne);
                //skip to next loop.
                break;
            }
            case IgnoreAll: {
                file->setState(FileNode::Unhandled);
                file->setErrorResponse(IgnoreOne);
                m_prehandle_hash.insert(err->code, IgnoreOne);
                break;
            }
            case OverWriteOne: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(OverWriteOne);
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS|
                                           G_FILE_COPY_ALL_METADATA|
                                           G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            &handled_err);
                break;
            }
            case OverWriteAll: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(OverWriteOne);
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS|
                                           G_FILE_COPY_ALL_METADATA|
                                           G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            &handled_err);
                m_prehandle_hash.insert(err->code, OverWriteOne);
                break;
            }
            case BackupOne: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(BackupOne);
                // use custom name
                QString name = "";
                QStringList extendStr = file->destBaseName().split(".");
                if (extendStr.length() > 0) {
                    extendStr.removeAt(0);
                }
                QString endStr = extendStr.join(".");
                if (except.respValue.contains("name")) {
                    name = except.respValue["name"].toString();
                    if (endStr != "" && name.endsWith(endStr)) {
                        file->setDestFileName(name);
                    } else if ("" != endStr && "" != name) {
                        file->setDestFileName(name + "." + endStr);
                    }
                }
                if (FileUtils::isFileExsit(file->destUri())) {
                    handleDuplicate(file);
                }
                auto handledDestFileUri = file->resolveDestFileUri(m_dest_dir_uri);
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                g_file_copy(srcFile.get()->get(),
                            handledDestFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            &handled_err);
                break;
            }
            case BackupAll: {
                file->setState(FileNode::Handled);
                file->setErrorResponse(BackupOne);
                auto handledDestFileUri = file->resolveDestFileUri(m_dest_dir_uri);
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                g_file_copy(srcFile.get()->get(),
                            handledDestFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            &handled_err);
                m_prehandle_hash.insert(err->code, BackupOne);
                break;
            }
            case Retry: {
                goto retry;
            }
            case Cancel: {
                file->setState(FileNode::Handled);
                cancel();
                break;
            }
            default:
                break;
            }

            except.srcUri = srcUri;
            except.errorType = ET_GIO;
            except.errorCode = err->code;
            except.errorStr = err->message;
            except.op = FileOpMove;
            except.title = tr("Move file error");
            except.destDirUri = m_dest_dir_uri;
            except.isCritical = true;
            if (handled_err) {
                auto handledErr = GErrorWrapper::wrapFrom(handled_err);
                FileOperationError except;
                if (G_IO_ERROR_EXISTS == handled_err->code) {
                    except.dlgType = ED_CONFLICT;
                    Q_EMIT errored(except);
                } else {
                    except.dlgType = ED_WARNING;
                    Q_EMIT errored(except);
                }

                auto response = except.respCode;
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
    case FileNode::Handling:
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

    node->setState(FileNode::Handling);

    QString relativePath = node->getRelativePath();
    //FIXME: the smart pointers' deconstruction spends too much time.
    GFileWrapperPtr destRoot = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));
    GFileWrapperPtr destFile = wrapGFile(g_file_resolve_relative_path(destRoot.get()->get(), relativePath.toUtf8().constData()));

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
        auto fileIconName = FileUtils::getFileIconName(m_current_src_uri, false);
        auto destFileName = FileUtils::isFileDirectory(m_current_dest_dir_uri) ? nullptr : m_current_dest_dir_uri;
        //NOTE: mkdir doesn't have a progress callback.
        Q_EMIT FileProgressCallback(m_current_src_uri, destFileName, fileIconName, node->size(), node->size());
        g_file_make_directory(destFile.get()->get(),getCancellable().get()->get(), &err);
        if (err) {
            FileOperationError except;
            if (err->code == G_IO_ERROR_CANCELLED) {
                return;
            }
            auto errWrapperPtr = GErrorWrapper::wrapFrom(err);
            int handle_type = prehandle(err);
            except.errorType = ET_GIO;
            except.op = FileOpMove;
            except.title = tr("Move file error");
            except.errorCode = err->code;
            except.errorStr = err->message;
            except.srcUri = m_current_src_uri;
            except.destDirUri = m_current_dest_dir_uri;
            except.isCritical = false;
            if (handle_type == Other) {
                auto typeData = Invalid;
                if (G_IO_ERROR_EXISTS == err->code) {
                    except.dlgType = ED_CONFLICT;
                    Q_EMIT errored(except);
                    typeData = except.respCode;
                } else {
                    except.dlgType = ED_WARNING;
                    Q_EMIT errored(except);
                    typeData = except.respCode;
                }
                // ignore multiple bounces
                if (except.errorCode == G_IO_ERROR_NOT_SUPPORTED) {
                    m_prehandle_hash.insert(err->code, IgnoreOne);
                }
                handle_type = typeData;
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
                // use custom name
                QString name = "";
                QStringList extendStr = node->destBaseName().split(".");
                if (extendStr.length() > 0) {
                    extendStr.removeAt(0);
                }
                QString endStr = extendStr.join(".");
                if (except.respValue.contains("name")) {
                    name = except.respValue["name"].toString();
                    if (endStr != "" && name.endsWith(endStr)) {
                        node->setDestFileName(name);
                    } else if ("" != endStr && "" != name) {
                        node->setDestFileName(name + "." + endStr);
                    } else if ("" == endStr) {
                        node->setDestFileName(name);
                    }
                }
                if (FileUtils::isFileExsit(node->destUri())) {
                    handleDuplicate(node);
                }
                g_object_unref(destFile.get());
                destFile = wrapGFile(g_file_new_for_uri(node->destUri().toUtf8().constData()));
                goto fallback_retry;
            }
            case BackupAll: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                if (FileUtils::isFileExsit(node->destUri())) {
                    handleDuplicate(node);
                }
                m_prehandle_hash.insert(err->code, BackupOne);
                g_object_unref(destFile.get());
                destFile = wrapGFile(g_file_new_for_uri(node->destUri().toUtf8().constData()));
                goto fallback_retry;
            }
            case Retry: {
                goto fallback_retry;
            }
            case Cancel: {
                node->setState(FileNode::Handled);
                cancel();
                break;
            }
            default:
                break;
            }
        } else {
            node->setState(FileNode::Handled);
        }

        fileIconName = FileUtils::getFileIconName(m_current_src_uri, false);
        destFileName = FileUtils::isFileDirectory(m_current_dest_dir_uri) ? nullptr : m_current_dest_dir_uri;
        //assume that make dir finished anyway
        m_current_offset += node->size();
        Q_EMIT FileProgressCallback(m_current_src_uri, destFileName, fileIconName, m_current_offset, m_total_szie);
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
            FileOperationError except;
            if (err->code == G_IO_ERROR_CANCELLED) {
                return;
            }
            auto errWrapperPtr = GErrorWrapper::wrapFrom(err);
            int handle_type = prehandle(err);
            except.isCritical = true;
            except.errorType = ET_GIO;
            except.errorCode = err->code;
            except.errorStr = err->message;
            except.op = FileOpMove;
            except.title = tr("Create file error");
            except.srcUri = m_current_src_uri;
            except.destDirUri = m_current_dest_dir_uri;
            if (handle_type == Other) {
                auto typeData = Invalid;
                if (G_IO_ERROR_EXISTS == err->code) {
                    except.dlgType = ED_CONFLICT;
                    Q_EMIT errored(except);
                    typeData = except.respCode;
                } else {
                    except.dlgType = ED_WARNING;
                    Q_EMIT errored(except);
                    typeData = except.respCode;
                }
                handle_type = typeData;
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
                // use custom name
                QString name = "";
                QStringList extendStr = node->destBaseName().split(".");
                if (extendStr.length() > 0) {
                    extendStr.removeAt(0);
                }
                QString endStr = extendStr.join(".");
                if (except.respValue.contains("name")) {
                    name = except.respValue["name"].toString();
                    if (endStr != "" && name.endsWith(endStr)) {
                        node->setDestFileName(name);
                    } else if ("" != endStr && "" != name) {
                        node->setDestFileName(name + "." + endStr);
                    }
                }
                if (FileUtils::isFileExsit(node->destUri())) {
                    handleDuplicate(node);
                }
                auto handledDestFileUri = node->resolveDestFileUri(m_dest_dir_uri);
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                g_file_copy(sourceFile.get()->get(),
                            handledDestFile.get()->get(),
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
                handleDuplicate(node);
                auto handledDestFileUri = node->resolveDestFileUri(m_dest_dir_uri);
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                g_file_copy(sourceFile.get()->get(),
                            handledDestFile.get()->get(),
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
                node->setState(FileNode::Handled);
                cancel();
                break;
            }
            default:
                break;
            }
        } else {
            node->setState(FileNode::Handled);
        }
        m_current_offset += node->size();
        auto fileIconName = FileUtils::getFileIconName(m_current_src_uri, false);
        auto destFileName = FileUtils::isFileDirectory(node->destUri()) ? nullptr : node->destUri();
        Q_EMIT FileProgressCallback(node->uri(), destFileName, fileIconName, m_current_offset, m_total_szie);
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

    if (!m_copy_move) {
        for (auto node : nodes) {
            deleteRecursively(node);
        }
    }

    if (isCancelled())
        Q_EMIT operationStartRollbacked();

    for (auto file : nodes) {
        qDebug()<<file->uri();
        if (isCancelled() && !hasError()) {
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
    int index = 0;
    bool isInvalid = false;
    for (auto srcUri : m_source_uris) {
        auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
        auto destFile = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));
        auto parentFile = wrapGFile(g_file_get_parent(srcFile.get()->get()));
        if (g_file_equal(destFile.get()->get(), parentFile.get()->get())) {
            m_source_uris.removeAt(index);
            --index;
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
        ++index;
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
        FileOperationError except;
        except.errorType = ET_GIO;
        except.dlgType = ED_WARNING;
        except.srcUri = nullptr;
        except.destDirUri = nullptr;
        except.op = FileOpMove;
        except.title = tr("File delete error");
        except.errorCode = G_IO_ERROR_INVAL;
        except.errorStr = tr("Invalid Operation");
        Q_EMIT errored(except);
        auto response = except.respCode;
        switch (response) {
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
        operationStartSnyc();
        QProcess p;
        p.start("sync");
        p.waitForFinished(-1);
    }
    qDebug()<<"finished";
end:
    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}

void FileMoveOperation::cancel()
{
    FileOperation::cancel();
    if (m_reporter)
        m_reporter->cancel();
}
