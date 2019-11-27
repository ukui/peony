#include "file-meta-info.h"
#include "file-info-manager.h"

#include <QDebug>

using namespace Peony;

std::shared_ptr<FileMetaInfo> FileMetaInfo::fromGFileInfo(const QString &uri, GFileInfo *g_info)
{
    return std::make_shared<FileMetaInfo>(uri, g_info);
}

std::shared_ptr<FileMetaInfo> FileMetaInfo::fromUri(const QString &uri)
{
    auto mgr = FileInfoManager::getInstance();
    auto info = mgr->findFileInfoByUri(uri);
    if (info)
        return mgr->findFileInfoByUri(uri)->m_meta_info;
    return nullptr;
}

FileMetaInfo::FileMetaInfo(const QString &uri, GFileInfo *g_info)
{
    m_uri = uri;
    if (g_info) {
        char **metainfo_attributes = g_file_info_list_attributes(g_info, "metadata");
        if (metainfo_attributes) {
            for (int i = 0; metainfo_attributes[i] != nullptr; i++) {
                char *string = g_file_info_get_attribute_as_string(g_info, metainfo_attributes[i]);
                if (string) {
                    auto var = QVariant(string);
                    this->setMetaInfoVariant(metainfo_attributes[i], var);
                    //qDebug()<<"======"<<m_uri<<metainfo_attributes[i]<<var.toString();
                    g_free(string);
                }
            }
            g_strfreev(metainfo_attributes);
        }
    }
}

void FileMetaInfo::setMetaInfoInt(const QString &key, int value)
{
    setMetaInfoVariant(key, QString::number(value));
}

void FileMetaInfo::setMetaInfoString(const QString &key, const QString &value)
{
    setMetaInfoVariant(key, value);;
}

void FileMetaInfo::setMetaInfoStringList(const QString &key, const QStringList &value)
{
    QString string = value.join('\n');
    setMetaInfoVariant(key, string);
}

void FileMetaInfo::setMetaInfoVariant(const QString &key, const QVariant &value)
{
    m_mutex.lock();
    auto realKey = "metadata::" + key;
    m_meta_hash.remove(key);
    m_meta_hash.insert(key, value);
    GFile *file = g_file_new_for_uri(m_uri.toUtf8().constData());
    GFileInfo *info = g_file_info_new();
    std::string tmp = realKey.toStdString();
    g_file_info_set_attribute(info, tmp.c_str(), G_FILE_ATTRIBUTE_TYPE_INVALID, (gpointer)value.data());
    //FIXME: should i add callback?
    g_file_set_attributes_async(file,
                                info,
                                G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                0,
                                nullptr,
                                nullptr,
                                nullptr);
    g_object_unref(info);
    g_object_unref(file);
    m_mutex.unlock();
}

const QVariant FileMetaInfo::getMetaInfoVariant(const QString &key)
{
    if (m_meta_hash.value(key).isValid())
        return m_meta_hash.value(key);
    //FIXME: should i use gio query meta here?
    return QVariant();
}

const QString FileMetaInfo::getMetaInfoString(const QString &key)
{
    return getMetaInfoVariant(key).toString();
}

const QStringList FileMetaInfo::getMetaInfoStringList(const QString &key)
{
    return getMetaInfoVariant(key).toString().split('\n');
}

int FileMetaInfo::getMetaInfoInt(const QString &key)
{
    return getMetaInfoVariant(key).toString().toInt();
}
