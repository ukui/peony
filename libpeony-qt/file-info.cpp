#include "file-info.h"

#include "file-info-manager.h"
#include "file-info-job.h"
#include <QUrl>

#include <QDebug>

using namespace Peony;

FileInfo::FileInfo(QObject *parent) : QObject (parent)
{
    m_cancellable = g_cancellable_new();
}

FileInfo::FileInfo(const QString &uri, QObject *parent) : QObject (parent)
{
    m_cancellable = g_cancellable_new();
    /*!
     * \note
     * In qt program we alwas handle file's uri format as unicode,
     * but in glib/gio it might be not.
     * I want to keep the uri strings format in peony-qt,
     * this would help me avoid some problem, such as the uri path completion
     * bug in PathBarModel enumeration.
     */
    QUrl url(uri);
    m_uri = url.toDisplayString();
    m_file = g_file_new_for_uri(m_uri.toUtf8().constData());
    m_parent = g_file_get_parent(m_file);
    m_is_remote = !g_file_is_native(m_file);
    GFileType type = g_file_query_file_type(m_file,
                                            G_FILE_QUERY_INFO_NONE,
                                            nullptr);
    switch (type) {
    case G_FILE_TYPE_DIRECTORY:
        //qDebug()<<"dir";
        m_is_dir = true;
        break;
    case G_FILE_TYPE_MOUNTABLE:
        //qDebug()<<"mountable";
        m_is_volume = true;
        break;
    default:
        break;
    }
}

FileInfo::~FileInfo()
{
    //qDebug()<<"~FileInfo"<<m_uri;
    disconnect();

    g_object_unref(m_cancellable);
    g_object_unref(m_file);

    if (m_target_file)
        g_object_unref(m_target_file);
    if (m_parent)
        g_object_unref(m_parent);

    m_uri = nullptr;
}

std::shared_ptr<FileInfo> FileInfo::fromUri(QString uri, bool addToHash)
{
    FileInfoManager *info_manager = FileInfoManager::getInstance();
    info_manager->lock();
    std::shared_ptr<FileInfo> info = info_manager->findFileInfoByUri(uri);
    if (info != nullptr) {
        info_manager->unlock();
        return info;
    } else {
        std::shared_ptr<FileInfo> newly_info = std::make_shared<FileInfo>();
        QUrl url(uri);
        newly_info->m_uri = url.toDisplayString();
        newly_info->m_file = g_file_new_for_uri(newly_info->m_uri.toUtf8().constData());
        newly_info->m_parent = g_file_get_parent(newly_info->m_file);
        newly_info->m_is_remote = !g_file_is_native(newly_info->m_file);
        GFileType type = g_file_query_file_type(newly_info->m_file,
                                                G_FILE_QUERY_INFO_NONE,
                                                nullptr);
        switch (type) {
        case G_FILE_TYPE_DIRECTORY:
            //qDebug()<<"dir";
            newly_info->m_is_dir = true;
            break;
        case G_FILE_TYPE_MOUNTABLE:
            //qDebug()<<"mountable";
            newly_info->m_is_volume = true;
            break;
        default:
            break;
        }
        if (addToHash) {
            info_manager->insertFileInfo(newly_info);
        }
        info_manager->unlock();
        return newly_info;
    }
}

std::shared_ptr<FileInfo> FileInfo::fromPath(QString path, bool addToHash)
{
    QString uri = "file://"+path;
    return fromUri(uri, addToHash);
}

std::shared_ptr<FileInfo> FileInfo::fromGFile(GFile *file, bool addToHash)
{
    char *uri_str = g_file_get_uri(file);
    QString uri = uri_str;
    g_free(uri_str);
    return fromUri(uri, addToHash);
}
