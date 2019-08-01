#include "file-item.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info-manager.h"
#include "file-watcher.h"

#include "file-item-model.h"

#include "gerror-wrapper.h"

#include <QDebug>
#include <QStandardPaths>

#include <QMessageBox>

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
    //qDebug()<<"~FileItem"<<m_info->uri();
    disconnect();
    if (m_info.use_count() <= 2) {
        Peony::FileInfoManager::getInstance()->remove(m_info);
    }
    if (m_watcher) {
        delete m_watcher;
    }
    for (auto child : *m_children) {
        delete child;
    }
    m_children->clear();

    delete m_children;
}

bool FileItem::operator==(const FileItem &item)
{
    //qDebug()<<m_info->uri()<<item.m_info->uri();
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
    if (m_expanded)
        return;

    m_expanded = true;
    Peony::FileEnumerator *enumerator = new Peony::FileEnumerator;
    enumerator->setEnumerateDirectory(m_info->uri());
    enumerator->connect(enumerator, &FileEnumerator::prepared, [=](std::shared_ptr<GErrorWrapper> err){
        if (err) {
            qDebug()<<err->message();
        }
        enumerator->enumerateAsync();
    });
    enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, [=](bool successed){
        if (successed) {
            auto infos = enumerator->getChildren();
            m_async_count = infos.count();
            for (auto info : infos) {
                FileItem *child = new FileItem(info, this, m_model);
                m_children->prepend(child);
                FileInfoJob *job = new FileInfoJob(info);
                job->setAutoDelete();
                FileInfo *shared_info = info.get();
                int row = infos.indexOf(info);
                //qDebug()<<info->uri()<<row;
                job->connect(job, &FileInfoJob::infoUpdated, [=](){
                    qDebug()<<shared_info->iconName()<<row;
                });
                connect(job, &FileInfoJob::destroyed, [=](){
                    //the query job is finished and will be deleted soon,
                    //whatever info was updated, we need decrease the async count.
                    m_async_count--;
                    if (m_async_count == 0) {
                        m_model->insertRows(0, m_children->count(), this->firstColumnIndex());
                    }
                });
                job->queryAsync();
            }
        }

        delete enumerator;

        m_watcher = new FileWatcher(this->m_info->uri());
        connect(m_watcher, &FileWatcher::fileCreated, [=](QString uri){
            //add new item to m_children
            //tell the model update
            this->onChildAdded(uri);
            Q_EMIT this->childAdded(uri);
        });
        connect(m_watcher, &FileWatcher::fileDeleted, [=](QString uri){
            //remove the crosponding child
            //tell the model update
            this->onChildRemoved(uri);
            Q_EMIT this->childRemoved(uri);
        });
        connect(m_watcher, &FileWatcher::directoryDeleted, [=](QString uri){
            //clean all the children, if item index is root index, cd up.
            //this might use FileItemModel::setRootItem()
            Q_EMIT this->deleted(uri);
            this->onDeleted(uri);
        });
        connect(m_watcher, &FileWatcher::locationChanged, [=](QString oldUri, QString newUri){
            //this might use FileItemModel::setRootItem()
            Q_EMIT this->renamed(oldUri, newUri);
            this->onRenamed(oldUri, newUri);
        });
        //qDebug()<<"startMonitor";
        m_watcher->startMonitor();
    });

    enumerator->prepare();
}

QModelIndex FileItem::firstColumnIndex()
{
    return m_model->firstColumnIndex(this);
}

QModelIndex FileItem::lastColumnIndex()
{
    return m_model->lastColumnIndex(this);
}

bool FileItem::hasChildren()
{
    return m_info->isDir() || m_info->isVolume() || m_children->count() > 0;
}

FileItem *FileItem::getChildFromUri(QString uri)
{
    for (auto item : *m_children) {
        if (item->m_info->uri() == uri)
            return item;
    }
    return nullptr;
}

void FileItem::onChildAdded(const QString &uri)
{
    qDebug()<<"add child" + uri;
    FileItem *child = getChildFromUri(uri);
    if (child) {
        qDebug()<<"has added";
        return;
    }
    FileItem *newChild = new FileItem(FileInfo::fromUri(uri), this, m_model);
    m_children->append(newChild);
    m_model->insertRow(m_children->count() - 1, this->firstColumnIndex());
    //use sync update here.
    newChild->updateInfoSync();
    m_model->updated();
}

void FileItem::onChildRemoved(const QString &uri)
{
    FileItem *child = getChildFromUri(uri);
    if (child) {
        m_model->removeRow(m_children->indexOf(child), this->firstColumnIndex());
        m_children->removeOne(child);
    }
    delete child;
    m_model->updated();
}

void FileItem::onDeleted(const QString &thisUri)
{
    qDebug()<<"deleted";
    //TODO: find parent file and go to parent directory
    Q_UNUSED(thisUri);
    if (m_parent) {
        m_model->removeRow(m_parent->m_children->indexOf(this), m_parent->firstColumnIndex());
        m_parent->m_children->removeOne(this);
    } else {
        //cd up
        GFile *file = m_info->gFileHandle();
        GFile *parent_file = g_file_get_parent(file);
        auto parentInfo = FileInfo::fromGFile(parent_file);
        g_object_unref(parent_file);
        FileItem *newRootItem = new FileItem(parentInfo, nullptr, m_model);
        m_model->setRootItem(newRootItem);
    }
}

void FileItem::onRenamed(const QString &oldUri, const QString &newUri)
{
    qDebug()<<"renamed";
    Q_UNUSED(oldUri);
    if (m_parent) {
        FileItem *newRootItem = new FileItem(FileInfo::fromUri(newUri), nullptr, m_model);
        m_model->setRootItem(newRootItem);
    }
}

void FileItem::updateInfoSync()
{
    FileInfoJob *job = new FileInfoJob(m_info);
    if (job->querySync()) {
        m_model->dataChanged(this->firstColumnIndex(), this->lastColumnIndex());
    }
    job->deleteLater();
}

void FileItem::updateInfoAsync()
{
    FileInfoJob *job = new FileInfoJob(m_info);
    job->setAutoDelete();
    job->connect(job, &FileInfoJob::infoUpdated, [=](){
        m_model->dataChanged(this->firstColumnIndex(), this->lastColumnIndex());
    });
    job->queryAsync();
}

void FileItem::clearChildren()
{
    auto parent = firstColumnIndex();
    m_model->removeRows(0, m_model->rowCount(parent), parent);
    for (auto child : *m_children) {
        delete child;
    }
    m_children->clear();
    m_expanded = false;
    delete m_watcher;
    m_watcher = nullptr;
}
