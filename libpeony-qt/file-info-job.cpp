#include "file-info-job.h"

#include "file-info.h"

#include "file-info-manager.h"

#include <QDebug>
#include <QDateTime>

using namespace Peony;

FileInfoJob::FileInfoJob(std::shared_ptr<FileInfo> info, QObject *parent) : QObject(parent)
{
    m_info = info;
    connect(m_info.get(), &FileInfo::updated, this, &FileInfoJob::infoUpdated);
}

FileInfoJob::FileInfoJob(const QString &uri, QObject *parent) : QObject (parent)
{
    auto info = FileInfo::fromUri(uri);
    m_info = info;
    connect(m_info.get(), &FileInfo::updated, this, &FileInfoJob::infoUpdated);
}

/*!
 * \brief FileInfoJob::~FileInfoJob
 * <br>
 * For FileInfo having a handle shared by all classes instance,
 * the shared data first is created in a global hash hold by
 * Peony::FileInfoManager instance. This makes shared_ptr
 * ref count always more than 1.
 * </br>
 * <br>
 * At this class we aslo hold a shared data from global hash
 * (though we aslo change its content here).
 * When info job instance in destructor, it means shared data
 * use count will reduce 1 soon. If current use count is 2,
 * that means there won't be another holder expect global hash.
 * So we can remove the last shared_ptr in the hash, then FileInfo
 * instance will be removed automaticly.
 * </br>
 * \see FileInfo::~FileInfo(), FileEnumerator::~FileEnumerator().
 */
FileInfoJob::~FileInfoJob()
{
    //qDebug()<<"~Job"<<m_info.use_count();
    if (m_info.use_count() <= 2) {
        Peony::FileInfoManager *mgr = Peony::FileInfoManager::getInstance();
        mgr->remove(m_info);
    }
}

void FileInfoJob::cancel()
{
    //NOTE: do not use same cancellble for cancelling, otherwise all job might be cancelled.
    g_cancellable_cancel(m_info->m_cancellable);
    g_object_unref(m_info->m_cancellable);
    m_info->m_cancellable = g_cancellable_new();
}

bool FileInfoJob::querySync()
{
    FileInfo *info = nullptr;
    if (auto data = m_info.get()) {
        info = data;
    } else {
        if (m_auto_delete)
            deleteLater();
        return false;
    }
    GError *err = nullptr;

    auto _info = g_file_query_info(info->m_file,
                                   "standard::*," "time::*," "access::*," G_FILE_ATTRIBUTE_ID_FILE,
                                   G_FILE_QUERY_INFO_NONE,
                                   nullptr,
                                   &err);

    if (err) {
        qDebug()<<err->code<<err->message;
        g_error_free(err);
        if (m_auto_delete)
            deleteLater();
        return false;
    }

    refreshInfoContents(_info);
    g_object_unref(_info);
    if (m_auto_delete)
        deleteLater();
    return true;
}

GAsyncReadyCallback FileInfoJob::query_info_async_callback(GFile *file, GAsyncResult *res, FileInfoJob *thisJob)
{
    //qDebug()<<"query_info_async_callback"<<thisJob->m_info->uri();

    GError *err = nullptr;

    GFileInfo *_info = g_file_query_info_finish(file,
                                                res,
                                                &err);

    if (_info != nullptr) {
        thisJob->refreshInfoContents(_info);
        g_object_unref(_info);
        Q_EMIT thisJob->queryAsyncFinished(true);
    }
    else {
        qDebug()<<err->code<<err->message;
        g_error_free(err);
        Q_EMIT thisJob->queryAsyncFinished(false);
        return nullptr;
    }

    return nullptr;
}

void FileInfoJob::queryAsync()
{
    FileInfo *info = nullptr;
    if (auto data = m_info) {
        info = data.get();
        cancel();
    } else {
        Q_EMIT queryAsyncFinished(false);
        return;
    }
    g_file_query_info_async(info->m_file,
                            "standard::*," "time::*," "access::*," G_FILE_ATTRIBUTE_ID_FILE,
                            G_FILE_QUERY_INFO_NONE,
                            G_PRIORITY_DEFAULT,
                            info->m_cancellable,
                            GAsyncReadyCallback(query_info_async_callback),
                            this);

    if (m_auto_delete)
        connect(this, &FileInfoJob::queryAsyncFinished, this, &FileInfoJob::deleteLater, Qt::QueuedConnection);
}

void FileInfoJob::refreshInfoContents(GFileInfo *new_info)
{
    m_info->m_mutex.lock();
    FileInfo *info = nullptr;
    if (auto data = m_info) {
        info = data.get();
    } else {
        return;
    }
    GFileType type = g_file_info_get_file_type (new_info);
    switch (type) {
    case G_FILE_TYPE_DIRECTORY:
        //qDebug()<<"dir";
        info->m_is_dir = true;
        break;
    case G_FILE_TYPE_MOUNTABLE:
        //qDebug()<<"mountable";
        info->m_is_volume = true;
        break;
    default:
        break;
    }

    info->m_is_symbol_link = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK);
    info->m_can_read = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
    info->m_can_write = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
    info->m_can_excute = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_EXECUTE);
    info->m_can_delete = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_DELETE);
    info->m_can_trash = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_TRASH);
    info->m_can_rename = g_file_info_get_attribute_boolean(new_info, G_FILE_ATTRIBUTE_ACCESS_CAN_RENAME);

    info->m_display_name = QString (g_file_info_get_display_name(new_info));
    GIcon *g_icon = g_file_info_get_icon (new_info);
    if (G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        if (icon_names)
            info->m_icon_name = QString (*icon_names);
        //g_object_unref(g_icon);
    }

    //qDebug()<<m_display_name<<m_icon_name;
    GIcon *g_symbolic_icon = g_file_info_get_symbolic_icon (new_info);
    if (G_IS_ICON(g_symbolic_icon)) {
        const gchar* const* symbolic_icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_symbolic_icon));
        if (symbolic_icon_names)
            info->m_symbolic_icon_name = QString (*symbolic_icon_names);
        //g_object_unref(g_symbolic_icon);
    }

    info->m_file_id = g_file_info_get_attribute_string(new_info, G_FILE_ATTRIBUTE_ID_FILE);

    info->m_content_type = g_file_info_get_content_type (new_info);
    info->m_size = g_file_info_get_attribute_uint64(new_info, G_FILE_ATTRIBUTE_STANDARD_SIZE);
    info->m_modified_time = g_file_info_get_attribute_uint64(new_info, G_FILE_ATTRIBUTE_TIME_MODIFIED);

    const char *content_type_str = g_file_info_get_content_type (new_info);
    info->m_mime_type_string = content_type_str;
    if (content_type_str) {
        char *content_type = g_content_type_get_description (content_type_str);
        info->m_file_type = content_type;
        g_free (content_type);
        content_type = nullptr;
    }

    char *size_full = g_format_size_full(info->m_size, G_FORMAT_SIZE_DEFAULT);
    info->m_file_size = size_full;
    g_free(size_full);

    QDateTime date = QDateTime::fromMSecsSinceEpoch(info->m_modified_time*1000);
    info->m_modified_date = date.toString(Qt::SystemLocaleShortDate);

    Q_EMIT info->updated();
    m_info->m_mutex.unlock();
}
