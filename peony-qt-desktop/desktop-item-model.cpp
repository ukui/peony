/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "desktop-item-model.h"

#include "file-enumerator.h"
#include "file-info.h"
#include "file-info-job.h"
#include "file-info-manager.h"
#include "file-watcher.h"
#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-trash-operation.h"
#include "file-copy-operation.h"
#include "file-operation-utils.h"

#include "thumbnail-manager.h"

#include "file-meta-info.h"

#include "peony-desktop-application.h"
#include "desktop-icon-view.h"

#include <QStandardPaths>
#include <QIcon>

#include <QMimeData>
#include <QUrl>

#include <QTimer>

#include <QMessageBox>

#include <QDebug>

using namespace Peony;

DesktopItemModel::DesktopItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // do not redo layout new items while we start an operation with peony's api.
    connect(FileOperationManager::getInstance(), &FileOperationManager::operationStarted, this, [=](){
        m_new_file_info_query_queue.clear();
    });

    m_thumbnail_watcher = std::make_shared<FileWatcher>("thumbnail:///, this");

    connect(m_thumbnail_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri) {
        for (auto info : m_files) {
            if (info->uri() == uri) {
                auto index = indexFromUri(uri);
                Q_EMIT this->dataChanged(index, index);
            }
        }
    });

    m_trash_watcher = std::make_shared<FileWatcher>("trash:///", this);

    this->connect(m_trash_watcher.get(), &FileWatcher::fileCreated, [=]() {
        //qDebug()<<"trash changed";
        auto trash = FileInfo::fromUri("trash:///", true);
        auto job = new FileInfoJob(trash);
        job->setAutoDelete();
        connect(job, &FileInfoJob::infoUpdated, [=]() {
            auto trashIndex = this->indexFromUri("trash:///");
            this->dataChanged(trashIndex, trashIndex);
            Q_EMIT this->requestClearIndexWidget();
        });
        job->queryAsync();
    });

    this->connect(m_trash_watcher.get(), &FileWatcher::fileDeleted, [=]() {
        //qDebug()<<"trash changed";
        auto trash = FileInfo::fromUri("trash:///", true);
        auto job = new FileInfoJob(trash);
        job->setAutoDelete();
        connect(job, &FileInfoJob::infoUpdated, [=]() {
            auto trashIndex = this->indexFromUri("trash:///");
            this->dataChanged(trashIndex, trashIndex);
            Q_EMIT this->requestClearIndexWidget();
        });
        job->queryAsync();
    });

    m_desktop_watcher = std::make_shared<FileWatcher>("file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), this);
    m_desktop_watcher->setMonitorChildrenChange(true);
    this->connect(m_desktop_watcher.get(), &FileWatcher::fileCreated, [=](const QString &uri) {
        qDebug()<<"desktop file created"<<uri;

        auto info = FileInfo::fromUri(uri, true);
        bool exsited = false;
        for (auto file : m_files) {
            if (file->uri() == info->uri()) {
                exsited = true;
                break;
            }
        }

        if (m_new_file_info_query_queue.contains(uri)) {
            exsited = true;
        } else {
            m_new_file_info_query_queue<<uri;
        }

        if (!exsited) {
            auto job = new FileInfoJob(info);
            job->setAutoDelete();
            job->querySync();

            // locate new item =====

            auto view = PeonyDesktopApplication::getIconView();
            auto itemRectHash = view->getCurrentItemRects();
            auto grid = view->gridSize();
            auto viewRect = view->rect();

            QRegion notEmptyRegion;
            for (auto rect : itemRectHash.values()) {
                notEmptyRegion += rect;
            }

            if (!view->isRenaming()) {
                view->setFileMetaInfoPos(uri, QPoint(-1, -1));
            } else {
                view->setRenaming(false);
            }

            auto metaInfoPos = view->getFileMetaInfoPos(uri);
            if (metaInfoPos.x() >= 0) {
                // check if overlapped, it might happend whild drag out and in desktop view.
                auto indexRect = QRect(metaInfoPos, itemRectHash.values().first().size());
                if (notEmptyRegion.contains(indexRect.center())) {

                    // move index to closest empty grid.
                    auto next = indexRect;
                    bool isEmptyPos = false;
                    while (!isEmptyPos) {
                        next.translate(0, grid.height());
                        if (next.bottom() > viewRect.bottom()) {
                            int top = next.y();
                            while (true) {
                                if (top < grid.height()) {
                                    break;
                                }
                                top-=grid.height();
                            }
                            //put item to next column first row
                            next.moveTo(next.x() + grid.width(), top);
                        }
                        if (notEmptyRegion.contains(next.center()))
                            continue;

                        isEmptyPos = true;
                        itemRectHash.insert(info->uri(), next);
                        notEmptyRegion += next;

                        // handle position locate in DesktopIconView::itemInserted().
                        view->setFileMetaInfoPos(info->uri(), next.topLeft());
                    }
                }

                this->beginInsertRows(QModelIndex(), m_files.count(), m_files.count());
                ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_thumbnail_watcher);
                m_files<<info;
                //this->insertRows(m_files.indexOf(info), 1);
                this->endInsertRows();

                // end locate new item=======

                //this->endResetModel();
                Q_EMIT this->requestUpdateItemPositions();
                Q_EMIT this->requestLayoutNewItem(info->uri());
                Q_EMIT this->fileCreated(uri);
                return;
            }

            // aligin exsited rect
            int marginTop = notEmptyRegion.boundingRect().top();
            while (marginTop - grid.height() > 0) {
                marginTop -= grid.height();
            }

            int marginLeft = notEmptyRegion.boundingRect().left();
            while (marginLeft - grid.width() > 0) {
                marginLeft -= grid.width();
            }

            auto indexRect = QRect(QPoint(marginLeft, marginTop), itemRectHash.isEmpty()? QSize(): itemRectHash.values().first().size());
            if (notEmptyRegion.contains(indexRect.center())) {

                // move index to closest empty grid.
                auto next = indexRect;
                bool isEmptyPos = false;
                while (!isEmptyPos) {
                    next.translate(0, grid.height());
                    if (next.bottom() > viewRect.bottom()) {
                        int top = next.y();
                        while (true) {
                            if (top < grid.height()) {
                                break;
                            }
                            top-=grid.height();
                        }
                        //put item to next column first row
                        next.moveTo(next.x() + grid.width(), top);
                    }
                    if (notEmptyRegion.contains(next.center()))
                        continue;

                    isEmptyPos = true;
                    itemRectHash.insert(info->uri(), next);
                    notEmptyRegion += next;

                    view->setFileMetaInfoPos(info->uri(), next.topLeft());
                }
            } else {
                view->setFileMetaInfoPos(info->uri(), indexRect.topLeft());
            }

            //this->beginResetModel();
            this->beginInsertRows(QModelIndex(), m_files.count(), m_files.count());
            ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_thumbnail_watcher);
            m_files<<info;
            //this->insertRows(m_files.indexOf(info), 1);
            this->endInsertRows();

            // end locate new item=======

            //this->endResetModel();
            Q_EMIT this->requestUpdateItemPositions();
            Q_EMIT this->requestLayoutNewItem(info->uri());
            Q_EMIT this->fileCreated(uri);
        }
    });

    this->connect(m_desktop_watcher.get(), &FileWatcher::fileDeleted, [=](const QString &uri) {
        // try relayout floating items, it's often occurred when unarchive a archive file, or modify a file with vim outside of peony.
        m_last_deleted_item_uri = uri;
        int index = m_new_file_info_query_queue.indexOf(uri);
        if (index >= 0) {
            m_items_need_relayout = m_new_file_info_query_queue;
            for (int i = 0; i <= index; i++) {
                m_items_need_relayout.removeFirst();
            }
        } else {
            m_items_need_relayout.clear();
        }
        m_new_file_info_query_queue.removeOne(uri);
        auto view = PeonyDesktopApplication::getIconView();
        view->removeItemRect(uri);

        auto itemRectHash = view->getCurrentItemRects();

        for (auto info : m_files) {
            if (info->uri() == uri) {
                //this->beginResetModel();
                this->beginRemoveRows(QModelIndex(), m_files.indexOf(info), m_files.indexOf(info));
                m_files.removeOne(info);
                this->endRemoveRows();
                //this->endResetModel();
                Q_EMIT this->requestClearIndexWidget();
                Q_EMIT this->requestUpdateItemPositions();
            }
        }
    });

    this->connect(m_desktop_watcher.get(), &FileWatcher::fileChanged, [=](const QString &uri) {
        auto view = PeonyDesktopApplication::getIconView();
        auto itemRectHash = view->getCurrentItemRects();

        for (auto info : m_files) {
            if (info->uri() == uri) {
                auto job = new FileInfoJob(info);
                job->setAutoDelete();
                connect(job, &FileInfoJob::infoUpdated, this, [=]() {
                    ThumbnailManager::getInstance()->createThumbnail(uri, m_thumbnail_watcher);
                    this->dataChanged(indexFromUri(uri), indexFromUri(uri));
                    Q_EMIT this->requestClearIndexWidget();

                });
                job->queryAsync();
                this->dataChanged(indexFromUri(uri), indexFromUri(uri));
                return;
            }
        }
    });

    //when system app uninstalled, delete link in desktop if exist
    QString system_app_path = "file:///usr/share/applications/";
    m_system_app_watcher = std::make_shared<FileWatcher>(system_app_path, this);
    m_system_app_watcher->setMonitorChildrenChange(true);
    auto mInfo = FileInfo::fromUri(system_app_path);
    qDebug() <<"system_app_path:" <<mInfo->isDir();
    this->connect(m_system_app_watcher.get(), &FileWatcher::fileDeleted, [=](const QString &uri) {
        qDebug() << "m_system_app_watcher:" <<uri;
        if (uri.endsWith(".desktop"))
        {
            QString fileName = uri;
            fileName = fileName.replace(system_app_path, "");
            qDebug() << "m_system_app_watcher:" <<fileName <<uri;
            for (auto info : m_files) {
                if (info->uri().endsWith(fileName)) {
                    //this->beginResetModel();
                    this->beginRemoveRows(QModelIndex(), m_files.indexOf(info), m_files.indexOf(info));
                    m_files.removeOne(info);
                    this->endRemoveRows();
                    //this->endResetModel();
                    Q_EMIT this->requestClearIndexWidget();
                    Q_EMIT this->requestUpdateItemPositions();
                    QStringList list;
                    list.append(info->uri());
                    FileOperationUtils::trash(list, false);
                }
            }
        }
    });

    //when andriod app uninstalled, delete link in desktop if exist
    QString homePath = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    auto andriod_app_path = homePath + "/.local/share/applications/";
    auto app_info = FileInfo::fromUri(andriod_app_path);
    qDebug() <<"andriod_app_path:" <<app_info->isDir();
    m_andriod_app_watcher = std::make_shared<FileWatcher>(andriod_app_path, this);
    m_andriod_app_watcher->setMonitorChildrenChange(true);
    this->connect(m_andriod_app_watcher.get(), &FileWatcher::fileDeleted, [=](const QString &uri) {
        if (uri.endsWith(".desktop"))
        {
            QString fileName = uri;
            fileName = fileName.replace(andriod_app_path, "");
            qDebug() << "andriod_app_path:" <<fileName <<uri;
            for (auto info : m_files) {
                if (info->uri().endsWith(fileName)) {
                    //this->beginResetModel();
                    this->beginRemoveRows(QModelIndex(), m_files.indexOf(info), m_files.indexOf(info));
                    m_files.removeOne(info);
                    this->endRemoveRows();
                    //this->endResetModel();
                    Q_EMIT this->requestClearIndexWidget();
                    Q_EMIT this->requestUpdateItemPositions();
                    QStringList list;
                    list.append(info->uri());
                    FileOperationUtils::trash(list, false);
                }
            }
        }
    });

    //handle standard dir changing.
    m_dir_manager =new UserdirManager(this);
    //refresh after standard dir changed.
    connect(m_dir_manager,&UserdirManager::desktopDirChanged,[=](){
        refresh();
    });
    connect(m_dir_manager,&UserdirManager::thumbnailSetingChange,[=](){
        refresh();
    });
}

