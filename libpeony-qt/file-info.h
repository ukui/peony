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
    enum Access {
        Readable,
        Writeable,
        Executable,
        Deleteable,
        Trashable,
        Renameable
    };
    Q_DECLARE_FLAGS(AccessFlags, Access)

    explicit FileInfo(QObject *parent = nullptr);
    explicit FileInfo(const QString &uri, QObject *parent = nullptr);
    ~FileInfo();
    static std::shared_ptr<FileInfo> fromUri(QString uri, bool addToHash = true);
    static std::shared_ptr<FileInfo> fromPath(QString path, bool addToHash = true);
    static std::shared_ptr<FileInfo> fromGFile(GFile *file, bool addToHash = true);

    QString uri() {return m_uri;}
    bool isDir() {return m_is_dir | m_content_type == "inode/directory";}
    bool isVolume() {return m_is_volume;}
    bool isSymbolLink() {return m_is_symbol_link;}
    bool isVirtual() {return m_is_virtual;}

    QString displayName() {return m_display_name;}
    QString iconName() {return m_icon_name;}
    QString symbolicIconName() {return m_symbolic_icon_name;}
    QString fileID() {return m_file_id;}
    QString mimeType() {return m_mime_type_string;}
    QString fileType() {return m_file_type;}

    QString fileSize() {return m_file_size;}
    QString modifiedDate() {return m_modified_date;}

    QString type() {return m_content_type;}
    quint64 size() {return m_size;}
    quint64 modifiedTime() {return m_modified_time;}

    bool canRead() {return m_can_read;}
    bool canWrite() {return m_can_write;}
    bool canExecute() {return m_can_excute;}
    bool canDelete() {return m_can_delete;}
    bool canTrash() {return m_can_trash;}
    bool canRename() {return m_can_rename;}

    bool isDesktopFile() {return m_can_excute && m_uri.endsWith(".desktop");}
    bool isEmptyInfo() {return m_display_name == nullptr;}

    AccessFlags accesses() {
        auto flags = AccessFlags();
        flags.setFlag(Readable, m_can_read);
        flags.setFlag(Writeable, m_can_write);
        flags.setFlag(Executable, m_can_excute);
        flags.setFlag(Deleteable, m_can_delete);
        flags.setFlag(Trashable, m_can_trash);
        flags.setFlag(Renameable, m_can_rename);
        return flags;
    }

    GFile *gFileHandle() {return m_file;}

Q_SIGNALS:
    void updated();

private:
    QString m_uri = nullptr;
    bool m_is_valid = false;
    bool m_is_dir = false;
    bool m_is_volume = false;
    bool m_is_remote = false;
    bool m_is_symbol_link = false;
    bool m_is_virtual = false;

    bool m_is_loaded = false;

    QString m_display_name = nullptr;
    QString m_icon_name = nullptr;
    QString m_symbolic_icon_name = nullptr;
    QString m_file_id = nullptr;

    QString m_content_type = nullptr;
    guint64 m_size = 0;
    guint64 m_modified_time = 0;

    QString m_mime_type_string = nullptr;
    QString m_file_type = nullptr;
    QString m_file_size = nullptr;
    QString m_modified_date = nullptr;

    //access
    bool m_can_read = true;
    bool m_can_write = false;
    bool m_can_excute = false;
    bool m_can_delete = false;
    bool m_can_trash = false;
    bool m_can_rename = false;

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
