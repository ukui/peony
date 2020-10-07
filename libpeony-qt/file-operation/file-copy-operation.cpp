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

#include "file-copy-operation.h"

#include "file-node-reporter.h"
#include "file-node.h"
#include "file-enumerator.h"
#include "file-info.h"

#include "file-utils.h"

#include "file-operation-manager.h"

#include "clipboard-utils.h"
#include <QProcess>
#include <QDebug>

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

FileCopyOperation::FileCopyOperation(QStringList sourceUris, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    QUrl destDirUrl = destDirUri;
    QUrl firstSrcUrl = sourceUris.first();
    if (destDirUrl.isParentOf(firstSrcUrl)) {
        m_is_duplicated_copy = true;
    } else {
        auto lastPasteDirectoryUri = ClipboardUtils::getInstance()->getLastTargetDirectoryUri();
        QUrl lastPasteDirectoryUrl = lastPasteDirectoryUri;
        if (destDirUrl == lastPasteDirectoryUrl) {
            m_is_duplicated_copy = true;
        }
    }

    m_conflict_files.clear();
    m_source_uris = sourceUris;
    m_dest_dir_uri = destDirUri;
    m_reporter = new FileNodeReporter;
    connect(m_reporter, &FileNodeReporter::nodeFound, this, &FileOperation::operationPreparedOne);

    m_info = std::make_shared<FileOperationInfo>(sourceUris, destDirUri, FileOperationInfo::Copy);
}

FileCopyOperation::~FileCopyOperation()
{
    delete m_reporter;
    m_conflict_files.clear();
}

ExceptionResponse FileCopyOperation::prehandle(GError *err)
{
    setHasError(true);

    if (G_IO_ERROR_NO_SPACE == err->code) {
        return Other;
    }

    if (m_is_duplicated_copy && G_IO_ERROR_EXISTS == err->code)
        return BackupAll;

    if (m_prehandle_hash.contains(err->code))
        return m_prehandle_hash.value(err->code);

    return Other;
}

void FileCopyOperation::progress_callback(goffset current_num_bytes,
        goffset total_num_bytes,
        FileCopyOperation *p_this)
{
    if (total_num_bytes < current_num_bytes)
        return;

    auto currnet = p_this->m_current_offset + current_num_bytes;
    auto total = p_this->m_total_szie;
    auto fileIconName = FileUtils::getFileIconName(p_this->m_current_src_uri);
    qDebug()<<currnet*1.0/total;
    Q_EMIT p_this->FileProgressCallback(p_this->m_current_src_uri,
                                        p_this->m_current_dest_dir_uri,
                                        fileIconName, currnet, total);
}

void FileCopyOperation::FileCopyOperationErrDlg(
                        FileOperationError &except,
                        GError *err)
{
    except.errorType = ET_GIO;
    except.srcUri = m_current_src_uri;
    except.destDirUri = m_current_dest_dir_uri;
    except.op = FileOpCopy;
    except.title = tr("File copy error");
    except.errorCode = err->code;
    if (G_IO_ERROR_EXISTS == err->code) {
        except.dlgType = ED_CONFLICT;
    } else {
        except.dlgType = ED_WARNING;
    }

    Q_EMIT errored(except);
}

void FileCopyOperation::getBackupName(
                        FileNode *nodePtr,
                        FileOperationError &except)
{
    QString name = "";
    QStringList extendStr;

    if (except.respValue.contains("name")) {
        /*
        * 这里应该检查名字的合法性：长度、特殊字符等。
        * todo
        */
        name = except.respValue["name"].toString();
        if (name.isEmpty()){
            return;
        }

        extendStr = nodePtr->destBaseName().split(".");
        if (extendStr.length() > 0) {
            extendStr.removeAt(0);
        }
        
        /*
        * 如果文件名中有多个点，比如 111.297.345.txt，文件名后缀
        * 是取txt，还是297.345.txt?
        * todo
        */
        QString endStr = extendStr.join(".");
        if ("" != endStr){
            endStr = "." + endStr;
            if (!name.endsWith(endStr)) {
                nodePtr->setDestFileName(name + endStr);
            }
            else{
                nodePtr->setDestFileName(name);
            }
        }
        else{
            nodePtr->setDestFileName(name);
        } 
    }

    return;
}

