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

    bool operator == (const FileItem &item){
        return this->m_info == item.m_info;
    }

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
