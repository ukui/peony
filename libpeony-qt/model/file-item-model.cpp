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

#include "file-item-model.h"
#include "file-item.h"
#include "file-info.h"
#include "file-info-job.h"

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"

#include "file-utils.h"

#include "thumbnail-manager.h"
#include "global-settings.h"

#include "file-operation-utils.h"

#include <QIcon>
#include <QMimeData>
#include <QUrl>

#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QGSettings>

using namespace Peony;

FileItemModel::FileItemModel(QObject *parent) : QAbstractItemModel (parent)
{
    setPositiveResponse(true);
}

FileItemModel::~FileItemModel()
{
    qDebug()<<"~FileItemModel";
    disconnect();
    if (m_root_item)
        delete m_root_item;
}

const QString FileItemModel::getRootUri()
{
    if (!m_root_item)
        return nullptr;
    return m_root_item->uri();
}

void FileItemModel::setRootUri(const QString &uri)
{
    if (uri.isNull()) {
        setRootUri("file:///");
        m_root_uri = "file:///";
        return;
    }
    m_root_uri = uri;
    auto info = FileInfo::fromUri(uri);
    auto item = new FileItem(info, nullptr, this, this);
    setRootItem(item);
}

void FileItemModel::setRootItem(FileItem *item)
{
    beginResetModel();
    m_root_item->deleteLater();

    m_root_item = item;
    m_root_item->findChildrenAsync();

    endResetModel();
}

QModelIndex FileItemModel::index(int row, int column, const QModelIndex &parent) const
{
    //root children
    if (!parent.isValid()) {
        if (row < 0 || row > m_root_item->m_children->count()-1)
            return QModelIndex();
        return createIndex(row, column, m_root_item->m_children->at(row));
    }

    FileItem *item = static_cast<FileItem*>(parent.internalPointer());
    if (row < 0 || row > item->m_children->count()-1)
        return QModelIndex();
    return createIndex(row, column, item->m_children->at(row));
}

FileItem *FileItemModel::itemFromIndex(const QModelIndex &index) const
{
    return static_cast<FileItem*>(index.internalPointer());
}

QModelIndex FileItemModel::firstColumnIndex(FileItem *item)
{
    //root children
    if (item->m_parent == nullptr) {
        for (int i = 0; i < m_root_item->m_children->count(); i++) {
            //qDebug()<<i<<item->m_info->uri()<<m_root_item->m_children->at(i)->m_info->uri();
            if (item == m_root_item->m_children->at(i)) {
                //qDebug()<<i<<item->m_info->uri();
                return createIndex(i, 0, item);
            }
        }
        return QModelIndex();
    } else {
        //has parent item
        for (int i = 0; i < item->m_parent->m_children->count(); i++) {
            if (item == item->m_parent->m_children->at(i))
                return createIndex(i, 0, item);
        }
        return QModelIndex();
    }
}

QModelIndex FileItemModel::lastColumnIndex(FileItem *item)
{
    if (!item->m_parent) {
        for (int i = 0; i < m_root_item->m_children->count(); i++) {
            //qDebug()<<i<<item->m_info->uri()<<m_root_item->m_children->at(i)->m_info->uri();
            if (item == m_root_item->m_children->at(i)) {
                //qDebug()<<i<<item->m_info->uri();
                return createIndex(i, Other, item);
            }
        }
        return QModelIndex();
    } else {
        //has parent item
        for (int i = 0; i < item->m_parent->m_children->count(); i++) {
            if (item == item->m_parent->m_children->at(i))
                return createIndex(i, Other, item);
        }
        return QModelIndex();
    }
}

const QModelIndex FileItemModel::indexFromUri(const QString &uri)
{
    //FIXME: support recursively finding?
    for (auto child : *m_root_item->m_children) {
        GFile *left = g_file_new_for_uri(child->uri().toUtf8().constData());
        GFile *right = g_file_new_for_uri(uri.toUtf8().constData());
        bool equal = g_file_equal(left, right);
        g_object_unref(left);
        g_object_unref(right);
        if (equal) {
            return child->firstColumnIndex();
        }
    }
    return QModelIndex();
}

