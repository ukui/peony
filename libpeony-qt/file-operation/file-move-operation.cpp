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
#include <file-copy.h>

using namespace Peony;

static void handleDuplicate(FileNode *node)
{
    node->setDestFileName(FileUtils::handleDuplicateName(node->destBaseName()));
}

FileMoveOperation::FileMoveOperation(QStringList sourceUris, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    m_source_uris = sourceUris;

    /* favorite://xxx特殊处理,例如本机共享，bug#83353 */
    if (destDirUri.startsWith("favorite://")) {
        destDirUri = FileUtils::getTargetUri(destDirUri);
        if (destDirUri.isEmpty ()) {
            destDirUri = "favorite:///";
        }
    }

    m_dest_dir_uri = FileUtils::urlEncode(destDirUri);
    m_info = std::make_shared<FileOperationInfo>(sourceUris, destDirUri, FileOperationInfo::Move);
}

FileMoveOperation::~FileMoveOperation()
{
    if (m_reporter)
        delete m_reporter;
}

void FileMoveOperation::setCopyMove(bool copyMove)
{
    m_copy_move = copyMove;
}

void FileMoveOperation::setAction(Qt::DropAction action)
{
    m_move_action = action;
    m_info.get()->m_drop_action = action;
    switch (action) {
    case Qt::CopyAction: {
        m_info.get()->m_type = FileOperationInfo::Copy;
        m_info.get()->m_opposite_type = FileOperationInfo::Delete;
        break;
    }
    default: {
        m_info.get()->m_type = FileOperationInfo::Move;
        break;
    }
    }
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
    auto fileIconName = FileUtilsPrivate::getFileIconName(p_this->m_current_src_uri);
    auto destFileName = FileUtils::isFileDirectory(p_this->m_current_dest_dir_uri) ?
                p_this->m_current_dest_dir_uri + "/" + url.fileName() : p_this->m_current_dest_dir_uri;

//    qDebug() << "move: " << p_this->m_current_src_uri << "  ---  "  << destFileName << currnet << "/" << total << (float(currnet) / total);
    Q_EMIT p_this->FileProgressCallback(p_this->m_current_src_uri, destFileName, fileIconName, currnet, total);
    //format: move srcUri to destDirUri: curent_bytes(count) of total_bytes(count).
}

ExceptionResponse FileMoveOperation::prehandle(GError *err)
{
    //setHasError(true);

    /**
     * @note FIX BUG 79363
     * 文件传输中拔掉U盘，第一次报错:G_IO_ERROR_FAILED，后续报错:G_IO_ERROR_NOT_FOUND
     */
    if (G_IO_ERROR_NOT_FOUND == err->code && m_prehandle_hash.contains(G_IO_ERROR_FAILED)) {
        return IgnoreAll;
    }

    if (m_prehandle_hash.contains(err->code))
        return m_prehandle_hash.value(err->code);

    return Other;
}

