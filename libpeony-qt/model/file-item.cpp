/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
#include "file-operation-utils.h"

#include "file-item-model.h"

#include "thumbnail-manager.h"

#include "gerror-wrapper.h"
#include "bookmark-manager.h"
#include "audio-play-manager.h"

#include <QDebug>
#include <QStandardPaths>

#include <QMessageBox>
#include <QUrl>
#include <QTimer>
#include <KWindowSystem>

#include <QApplication>

using namespace Peony;

FileItem::FileItem(std::shared_ptr<Peony::FileInfo> info, FileItem *parentItem, FileItemModel *model, QObject *parent) : QObject(parent)
{
    m_parent = parentItem;
    m_info = info;
    m_children = new QVector<FileItem*>();

    m_model = model;

    m_backend_enumerator = new FileEnumerator(this);

    m_thumbnail_watcher = std::make_shared<Peony::FileWatcher>("thumbnail://");
    connect(m_thumbnail_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri){
        auto index = m_model->indexFromUri(uri);
        if (index.isValid()) {
            auto item = m_model->itemFromIndex(index);
            if (item) {
                /*!
                  \note
                  fix the probabilistic jamming while thumbnailing with list view.

                  we have to only trigger first column index dataChanged signal,
                  otherwise there will be probility stucked whole program.

                  i'm not sure if it is a bug of qtreeview.
                  */

                //m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
                m_model->dataChanged(item->firstColumnIndex(), item->firstColumnIndex());
            }
        }
    });

    // avoid call any method when model is deleted.
    setParent(m_model);
}

FileItem::~FileItem()
{
    //qDebug()<<"~FileItem"<<m_info->uri();
    Q_EMIT cancelFindChildren();
    //disconnect();

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
    //qDebug() << "FileItem findChildrenSync";
    Q_EMIT m_model->findChildrenFinished();
    return m_children;
}

