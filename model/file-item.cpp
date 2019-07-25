#include "file-item.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info-manager.h"

#include "file-item-model.h"

using namespace Peony;

FileItem::FileItem(std::shared_ptr<Peony::FileInfo> info, FileItem *parentItem, FileItemModel *model, QObject *parent) : QObject(parent)
{
    m_parent = parentItem;
    m_info = info;
    m_children = new QVector<FileItem*>();

    m_model = model;
}

FileItem::~FileItem()
{
    disconnect();
    if (m_info.use_count() <= 2) {
        Peony::FileInfoManager::getInstance()->remove(m_info);
    }
    delete m_children;
}

QVector<FileItem*> *FileItem::findChildrenSync()
{
    std::shared_ptr<Peony::FileEnumerator> enumerator = std::make_shared<Peony::FileEnumerator>();
    enumerator->setEnumerateDirectory(m_info->uri());
    enumerator->enumerateSync();
    auto infos = enumerator->getChildren();
    for (auto info : infos) {
        FileItem *child = new FileItem(info, this, m_model);
        m_children->append(child);
        FileInfoJob *job = new FileInfoJob(info);
        job->setAutoDelete();
        job->querySync();
    }
    return m_children;
}

//FIXME: findChildrenAsync is not completely asynchously.
//because after async enumerating, it will do a sync querying.
void FileItem::findChildrenAsync()
{
    Peony::FileEnumerator *enumerator = new Peony::FileEnumerator;
    enumerator->setEnumerateDirectory(m_info->uri());
    enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, [=](bool successed){
        if (successed) {
            auto infos = enumerator->getChildren();
            for (auto info : infos) {
                FileItem *child = new FileItem(info, this, m_model);
                m_children->append(child);
                //TODO: add a list query async function,
                //wait signal when a child was updated.
                FileInfoJob *job = new FileInfoJob(info);
                job->setAutoDelete();
                job->querySync();
            }
        }
        //TODO: if we use async method, we may need dynamicly add
        //item with insertRow()
        Q_EMIT findChildrenAsyncFinished(this);
        Q_EMIT m_model->foundChildren(this->firstColumnIndex());
        //enumerator->disconnect();
        //enumerator->deleteLater();
    });
    enumerator->enumerateAsync();
}

QModelIndex FileItem::firstColumnIndex()
{
    return m_model->firstColumnIndex(this);
}

bool FileItem::hasChildren()
{
    return m_info->isDir() || m_info->isVolume() || m_children->count() > 0;
}
