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

/*!
 * \brief The FileItem class
 * <br>
 * FileItem is the absctract item class contract with FileItemModel.
 * The different from FileInfo to FileItem is that FileItem has concept of children and parent.
 * This makes FileItem instance can represent an item in model.
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

    bool operator == (const FileItem &item);

    QVector<FileItem*> *findChildrenSync();
    void findChildrenAsync();

    QModelIndex firstColumnIndex();

    bool hasChildren();

Q_SIGNALS:
    void findChildrenAsyncFinished(FileItem *parentItem);

private:
    FileItem *m_parent = nullptr;
    std::shared_ptr<Peony::FileInfo> m_info;
    QVector<FileItem*> *m_children = nullptr;

    FileItemModel *m_model = nullptr;

    bool m_expanded = false;
};

}

#endif // FILEITEM_H