void FileItem::findChildrenAsync()
{
    auto info = FileInfo::fromUri(m_info.get()->uri());
    auto infoJob = new FileInfoJob(info);
    infoJob->setAutoDelete();
    infoJob->queryAsync();

    if (m_expanded)
        return;

    Q_EMIT m_model->findChildrenStarted();
    m_expanded = true;
    Peony::FileEnumerator *enumerator = new Peony::FileEnumerator;
    enumerator->setEnumerateDirectory(m_info->uri());
    //NOTE: entry a new root might destroyed the current enumeration work.
    //the root item will be delete, so we should cancel the previous enumeration.
    enumerator->connect(this, &FileItem::cancelFindChildren, enumerator, &FileEnumerator::cancel);
    enumerator->connect(enumerator, &FileEnumerator::cancelled, m_model, [=](){
        m_model->findChildrenFinished();
    });
    enumerator->connect(enumerator, &FileEnumerator::prepared, this, [=](std::shared_ptr<GErrorWrapper> err, const QString &targetUri, bool critical) {
        if (critical) {
            Peony::AudioPlayManager::getInstance()->playWarningAudio();
            QMessageBox::critical(nullptr, tr("Error"), err->message());
            enumerator->cancel();
            return;
        }

        if (!targetUri.isNull()) {
            if (targetUri != this->uri()) {
                this->m_info = FileInfo::fromUri(targetUri);

                GFile *targetFile = g_file_new_for_uri(targetUri.toUtf8().constData());
                QUrl targetUrl = targetUri;
                auto path = g_file_get_path(targetFile);
                if (path && !targetUrl.isLocalFile() && false) {
                    QString localUri = QString("file://%1").arg(path);
                    this->m_info = FileInfo::fromUri(localUri);
                    enumerator->setEnumerateDirectory(localUri);
                    g_free(path);
                } else {
                    enumerator->setEnumerateDirectory(targetFile);
                }

                g_object_unref(targetFile);
            }

            enumerator->enumerateAsync();
            return;
        }

        auto target = FileUtils::getTargetUri(m_info->uri());
        if (!target.isEmpty()) {
            enumerator->cancel();
            //enumerator->deleteLater();
            m_model->setRootUri(target);
            return;
        }
        if (err) {
            qDebug()<<"file item error:" <<err->message()<<enumerator->getEnumerateUri();
            Peony::AudioPlayManager::getInstance()->playWarningAudio();
            if (err.get()->code() == G_IO_ERROR_NOT_FOUND || err.get()->code() == G_IO_ERROR_PERMISSION_DENIED) {
                enumerator->cancel();
                //fix goto removed path in case device is ejected
                if (this->uri().startsWith("file:///media"))
                {
                    //check bookmark and delete
                    BookMarkManager::getInstance()->removeBookMark(this->uri());
                    m_model->setRootUri("computer:///");
                }
                else
                    m_model->setRootUri(FileUtils::getParentUri(this->uri()));

                auto fileInfo = FileInfo::fromUri(this->uri());
                if (err.get()->code() == G_IO_ERROR_NOT_FOUND && fileInfo->isSymbolLink())
                {
                    auto result = QMessageBox::question(nullptr, tr("Open Link failed"),
                                          tr("File not exist, do you want to delete the link file?"));
                    if (result == QMessageBox::Yes) {
                        qDebug() << "Delete unused symbollink.";
                        QStringList selections;
                        selections.push_back(this->uri());
                        FileOperationUtils::trash(selections, true);
                    }
                }
                else if (err.get()->code() == G_IO_ERROR_PERMISSION_DENIED)
                {
                    QMessageBox *msgBox = new QMessageBox();
                    msgBox->setWindowTitle(tr("Error"));
                    QString errorInfo = tr("Can not open path \"%1\"，permission denied.").arg(this->uri().unicode());
                    msgBox->setText(errorInfo);
                    msgBox->setModal(false);
                    msgBox->setAttribute(Qt::WA_DeleteOnClose);
                    KWindowSystem::setState(msgBox->winId(), KWindowSystem::KeepAbove);
                    msgBox->show();
                   //QMessageBox::critical(nullptr, tr("Error"), errorInfo);
                }
                else if(err.get()->code() == G_IO_ERROR_NOT_FOUND)
                {
                    QString errorInfo = tr("Can not find path \"%1\"，are you moved or renamed it?").arg(fileInfo->uri().unicode());
                    QMessageBox::critical(nullptr, tr("Error"), errorInfo);
                }
                return;
            }
            else {
                QMessageBox::critical(nullptr, tr("Error"), err->message());
                enumerator->cancel();
                return;
            }
        }
        enumerator->enumerateAsync();
    });

    if (!m_model->isPositiveResponse()) {
        enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, this, [=](bool successed) {
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
                    job->connect(job, &FileInfoJob::infoUpdated, this, [=](){
                        qDebug()<<shared_info->iconName()<<row;
                    });
                    */
                    connect(job, &FileInfoJob::queryAsyncFinished, this, [=]() {
                        //the query job is finished and will be deleted soon,
                        //whatever info was updated, we need decrease the async count.
                        m_async_count--;
                        if (m_async_count == 0) {
                            m_model->insertRows(0, m_children->count(), this->firstColumnIndex());
                            Q_EMIT this->m_model->findChildrenFinished();
                            Q_EMIT m_model->updated();
                            for (auto info : infos) {
                                ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_thumbnail_watcher);
                            }
                        }
                    });

                    connect(job, &FileInfoJob::infoUpdated, this, [=](){
                        m_model->dataChanged(child->firstColumnIndex(), child->lastColumnIndex());
                    });

                    job->queryAsync();
                }
            } else {
                //qDebug() << "enumerateFinished false" <<successed;
                Q_EMIT m_model->findChildrenFinished();
                return;
            }

            enumerator->cancel();
            delete enumerator;

            m_watcher = std::make_shared<FileWatcher>(this->m_info->uri());
            m_watcher->setMonitorChildrenChange(true);
            connect(m_watcher.get(), &FileWatcher::fileCreated, this, [=](QString uri) {
                //add new item to m_children
                //tell the model update
                this->onChildAdded(uri);
                Q_EMIT this->childAdded(uri);
                qDebug() << "inpositive Model onChildAdded:" <<uri;
            });
            connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](QString uri) {
                //check bookmark and delete
                auto info = FileInfo::fromUri(uri);
                if (info->isDir())
                {
                    BookMarkManager::getInstance()->removeBookMark(uri);
                }
                //remove the crosponding child
                //tell the model update
                this->onChildRemoved(uri);
                Q_EMIT this->childRemoved(uri);
            });
            connect(m_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri) {
                auto index = m_model->indexFromUri(uri);
                if (index.isValid()) {
                    auto infoJob = new FileInfoJob(FileInfo::fromUri(index.data(FileItemModel::UriRole).toString()));
                    infoJob->setAutoDelete();
                    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=]() {
                        m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
                        auto info = FileInfo::fromUri(uri);
                        ThumbnailManager::getInstance()->createThumbnail(uri, m_thumbnail_watcher, true);
                        /*
                        if (info->isDesktopFile()) {
                            ThumbnailManager::getInstance()->updateDesktopFileThumbnail(info->uri(), m_thumbnail_watcher);
                        }
                        */
                    });
                    infoJob->queryAsync();
                }
            });
            connect(m_watcher.get(), &FileWatcher::thumbnailUpdated, this, [=](const QString &uri) {
                m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
            });
            connect(m_watcher.get(), &FileWatcher::directoryDeleted, this, [=](QString uri) {
                //clean all the children, if item index is root index, cd up.
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->deleted(uri);
                this->onDeleted(uri);
            });

            connect(m_watcher.get(), &FileWatcher::locationChanged, this, [=](QString oldUri, QString newUri) {
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->renamed(oldUri, newUri);
                this->onRenamed(oldUri, newUri);
            });

            connect(m_watcher.get(), &FileWatcher::directoryUnmounted, this, [=]() {
                m_model->setRootUri("computer:///");
            });
            //qDebug()<<"startMonitor";

            connect(m_watcher.get(), &FileWatcher::requestUpdateDirectory, this, &FileItem::onUpdateDirectoryRequest);
            m_watcher->startMonitor();
        });
    } else {
        enumerator->connect(enumerator, &Peony::FileEnumerator::childrenUpdated, this, [=](const QStringList &uris, bool isEnding) {
            if (uris.isEmpty()) {
                if (isEnding) {
                    //qDebug() << "enumerateFinished childrenUpdated:" <<isEnding;
                    Q_EMIT m_model->findChildrenFinished();
                    Q_EMIT m_model->updated();
                }
            }

            if (!m_children) {
                enumerator->disconnect();
                delete enumerator;
                return ;
            }

            if (isEnding) {
                m_ending_uris.clear();
                m_ending_uris = uris;
            }
            for (auto uri : uris) {
                auto info = FileInfo::fromUri(uri);
                auto infoJob = new FileInfoJob(info);
                infoJob->setAutoDelete();
                infoJob->connect(infoJob, &FileInfoJob::infoUpdated, this, [=]() {
                    auto item = new FileItem(info, this, m_model);
                    m_model->beginInsertRows(firstColumnIndex(), m_children->count(), m_children->count());
                    m_children->append(item);
                    m_model->endInsertRows();
                    //Q_EMIT m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
                    //Q_EMIT m_model->updated();
                    ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_thumbnail_watcher);

                    m_ending_uris.removeOne(uri);
                    if (isEnding && m_ending_uris.isEmpty()) {
                        qApp->processEvents();
                        Q_EMIT m_model->findChildrenFinished();
                        Q_EMIT m_model->updated();
                    }
                });
                infoJob->queryAsync();
            }
        });

        enumerator->connect(enumerator, &Peony::FileEnumerator::enumerateFinished, this, [=](bool successed) {
            delete enumerator;

            if (!successed) {
                Q_EMIT m_model->findChildrenFinished();
                return;
            }

            if (!m_model||!m_children||!m_info)
                return;

            m_watcher = std::make_shared<FileWatcher>(this->m_info->uri());
            m_watcher->setMonitorChildrenChange(true);
            connect(m_watcher.get(), &FileWatcher::fileCreated, this, [=](QString uri) {
                //add new item to m_children
                //tell the model update
                this->onChildAdded(uri);
                Q_EMIT this->childAdded(uri);
                qDebug() << "positive onChildAdded:" <<uri;
                ThumbnailManager::getInstance()->createThumbnail(uri, m_thumbnail_watcher);
            });
            connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](QString uri) {
                //check bookmark and delete
                auto info = FileInfo::fromUri(uri);
                if (info->isDir())
                {
                    BookMarkManager::getInstance()->removeBookMark(uri);
                }
                //remove the crosponding child
                //tell the model update
                this->onChildRemoved(uri);
                Q_EMIT this->childRemoved(uri);
                qDebug() << "childRemoved:" <<uri;
            });
            connect(m_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri) {
                auto index = m_model->indexFromUri(uri);
                if (index.isValid()) {
                    auto infoJob = new FileInfoJob(FileInfo::fromUri(index.data(FileItemModel::UriRole).toString()));
                    infoJob->setAutoDelete();
                    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=]() {
                        m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
                        auto info = FileInfo::fromUri(uri);
                        if (info->isDesktopFile()) {
                            ThumbnailManager::getInstance()->updateDesktopFileThumbnail(info->uri(), m_watcher);
                        }
                    });
                    infoJob->queryAsync();
                }
            });
            connect(m_watcher.get(), &FileWatcher::thumbnailUpdated, this, [=](const QString &uri) {
                m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
            });
            connect(m_watcher.get(), &FileWatcher::directoryDeleted, this, [=](QString uri) {
                //clean all the children, if item index is root index, cd up.
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->deleted(uri);
                this->onDeleted(uri);
            });
            connect(m_watcher.get(), &FileWatcher::locationChanged, this, [=](QString oldUri, QString newUri) {
                //this might use FileItemModel::setRootItem()
                Q_EMIT this->renamed(oldUri, newUri);
                this->onRenamed(oldUri, newUri);
            });

            connect(m_watcher.get(), &FileWatcher::directoryUnmounted, this, [=]() {
                m_model->setRootUri("computer:///");
            });
            //qDebug()<<"startMonitor";
            connect(m_watcher.get(), &FileWatcher::requestUpdateDirectory, this, &FileItem::onUpdateDirectoryRequest);
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
        QUrl itemUrl = item->uri();
        QUrl url = uri;
        QString decodedUri = url.toDisplayString();
        if (decodedUri == itemUrl.toDisplayString())
            return item;
    }
    return nullptr;
}