QModelIndex FileItemModel::parent(const QModelIndex &child) const
{
    FileItem *childItem = static_cast<FileItem*>(child.internalPointer());
    //root children
    if (childItem->m_parent == nullptr)
        return QModelIndex();
    return childItem->m_parent->firstColumnIndex();
}

int FileItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return FileSize+1;
}

int FileItemModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        if (!m_root_item) {
            return 0;
        }
        return m_root_item->m_children->count();
    }
    FileItem *parent_item = static_cast<FileItem*>(parent.internalPointer());
    return parent_item->m_children->count();
}

QVariant FileItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    FileItem *item = static_cast<FileItem*>(index.internalPointer());

    // we have to add uri role to every valid index, so that we can ensure
    // that we can open the file/directory correctly.
    if (role == FileItemModel::UriRole)
        return QVariant(item->uri());

    //qDebug()<<"data:" <<item->m_info->uri() << index.column();
    switch (index.column()) {
    case FileName: {
        switch (role) {
        case Qt::TextAlignmentRole: {
            return QVariant(Qt::AlignHCenter | Qt::AlignBaseline);
        }
        case Qt::DisplayRole: {
            //fix bug#53504, desktop files not show same name issue
            if (item->m_info->isDesktopFile())
            {
                auto displayName = FileUtils::handleDesktopFileName(item->m_info->uri(), item->m_info->displayName());
                return QVariant(displayName);
            }
            return QVariant(item->m_info->displayName());
        }
        case Qt::DecorationRole: {
            auto thumbnail = ThumbnailManager::getInstance()->tryGetThumbnail(item->m_info->uri());
            if (!thumbnail.isNull()) {
                return thumbnail;
            }
            QIcon icon = QIcon::fromTheme(item->m_info->iconName(), QIcon::fromTheme("text-x-generic"));
            return QVariant(icon);
        }
        case Qt::ToolTipRole: {
            //fix bug#53504, desktop files not show same name issue
            if (item->m_info->isDesktopFile())
            {
                auto displayName = FileUtils::handleDesktopFileName(item->m_info->uri(), item->m_info->displayName());
                return QVariant(displayName);
            }
            return QVariant(item->m_info->displayName());
        }
        default:
            return QVariant();
        }
    }
    case ModifiedDate: {
        switch (role) {
        case Qt::DisplayRole:
            //trash files show delete Date
            if (m_root_uri.startsWith("trash://") && !item->m_info->deletionDate().isNull()) {
                QDateTime deleteTime = QDateTime::fromMSecsSinceEpoch(item->m_info->deletionTime (), Qt::LocalTime);
                QString format = GlobalSettings::getInstance()->getSystemTimeFormat();
                return QVariant(deleteTime.toString(format));
            }
            return QVariant(item->m_info->modifiedDate());
        default:
            return QVariant();
        }
    }
    case FileType:
        switch (role) {
        case Qt::DisplayRole: {
            if (item->m_info->isSymbolLink()) {
                return QVariant(tr("Symbol Link, ") + item->m_info->fileType());
            }
            return QVariant(item->m_info->fileType());
        }
        default:
            return QVariant();
        }
    case FileSize: {
        switch (role) {
        case Qt::DisplayRole: {
            if (item->hasChildren()) {
                if (item->m_expanded) {
                    return QVariant(QString::number(item->m_children->count()) + tr("child(ren)"));
                }
                return QVariant();
            }
            return QVariant(item->m_info->fileSize());
        }
        default:
            return QVariant();
        }
    }
    default:
        return QVariant();
    }
}

