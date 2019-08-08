#include "file-move-operation.h"
#include "file-node-reporter.h"
#include "file-node.h"

using namespace Peony;

FileMoveOperation::FileMoveOperation(QStringList sourceUris, QString destDirUri, QObject *parent) : FileOperation (parent)
{
    m_source_uris = sourceUris;
    m_dest_dir_uri = destDirUri;
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
    if (p_this->m_force_use_fallback) {
        Q_EMIT p_this->fallbackMoveProgressCallbacked(p_this->m_current_src_uri,
                                                      p_this->m_current_dest_dir_uri,
                                                      current_num_bytes,
                                                      total_num_bytes);
    } else {
        Q_EMIT p_this->nativeMoveProgressCallbacked(p_this->m_current_src_uri,
                                                    p_this->m_current_dest_dir_uri,
                                                    p_this->m_current_count,
                                                    p_this->m_total_count);
    }
    //format: move srcUri to destDirUri: curent_bytes(count) of total_bytes(count).
}

FileOperation::ResponseType FileMoveOperation::prehandle(GError *err)
{
    if (m_prehandle_hash.contains(err->code))
        return m_prehandle_hash.value(err->code);

    return Other;
}

void FileMoveOperation::move()
{
    if (isCancelled())
        return;

    for (auto srcUri : m_source_uris) {
        //FIXME: ignore the total size when using native move.
        addOne(srcUri, 0);
    }
    operationPrepared();

    auto destDir = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));
    m_total_count = m_source_uris.count();
    for (auto srcUri : m_source_uris) {
        if (isCancelled())
            return;

        m_current_count = m_source_uris.indexOf(srcUri) + 1;
        m_current_src_uri = srcUri;
        m_current_dest_dir_uri = m_dest_dir_uri;

        auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
        char *base_name = g_file_get_basename(srcFile.get()->get());
        auto destFile = wrapGFile(g_file_resolve_relative_path(destDir.get()->get(),
                                                               base_name));
        g_free(base_name);
        GError *err = nullptr;

retry:
        g_file_move(srcFile.get()->get(),
                    destFile.get()->get(),
                    m_default_copy_flag,
                    getCancellable().get()->get(),
                    GFileProgressCallback(progress_callback),
                    this,
                    &err);

        if (err) {
            auto errWrapper = GErrorWrapper::wrapFrom(err);
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
                //skip to next loop.
                continue;
            }
            case IgnoreAll: {
                m_prehandle_hash.insert(err->code, IgnoreOne);
                continue;
            }
            case OverWriteOne: {
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
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_BACKUP),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
                break;
            }
            case BackupAll: {
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
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
            //FIXME: ignore the total size when using native move.
            fileMoved(srcUri, 0);
        }
        //native move has not clear operation.
        operationProgressed();
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
        Q_EMIT fallbackMoveProgressCallbacked(m_current_src_uri,
                                              m_current_dest_dir_uri,
                                              0,
                                              node->size());
        g_file_make_directory(destFile.get()->get(),
                              getCancellable().get()->get(),
                              &err);
        if (err) {
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
                break;
            }
            case IgnoreAll: {
                m_prehandle_hash.insert(err->code, IgnoreOne);
                break;
            }
            case OverWriteOne: {
                //make dir has no overwrite
                break;
            }
            case OverWriteAll: {
                m_prehandle_hash.insert(err->code, OverWriteOne);
                break;
            }
            case BackupOne: {
                //make dir has no backup
                break;
            }
            case BackupAll: {
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
        }
        //assume that make dir finished anyway
        Q_EMIT fallbackMoveProgressCallbacked(m_current_src_uri,
                                              m_current_dest_dir_uri,
                                              node->size(),
                                              node->size());
        Q_EMIT fileMoved(node->uri(), node->size());
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
                break;
            }
            case IgnoreAll: {
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
        }
        Q_EMIT fileMoved(node->uri(), node->size());
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
            g_file_delete(file,
                          getCancellable().get()->get(),
                          nullptr);
        }
    } else {
        g_file_delete(file,
                      getCancellable().get()->get(),
                      nullptr);
    }
    g_object_unref(file);
    qDebug()<<"deleted";
    srcFileDeleted(node->uri());
}

void FileMoveOperation::moveForceUseFallback()
{
    if (isCancelled())
        return;

    m_reporter = new FileNodeReporter;
    connect(m_reporter, &FileNodeReporter::nodeFound, this, &FileMoveOperation::addOne);

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
        delete node;
    }

    nodes.clear();
}

void FileMoveOperation::run()
{
    Q_EMIT operationStarted();
    //should block and wait for other object prepared.
    if (!m_force_use_fallback) {
        move();
    } else {
        //FIXME: delete files aslo need time.
        //but the copy is done, that makes operation seems slow.
        moveForceUseFallback();
    }
    qDebug()<<"finished";
    Q_EMIT operationFinished();
}