void FileItem::onChildAdded(const QString &uri)
{
    qDebug()<<"add child:" << uri;
    FileItem *child = getChildFromUri(uri);
    if (child) {
        qDebug()<<"has added, return";
        //child info maybe changed, so need sync update again
        child->updateInfoAsync();
        //m_model->updated();
        return;
    }

    //add waiting queue to fix show item duplicated issue
    if (m_waiting_add_queue.contains(uri))
    {
        qDebug()<<"is in m_waiting_add_queue, return";
        return;
    }

    auto info = FileInfo::fromUri(uri);
    auto infoJob = new FileInfoJob(info);
    infoJob->setAutoDelete();
    m_waiting_add_queue.append(uri);
    infoJob->connect(infoJob, &FileInfoJob::infoUpdated, this, [=]() {
        auto item = new FileItem(info, this, m_model);
        m_model->beginInsertRows(firstColumnIndex(), m_children->count(), m_children->count());
        m_children->append(item);
        m_model->endInsertRows();
        qDebug() <<"successfully added child:" <<uri;
        m_waiting_add_queue.removeOne(uri);
        //Q_EMIT m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
        //Q_EMIT m_model->updated();
        QTimer::singleShot(1000, this, [=](){
            ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_thumbnail_watcher);
        });
    });
    infoJob->queryAsync();