QVariant FileItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
        return QVariant();
    if (role == Qt::DisplayRole) {
        //qDebug() <<"headerData:" <<section;
        switch (section) {
        case FileName:
            return tr("File Name");
        case ModifiedDate:
            //trash files show delete Date
            if (m_root_uri.startsWith("trash:///"))
                return tr("Delete Date");
            return tr("Modified Date");
        case FileType:
            return tr("File Type");
        case FileSize:
            return tr("File Size");
        default:
            return QVariant();
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool FileItemModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return true;
    FileItem *parent_item = static_cast<FileItem*>(parent.internalPointer());
    if (parent_item->hasChildren() && m_can_expand)
        return true;
    return false;
}

Qt::ItemFlags FileItemModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        Qt::ItemFlags flags = QAbstractItemModel::flags(index);

        auto item = itemFromIndex(index);
        if (item->m_info->isDir()) {
            flags |= Qt::ItemIsDropEnabled;
        }
        if (index.column() == FileName) {
            flags |= Qt::ItemIsDragEnabled;
            flags |= Qt::ItemIsEditable;
        }
        return flags;
    } else {
        return Qt::ItemIsDropEnabled;
    }
}

bool FileItemModel::canFetchMore(const QModelIndex &parent) const
{
    //qDebug()<<"canFetchMore";
    if (!parent.isValid())
        return true;
    FileItem *parent_item = static_cast<FileItem*>(parent.internalPointer());
    if (!parent_item->m_expanded) {
        return true;
    }
    return false;
}

void FileItemModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
    //do not fetch more here
}

bool FileItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool FileItemModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return true;
}

bool FileItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool FileItemModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}

void FileItemModel::onFoundChildren(const QModelIndex &parent)
{
    if (!parent.isValid()) {
        return;
    }
    FileItem *parentItem = static_cast<FileItem*>(parent.internalPointer());
    beginInsertRows(parent, 0, parentItem->m_children->count() - 1);
    endInsertRows();
}

void FileItemModel::onItemAdded(FileItem *item)
{
    if (!item->m_parent)
        insertRow(item->firstColumnIndex().row());
    insertRow(item->firstColumnIndex().row(), item->m_parent->firstColumnIndex());
}

void FileItemModel::onItemRemoved(FileItem *item)
{
    if (!item->m_parent)
        removeRow(item->firstColumnIndex().row());
    removeRow(item->firstColumnIndex().row(), item->m_parent->firstColumnIndex());
}

void FileItemModel::cancelFindChildren()
{
    qDebug()<<"cancel";
    m_root_item->cancelFindChildren();
}

void FileItemModel::setRootIndex(const QModelIndex &index)
{
    //NOTE: if we use proxy model, we might get the wrong item from index.
    //add the new data role save the file's uri to resolve this problem.
    if (index.isValid()) {
        auto new_root_info = FileInfo::fromUri(index.data(UriRole).toString());
        auto new_root_item = new FileItem(new_root_info,
                                          nullptr,
                                          this);
        if (new_root_item->hasChildren()) {
            setRootItem(new_root_item);
        }
    }
}

QMimeData *FileItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* data = QAbstractItemModel::mimeData(indexes);
    //set urls data URLs correspond to the MIME type text/uri-list.
    QList<QUrl> urls;
    QStringList uris;
    for (auto index : indexes) {
        auto item = itemFromIndex(index);
        auto encodeUrl = Peony::FileUtils::urlEncode(item->m_info->uri());
        QUrl url = encodeUrl;
        if (!urls.contains(url)) {
            qDebug() << "mimeData:" << url;

            urls << url;
            uris << encodeUrl;
        }
    }
    data->setUrls(urls);
    auto string = uris.join(" ");
    data->setData("peony-qt/encoded-uris", string.toUtf8());
    data->setText(string);
    return data;
}

Qt::DropActions FileItemModel::supportedDropActions() const
{
    //qDebug()<<"supportedDropActions";
    return Qt::MoveAction|Qt::CopyAction;
}

Qt::DropActions FileItemModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

