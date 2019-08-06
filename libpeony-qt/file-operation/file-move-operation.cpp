#include "file-move-operation.h"
#include "file-node-reporter.h"
#include "file-node.h"

using namespace Peony;

FileMoveOperation::FileMoveOperation(QStringList sourceUris, QString destUri, QObject *parent) : FileOperation (parent)
{
    m_source_uris = sourceUris;
    m_dest_uri = destUri;
}

void FileMoveOperation::progress_callback(goffset current_num_bytes,
                                          goffset total_num_bytes,
                                          FileMoveOperation *p_this)
{
    //FIXME: do i need compute total size at first anyway?
    Q_UNUSED(total_num_bytes);
    p_this->m_current_offset = p_this->m_total_szie + current_num_bytes;
}

void FileMoveOperation::move()
{
    auto destDir = wrapGFile(g_file_new_for_uri(m_dest_uri.toUtf8().constData()));
    for (auto srcUri : m_source_uris) {
        if (isCancelled())
            return;
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

        if (!m_ignore_all_errors && !m_overwrite_all_duplicated && !m_backup_all_duplicated) {
            if (err) {
                auto responseTypeWrapper = Q_EMIT errored(GErrorWrapper::wrapFrom(err));
                //block until error has been handled.
                switch (responseTypeWrapper.value<ResponseType>()) {
                case IgnoreOne: {
                    //skip to next loop.
                    continue;
                }
                case IgnoreAll: {
                    m_ignore_all_errors = true;
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
                    m_overwrite_all_duplicated = true;
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
                    m_backup_all_duplicated = true;
                    break;
                }
                case Retry: {
                    goto retry;
                }
                case Cancel: {
                    cancel();
                    break;
                }
                }
            }
        } else {
            if (m_ignore_all_errors) {
                continue;
            } else if (m_overwrite_all_duplicated) {
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
            } else {
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
            }
        }
    }
}

void FileMoveOperation::copyRecursively(FileNode *node)
{
    if (isCancelled())
        return;

    QString relativePath = node->getRelativePath();
    GFileWrapperPtr destRoot = wrapGFile(g_file_new_for_uri(m_dest_uri.toUtf8().constData()));
    GFileWrapperPtr destFile = wrapGFile(g_file_resolve_relative_path(destRoot.get()->get(),
                                                                      relativePath.toUtf8().constData()));
    if (node->isFolder()) {
        GError *err = nullptr;
        g_file_make_directory(destFile.get()->get(),
                              getCancellable().get()->get(),
                              &err);
        if (err) {
            auto typeData = errored(GErrorWrapper::wrapFrom(err));
            ResponseType type = typeData.value<ResponseType>();
            //handle.
        }
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
            auto typeData = errored(GErrorWrapper::wrapFrom(err));
            ResponseType type = typeData.value<ResponseType>();
            //handle.
        }
    }
}

void FileMoveOperation::deleteRecursively(FileNode *node)
{
    if (isCancelled())
        return;

    GFileWrapperPtr file = wrapGFile(g_file_new_for_uri(node->uri().toUtf8().constData()));
    if (node->isFolder()) {
        for (auto child : *(node->children())) {
            deleteRecursively(child);
            g_file_delete(file.get()->get(),
                          getCancellable().get()->get(),
                          nullptr);
        }
    } else {
        g_file_delete(file.get()->get(),
                      getCancellable().get()->get(),
                      nullptr);
    }
}

void FileMoveOperation::moveForceUseFallback()
{
    if (isCancelled())
        return;

    QList<FileNode*> nodes;
    for (auto uri : m_source_uris) {
        FileNode *node = new FileNode(uri, nullptr, m_reporter);
        node->findChildrenRecursively();
        nodes<<node;
    }

    if (m_reporter)
        m_reporter->enumerateNodeFinished();

    for (auto node : nodes) {
        copyRecursively(node);
        deleteRecursively(node);
        delete node;
    }
    nodes.clear();
}

void FileMoveOperation::run()
{
    if (!m_force_use_fallback) {
        move();
    } else {
        moveForceUseFallback();
    }
    qDebug()<<"finished";
}