DesktopItemModel::~DesktopItemModel()
{

}

void DesktopItemModel::refresh()
{
    ThumbnailManager::getInstance()->syncThumbnailPreferences();
    beginResetModel();
    //removeRows(0, m_files.count());
    //m_trash_watcher->stopMonitor();
    //m_desktop_watcher->stopMonitor();
    for (auto info : m_files) {
        ThumbnailManager::getInstance()->releaseThumbnail(info->uri());
    }
    m_files.clear();

    auto desktopUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    //FIXME: replace BLOCKING api in ui thread.
    if (!FileUtils::isFileExsit(desktopUri)) {
        // try get correct desktop path delay.
        //FIXME: replace BLOCKING api in ui thread.
        QTimer::singleShot(1000, this, [=](){
            if (!FileUtils::isFileExsit(desktopUri)) {
                endResetModel();
                Q_EMIT refreshed();
                refresh();
            } else {
                m_enumerator = new FileEnumerator(this);
                m_enumerator->setAutoDelete();
                m_enumerator->setEnumerateDirectory(desktopUri);
                m_enumerator->connect(m_enumerator, &FileEnumerator::enumerateFinished, this, &DesktopItemModel::onEnumerateFinished);
                m_enumerator->enumerateAsync();
                endResetModel();
            }
        });
        return;
    }

    m_enumerator = new FileEnumerator(this);
    m_enumerator->setAutoDelete();
    m_enumerator->setEnumerateDirectory(desktopUri);
    m_enumerator->connect(m_enumerator, &FileEnumerator::enumerateFinished, this, &DesktopItemModel::onEnumerateFinished);
    m_enumerator->enumerateAsync();
    endResetModel();
}

int DesktopItemModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_files.count();
}

QVariant DesktopItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    //qDebug()<<"data"<<m_files.at(index.row())->uri();
    auto info = m_files.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return info->displayName();
    case Qt::ToolTipRole:
        return info->displayName();
    case Qt::DecorationRole: {
        //auto thumbnail = info->thumbnail();
        auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(info->uri());
        if (!thumbnail.isNull()) {
            if (info->uri().endsWith(".desktop") && !info->canExecute()) {
                return QIcon::fromTheme(info->iconName(), QIcon::fromTheme("text-x-generic"));
            }
            return thumbnail;
        }
        return QIcon::fromTheme(info->iconName(), QIcon::fromTheme("text-x-generic"));
    }
    case UriRole:
        return info->uri();
    case IsLinkRole:
        return info->isSymbolLink();
    }
    return QVariant();
}

void DesktopItemModel::onEnumerateFinished()
{
    //beginResetModel();
    beginRemoveRows(QModelIndex(), 0, m_files.count() - 1);
    m_files.clear();
    endRemoveRows();

    auto computer = FileInfo::fromUri("computer:///", true);
    auto personal = FileInfo::fromPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation), true);
    auto trash = FileInfo::fromUri("trash:///", true);

    QList<std::shared_ptr<FileInfo>> infos;

    infos<<computer;
    infos<<trash;
    infos<<personal;

    infos<<m_enumerator->getChildren();

    //qDebug()<<m_files.count();
    //this->endResetModel();
    for (auto info : infos) {
        beginInsertRows(QModelIndex(), m_files.count(), m_files.count());
        auto syncJob = new FileInfoJob(info);
        syncJob->querySync();
        syncJob->deleteLater();
        m_files<<info;
        endInsertRows();

        if (info->isDesktopFile()) {
            ThumbnailManager::getInstance()->updateDesktopFileThumbnail(info->uri(), m_thumbnail_watcher);
        } else {
            ThumbnailManager::getInstance()->createThumbnail(info->uri(), m_thumbnail_watcher);
        }
    }
    for (auto info : m_files) {
        auto uri = info->uri();
        auto view = PeonyDesktopApplication::getIconView();
        auto pos = view->getFileMetaInfoPos(info->uri());
        if (pos.x() >= 0) {
            view->updateItemPosByUri(info->uri(), pos);
        } else {
            view->ensureItemPosByUri(uri);
        }
    }

    Q_EMIT refreshed();

    //qDebug()<<"startMornitor";
    m_trash_watcher->startMonitor();
    if (m_desktop_watcher->currentUri() != "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)) {
        m_desktop_watcher->stopMonitor();
        m_desktop_watcher->forceChangeMonitorDirectory("file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
        m_desktop_watcher->setMonitorChildrenChange(true);
    }
    m_desktop_watcher->startMonitor();
    m_system_app_watcher->startMonitor();
    m_andriod_app_watcher->startMonitor();
}

