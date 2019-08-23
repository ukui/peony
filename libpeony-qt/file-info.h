#ifndef FILEINFO_H
#define FILEINFO_H

#include "peony-core_global.h"

#include <memory>
#include <gio/gio.h>
#include <QString>
#include <QObject>

#include <QMutex>

namespace Peony {

class FileInfoJob;

/*!
 * \brief The FileInfo class
 * <br>
 * This class represents the information of this file and file itself.
 * Although you can use constructor new a FileInfo instance,
 * you should use FileInfo::fromUri(). Because it provides an shared data,
 * and FileInfoJob need hold a shared_ptr reference, too.
 * This will help to reduce the risk of memory leaks.
 * </br>
 */
class PEONYCORESHARED_EXPORT FileInfo : public QObject
{
    friend class FileInfoJob;

    Q_OBJECT
public:
    explicit FileInfo(QObject *parent = nullptr);
    explicit FileInfo(const QString &uri, QObject *parent = nullptr);
    ~FileInfo();
    static std::shared_ptr<FileInfo> fromUri(QString uri);
    static std::shared_ptr<FileInfo> fromPath(QString path);
    static std::shared_ptr<FileInfo> fromGFile(GFile *file);

    QString uri() {return m_uri;}
    bool isDir() {return m_is_dir;}
    bool isVolume() {return m_is_volume;}

    QString displayName() {return m_display_name;}
    QString iconName() {return m_icon_name;}
    QString symbolicIconName() {return m_symbolic_icon_name;}
    QString fileID() {return m_file_id;}
    QString fileType() {return m_file_type;}

    QString fileSize() {return m_file_size;}
    QString modifiedDate() {return m_modified_date;}

    quint64 size() {return m_size;}
    quint64 modifiedTime() {return m_modified_time;}

    GFile *gFileHandle() {return m_file;}

Q_SIGNALS:
    void updated();

private:
    QString m_uri = nullptr;
    bool m_is_valid = false;
    bool m_is_dir = false;
    bool m_is_volume = false;
    bool m_is_remote = false;

    bool m_is_loaded = false;

    QString m_display_name = nullptr;
    QString m_icon_name = nullptr;
    QString m_symbolic_icon_name = nullptr;
    QString m_file_id = nullptr;

    QString m_content_type = nullptr;
    guint64 m_size = 0;
    guint64 m_modified_time = 0;

    QString m_file_type = nullptr;
    QString m_file_size = nullptr;
    QString m_modified_date = nullptr;

    //FIXME: should i use smart pointer wrap these data?
    GFile *m_file = nullptr;
    GFile *m_parent = nullptr;

    GFile *m_target_file = nullptr;

    /*!
     * \brief m_cancellable
     * This cancellable is used in async query file info in FileInfoJob instance.
     */
    GCancellable *m_cancellable = nullptr;

    QMutex m_mutex;
};

}

#endif // FILEINFO_H
