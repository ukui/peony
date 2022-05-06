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
#include "file-copy.h"
#include <gio/gdesktopappinfo.h>

using namespace Peony;

static void handleDuplicate(FileNode *node)
{
    node->setDestFileName(FileUtils::handleDuplicateName(node->destBaseName()));
}

FileCopyOperation::FileCopyOperation(QStringList sourceUris, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    QUrl destDirUrl = Peony::FileUtils::urlEncode(destDirUri);
    QUrl firstSrcUrl = Peony::FileUtils::urlEncode(sourceUris.first());

    if (destDirUrl.isParentOf(firstSrcUrl)) {
        m_is_duplicated_copy = true;
    }/* else {
        // fix #83068
        // windows里的重复复制操作没有备份选项，但是会一直弹框提示，这里和windows的行为靠拢
        auto lastPasteDirectoryUri = ClipboardUtils::getInstance()->getLastTargetDirectoryUri();
        QUrl lastPasteDirectoryUrl = Peony::FileUtils::urlEncode(lastPasteDirectoryUri);
        if (destDirUrl == lastPasteDirectoryUrl) {
            m_is_duplicated_copy = true;
        }
    }*/

    m_conflict_files.clear();
    m_source_uris = sourceUris;
    m_dest_dir_uri = FileUtils::urlDecode(destDirUri);
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

    switch (err->code) {
        case G_IO_ERROR_BUSY:
        case G_IO_ERROR_PENDING:
        case G_IO_ERROR_NO_SPACE:
        case G_IO_ERROR_CANCELLED:
        case G_IO_ERROR_INVALID_DATA:
        case G_IO_ERROR_NOT_SUPPORTED:
        case G_IO_ERROR_PERMISSION_DENIED:
        case G_IO_ERROR_CANT_CREATE_BACKUP:
        case G_IO_ERROR_TOO_MANY_OPEN_FILES:
            return Other;
    }

    /**
     * @note FIX BUG 79363
     * 文件传输中拔掉U盘，第一次报错:G_IO_ERROR_FAILED，后续报错:G_IO_ERROR_NOT_FOUND
     */
    if (G_IO_ERROR_NOT_FOUND == err->code && m_prehandle_hash.contains(G_IO_ERROR_FAILED)) {
        return IgnoreAll;
    }

    if (G_IO_ERROR_EXISTS == err->code && m_is_duplicated_copy) {
        return BackupAll;
    }

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

    QUrl url(Peony::FileUtils::urlEncode(p_this->m_current_src_uri));
    auto currnet = p_this->m_current_offset + current_num_bytes;
    auto total = p_this->m_total_szie;
    auto fileIconName = FileUtilsPrivate::getFileIconName(p_this->m_current_src_uri);
    auto destFileName = FileUtils::isFileDirectory(p_this->m_current_dest_dir_uri) ?
                p_this->m_current_dest_dir_uri + "/" + url.fileName() : p_this->m_current_dest_dir_uri;
//    qDebug()<<currnet*1.0/total;
    Q_EMIT p_this->FileProgressCallback(p_this->m_current_src_uri, destFileName, fileIconName, currnet, total);
}

