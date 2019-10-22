#include "file-item-model.h"
#include "file-item.h"
#include "file-info.h"

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"

#include "file-utils.h"

#include <QIcon>
#include <QMimeData>
#include <QUrl>

#include <QDebug>

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
        return;
    }
    auto info = FileInfo::fromUri(uri);
    auto item = new FileItem(info, nullptr, this);
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
        if (child->uri() == uri) {
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
    return ModifiedDate+1;
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

    //qDebug()<<item->m_info->uri();
    switch (index.column()) {
    case FileName:{
        switch (role) {
        case Qt::TextAlignmentRole: {
            return QVariant(Qt::AlignHCenter | Qt::AlignBaseline);
        }
        case Qt::DisplayRole:{
            return QVariant(item->m_info->displayName());
        }
        case Qt::DecorationRole:{
            /**
              \todo handle the desktop file icon
              */
            QIcon icon = QIcon::fromTheme(item->m_info->iconName(), QIcon::fromTheme("text-x-generic"));
            return QVariant(icon);
        }
        case Qt::ToolTipRole: {
            return QVariant(item->m_info->displayName());
        }
        case UriRole: {
            return QVariant(item->uri());
        }
        default:
            return QVariant();
        }
    }
    case FileSize:{
        switch (role) {
        case Qt::DisplayRole:{
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
    case FileType:
        switch (role) {
        case Qt::DisplayRole:
            return QVariant(item->m_info->fileType());
        default:
            return QVariant();
        }
    case ModifiedDate:{
        switch (role) {
        case Qt::DisplayRole:
            return QVariant(item->m_info->modifiedDate());
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
        switch (section) {
        case FileName:
            return tr("File Name");
        case FileSize:
            return tr("File Size");
        case FileType:
            return tr("File Type");
        case ModifiedDate:
            return tr("Modified Date");
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
        flags |= Qt::ItemIsDragEnabled;
        auto item = itemFromIndex(index);
        if (item->m_info->isDir()) {
            flags |= Qt::ItemIsDropEnabled;
        }
        if (index.column() == FileName) {
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
    for (auto index : indexes) {
        auto item = itemFromIndex(index);
        QUrl url = item->m_info->uri();
        urls<<url;
    }
    data->setUrls(urls);
    return data;
}

Qt::DropActions FileItemModel::supportedDropActions() const
{
    //qDebug()<<"supportedDropActions";
    return Qt::MoveAction;//|Qt::CopyAction;
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

    //NOTE:
    //do not allow drop on it self.
    auto urls = data->urls();
    if (urls.isEmpty()) {
        return false;
    }

    QStringList srcUris;
    for (auto url : urls) {
        srcUris<<url.url();
    }

    if (srcUris.contains(destDirUri)) {
        return false;
    }

    auto fileOpMgr = FileOperationManager::getInstance();
    bool addHistory = true;
    switch (action) {
    case Qt::MoveAction: {
        FileMoveOperation *moveOp = new FileMoveOperation(srcUris, destDirUri);
        fileOpMgr->startOperation(moveOp, addHistory);
        break;
    }
    case Qt::CopyAction: {
        FileCopyOperation *copyOp = new FileCopyOperation(srcUris, destDirUri);
        fileOpMgr->startOperation(copyOp);
        break;
    }
    default:
        break;
    }

    //NOTE:
    //we have to handle the dnd with file operation, so do not
    //use QAbstractModel::dropMimeData() here;
    return true;
}
