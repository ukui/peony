#include "file-item-model.h"
#include "file-item.h"
#include "file-info.h"

#include <QIcon>

#include <QDebug>

using namespace Peony;

FileItemModel::FileItemModel(QObject *parent) : QAbstractItemModel (parent)
{
    connect(this, &FileItemModel::foundChildren, this, &FileItemModel::onFoundChildren);
}

FileItemModel::~FileItemModel()
{
    disconnect();
    if (m_root_item)
        delete m_root_item;
}

void FileItemModel::setRootItem(FileItem *item)
{
    beginResetModel();
    m_root_item->disconnect();
    delete m_root_item;

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

FileItem *FileItemModel::itemFromIndex(const QModelIndex &index)
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

QModelIndex FileItemModel::parent(const QModelIndex &child) const
{
    FileItem *childItem = static_cast<FileItem*>(child.internalPointer());
    //root children
    if (!childItem->m_parent)
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
        return m_root_item->m_children->count();
    }
    FileItem *parent_item = static_cast<FileItem*>(parent.internalPointer());
    return parent_item->m_children->count();
}

QVariant FileItemModel::data(const QModelIndex &index, int role) const
{
    FileItem *item = static_cast<FileItem*>(index.internalPointer());
    //qDebug()<<item->m_info->uri();
    switch (index.column()) {
    case FileName:{
        switch (role) {
        case Qt::DisplayRole:{
            return QVariant(item->m_info->displayName());
        }
        case Qt::DecorationRole:{
            return QVariant(QIcon::fromTheme(item->m_info->iconName()));
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
    if (parent_item->hasChildren())
        return true;
    return false;
}

Qt::ItemFlags FileItemModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
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