ExceptionResponse FileCopyOperation::copyFolder(
                               FileNode *node,
                               GFileWrapperPtr destFile)
{
    GError *err = nullptr;

    //NOTE: mkdir doesn't have a progress callback.
    g_file_make_directory(destFile.get()->get(),
                          getCancellable().get()->get(),
                          &err);
    if (err) {
        if (err->code == G_IO_ERROR_CANCELLED) {
            g_error_free(err);
            err = nullptr;
            return Other;
        }

        FileOperationError except;
        int handle_type = prehandle(err);
        if (handle_type == Other) {
            FileCopyOperationErrDlg(except, err);
            auto typeData = except.respCode;
            handle_type = typeData;
        }

        g_error_free(err);
        err = nullptr;

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
            m_prehandle_hash.insert(except.errorCode, IgnoreOne);
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
            m_prehandle_hash.insert(except.errorCode, OverWriteOne);
            break;
        }
        case BackupOne: {
            node->setState(FileNode::Handled);
            node->setErrorResponse(BackupOne);

            // use custom name
            getBackupName(node, except);
            while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                handleDuplicate(node);
            }

            return Retry;
        }
        case BackupAll: {
            node->setState(FileNode::Handled);
            node->setErrorResponse(BackupOne);

            while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                handleDuplicate(node);
            }

            //make dir has no backup
            m_prehandle_hash.insert(except.errorCode, BackupOne);

            return Retry;
        }
        case Retry: {
            return Retry;
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

    return Other;
}

ExceptionResponse FileCopyOperation::copyFile(
                        FileNode *node,
                        GFileWrapperPtr destFile)
{
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
            g_error_free(err);
            err = nullptr;
            return Other;
        }

        if (err->code == G_IO_ERROR_EXISTS) {
            char* destFileName = g_file_get_uri(destFile.get()->get());
            if (NULL != destFileName) {
                m_conflict_files << destFileName;
                g_free(destFileName);
            }
        }

        int handle_type = prehandle(err);
        if (handle_type == Other) {
            FileCopyOperationErrDlg(except, err);
            auto typeData = except.respCode;
            handle_type = typeData;
        }

       // g_error_free(err);
      //  err = nullptr;

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
            m_prehandle_hash.insert(except.errorCode, IgnoreOne);
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
            m_prehandle_hash.insert(except.errorCode, OverWriteOne);
            break;
        }
        case BackupOne: {
            node->setState(FileNode::Handled);
            node->setErrorResponse(BackupOne);

            // use custom name
            getBackupName(node, except);
            while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                handleDuplicate(node);
            }

            return Retry;
        }
        case BackupAll: {
            node->setState(FileNode::Handled);
            node->setErrorResponse(BackupOne);
            while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                handleDuplicate(node);
            }
            m_prehandle_hash.insert(except.errorCode, BackupOne);
            return Retry;
        }
        case Retry: {
            return Retry;
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

    return Other;
}

void FileCopyOperation::copyRecursively(FileNode *node)
{
    node->setState(FileNode::Handling);

fallback_retry:
    if (isCancelled())
        return;

    QString destFileUri = node->resolveDestFileUri(m_dest_dir_uri);
    QUrl destFileUrl = destFileUri;
    node->setDestUri(destFileUri);
    qDebug()<<"dest file uri:"<<destFileUri;

    GFileWrapperPtr destFile = wrapGFile(g_file_new_for_uri(destFileUri.toUtf8().constData()));

    m_current_src_uri = node->uri();
    m_current_dest_dir_uri = destFileUri;

    if (node->isFolder()) {

        if (Retry == copyFolder(node, destFile))
        {
            goto fallback_retry;
        }

        //assume that make dir finished anyway
        m_current_offset += node->size();
        Q_EMIT operationProgressedOne(node->uri(), node->destUri(), node->size());
        for (auto child : *(node->children())) {
            copyRecursively(child);
        }
    } else {
        if (Retry == copyFile(node, destFile))
        {
            goto fallback_retry;
        }

        m_current_offset += node->size();

        Q_EMIT operationProgressedOne(node->uri(), node->destUri(), node->size());
    }
    destFile.reset();
}