const QModelIndex DesktopItemModel::indexFromUri(const QString &uri)
{
    for (auto info : m_files) {
        if (info->uri() == uri) {
            return index(m_files.indexOf(info));
        }
    }
    return QModelIndex();
}

const QString DesktopItemModel::indexUri(const QModelIndex &index)
{
    if (index.row() < 0 || index.row() >= m_files.count()) {
        return nullptr;
    }
    return m_files.at(index.row())->uri();
}

bool DesktopItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count);
    endInsertRows();
    return true;
}

bool DesktopItemModel::insertRow(int row, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row);
    endInsertRows();
    return true;
}

bool DesktopItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count);
    endRemoveRows();
    return true;
}

bool DesktopItemModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    endRemoveRows();
    return true;
}

Qt::ItemFlags DesktopItemModel::flags(const QModelIndex &index) const
{
    auto uri = index.data(UriRole).toString();
    auto info = FileInfo::fromUri(uri, false);
    if (index.isValid()) {
        Qt::ItemFlags flags = QAbstractItemModel::flags(index);
        flags |= Qt::ItemIsDragEnabled;
        flags |= Qt::ItemIsEditable;
        if (info->isDir()) {
            flags |= Qt::ItemIsDropEnabled;
        }
        return flags;
    } else {
        return Qt::ItemIsDropEnabled;
    }
}