void FileMoveOperation::move()
{
    if (isCancelled())
        return;

    // 特殊处理 favorite:///
    g_autoptr (GFile) destFile = g_file_new_for_uri (m_dest_dir_uri.toUtf8 ().constData ());
    g_autofree gchar* destSchema = g_file_get_uri_scheme (destFile);

    if (G_IS_FILE (destFile) && destSchema && !g_ascii_strcasecmp (destSchema, "favorite")) {
        for (auto src : m_source_uris) {
            g_autoptr (GFile) srcFile = g_file_new_for_uri (src.toUtf8 ().constData ());
            g_autoptr (GError) error = NULL;
            // 注意：不可能报冲突错误
            g_file_move (srcFile, destFile, m_default_copy_flag, getCancellable().get()->get(), GFileProgressCallback (progress_callback), this, &error);
            if (error) {
                setHasError ();
                FileOperationError except;
                int handle_type = prehandle(error);
                except.errorType = ET_GIO;
                except.op = FileOpMove;
                except.title = tr("Move file error");
                except.errorCode = error->code;
                except.errorStr = error->message;
                except.srcUri = m_current_src_uri;
                except.destDirUri = m_current_dest_dir_uri;
                except.isCritical = false;
                if (handle_type == Other) {
                    if (G_IO_ERROR_EXISTS != error->code) {
                        except.dlgType = ED_WARNING;
                        Q_EMIT errored(except);
                    }
                    // ignore multiple bounces
                    if (except.errorCode == G_IO_ERROR_NOT_SUPPORTED) {
                        m_prehandle_hash.insert(error->code, IgnoreOne);
                    }
                }
            }
        }

        Q_EMIT operationFinished();

        return;
    }

    QList<FileNode*> nodes;
    QList<FileNode*> errNode;

    GError *err = nullptr;
    m_total_count = m_source_uris.count();
    auto destDir = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));

    // file move
    for (auto srcUri : m_source_uris) {
        if (isCancelled())
            return;

        auto node = new FileNode(srcUri, nullptr, nullptr);

        auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
        char *base_name = g_file_get_basename(srcFile.get()->get());
        auto destFile = wrapGFile(g_file_resolve_relative_path(destDir.get()->get(), base_name));
        g_autofree char* destUri = g_file_get_uri(destFile.get()->get());
        node->setDestUri(destUri);

        g_file_move(srcFile.get()->get(),
                    destFile.get()->get(),
                    m_default_copy_flag,
                    getCancellable().get()->get(),
                    GFileProgressCallback(progress_callback), this, &err);
        if (err) {
            errNode << node;
        } else {
            node->setState(FileNode::Handled);
        }

        nodes << node;
    }

    // file copy-delete
    goffset *total_size = new goffset(0);
    for (auto node : errNode) {
        if (isCancelled())
            return;

        node->findChildrenRecursively();
        node->computeTotalSize(total_size);
    }
    m_total_szie = *total_size;
    operationPreparedOne("", m_total_szie);
    delete total_size;

    operationPrepared();

    if (!errNode.isEmpty()) {
        if (m_move_action == Qt::TargetMoveAction) {
            m_info.get()->m_type = FileOperationInfo::Move;
        } else {
            m_info.get()->m_type = FileOperationInfo::Copy;
            m_info.get()->m_opposite_type = FileOperationInfo::Delete;
        }
    }
    for (auto node : errNode) {
        if (isCancelled())
            return;
        moveForceUseFallback(node);
        fileSync(node->uri(), node->destUri());
    }

    operationStartSnyc();

#if 0
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

