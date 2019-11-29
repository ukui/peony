#ifndef FILEMETAINFO_H
#define FILEMETAINFO_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QVariant>
#include <QMutex>

#include <memory>
#include <gio/gio.h>

namespace Peony {

/*!
 * \brief The FileMetaInfo class
 * \details
 * This class represent a data set abstracted from gvfs metadata.
 *
 * When a FileInfoJob queryed, a FileMetaInfo will be created for the FileInfo instance
 * which FileInfoJob hold. FileMetaInfo constructed with the GFileInfo handle the file query
 * job executed, and read all "metadata::" namespace datas then put them into a hash table.
 *
 * \note
 * You can use FileInfoMeta::fromUri(uri) to get a file's meta data in global, but you should make
 * sure that file's FileInfo is queryed yet.
 */
class FileMetaInfo
{
    friend class FileInfo;
    friend class FileInfoJob;
    friend class FileInfoManager;
public:
    static std::shared_ptr<FileMetaInfo> fromGFileInfo(const QString &uri, GFileInfo *g_info);
    static std::shared_ptr<FileMetaInfo> fromUri(const QString &uri);

    FileMetaInfo(const QString &uri, GFileInfo *g_info);

    void setMetaInfoString(const QString &key, const QString &value);
    const QString getMetaInfoString(const QString &key);
    void setMetaInfoStringList(const QString &key, const QStringList &value);
    const QStringList getMetaInfoStringList(const QString &key);
    void setMetaInfoInt(const QString &key, int value);
    int getMetaInfoInt(const QString &key);
    void setMetaInfoVariant(const QString &key, const QVariant &value);
    const QVariant getMetaInfoVariant(const QString &key);

    void removeMetaInfo(const QString &key);

private:
    QString m_uri;
    QHash<QString, QVariant> m_meta_hash;
    QMutex m_mutex;
};

}

#endif // FILEMETAINFO_H
