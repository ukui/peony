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

#include <QTime>

using namespace Peony;

QString uri2FavoriteUri(const QString &sourceUri)
{
    QUrl url = sourceUri;
    QString favoriteUri = "favorite://" + url.path() + "?schema=" + url.scheme();
    return favoriteUri;
}

FileItem::FileItem(std::shared_ptr<Peony::FileInfo> info, FileItem *parentItem, FileItemModel *model, QObject *parent) : QObject(parent)
{
    m_parent = parentItem;
    m_info = info;
    m_children = new QVector<FileItem*>();

    m_model = model;

    m_backend_enumerator = new FileEnumerator(this);

    m_idle = new QTimer(this);
    m_idle->setInterval(0);
    m_idle->setSingleShot(true);
    connect(m_idle, &QTimer::timeout, this, [=]{
        if (m_uris_to_be_removed.isEmpty())
            return;

        QStringList favoriteUris;

        if (m_uris_to_be_removed.count() < 10) {
            // do normal remove
            for (auto uri : m_uris_to_be_removed) {
                for (int row = 0; row < m_children->count(); row++) {
                    auto child = m_children->at(row);
                    // 此处实际可靠性还有待验证
                    if (FileUtils::isSamePath(uri, child->uri())) {
                        auto info = child->m_info;
                        if (info->isDir())
                        {
                            favoriteUris.append(uri2FavoriteUri(uri));
                        }
                        m_model->beginRemoveRows(this->firstColumnIndex(), row, row);
                        m_children->remove(row);
                        delete child;
                        m_model->endRemoveRows();
                        break;
                    }
                }
            }
            BookMarkManager::getInstance()->removeBookMark(favoriteUris);
            return;
        }

        // do reset model

        int time0 = QTime::currentTime().msecsSinceStartOfDay();
        qDebug()<<"execute deletion";
        m_model->beginResetModel();
        qDebug()<<"files deleted"<<m_uris_to_be_removed.count();
        for (auto uri : m_uris_to_be_removed) {
            for (int row = 0; row < m_children->count(); row++) {
                auto child = m_children->at(row);
                // 此处实际可靠性还有待验证
                if (FileUtils::isSamePath(uri, child->uri())) {
                    auto info = child->m_info;
                    if (info->isDir())
                    {
                        favoriteUris.append(uri2FavoriteUri(uri));
                    }
                    m_children->remove(row);
                    delete child;
                    break;
                }
            }
        }
        m_model->endResetModel();
        BookMarkManager::getInstance()->removeBookMark(favoriteUris);
        int time1 = QTime::currentTime().msecsSinceStartOfDay();
        qDebug()<<"excute deletion finished, cost"<<time1 - time0;
        //ThumbnailManager::getInstance()->releaseThumbnail(m_uris_to_be_removed);
    });

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
                //m_model->dataChanged(item->firstColumnIndex(), item->firstColumnIndex());
                m_model->updated();
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

#include"file-operation-manager.h"

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
            //Peony::AudioPlayManager::getInstance()->playWarningAudio();
            QMessageBox::critical(nullptr, tr("Error"), err->message());
            enumerator->cancel();
            //fix bug#77594
            enumerator->deleteLater();
            return;
        }

        if (!targetUri.isNull()) {
            if (targetUri != this->uri()) {
                this->m_info = FileInfo::fromUri(targetUri);

                GFile *targetFile = g_file_new_for_uri(targetUri.toUtf8().constData());
                QUrl targetUrl = targetUri;
                auto path = g_file_get_path(targetFile);
                enumerator->setEnumerateDirectory(targetFile);
                g_object_unref(targetFile);
            }

            enumerator->enumerateAsync();
            return;
        }

        auto target = FileUtils::getTargetUri(m_info->uri());
        if (!target.isEmpty()) {
            enumerator->cancel();
            //enumerator->deleteLater();

            //! \note fix direct setRootUri() prevents view switch error
            // m_model->setRootUri(target);
            m_model->sendPathChangeRequest(target, this->uri());
            return;
        }
        if (err) {
            qDebug()<<"file item error:" <<err->message()<<enumerator->getEnumerateUri();
            //Peony::AudioPlayManager::getInstance()->playWarningAudio();
            if (err.get()->code() == G_IO_ERROR_NOT_FOUND || err.get()->code() == G_IO_ERROR_PERMISSION_DENIED) {
                enumerator->cancel();
                //fix goto removed path in case device is ejected
                if (this->uri().startsWith("file:///media"))
                {
                    //check bookmark and delete
                    BookMarkManager::getInstance()->removeBookMark(uri2FavoriteUri(this->uri()));
                    m_model->sendPathChangeRequest("computer:///", this->uri());
                }
                else
                    //! \note fix direct setRootUri() prevents view switch error
                    // m_model->setRootUri(FileUtils::getParentUri(this->uri()));
                    m_model->sendPathChangeRequest(FileUtils::getParentUri(this->uri()), this->uri());

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

                    connect(job, &FileInfoJob::infoUpdated, child, [=](){
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

            m_watcher = std::make_shared<FileWatcher>(this->m_info->uri(), nullptr, true);
            m_watcher->setMonitorChildrenChange(true);
            connect(m_watcher.get(), &FileWatcher::fileCreated, this, [=](QString uri) {
                //add new item to m_children
                //tell the model update
                this->onChildAdded(uri);
                Q_EMIT this->childAdded(uri);
                qDebug() << "inpositive Model onChildAdded:" <<uri;
            });
            connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](QString uri) {
                this->onChildRemoved(uri);
            });
            connect(m_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri) {
                auto index = m_model->indexFromUri(uri);
                if (index.isValid()) {
                    auto infoJob = new FileInfoJob(FileInfo::fromUri(index.data(FileItemModel::UriRole).toString()));
                    infoJob->setAutoDelete();
                    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=]() {
                        m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
                        //auto info = FileInfo::fromUri(uri);
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
            connect(m_watcher.get(), &FileWatcher::fileRenamed, this, [=](const QString &oldUri, const QString &newUri) {
                Q_EMIT this->renamed(oldUri, newUri);
                this->onRenamed(oldUri, newUri);
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

            connect(m_watcher.get(), &FileWatcher::directoryUnmounted, this, [=](const QString &sourceUri) {
                m_model->sendPathChangeRequest("computer:///", sourceUri);
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
                        //qApp->processEvents();
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

            m_watcher = std::make_shared<FileWatcher>(this->m_info->uri(), nullptr, true);
            m_watcher->setMonitorChildrenChange(true);
            connect(m_watcher.get(), &FileWatcher::fileCreated, this, [=](QString uri) {
                //add new item to m_children
                //tell the model update
                this->onChildAdded(uri);
                Q_EMIT this->childAdded(uri);
                qDebug() << "positive onChildAdded:" <<uri;
                //file changed, force create thubnail, link tobug#83108
                ThumbnailManager::getInstance()->createThumbnail(uri, m_thumbnail_watcher, true);
            });
            connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](QString uri) {
                this->onChildRemoved(uri);
            });
            connect(m_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri) {
                auto index = m_model->indexFromUri(uri);
                if (index.isValid()) {
                    auto infoJob = new FileInfoJob(FileInfo::fromUri(index.data(FileItemModel::UriRole).toString()));
                    infoJob->setAutoDelete();
                    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=]() {
                        m_model->dataChanged(m_model->indexFromUri(uri), m_model->indexFromUri(uri));
                        //comment to fix endless loop and can not rename issue, related to bug#72642
//                        auto info = FileInfo::fromUri(uri);
//                        if (info->isDesktopFile()) {
//                            ThumbnailManager::getInstance()->updateDesktopFileThumbnail(info->uri(), m_watcher);
//                        }
                    });
                    infoJob->queryAsync();
                }
            });
            connect(m_watcher.get(), &FileWatcher::fileRenamed, this, [=](const QString &oldUri, const QString &newUri) {
                this->onRenamed(oldUri, newUri);
                BookMarkManager::getInstance()->bookmarkChanged(oldUri, newUri);
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

            connect(m_watcher.get(), &FileWatcher::directoryUnmounted, this, [=](const QString &sourceUri) {
                m_model->sendPathChangeRequest("computer:///", sourceUri);
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
    // optimize
    auto index = m_model->indexFromUri(uri);
    if (index.isValid()) {
        return m_model->itemFromIndex(index);
    }
    return nullptr;
    /*
    for (auto item : *m_children) {
        QUrl itemUrl = item->uri();
        QUrl url = uri;
        QString decodedUri = url.toDisplayString();
        if (decodedUri == itemUrl.toDisplayString())
            return item;
    }
    return nullptr;
    */
}

void FileItem::onChildAdded(const QString &uri)
{
    m_uris_to_be_removed.removeOne(uri);

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
        m_waiting_add_queue.removeOne(uri);
        auto item = getChildFromUri(uri);
        // add exsited checkment. link to: #66999
        if (!item) {
            item = new FileItem(info, this, m_model);
            m_model->beginInsertRows(firstColumnIndex(), m_children->count(), m_children->count());
            m_children->append(item);
            m_model->endInsertRows();
            qDebug() <<"successfully added child:" <<uri;

            /* Fixbug#82649:在手机内部存储里新建文件/文件夹时，名称不是可编辑状态,都是默认文件名/文件夹名 */
            Q_EMIT m_model->signal_itemAdded(uri);//end

            QTimer::singleShot(1000, this, [=](){
                ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_thumbnail_watcher);
            });

        } else {
            qInfo()<<"file"<<uri<<"has arealy in file item model";
        }
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
    // fix #62925
    m_waiting_add_queue.removeOne(uri);
    m_uris_to_be_removed.append(uri);
    if (!m_idle->isActive()) {
        m_idle->start();
    }
    return;
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
            if(tmpUri.startsWith("file:///media"))
                m_model->sendPathChangeRequest("computer:///", tmpItem->uri());
            else
                m_model->setRootUri(tmpUri);
        } else {
            //! \note Fix direct setRootUri() prevents view switch error
            // m_model->setRootUri("file:///");
            m_model->sendPathChangeRequest("file:///, tmpItem->uri()");
        }
    }
    m_model->updated();
}

void FileItem::onRenamed(const QString &oldUri, const QString &newUri)
{
    qDebug()<<"renamed";

    // fix #77076, which caused by idle removing item due to #66255.
    if (m_uris_to_be_removed.contains(newUri)) {
        m_uris_to_be_removed.removeOne(newUri);
    }

    // note that some times new file has arealy in directory view,
    // and there is no delete event triggered. for example. copy
    // a .desktop file in current view. in this case there might
    // be an outdated tmp file left.
    //
    // to avoid that we add an existing checkment, and handle old
    // file with different situation.
    auto newChild = getChildFromUri(newUri);
    if (newChild) {
        qDebug()<<"new child has arealy in view";
        newChild->updateInfoAsync();
    }

    FileItem *child = getChildFromUri(oldUri);
    if (child) {
        if (!newChild) {
            int index = m_children->indexOf(child);
            m_children->at(index)->m_info= FileInfo::fromUri(newUri);
            child->updateInfoAsync();
        } else {
            m_model->beginRemoveRows(this->firstColumnIndex(), m_children->indexOf(child), m_children->indexOf(child));
            m_children->removeOne(child);
            child->deleteLater();
            m_model->endRemoveRows();
        }
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
        QVector<FileItem*> removeFileItem ;

        for (auto child : *m_model->m_root_item->m_children) {
            rawUris<<child->uri();
            if (!currentUris.contains(child->uri())) {
                removedUris<<child->uri();
                removeFileItem.append(child);
                //m_model->m_root_item->onChildRemoved(child->uri());
            }
        }

        for (auto item : removeFileItem ) {
            m_model->m_root_item->onChildRemoved(item->uri());
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
        //m_model->dataChanged(this->firstColumnIndex(), this->lastColumnIndex());
        m_model->updated();
        ThumbnailManager::getInstance()->createThumbnail(this->uri(), m_thumbnail_watcher, true);
    }
    job->deleteLater();
}

void FileItem::updateInfoAsync()
{
    FileInfoJob *job = new FileInfoJob(m_info);
    job->setAutoDelete();
    job->connect(job, &FileInfoJob::infoUpdated, this, [=]() {
        //m_model->dataChanged(this->firstColumnIndex(), this->lastColumnIndex());
        m_model->updated();
        ThumbnailManager::getInstance()->createThumbnail(this->uri(), m_thumbnail_watcher, true);
    });
    job->queryAsync();
}

void FileItem::removeChildren()
{

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
    if (! uri.startsWith("computer:///") || ! uri.endsWith(".drive"))
        return true;

    displayName = FileUtils::getFileDisplayName(uri);
    if(displayName.isEmpty())
        return false;

    if (displayName.contains(":"))
        return true;

    //if needed, comment to not use this IO stuck API
    unixDevice = FileUtils::getUnixDevice(uri);
    if(!unixDevice.isEmpty()){
        return false;
    }
    return true;
}
