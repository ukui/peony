/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "file-item.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info-manager.h"
#include "file-watcher.h"
#include "file-utils.h"

#include "file-item-model.h"

#include "thumbnail-manager.h"

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
        auto info = FileInfoManager::getInstance()->findFileInfoByUri(m_info->uri()).get();
        if (info == m_info.get()) {
            Peony::FileInfoManager::getInstance()->lock();
            Peony::FileInfoManager::getInstance()->remove(m_info);
            Peony::FileInfoManager::getInstance()->unlock();
        }
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
    auto infos = enumerator->getChildren(true);
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
    enumerator->connect(enumerator, &FileEnumerator::prepared, this, [=](std::shared_ptr<GErrorWrapper> err){
        auto target = FileUtils::getTargetUri(m_info->uri());
        if (!target.isEmpty()) {
            enumerator->cancel();
            //enumerator->deleteLater();
            m_model->setRootUri(target);
            return;
        }
        if (err) {
            qDebug()<<err->message();
            if (err.get()->code() == G_IO_ERROR_NOT_FOUND) {
                enumerator->cancel();
                //enumerator->deleteLater();
                m_model->setRootUri(FileUtils::getParentUri(this->uri()));
                return;
            }
        }
        enumerator->enumerateAsync();
    });

    if (!m_model->isPositiveResponse()) {
        enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, this, [=](bool successed){
            if (successed) {
                auto infos = enumerator->getChildren(true);
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
                    job->connect(job, &FileInfoJob::infoUpdated, this, [=](){
                        qDebug()<<shared_info->iconName()<<row;
                    });
                    */
                    connect(job, &FileInfoJob::infoUpdated, this, [=](){
                        //the query job is finished and will be deleted soon,
                        //whatever info was updated, we need decrease the async count.
                        m_async_count--;
                        if (m_async_count == 0) {
                            m_model->insertRows(0, m_children->count(), this->firstColumnIndex());
                            Q_EMIT this->m_model->findChildrenFinished();
                            Q_EMIT m_model->updated();
                            ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_watcher);
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

             m_watcher = std::make_shared<FileWatcher>(this->m_info->uri());
            m_watcher->setMonitorChildrenChange(true);
            connect(m_watcher.get(), &FileWatcher::fileCreated, this, [=](QString uri){
                //add new item to m_children
                //tell the model update
                this->onChildAdded(uri);
                Q_EMIT this->childAdded(uri);
            });
            connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](QString uri){
                //remove the crosponding child
                //tell the model update
                this->onChildRemoved(uri);
                Q_EMIT this->childRemoved(uri);
            });
            connect(m_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri){
                auto index = m_model->indexFromUri(uri);
                if (index.isValid()) {
                    auto infoJob = new FileInfoJob(FileInfo::fromUri(index.data(FileItemModel::UriRole).toString()));
                    infoJob->setAutoDelete();
                    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=](){
                        m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
                        auto info = FileInfo::fromUri(uri);
                        if (info->isDesktopFile()) {
                            ThumbnailManager::getInstance()->updateDesktopFileThumbnail(info->uri(), m_watcher);
                        }
                    });
                    infoJob->queryAsync();
                }
            });
            connect(m_watcher.get(), &FileWatcher::thumbnailUpdated, this, [=](const QString &uri){
                m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
            });
            connect(m_watcher.get(), &FileWatcher::directoryDeleted, this, [=](QString uri){
                //clean all the children, if item index is root index, cd up.
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->deleted(uri);
                this->onDeleted(uri);
            });

            connect(m_watcher.get(), &FileWatcher::locationChanged, this, [=](QString oldUri, QString newUri){
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->renamed(oldUri, newUri);
                this->onRenamed(oldUri, newUri);
            });

            connect(m_watcher.get(), &FileWatcher::directoryUnmounted, this, [=](){
                m_model->setRootUri("computer:///");
            });
            //qDebug()<<"startMonitor";
            m_watcher->startMonitor();
        });
    } else {
        enumerator->connect(enumerator, &Peony::FileEnumerator::childrenUpdated, this, [=](const QStringList &uris){
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
                m_model->beginInsertRows(firstColumnIndex(), m_children->count(), m_children->count());
                m_children->append(item);
                m_model->endInsertRows();
                auto infoJob = new FileInfoJob(info);
                infoJob->setAutoDelete();
                infoJob->connect(infoJob, &FileInfoJob::infoUpdated, this, [=](){
                    Q_EMIT m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
                    //Q_EMIT m_model->updated();
                    ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_watcher);
                });
                infoJob->queryAsync();
            }
        });

        enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, this, [=](){
            delete enumerator;
            if (!m_model||!m_children||!m_info)
                return;

            Q_EMIT m_model->findChildrenFinished();
            Q_EMIT m_model->updated();

            m_watcher = std::make_shared<FileWatcher>(this->m_info->uri());
            m_watcher->setMonitorChildrenChange(true);
            connect(m_watcher.get(), &FileWatcher::fileCreated, this, [=](QString uri){
                //add new item to m_children
                //tell the model update
                this->onChildAdded(uri);
                Q_EMIT this->childAdded(uri);
                ThumbnailManager::getInstance()->createThumbnail(uri, m_watcher);
            });
            connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](QString uri){
                //remove the crosponding child
                //tell the model update
                this->onChildRemoved(uri);
                Q_EMIT this->childRemoved(uri);
            });
            connect(m_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri){
                auto index = m_model->indexFromUri(uri);
                if (index.isValid()) {
                    auto infoJob = new FileInfoJob(FileInfo::fromUri(index.data(FileItemModel::UriRole).toString()));
                    infoJob->setAutoDelete();
                    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=](){
                        m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
                        auto info = FileInfo::fromUri(uri);
                        if (info->isDesktopFile()) {
                            ThumbnailManager::getInstance()->updateDesktopFileThumbnail(info->uri(), m_watcher);
                        }
                    });
                    infoJob->queryAsync();
                }
            });
            connect(m_watcher.get(), &FileWatcher::thumbnailUpdated, this, [=](const QString &uri){
                m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
            });
            connect(m_watcher.get(), &FileWatcher::directoryDeleted, this, [=](QString uri){
                //clean all the children, if item index is root index, cd up.
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->deleted(uri);
                this->onDeleted(uri);
            });
            connect(m_watcher.get(), &FileWatcher::locationChanged, this, [=](QString oldUri, QString newUri){
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->renamed(oldUri, newUri);
                this->onRenamed(oldUri, newUri);
            });

            connect(m_watcher.get(), &FileWatcher::directoryUnmounted, this, [=](){
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
        //child info maybe changed, so need sync update again
        child->updateInfoSync();
        m_model->updated();
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
    job->connect(job, &FileInfoJob::infoUpdated, this, [=](){
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
    m_watcher.reset();
    m_watcher = nullptr;
}