void FileCopyOperation::rollbackNodeRecursively(FileNode *node)
{
    switch (node->state()) {
    case FileNode::Handling:
    case FileNode::Handled: {
        if (node->isFolder()) {
            auto children = node->children();
            for (auto child : *children) {
                rollbackNodeRecursively(child);
            }
            GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
            //FIXME: there's a certain probability of failure to delete the folder without
            //any problem happended. because somehow an empty file will created in the folder.
            //i don't know why, but it is obvious that i have to delete them at first.
            bool is_folder_deleted = g_file_delete(dest_file, nullptr, nullptr);
            if (!is_folder_deleted) {
                FileEnumerator e;
                e.setEnumerateDirectory(node->destUri());
                e.enumerateSync();
                for (auto folder_child : *node->children()) {
                    if (!folder_child->destUri().isEmpty()) {
                        GFile *tmp_file = g_file_new_for_uri(folder_child->destUri().toUtf8().constData());
                        g_file_delete(tmp_file, nullptr, nullptr);
                        g_object_unref(tmp_file);
                    }
                    g_file_delete(dest_file, nullptr, nullptr);
                }
            }
            g_object_unref(dest_file);
        } else {
            if (!m_conflict_files.contains(node->destUri())) {
                GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
                g_file_delete(dest_file, nullptr, nullptr);
                g_object_unref(dest_file);
            }
        }
        operationRollbackedOne(node->destUri(), node->uri());
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

void FileCopyOperation::run()
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
        nodes << node;
    }

    Q_EMIT operationPrepared();

    m_total_szie = *total_size;
    delete total_size;

    for (auto node : nodes) {
        copyRecursively(node);
    }
    Q_EMIT operationProgressed();

    if (isCancelled()) {
        Q_EMIT operationStartRollbacked();
        for (auto file : nodes) {
            qDebug()<<file->uri();
            if (isCancelled()) {
                rollbackNodeRecursively(file);
            }
        }
    }

    setHasError(false);

    for (auto node : nodes) {
        if (!isCancelled())
            m_info->m_node_map.insert(node->uri(), node->destUri());
        delete node;
    }

    nodes.clear();

    // judge if the operation should sync.
    bool needSync = false;
    GFile *src_first_file = g_file_new_for_uri(m_source_uris.first().toUtf8().constData());
    GMount *src_first_mount = g_file_find_enclosing_mount(src_first_file, nullptr, nullptr);
    if (src_first_mount) {
        needSync = g_mount_can_unmount(src_first_mount);
        g_object_unref(src_first_mount);
    } else {
        // maybe a vfs file.
        needSync = true;
    }
    g_object_unref(src_first_file);

    GFile *dest_dir_file = g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData());
    GMount *dest_dir_mount = g_file_find_enclosing_mount(dest_dir_file, nullptr, nullptr);
    if (src_first_mount) {
        needSync = g_mount_can_unmount(dest_dir_mount);
        g_object_unref(dest_dir_mount);
    } else {
        needSync = true;
    }
    g_object_unref(dest_dir_file);

    //needSync = true;

    if (needSync) {
        operationStartSnyc();
        QProcess p;
        p.start("sync");
        p.waitForFinished(-1);
    }

    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}

void FileCopyOperation::cancel()
{
    if (m_reporter)
        m_reporter->cancel();
    FileOperation::cancel();
}
