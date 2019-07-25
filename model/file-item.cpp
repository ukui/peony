#include "file-item.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info-manager.h"

#include "file-item-model.h"

#include <QDebug>

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

bool FileItem::operator==(const FileItem &item)
{
    qDebug()<<m_info->uri()<<item.m_info->uri();
    return this->m_info->uri() == item.m_info->uri();
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

void FileItem::findChildrenAsync()
{
    Peony::FileEnumerator *enumerator = new Peony::FileEnumerator;
    enumerator->setEnumerateDirectory(m_info->uri());
    enumerator->connect(enumerator, &FileEnumerator::prepared, [=](GError *err){
        if (err) {
            qDebug()<<err->message;
        }
        enumerator->enumerateAsync();
    });
    enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, [=](bool successed){
        if (successed) {
            auto infos = enumerator->getChildren();
            for (auto info : infos) {
                FileItem *child = new FileItem(info, this, m_model);
                m_children->append(child);
                FileInfoJob *job = new FileInfoJob(info);
                job->setAutoDelete();
                FileInfo *shared_info = info.get();
                int row = infos.indexOf(info);
                //qDebug()<<info->uri()<<row;
                job->connect(job, &FileInfoJob::queryAsyncFinished, [=](){
                    qDebug()<<shared_info->iconName();
                    m_model->beginInsertRows(this->firstColumnIndex(), 0, m_children->count() - 1);
                    qDebug()<<m_model->insertRow(row, this->firstColumnIndex());
                    m_model->endInsertRows();
                });
                job->queryAsync();
            }
        }
        enumerator->disconnect();
        enumerator->deleteLater();
    });

    enumerator->prepare();
}

QModelIndex FileItem::firstColumnIndex()
{
    return m_model->firstColumnIndex(this);
}

bool FileItem::hasChildren()
{
    return m_info->isDir() || m_info->isVolume() || m_children->count() > 0;
}