QMimeData *DesktopItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* data = QAbstractItemModel::mimeData(indexes);
    //set urls data URLs correspond to the MIME type text/uri-list.
    QList<QUrl> urls;
    for (auto index : indexes) {
        QUrl url = index.data(UriRole).toString();
        if (!urls.contains(url))
            urls<<url;
    }
    data->setUrls(urls);
    return data;
}

bool DesktopItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    //qDebug()<<row<<column;
    //qDebug()<<"drop mime data"<<parent.data()<<index(row, column, parent).data();
    //judge the drop dest uri.
    QString destDirUri = nullptr;
    if (parent.isValid()) {
        destDirUri = parent.data(UriRole).toString();
    } else {
        destDirUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    }

    //if destDirUri was not set, do not execute a drop.
    if (destDirUri.isNull()) {
        return false;
    }

    auto info = FileInfo::fromUri(destDirUri);
    if (!info->isDir()) {
        return false;
    }

    //NOTE:
    //do not allow drop on it self.
    auto urls = data->urls();
    if (urls.isEmpty()) {
        return false;
    }

    QStringList srcUris;
    for (auto url : urls) {
        //can not drag file from recent
        if (url.url().startsWith("recent://"))
            return false;
        srcUris<<url.url();
    }
    srcUris.removeDuplicates();

    //can not drag file to recent
    if (destDirUri.startsWith("recent://"))
        return false;

    //not allow drag file to itself
    if (srcUris.contains(destDirUri)) {
        return false;
    }

    bool b_trash_item = false;
    for(auto path : srcUris)
    {
        if (path.contains("trash:///"))
        {
            b_trash_item = true;
            break;
        }
    }
    //drag from trash to another place, return false
    if (b_trash_item && destDirUri != "trash///")
        return false;

    auto fileOpMgr = FileOperationManager::getInstance();
    bool addHistory = true;
    if (destDirUri == "trash:///") {
        FileTrashOperation *trashOp = new FileTrashOperation(srcUris);
        fileOpMgr->startOperation(trashOp, addHistory);
    } else {
        qDebug() << "DesktopItemModel dropMimeData:" <<action;
        switch (action) {
        case Qt::MoveAction: {
            qDebug() << "DesktopItemModel moveOp";
            FileMoveOperation *moveOp = new FileMoveOperation(srcUris, destDirUri);
            moveOp->setCopyMove(true);
            fileOpMgr->startOperation(moveOp, addHistory);
            break;
        }
        case Qt::CopyAction: {
            qDebug() << "DesktopItemModel copyOp";
            FileCopyOperation *copyOp = new FileCopyOperation(srcUris, destDirUri);
            fileOpMgr->startOperation(copyOp);
            break;
        }
        default:
            break;
        }
    }

    //NOTE:
    //we have to handle the dnd with file operation, so do not
    //use QAbstractModel::dropMimeData() here;
    return false;
}

Qt::DropActions DesktopItemModel::supportedDropActions() const
{
    //return Qt::MoveAction|Qt::CopyAction;
    return QAbstractItemModel::supportedDropActions();
}