//retry:
        GError *err = nullptr;
        g_file_move(srcFile.get()->get(),
                    destFile.get()->get(),
                    m_default_copy_flag,
                    getCancellable().get()->get(),
                    GFileProgressCallback(progress_callback),
                    this,
                    &err);
        if (err) {
            errNode << file;
        } else {
            file->setState(FileNode::Handled);
        }

        if (err) {
            setHasError(true);
            auto errWrapper = GErrorWrapper::wrapFrom(err);
            switch (errWrapper.get()->code()) {
            case G_IO_ERROR_CANCELLED: {
                for (auto node : nodes) {
                    delete node;
                }
                nodes.clear();
                return;
            }
            case G_IO_ERROR_NOT_SUPPORTED:
            case G_IO_ERROR_WOULD_RECURSE:
            case G_IO_ERROR_EXISTS: {
                moveForceUseFallback(file);
                operationStartSnyc();
                continue;
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
                while (FileUtils::isFileExsit(file->destUri())) {
                    handleDuplicate(file);
                    file->resolveDestFileUri(m_dest_dir_uri);
                }
                auto handledDestFileUri = file->resolveDestFileUri(m_dest_dir_uri);
                while (FileUtils::isFileExsit(handledDestFileUri))
                {
                    handledDestFileUri = file->resolveDestFileUri(m_dest_dir_uri);
                }
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                g_file_copy(srcFile.get()->get(),
                            handledDestFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            &handled_err);
                setHasError(false);
                break;
            }
            case BackupAll: {
                m_prehandle_hash.insert(err->code, BackupOne);
                goto retry;
                break;
            }
            case Retry: {
                goto retry;
            }
            case Cancel: {
                file->setState(FileNode::Unhandled);
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
        fileSync(file->uri(), file->destUri());
    }
#endif
    //native move has not clear operation.
    operationProgressed();

    //FIXME: if native move function get into error,
    //such as the target is existed, the rollback might
    //get into error too.

    if (isCancelled()) {
        for (auto node : nodes) {
            rollbackNodeRecursively(node);
        }
    }

    //release node
    m_info.get()->m_src_uris.clear();
    m_info.get()->m_dest_uris.clear();
    for (auto file : nodes) {
        m_info.get()->m_src_uris<<file->uri();
        m_info.get()->m_dest_uris<<file->destUri();
        delete file;
    }
    nodes.clear();
}

void FileMoveOperation::rollbackNodeRecursively(FileNode *node)
{
    if (node->isFolder()) {
        if (node->state() == FileNode::Handled) {
            auto dir = wrapGFile(g_file_new_for_uri(node->uri().toUtf8().constData()));
            g_file_make_directory(dir.get()->get(), nullptr, nullptr);
        }
        for (auto child : *node->children()) {
            rollbackNodeRecursively(child);
        }

        if (node->responseType() != OverWriteOne && node->responseType() != OverWriteAll && !isCancelled()) {
            auto destDir = wrapGFile(g_file_new_for_uri(node->destUri().toUtf8().constData()));
            g_file_delete(destDir.get()->get(), nullptr, nullptr);
        }

        operationRollbackedOne(node->destUri(), node->uri());
    } else {
        switch (node->state()) {
        case FileNode::Handled: {
            auto sourceFile = wrapGFile(g_file_new_for_uri(node->uri().toUtf8().constData()));
            auto destFile = wrapGFile(g_file_new_for_uri(node->destUri().toUtf8().constData()));
            if (node->responseType() == OverWriteOne || node->responseType() == OverWriteAll) {
                // note: this won't fully rollback, the file which has been overwriten will not be recovered.
                g_file_copy(destFile.get()->get(), sourceFile.get()->get(), m_default_copy_flag, nullptr, nullptr, nullptr, nullptr);
                break;
            } else {
                g_file_move(destFile.get()->get(), sourceFile.get()->get(), m_default_copy_flag, nullptr, nullptr, nullptr, nullptr);
            }
            break;
        }
        case FileNode::Handling: {
            if (node->responseType() == OverWriteOne || node->responseType() == OverWriteAll || node->responseType() == Cancel) {
                break;
            }
            auto destFile = wrapGFile(g_file_new_for_uri(node->destUri().toUtf8().constData()));
            g_file_delete(destFile.get()->get(), nullptr, nullptr);
            break;
        }
        default:
            break;
        }
    }

//    switch (node->state()) {
//    case FileNode::Handling: {
//        break;
//    }
//    case FileNode::Handled: {
//        //do not clear the dest file if ignored or overwrite or backuped.
//        if (node->responseType() != Other)
//            break;

//        if (node->isFolder()) {
//            auto children = node->children();
//            for (auto child : *children) {
//                rollbackNodeRecursively(child);
//            }
//            GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
//            g_file_delete(dest_file, nullptr, nullptr);
//            g_object_unref(dest_file);
//        } else {
//            GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
//            g_file_delete(dest_file, nullptr, nullptr);
//            g_object_unref(dest_file);
//        }
//        operationRollbackedOne(node->destUri(), node->uri());
//        break;
//    }
//    case FileNode::Cleared: {
//        switch (node->responseType()) {
//        case Other: {
//            if (node->isFolder()) {
//                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
//                g_file_make_directory(src_file, nullptr, nullptr);
//                g_object_unref(src_file);
//                auto children = node->children();
//                for (auto child : *children) {
//                    rollbackNodeRecursively(child);
//                }
//                //try deleting the dest directory
//                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
//                g_file_delete(dest_file, nullptr, nullptr);
//                g_object_unref(dest_file);
//            } else {
//                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
//                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
//                //"rollback"
//                GError *err = nullptr;
//                g_file_move(dest_file,
//                            src_file,
//                            m_default_copy_flag,
//                            nullptr,
//                            nullptr,
//                            nullptr,
//                            &err);
//                if (err) {
//                    qDebug()<<node->destUri();
//                    qDebug()<<node->uri();
//                    qDebug()<<err->message;
//                    g_error_free(err);
//                }
//                g_object_unref(dest_file);
//                g_object_unref(src_file);
//            }
//            operationRollbackedOne(node->destUri(), node->uri());
//            break;
//        }
//        default: {
//            //copy if err handle response type is valid.
//            if (node->isFolder()) {
//                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
//                g_file_make_directory(src_file, nullptr, nullptr);
//                g_object_unref(src_file);
//                auto children = node->children();
//                for (auto child : *children) {
//                    rollbackNodeRecursively(child);
//                }
//                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
//                g_object_unref(dest_file);
//            } else {
//                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
//                GFile *src_file = g_file_new_for_uri(node->uri().toUtf8().constData());
//                //"rollback"
//                GError *err = nullptr;
//                g_file_copy(dest_file,
//                            src_file,
//                            m_default_copy_flag,
//                            nullptr,
//                            nullptr,
//                            nullptr,
//                            &err);
//                if (err) {
//                    qDebug()<<node->destUri();
//                    qDebug()<<node->uri();
//                    qDebug()<<err->message;
//                    g_error_free(err);
//                }
//                g_object_unref(dest_file);
//                g_object_unref(src_file);
//            }
//            operationRollbackedOne(node->destUri(), node->uri());
//            break;
//        }
//        }
//        break;
//    }
//    default: {
//        //make sure all nodes were rollbacked.
//        if (node->isFolder()) {
//            auto children = node->children();
//            for (auto child : *children) {
//                rollbackNodeRecursively(child);
//            }
//        }
//        break;
//    }
//    }
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
    QString destName = "";

fallback_retry:
    if (node->isFolder()) {
        auto realDestUri = node->resolveDestFileUri(m_dest_dir_uri);
        destFile = wrapGFile(g_file_new_for_uri(realDestUri.toUtf8().constData()));
        GError *err = nullptr;
        auto fileIconName = FileUtilsPrivate::getFileIconName(m_current_src_uri);
        auto destFileName = FileUtils::isFileDirectory(m_current_dest_dir_uri) ? nullptr : m_current_dest_dir_uri;
        //NOTE: mkdir doesn't have a progress callback.
        Q_EMIT FileProgressCallback(m_current_src_uri, destFileName, fileIconName, node->size(), node->size());
        g_file_make_directory(destFile.get()->get(),getCancellable().get()->get(), &err);
        if (err) {
            setHasError(true);
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
                //node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                //make dir has no overwrite
                break;
            }
            case OverWriteAll: {
                //node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                m_prehandle_hash.insert(err->code, OverWriteOne);
                break;
            }
            case BackupOne: {
                //node->setState(FileNode::Handled);
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
                node->resolveDestFileUri(m_dest_dir_uri);
                while (FileUtils::isFileExsit(node->destUri())) {
                    handleDuplicate(node);
                    node->resolveDestFileUri(m_dest_dir_uri);
                }
                g_object_unref(destFile.get());
                destFile = wrapGFile(g_file_new_for_uri(node->destUri().toUtf8().constData()));
                setHasError(false);
                goto fallback_retry;
            }
            case BackupAll: {
                m_prehandle_hash.insert(err->code, BackupOne);
                goto fallback_retry;
            }
            case Retry: {
                goto fallback_retry;
            }
            case Cancel: {
                node->setState(FileNode::Unhandled);
                cancel();
                break;
            }
            default:
                break;
            }
        } else {
            //node->setState(FileNode::Handled);
        }

        fileIconName = FileUtilsPrivate::getFileIconName(m_current_src_uri);
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
        auto realDestUri = node->resolveDestFileUri(m_dest_dir_uri);
        destFile = wrapGFile(g_file_new_for_uri(realDestUri.toUtf8().constData()));

        FileCopy fileCopy (node->uri(), realDestUri, m_default_copy_flag,
                           getCancellable().get()->get(),
                           GFileProgressCallback(progress_callback),
                           this,
                           &err);
        fileCopy.connect(this, &FileOperation::operationPause, &fileCopy, &FileCopy::pause, Qt::DirectConnection);
        fileCopy.connect(this, &FileOperation::operationResume, &fileCopy, &FileCopy::resume, Qt::DirectConnection);
        fileCopy.connect(this, &FileOperation::operationCancel, &fileCopy, &FileCopy::cancel, Qt::DirectConnection);
        if (m_is_pause) fileCopy.pause();
        fileCopy.run();

        if (err) {
            setHasError(true);
            switch (err->code) {
            case G_IO_ERROR_CANCELLED:
                return;
            case G_IO_ERROR_INVALID_FILENAME: {
                QString newDestUri;
                if (makeFileNameValidForDestFS(m_current_src_uri, m_dest_dir_uri, &newDestUri)) {
                    if (newDestUri != destName) {
                        destName = newDestUri;
                        node->setDestFileName(newDestUri);
                        goto fallback_retry;
                    }
                }
                break;
            }
            }

            FileOperationError except;
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
                FileCopy fileCopy (node->uri(), realDestUri, GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_OVERWRITE),
                                   getCancellable().get()->get(),
                                   GFileProgressCallback(progress_callback),
                                   this,
                                   &err);
                fileCopy.connect(this, &FileOperation::operationPause, &fileCopy, &FileCopy::pause, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationResume, &fileCopy, &FileCopy::resume, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationCancel, &fileCopy, &FileCopy::cancel, Qt::DirectConnection);
                if (m_is_pause) fileCopy.pause();
                fileCopy.run();
                node->setErrorResponse(OverWriteOne);
                break;
            }
            case OverWriteAll: {
//                g_file_copy(sourceFile.get()->get(),
//                            destFile.get()->get(),
//                            GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_OVERWRITE),
//                            getCancellable().get()->get(),
//                            GFileProgressCallback(progress_callback),
//                            this,
//                            nullptr);
                FileCopy fileCopy (node->uri(), realDestUri, GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_OVERWRITE),
                                   getCancellable().get()->get(),
                                   GFileProgressCallback(progress_callback),
                                   this,
                                   &err);
                fileCopy.connect(this, &FileOperation::operationPause, &fileCopy, &FileCopy::pause, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationResume, &fileCopy, &FileCopy::resume, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationCancel, &fileCopy, &FileCopy::cancel, Qt::DirectConnection);
                if (m_is_pause) fileCopy.pause();
                fileCopy.run();
                //node->setState(FileNode::Handled);
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
                while (FileUtils::isFileExsit(node->destUri())) {
                    handleDuplicate(node);
                    node->resolveDestFileUri(m_dest_dir_uri);
                }
                auto handledDestFileUri = node->resolveDestFileUri(m_dest_dir_uri);
                auto handledDestFile = wrapGFile(g_file_new_for_uri(handledDestFileUri.toUtf8()));
                FileCopy fileCopy (node->uri(), realDestUri, GFileCopyFlags(m_default_copy_flag | G_FILE_COPY_BACKUP),
                                   getCancellable().get()->get(),
                                   GFileProgressCallback(progress_callback),
                                   this,
                                   &err);
                fileCopy.connect(this, &FileOperation::operationPause, &fileCopy, &FileCopy::pause, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationResume, &fileCopy, &FileCopy::resume, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationCancel, &fileCopy, &FileCopy::cancel, Qt::DirectConnection);
                if (m_is_pause) fileCopy.pause();
                fileCopy.run();
                //node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                setHasError(false);
                break;
            }
            case BackupAll: {
                m_prehandle_hash.insert(err->code, BackupOne);
                goto fallback_retry;
                break;
            }
            case Retry: {
                goto fallback_retry;
            }
            case Cancel: {
                node->setErrorResponse(Cancel);
                cancel();
                break;
            }
            default:
                break;
            }
        }
        fileSync(node->uri(), realDestUri);
        m_current_offset += node->size();
        auto fileIconName = FileUtilsPrivate::getFileIconName(m_current_src_uri);
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

    g_autoptr(GFile) file = g_file_new_for_uri(node->uri().toUtf8().constData());
    if (node->isFolder()) {
        for (auto child : *(node->children())) {
            deleteRecursively(child);
        }
        if (node->state() != FileNode::Unhandled) {
            g_file_delete(file, getCancellable().get()->get(), nullptr);
            node->setState(FileNode::Handled);
        }
    } else {
        if (node->state() != FileNode::Unhandled) {
            g_file_delete(file, getCancellable().get()->get(), nullptr);
            node->setState(FileNode::Handled);
        }
    }
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

    if (m_move_action == Qt::TargetMoveAction) {
        m_info.get()->m_type = FileOperationInfo::Move;
        for (auto node : nodes) {
            deleteRecursively(node);
        }
    } else {
        m_info.get()->m_type = FileOperationInfo::Copy;
        m_info.get()->m_opposite_type = FileOperationInfo::Delete;
    }

    if (isCancelled())
        Q_EMIT operationStartRollbacked();

    for (auto file : nodes) {
        qDebug()<<file->uri();
        if (isCancelled()) {
            rollbackNodeRecursively(file);
        }
    }

    m_info.get()->m_src_uris.clear();
    m_info.get()->m_dest_uris.clear();
    for (auto node : nodes) {
        m_info.get()->m_src_uris<<node->uri();
        m_info.get()->m_dest_uris<<node->destUri();
        delete node;
    }

    nodes.clear();
}

void FileMoveOperation::moveForceUseFallback(FileNode* node)
{
    if (isCancelled() || nullptr == node)
        return;

    operationPrepared();

    copyRecursively(node);

    if (isCancelled()) {
        Q_EMIT operationStartRollbacked();
    }

    if (m_move_action == Qt::TargetMoveAction) {
        deleteRecursively(node);
    }

    node->setState(FileNode::Handled);

    if (isCancelled()) {
        rollbackNodeRecursively(node);
    }
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
    //can not move StandardPath to any dir. see:#87912
    if (FileUtils::containsStandardPath(m_source_uris)) {
        isInvalid = true;
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
    move();
//    if (!m_force_use_fallback) {
//        move();
//    }


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