//    FileItem *newChild = new FileItem(FileInfo::fromUri(uri), this, m_model);
//    m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
//    m_children->append(newChild);
//    m_model->endInsertRows();
//    //use sync update here.
//    newChild->updateInfoAsync();
//    //m_model->updated();
}

void FileItem::onChildRemoved(const QString &uri)
{
    FileItem *child = getChildFromUri(uri);
    if (child) {
        int index = m_children->indexOf(child);
        m_model->beginRemoveRows(this->firstColumnIndex(), index, index);
        m_children->removeOne(child);
        qDebug() <<"successfully removed child:" <<uri;
        delete child;
        m_model->endRemoveRows();
    }
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

void FileItem::onUpdateDirectoryRequest()
{
    auto enumerator = new FileEnumerator(this);
    enumerator->setEnumerateDirectory(m_model->getRootUri());
    connect(enumerator, &FileEnumerator::enumerateFinished, m_model, [=](){
        if (m_model->getRootUri() != enumerator->getEnumerateUri())
            return;

        auto currentUris = enumerator->getChildrenUris();
        QStringList rawUris;
        QStringList removedUris;
        QStringList addedUris;

        for (auto child : *m_model->m_root_item->m_children) {
            rawUris<<child->uri();
            if (!currentUris.contains(child->uri())) {
                removedUris<<child->uri();
                m_model->m_root_item->onChildRemoved(child->uri());
            }
        }

        for (auto uri : currentUris) {
            if (!rawUris.contains(uri)) {
                addedUris<<uri;
                m_model->m_root_item->onChildAdded(uri);
            }
        }

        for (auto uri : currentUris) {
            if (!addedUris.contains(uri) && !removedUris.contains(uri)) {
                //m_model->m_root_item->getChildFromUri(uri)->updateInfoAsync();
            }
        }

        enumerator->deleteLater();
    });

    enumerator->enumerateAsync();
}

void FileItem::updateInfoSync()
{
    FileInfoJob *job = new FileInfoJob(m_info);
    if (job->querySync()) {
        m_model->dataChanged(this->firstColumnIndex(), this->lastColumnIndex());
        ThumbnailManager::getInstance()->createThumbnail(this->uri(), m_thumbnail_watcher, true);
    }
    job->deleteLater();
}

void FileItem::updateInfoAsync()
{
    FileInfoJob *job = new FileInfoJob(m_info);
    job->setAutoDelete();
    job->connect(job, &FileInfoJob::infoUpdated, this, [=]() {
        m_model->dataChanged(this->firstColumnIndex(), this->lastColumnIndex());
        ThumbnailManager::getInstance()->createThumbnail(this->uri(), m_thumbnail_watcher, true);
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

/* Func: if it isn't a vaild volume device,it should not be displayed.
 */
bool FileItem::shouldShow()
{
    QString uri,unixDevice,displayName;

    uri = m_info->uri();
    if(uri.isEmpty())
        return false;
    if("computer:///root.link" == uri)
        return true;

    //non computer path, no need check
    //to fix sftp IO stuck issue
    if (! uri.startsWith("computer:///"))
        return true;

    unixDevice = FileUtils::getUnixDevice(uri);
    displayName = FileUtils::getFileDisplayName(uri);

    if(displayName.isEmpty())
        return false;
    if(!unixDevice.isEmpty() && !displayName.contains(":")){
        if(uri.endsWith(".drive"))
            return false;
    }
    return true;
}
