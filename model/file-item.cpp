#include "file-item.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info-manager.h"
#include "file-watcher.h"

#include "file-item-model.h"

#include "gerror-wrapper.h"

#include <QDebug>
#include <QStandardPaths>

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
    qDebug()<<"~FileItem"<<m_info->uri();
    disconnect();
    if (m_info.use_count() <= 2) {
        Peony::FileInfoManager::getInstance()->remove(m_info);
    }
    if (m_watcher) {
        m_watcher->stopMonitor();
        m_watcher->disconnect();
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
            for (auto info : infos) {
                FileItem *child = new FileItem(info, this, m_model);
                m_children->append(child);
                FileInfoJob *job = new FileInfoJob(info);
                job->setAutoDelete();
                FileInfo *shared_info = info.get();
                int row = infos.indexOf(info);
                //qDebug()<<info->uri()<<row;
                job->connect(job, &FileInfoJob::infoUpdated, [=](){
                    qDebug()<<shared_info->iconName()<<row;
                    //NOTE:
                    //Actually, beginInsertRows does not really add an row.
                    //After endInsertRows, FileItemModel::rowCount will be called
                    //and data will be refershed soon.
                    //So insert a row is a 'fake', it might be a refresh.
                    //m_model->beginInsertRows(this->firstColumnIndex(), 0, 0);
                    //m_model->beginInsertRows(this->firstColumnIndex(), 0, m_children->count() - 1);
                    //qDebug()<<m_model->insertRow(row, this->firstColumnIndex());
                    //m_model->endInsertRows();
                    bool inserted = m_model->insertRow(row, this->firstColumnIndex());
                    qDebug()<<inserted;
                });
                job->queryAsync();
            }
        }
        enumerator->disconnect();
        enumerator->deleteLater();

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
            this->onDeleted(uri);
            Q_EMIT this->deleted(uri);
        });
        connect(m_watcher, &FileWatcher::locationChanged, [=](QString oldUri, QString newUri){
            //this might use FileItemModel::setRootItem()
            this->onRenamed(oldUri, newUri);
            Q_EMIT this->renamed(oldUri, newUri);
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
    m_model->beginInsertRows(this->firstColumnIndex(), 0, 0);
    m_children->append(newChild);
    m_model->endInsertRows();
    newChild->updateInfoAsync();
}

void FileItem::onChildRemoved(const QString &uri)
{
    FileItem *child = getChildFromUri(uri);
    if (child) {
        m_model->beginRemoveRows(this->firstColumnIndex(), 0, 0);
        m_children->removeOne(child);
        m_model->endRemoveRows();
    }
    child->disconnect();
    child->deleteLater();
}

void FileItem::onDeleted(const QString &thisUri)
{
    qDebug()<<"deleted";
    //TODO: find parent file and go to parent directory
    Q_UNUSED(thisUri);
    if (m_parent) {
        QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        FileItem *newRootItem = new FileItem(FileInfo::fromPath(homePath), nullptr, m_model);
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
    qDebug()<<m_info->uri();
    FileInfoJob *job = new FileInfoJob(m_info);
    if (job->querySync()) {
        m_model->beginInsertRows(this->firstColumnIndex(), 0, 0);
        m_model->endInsertRows();
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
    for (auto child : *m_children) {
        delete child;
    }
    m_children->clear();
    m_expanded = false;
    g_object_unref(m_watcher);
    m_watcher = nullptr;
}