bool FileItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    qDebug()<<"drop mime data";
    //judge the drop dest uri.
    QString destDirUri = nullptr;
    if (parent.isValid()) {
        QModelIndex child = index(row, column, parent);
        if (child.isValid()) {
            //unexpected drop.
            /*
            auto item = static_cast<FileItem*>(child.internalPointer());
            qDebug()<<item->m_info->uri();
            if (item->m_info->isDir()) {
                destDirUri = item->m_info->uri();
            }
            */
        } else {
            //drop on a folder item.
            auto parentItem = itemFromIndex(parent);
            destDirUri = parentItem->m_info->uri();
        }
    } else {
        //FIXME: for a mounted volume (for example, computer:///),
        //we have to set the dest dir uri as its mount point.
        //maybe i should do this when set model root item.
        destDirUri = m_root_item->m_info->uri();
        auto targetUri = FileUtils::getTargetUri(destDirUri);
        if (!targetUri.isEmpty()) {
            destDirUri = targetUri;
        }
    }

    //if destDirUri was not set, do not execute a drop.
    if (destDirUri.isNull()) {
        return false;
    }

    auto info = Peony::FileInfo::fromUri(destDirUri);
    //qDebug() << "FileItemModel::dropMimeData:" <<info->isDir() <<info->type();
    //if (!FileUtils::getFileIsFolder(destDirUri))
    //fix drag file to folder symbolic fail issue
    if (! info->isDir() && ! destDirUri.startsWith("trash:///"))
        return false;

    //NOTE:
    //do not allow drop on it self.
    auto urls = data->urls();

    QStringList srcUris;
    if (data->hasFormat("peony-qt/encoded-uris")) {
        srcUris = data->text().split(" ");
        for (QString uri : srcUris) {
            if (uri.startsWith("recent://"))
                srcUris.removeOne(uri);
        }
    } else {
        for (auto url : urls) {
            //can not drag file from recent
            if (url.url().startsWith("recent://"))
                return false;
            srcUris<<url.url();
        }
    }

    srcUris.removeDuplicates();
    if (srcUris.isEmpty())
        return false;

    //can not drag file to recent
    if (destDirUri.startsWith("recent://"))
        return false;

    if (srcUris.contains(destDirUri)) {
        return false;
    }

    //can not move StandardPath to any dir
    if (action == Qt::MoveAction && FileUtils::containsStandardPath(srcUris)) {
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
    if (b_trash_item && destDirUri != "trash:///")
        return false;

    //fix drag file to trash issue, #42328
    if (destDirUri.startsWith("trash:///"))
    {
        // fix drag filesafe to trash error issue, #81938
        if(!(srcUris.first().startsWith("filesafe:///") &&
            (QString(srcUris.first()).remove("filesafe:///").indexOf("/") == -1))) {
            FileOperationUtils::trash(srcUris, true);
        }
        return true;
    }

    qDebug() << "dropMimeData:" <<action<<destDirUri;
    bool addHistory = true;
    //krme files can not move to other place, default set as copy action
    if (srcUris.first().startsWith("kmre:///") || srcUris.first().startsWith("kydroid:///"))
        action = Qt::CopyAction;

    //filesafe files can not move to other place, default set as copy action
    if (srcUris.first().startsWith("filesafe:///") && destDirUri.startsWith("favorite:///"))
        return false;
    if (srcUris.first().startsWith("filesafe:///"))
        action = Qt::CopyAction;

    auto op = FileOperationUtils::moveWithAction(srcUris, destDirUri, addHistory, action);
    connect(op, &FileOperation::operationFinished, this, [=](){
        auto opInfo = op->getOperationInfo();
        auto targetUris = opInfo.get()->dests();
        Q_EMIT this->selectRequest(targetUris);
//            auto selectionModel = new QItemSelectionModel(this);
//            selectionModel->clearSelection();
//            QTimer::singleShot(1000, selectionModel, [=](){
//                for (auto destUri : targetUris) {
//                    auto index = indexFromUri(destUri);
//                    selectionModel->select(index, QItemSelectionModel::Select);
//                }
//                selectionModel->deleteLater();
//            });
    }, Qt::BlockingQueuedConnection);

    //NOTE:
    //we have to handle the dnd with file operation, so do not
    //use QAbstractModel::dropMimeData() here;
    return true;
}

void FileItemModel::sendPathChangeRequest(const QString &destUri, const QString &sourceUri)
{
    Q_EMIT this->changePathRequest(destUri, sourceUri);
}