void FileCopyOperation::copyRecursively(FileNode *node)
{
    if (isCancelled())
        return;

    node->setState(FileNode::Handling);
    QString destName = "";

fallback_retry:
    QString destFileUri = node->resolveDestFileUri(m_dest_dir_uri);
    //QUrl destFileUrl = Peony::FileUtils::urlEncode(destFileUri);
    destFileUri = FileUtils::urlEncode(destFileUri);
    node->setDestUri(destFileUri);
    QString srcUri = node->uri();
    qDebug()<<"dest file uri:"<<destFileUri;

    GFileWrapperPtr destFile = wrapGFile(g_file_new_for_uri(destFileUri.toUtf8().constData()));

    m_current_src_uri = node->uri();
    m_current_dest_dir_uri = destFileUri;

    if (node->isFolder()) {
        GError *err = nullptr;

        //NOTE: mkdir doesn't have a progress callback.
        g_file_make_directory(destFile.get()->get(),
                              getCancellable().get()->get(),
                              &err);
        if (err) {
            FileOperationError except;
            if (err->code == G_IO_ERROR_CANCELLED) {
                return;
            }
            auto errWrapperPtr = GErrorWrapper::wrapFrom(err);
            int handle_type = prehandle(err);
            except.errorType = ET_GIO;
            except.srcUri = m_current_src_uri;
            except.destDirUri = m_current_dest_dir_uri;
            except.op = FileOpCopy;
            except.title = tr("File copy error");
            except.errorCode = err->code;
            if (handle_type == Other) {
                if (G_IO_ERROR_EXISTS == err->code) {
                    except.dlgType = ED_CONFLICT;
                    Q_EMIT errored(except);
                    auto typeData = except.respCode;
                    handle_type = typeData;
                } else {
                    except.dlgType = ED_WARNING;
                    Q_EMIT errored(except);
                    auto typeData = except.respCode;
                    handle_type = typeData;
                }
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
                    }
                }
                while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                    handleDuplicate(node);
                }
                goto fallback_retry;
            }
            case BackupAll: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                    handleDuplicate(node);
                }
                //make dir has no backup
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
            node->setState(FileNode::Handled);
        }
        //assume that make dir finished anyway
        m_current_offset += node->size();
        Q_EMIT operationProgressedOne(node->uri(), node->destUri(), node->size());
        for (auto child : *(node->children())) {
            copyRecursively(child);
        }
    } else {
        GError *err = nullptr;
        QUrl url = node->uri();

        bool is_desktop_file = false;
        g_autoptr(GFile)        src = g_file_new_for_uri(node->uri().toUtf8().constData());
        g_autoptr(GFileInfo) srcInfo = nullptr;
        if (src) {
            srcInfo = g_file_query_info(src, "unix::*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
        }

        if (url.isLocalFile() && node->uri().endsWith(".desktop")) {
            GDesktopAppInfo* desktop_info = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
            if (G_IS_DESKTOP_APP_INFO(desktop_info)) {
                is_desktop_file = true;

                GKeyFile* key_file = g_key_file_new();
                QRegExp regExp (QString("\\ -\\ %1\\(\\d+\\)(\\.[0-9a-zA-Z\\.]+|)$").arg(QObject::tr("duplicate")));
                g_key_file_load_from_file(key_file, url.path().toUtf8().constData(), G_KEY_FILE_KEEP_COMMENTS, nullptr);
                QString locale_name = QLocale::system().name();
                QString local_generic_name_key = QString("Name[%1]").arg(locale_name);
                GError* error = NULL;
                if (g_key_file_has_key(key_file, G_KEY_FILE_DESKTOP_GROUP, local_generic_name_key.toUtf8().constData(), nullptr)) {
                    g_autofree char* val = g_key_file_get_value(key_file, G_KEY_FILE_DESKTOP_GROUP, local_generic_name_key.toUtf8().constData(), &error);
                    if (error) {
                        qWarning() << "get desktop file:" << node->uri() << " name error:" << error->code << " -- " << error->message;
                        g_error_free(error);
                        error = nullptr;
                    } else {
                        if (node->destBaseName().contains(regExp)) {
                            QString name1 = regExp.cap(0).replace(".desktop", "");
                            QString name = QString("%1%2.desktop").arg(val).arg(name1);
                            node->setDestFileName(name);
                        } else {
                            QString name = QString("%1 - %2(1).desktop").arg(val).arg(QObject::tr("duplicate"));
                            node->setDestFileName(name);
                        }
                    }
                } else if (g_key_file_has_key(key_file, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, nullptr)) {
                    g_autofree char* val = g_key_file_get_value(key_file, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, &error);
                    if (error) {
                        qWarning() << "get desktop file:" << node->uri() << " name error:" << error->code << " -- " << error->message;
                        g_error_free(error);
                        error = nullptr;
                    } else {
                        if (node->destBaseName().contains(regExp)) {
                            QString name1 = regExp.cap(0).replace(".desktop", "");
                            QString name = QString("%1%2.desktop").arg(val).arg(name1);
                            node->setDestFileName(name);
                        } else {
                            QString name = QString("%1 - %2(1).desktop").arg(val).arg(QObject::tr("duplicate"));
                            node->setDestFileName(name);
                        }
                    }
                }

                g_key_file_free(key_file);
                g_object_unref(desktop_info);
            }
        }


        FileCopy fileCopy (node->uri(), destFileUri, m_default_copy_flag,
                           getCancellable().get()->get(),
                           GFileProgressCallback(progress_callback),
                           this,
                           &err);
        if (m_is_pause) fileCopy.pause();
        fileCopy.connect(this, &FileOperation::operationPause, &fileCopy, &FileCopy::pause, Qt::DirectConnection);
        fileCopy.connect(this, &FileOperation::operationResume, &fileCopy, &FileCopy::resume, Qt::DirectConnection);
        fileCopy.connect(this, &FileOperation::operationCancel, &fileCopy, &FileCopy::cancel, Qt::DirectConnection);
        if (m_is_pause) fileCopy.pause();
        fileCopy.run();
        if (err) {
            switch (err->code) {
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
            case G_IO_ERROR_CANCELLED:
                return;
            case G_IO_ERROR_EXISTS:
                char* destFileName = g_file_get_uri(destFile.get()->get());
                if (NULL != destFileName) {
                    m_conflict_files << destFileName;
                    g_free(destFileName);
                }
                break;
            }

            FileOperationError except;
            auto errWrapperPtr = GErrorWrapper::wrapFrom(err);
            int handle_type = prehandle(err);
            except.errorType = ET_GIO;
            except.op = FileOpCopy;
            except.title = tr("File copy error");
            except.srcUri = m_current_src_uri;
            except.errorCode = err->code;
            except.errorStr = err->message;
            except.destDirUri = m_current_dest_dir_uri;

            //
            if (err->code == G_IO_ERROR_PERMISSION_DENIED) {
                except.errorStr = tr("Cannot opening file, permission denied!");
            }

            if (handle_type == Other) {
                if (G_IO_ERROR_EXISTS == err->code) {
                    except.dlgType = ED_CONFLICT;
                    Q_EMIT errored(except);
                    auto typeData = except.respCode;
                    qDebug()<<"get return";
                    handle_type = typeData;
                } else {
                    except.dlgType = ED_WARNING;
                    Q_EMIT errored(except);
                    auto typeData = except.respCode;
                    qDebug()<<"get return";
                    handle_type = typeData;
                }
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
                FileCopy fileOverWriteOneCopy (node->uri(), destFileUri,
                                   (GFileCopyFlags)(m_default_copy_flag | G_FILE_COPY_OVERWRITE),
                                   getCancellable().get()->get(),
                                   GFileProgressCallback(progress_callback),
                                   this,
                                   nullptr);
                fileCopy.connect(this, &FileOperation::operationPause, &fileOverWriteOneCopy, &FileCopy::pause, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationResume, &fileOverWriteOneCopy, &FileCopy::resume, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationCancel, &fileOverWriteOneCopy, &FileCopy::cancel, Qt::DirectConnection);
                if (m_is_pause) fileOverWriteOneCopy.pause();
                fileOverWriteOneCopy.run();
                node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                m_is_duplicated_copy = false;
                break;
            }
            case OverWriteAll: {
                FileCopy fileOverWriteOneCopy (node->uri(), destFileUri,
                                   (GFileCopyFlags)(m_default_copy_flag | G_FILE_COPY_OVERWRITE),
                                   getCancellable().get()->get(),
                                   GFileProgressCallback(progress_callback),
                                   this,
                                   nullptr);
                fileCopy.connect(this, &FileOperation::operationPause, &fileOverWriteOneCopy, &FileCopy::pause, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationResume, &fileOverWriteOneCopy, &FileCopy::resume, Qt::DirectConnection);
                fileCopy.connect(this, &FileOperation::operationCancel, &fileOverWriteOneCopy, &FileCopy::cancel, Qt::DirectConnection);
                if (m_is_pause) fileOverWriteOneCopy.pause();
                fileOverWriteOneCopy.run();
                node->setState(FileNode::Handled);
                node->setErrorResponse(OverWriteOne);
                m_prehandle_hash.insert(err->code, OverWriteOne);
                m_is_duplicated_copy = false;
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
                    }
                }

                while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                    handleDuplicate(node);
                }
                goto fallback_retry;
            }
            case BackupAll: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                while (FileUtils::isFileExsit(node->resolveDestFileUri(m_dest_dir_uri))) {
                    handleDuplicate(node);
                }
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
            node->setState(FileNode::Handled);
        }

        if (is_desktop_file) {
            QUrl url(node->destUri());
            QRegExp regExp (QString("\\ -\\ %1\\(\\d+\\)(\\.[0-9a-zA-Z\\.]+|)$").arg(QObject::tr("duplicate")));
            GDesktopAppInfo *desktop_info = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
            if (G_IS_DESKTOP_APP_INFO(desktop_info)) {
                GKeyFile *key_file = g_key_file_new();
                g_key_file_load_from_file(key_file, url.path().toUtf8().constData(), G_KEY_FILE_KEEP_COMMENTS, nullptr);
                QString locale_name = QLocale::system().name();
                QString ext;
                QString local_generic_name_key = QString("Name[%1]").arg(locale_name);

                g_autofree char* nameStr = nullptr;

                if (node->destBaseName().contains(regExp)) {
                    ext = regExp.cap(0).replace(".desktop", "");
                }

                if (g_key_file_has_key(key_file, G_KEY_FILE_DESKTOP_GROUP, local_generic_name_key.toUtf8().constData(), nullptr)) {
                    nameStr = g_key_file_get_value(key_file, G_KEY_FILE_DESKTOP_GROUP, local_generic_name_key.toUtf8().constData(), nullptr);
                    g_key_file_set_value(key_file, G_KEY_FILE_DESKTOP_GROUP, local_generic_name_key.toUtf8().constData(), QString(nameStr + ext).toUtf8().constData());
                } else {
                    nameStr = g_key_file_get_value(key_file, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, nullptr);
                    g_key_file_set_value(key_file, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, QString(nameStr + ext).toUtf8().constData());
                }

                qDebug() << "set desktop name:" << nameStr + ext << "  -- " << url.path();

                GError*         error = NULL;
                g_key_file_save_to_file(key_file, url.path().toUtf8().constData(), &error);
                if (error) {
                    qWarning() << "save file error:" << error->code << "  --  " << error->message;
                    g_error_free(error);
                    error = nullptr;
                }
                g_key_file_free(key_file);
                g_object_unref(desktop_info);

                if (nullptr != srcInfo) {
                    g_autoptr(GFile) destFile = g_file_new_for_uri(node->destUri().toUtf8().constData());
                    if (destFile) {
                        g_file_set_attribute_uint32(destFile, G_FILE_ATTRIBUTE_UNIX_MODE, g_file_info_get_attribute_uint32(srcInfo, G_FILE_ATTRIBUTE_UNIX_MODE), G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
                    }
                }
            } else {
                qDebug() << "desktop file:" << node->destUri() << " is wrong";
            }
        }


        m_current_offset += node->size();
        fileSync(srcUri, destFileUri);
        Q_EMIT operationProgressedOne(node->uri(), node->destUri(), node->size());

        if(srcUri.endsWith(".dsps") && destFileUri.endsWith(".dsps")){
            m_srcUrisOfCopyDspsFiles.append(FileUtils::urlDecode(srcUri));
            m_destUrisOfCopyDspsFiles.append(FileUtils::urlDecode(destFileUri));
        }
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
        qDebug() << "copy uri:" << uri;

        QString szTempUri = uri;
        if(szTempUri.startsWith("filesafe:///") && szTempUri.remove("filesafe:///").indexOf("/") == -1) {
            continue;
        }

        FileNode *node = new FileNode(uri, nullptr, m_reporter);
        node->findChildrenRecursively();
        node->computeTotalSize(total_size);
        nodes << node;
    }

    Q_EMIT operationPrepared();

    m_total_szie = *total_size;
    delete total_size;

    m_srcUrisOfCopyDspsFiles.clear();
    m_destUrisOfCopyDspsFiles.clear();

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

    m_info->m_dest_uris = m_info->m_node_map.values();

    nodes.clear();

    Q_EMIT operationFinished();

    sendSrcAndDestUrisOfCopyDspsFiles();
}

void FileCopyOperation::cancel()
{
    if (m_reporter) {
        m_reporter->cancel();
    }

    ClipboardUtils::popLastTargetDirectoryUri(m_dest_dir_uri);

    FileOperation::cancel();
}
