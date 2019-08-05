#include "file-move-operation.h"

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
    auto destFile = wrapGFile(g_file_new_for_uri(m_dest_uri.toUtf8().constData()));
    for (auto srcUri : m_source_uris) {
        if (isCancelled())
            return;
        auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
        GError *err = nullptr;

retry:
        g_file_move(srcFile.get()->get(),
                    destFile.get()->get(),
                    G_FILE_COPY_NONE,
                    getCancellable().get()->get(),
                    GFileProgressCallback(progress_callback),
                    this,
                    &err);

        if (!m_ignore_all_errors && !m_overwrite_all_duplicated && !m_backup_all_duplicated) {
            if (err) {
                auto responseType = Q_EMIT errored(GErrorWrapper::wrapFrom(err));
                //block until error has been handled.
                switch (responseType) {
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
                                G_FILE_COPY_OVERWRITE,
                                getCancellable().get()->get(),
                                GFileProgressCallback(progress_callback),
                                this,
                                nullptr);
                    break;
                }
                case OverWriteAll: {
                    g_file_move(srcFile.get()->get(),
                                destFile.get()->get(),
                                G_FILE_COPY_OVERWRITE,
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
                                G_FILE_COPY_BACKUP,
                                getCancellable().get()->get(),
                                GFileProgressCallback(progress_callback),
                                this,
                                nullptr);
                    break;
                }
                case BackupAll: {
                    g_file_move(srcFile.get()->get(),
                                destFile.get()->get(),
                                G_FILE_COPY_BACKUP,
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
                            G_FILE_COPY_OVERWRITE,
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
            } else {
                g_file_move(srcFile.get()->get(),
                            destFile.get()->get(),
                            G_FILE_COPY_BACKUP,
                            getCancellable().get()->get(),
                            GFileProgressCallback(progress_callback),
                            this,
                            nullptr);
            }
        }
    }
}

void FileMoveOperation::run()
{
    if (!m_force_use_fallback) {
        move();
    } else {
        moveForceUseFallback();
    }
}
