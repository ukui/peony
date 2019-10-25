#include "file-copy-operation.h"

#include "file-node-reporter.h"
#include "file-node.h"
#include "file-enumerator.h"
#include "file-info.h"

#include "file-utils.h"

#include "file-operation-manager.h"

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

FileCopyOperation::FileCopyOperation(QStringList sourceUris, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    if (sourceUris.first().contains(destDirUri)) {
        m_is_duplicated_copy = true;
    }
    m_source_uris = sourceUris;
    m_dest_dir_uri = destDirUri;
    m_reporter = new FileNodeReporter;
    connect(m_reporter, &FileNodeReporter::nodeFound, this, &FileOperation::operationPreparedOne);

    m_info = std::make_shared<FileOperationInfo>(sourceUris, destDirUri, FileOperationInfo::Copy);
}

FileCopyOperation::~FileCopyOperation()
{
    delete m_reporter;
}

FileOperation::ResponseType FileCopyOperation::prehandle(GError *err)
{
    if (m_is_duplicated_copy)
        return BackupAll;

    if (m_prehandle_hash.contains(err->code))
        return m_prehandle_hash.value(err->code);

    return Other;
}

void FileCopyOperation::progress_callback(goffset current_num_bytes,
                                          goffset total_num_bytes,
                                          FileCopyOperation *p_this)
{
    Q_EMIT p_this->FileProgressCallback(p_this->m_current_src_uri,
                                        p_this->m_current_dest_dir_uri,
                                        current_num_bytes,
                                        total_num_bytes);
}

void FileCopyOperation::copyRecursively(FileNode *node)
{
    if (isCancelled())
        return;

fallback_retry:
    QString destFileUri = node->resoveDestFileUri(m_dest_dir_uri);
    node->setDestUri(destFileUri);
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
                while (FileUtils::isFileExsit(node->resoveDestFileUri(m_dest_dir_uri))) {
                    handleDuplicate(node);
                }
                goto fallback_retry;
            }
            case BackupAll: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                while (FileUtils::isFileExsit(node->resoveDestFileUri(m_dest_dir_uri))) {
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
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                while (FileUtils::isFileExsit(node->resoveDestFileUri(m_dest_dir_uri))) {
                    handleDuplicate(node);
                }
                goto fallback_retry;
            }
            case BackupAll: {
                node->setState(FileNode::Handled);
                node->setErrorResponse(BackupOne);
                while (FileUtils::isFileExsit(node->resoveDestFileUri(m_dest_dir_uri))) {
                    handleDuplicate(node);
                }
                m_prehandle_hash.insert(err->code, BackupOne);
                goto fallback_retry;
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
}

void FileCopyOperation::rollbackNodeRecursively(FileNode *node)
{
    switch (node->state()) {
    case FileNode::Handled: {
        //do not clear the dest file if ignored or overwrite or backuped.
        //FIXME: should i put it after recusion?
        if (node->responseType() != Other)
            break;

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
            GFile *dest_file = g_file_new_for_uri(node->destUri().toUtf8().constData());
            g_file_delete(dest_file, nullptr, nullptr);
            g_object_unref(dest_file);
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
        nodes<<node;
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

    for (auto node : nodes) {
        delete node;
    }

    nodes.clear();

    Q_EMIT operationFinished();
}

void FileCopyOperation::cancel()
{
    if (m_reporter)
        m_reporter->cancel();
    FileOperation::cancel();
}
