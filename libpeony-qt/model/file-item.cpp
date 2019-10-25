#include "file-item.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info-manager.h"
#include "file-watcher.h"
#include "file-utils.h"

#include "file-item-model.h"

#include "gerror-wrapper.h"

#include <QDebug>
#include <QStandardPaths>

#include <QMessageBox>
#include <QUrl>

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
    Q_EMIT cancelFindChildren();
    //disconnect();
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

const QString FileItem::uri()
{
    return m_info->uri();
}

QVector<FileItem*> *FileItem::findChildrenSync()
{
    Q_EMIT m_model->findChildrenStarted();
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
    Q_EMIT m_model->findChildrenFinished();
    return m_children;
}

void FileItem::findChildrenAsync()
{
    if (m_expanded)
        return;

    Q_EMIT m_model->findChildrenStarted();
    m_expanded = true;
    Peony::FileEnumerator *enumerator = new Peony::FileEnumerator;
    enumerator->setEnumerateDirectory(m_info->uri());
    //NOTE: entry a new root might destroyed the current enumeration work.
    //the root item will be delete, so we should cancel the previous enumeration.
    enumerator->connect(this, &FileItem::cancelFindChildren, enumerator, &FileEnumerator::cancel);
    enumerator->connect(enumerator, &FileEnumerator::prepared, [=](std::shared_ptr<GErrorWrapper> err){
        if (err) {
            qDebug()<<err->message();
            if (err.get()->code() == G_IO_ERROR_NOT_FOUND) {
                enumerator->cancel();
                enumerator->deleteLater();
                m_model->setRootUri(FileUtils::getParentUri(this->uri()));
                return;
            }
        }
        enumerator->enumerateAsync();
    });

    if (!m_model->isPositiveResponse()) {
        enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, [=](bool successed){
            if (successed) {
                auto infos = enumerator->getChildren();
                m_async_count = infos.count();
                if (infos.count() == 0) {
                    Q_EMIT m_model->findChildrenFinished();
                }

                for (auto info : infos) {
                    FileItem *child = new FileItem(info, this, m_model);
                    m_children->prepend(child);
                    FileInfoJob *job = new FileInfoJob(info);
                    job->setAutoDelete();
                    /*
                    FileInfo *shared_info = info.get();
                    int row = infos.indexOf(info);
                    //qDebug()<<info->uri()<<row;
                    job->connect(job, &FileInfoJob::infoUpdated, [=](){
                        qDebug()<<shared_info->iconName()<<row;
                    });
                    */
                    connect(job, &FileInfoJob::infoUpdated, [=](){
                        //the query job is finished and will be deleted soon,
                        //whatever info was updated, we need decrease the async count.
                        m_async_count--;
                        if (m_async_count == 0) {
                            m_model->insertRows(0, m_children->count(), this->firstColumnIndex());
                            Q_EMIT this->m_model->findChildrenFinished();
                            Q_EMIT m_model->updated();
                        }
                    });
                    job->queryAsync();
                }
            } else {
                Q_EMIT m_model->findChildrenFinished();
                return;
            }

            enumerator->cancel();
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

            connect(m_watcher, &FileWatcher::directoryUnmounted, [=](){
                m_model->setRootUri("computer:///");
            });
            //qDebug()<<"startMonitor";
            m_watcher->startMonitor();
        });
    } else {
        enumerator->connect(enumerator, &Peony::FileEnumerator::childrenUpdated, [=](const QStringList &uris){
            if (uris.isEmpty()) {
                Q_EMIT m_model->findChildrenFinished();
            }

            if (!m_children) {
                enumerator->disconnect();
                delete enumerator;
                return ;
            }

            for (auto uri : uris) {
                auto info = FileInfo::fromUri(uri);
                auto item = new FileItem(info, this, m_model);
                m_children->append(item);
                m_model->insertRows(m_children->count() - 1, 1, firstColumnIndex());

                auto infoJob = new FileInfoJob(info);
                infoJob->setAutoDelete();
                infoJob->connect(infoJob, &FileInfoJob::infoUpdated, [=](){
                    Q_EMIT m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
                    //Q_EMIT m_model->updated();
                });
                infoJob->queryAsync();
            }
        });

        enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, [=](){
            delete enumerator;
            if (!m_model||!m_children||!m_info)
                return;

            Q_EMIT m_model->findChildrenFinished();
            Q_EMIT m_model->updated();

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

            connect(m_watcher, &FileWatcher::directoryUnmounted, [=](){
                m_model->setRootUri("computer:///");
            });
            //qDebug()<<"startMonitor";
            m_watcher->startMonitor();
        });
    }

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
    //qDebug()<<"has children"<<m_info->uri()<<(m_info->isDir() || m_info->isVolume() || m_children->count() > 0);
    return m_info->isDir() || m_info->isVolume() || m_children->count() > 0;
}

FileItem *FileItem::getChildFromUri(QString uri)
{
    for (auto item : *m_children) {
        QUrl url = uri;
        QString decodedUri = url.toDisplayString();
        if (decodedUri == item->uri())
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
    //FIXME: when a mount point unmounted, it was aslo assumed as "deleted",
    //in this case we should not delete this item here.
    //actually i don't think this desgin is good enough. maybe there is a
    //better choice.
    //another problem is that if we just clear the children of this item,
    //it will be "unexpandable". but the item index hasChildren() is true.
    //doublue clicked twice it will be expanded. a qt's bug?
    if (m_parent) {
        if (m_parent->m_info->uri() == thisUri) {
            m_model->removeRow(m_parent->m_children->indexOf(this), m_parent->firstColumnIndex());
            m_parent->m_children->removeOne(this);
        } else {
            //if just clear children, there will be a small problem.
            clearChildren();
            m_model->removeRow(m_parent->m_children->indexOf(this), m_parent->firstColumnIndex());
            m_parent->m_children->removeOne(this);
            m_parent->onChildAdded(m_info->uri());
        }
        this->deleteLater();
    } else {
        //cd up.
        auto tmpItem = this;
        auto tmpUri = FileUtils::getParentUri(tmpItem->uri());
        while(tmpItem && tmpUri.isNull()) {
            tmpUri = FileUtils::getParentUri(tmpUri);
            tmpItem = tmpItem->m_parent;
        }
        if (!tmpUri.isNull()) {
            m_model->setRootUri(tmpUri);
        } else {
            m_model->setRootUri("file:///");
        }
    }
    m_model->updated();
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
