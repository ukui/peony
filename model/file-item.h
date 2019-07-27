#ifndef FILEITEM_H
#define FILEITEM_H

#include "peony-core_global.h"
#include <memory>

#include <QObject>
#include <QVector>

namespace Peony {

class FileInfo;
class FileInfoManager;
class FileItemModel;
class FileWatcher;

/*!
 * \brief The FileItem class
 * <br>
 * FileItem is the absctract item class contract with FileItemModel.
 * The different from FileInfo to FileItem is that FileItem has concept of children and parent.
 * This makes FileItem instance can represent an item in model.
 * Other different is that FileItem instance is not shared. You can hold many FileItem instances
 * crosponding to the same FileInfo, but they are allocated in their own memory space.
 * Every FileItem instance which has children will aslo support monitoring. When find the children
 * of the item, it will start a monitor for this directory.
 * </br>
 * \note
 * Actually, every FileItem instance should bind with an model instance,
 * otherwise it will be useless.
 */
class FileItem : public QObject
{
    friend class FileItemModel;
    Q_OBJECT
public:
    explicit FileItem(std::shared_ptr<Peony::FileInfo> info,
                      FileItem *parentItem = nullptr,
                      FileItemModel *model = nullptr,
                      QObject *parent = nullptr);
    ~FileItem();

    void updateInfoSync();
    void updateInfoAsync();

    bool operator == (const FileItem &item);

    QVector<FileItem*> *findChildrenSync();
    void findChildrenAsync();

    QModelIndex firstColumnIndex();
    QModelIndex lastColumnIndex();

    bool hasChildren();

Q_SIGNALS:
    void findChildrenAsyncFinished(FileItem *parentItem);

    void childAdded(const QString &uri);
    void childRemoved(const QString &uri);
    void deleted(const QString &thisUri);
    void renamed(const QString &oldUri, const QString &newUri);

public Q_SLOTS:
    void onChildAdded(const QString &uri);
    void onChildRemoved(const QString &uri);
    void onDeleted(const QString &thisUri);
    void onRenamed(const QString &oldUri, const QString &newUri);

protected:
    FileItem *getChildFromUri(QString uri);

private:
    FileItem *m_parent = nullptr;
    std::shared_ptr<Peony::FileInfo> m_info;
    QVector<FileItem*> *m_children = nullptr;

    FileItemModel *m_model = nullptr;

    bool m_expanded = false;

    FileWatcher *m_watcher = nullptr;
};

}

#endif // FILEITEM_H
