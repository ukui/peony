#include "side-bar-model.h"
#include "side-bar-favorite-item.h"
#include "side-bar-personal-item.h"
#include "side-bar-file-system-item.h"
#include <QIcon>

#include <QDebug>

using namespace Peony;

SideBarModel::SideBarModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    beginResetModel();

    m_root_children = new QVector<SideBarAbstractItem*>();

    SideBarFavoriteItem *favorite_root_item = new SideBarFavoriteItem(nullptr, nullptr, this);
    m_root_children->append(favorite_root_item);
    //favorite_root_item->findChildren();

    SideBarPersonalItem *personal_root_item = new SideBarPersonalItem(nullptr, nullptr, this);
    m_root_children->append(personal_root_item);
    //personal_root_item->findChildren();

    SideBarFileSystemItem *computerItem = new SideBarFileSystemItem(nullptr,
                                                                    nullptr,
                                                                    this);
    m_root_children->append(computerItem);
    //computerItem->findChildren();

    endResetModel();
}

SideBarModel::~SideBarModel()
{
    for (auto child : *m_root_children) {
        delete child;
    }
    m_root_children->clear();
    delete m_root_children;
}

QModelIndex SideBarModel::firstCloumnIndex(SideBarAbstractItem *item)
{
    if (item->parent() != nullptr) {
        return createIndex(item->parent()->m_children->indexOf(item), 0, item);
    } else {
        for (auto child : *m_root_children) {
            if (item->type() == child->type()) {
                return createIndex(m_root_children->indexOf(child), 0, item);
            }
        }
    }
    return QModelIndex();
}

QModelIndex SideBarModel::lastCloumnIndex(SideBarAbstractItem *item)
{
    if (item->parent() != nullptr) {
        createIndex(item->parent()->m_children->indexOf(item), 1, item);
    } else {
        for (auto child : *m_root_children) {
            if (item->type() == child->type()) {
                return createIndex(m_root_children->indexOf(child), 1, item);
            }
        }
    }
    return QModelIndex();
}

SideBarAbstractItem *SideBarModel::itemFromIndex(const QModelIndex &index)
{
    return static_cast<SideBarAbstractItem*>(index.internalPointer());
}

QVariant SideBarModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

bool SideBarModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return false;
}

QModelIndex SideBarModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return createIndex(row, column, m_root_children->at(row));
    }
    SideBarAbstractItem *parentItem = static_cast<SideBarAbstractItem*>(parent.internalPointer());
    if (parentItem->m_children->count() > row) {
        return createIndex(row, column, parentItem->m_children->at(row));
    }
    return QModelIndex();
}

QModelIndex SideBarModel::parent(const QModelIndex &index) const
{
    SideBarAbstractItem *item = static_cast<SideBarAbstractItem*>(index.internalPointer());
    //qDebug()<<item->uri();
    if (!item)
        return QModelIndex();
    if (item->parent())
        return item->parent()->firstColumnIndex();

    return QModelIndex();
}

int SideBarModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_root_children->count();

    SideBarAbstractItem *parentItem = static_cast<SideBarAbstractItem*>(parent.internalPointer());
    qDebug()<<parentItem->uri();
    return parentItem->m_children->count();
}

int SideBarModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

bool SideBarModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return true;

    SideBarAbstractItem *parentItem = static_cast<SideBarAbstractItem*>(parent.internalPointer());
    return parentItem->hasChildren();
}

bool SideBarModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return true;
}

void SideBarModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent);
}

QVariant SideBarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    SideBarAbstractItem *item = static_cast<SideBarAbstractItem*>(index.internalPointer());
    if (index.column() == 1) {
        if (role == Qt::DecorationRole && item->isMounted())
            return QVariant(QIcon::fromTheme("media-eject"));
        else {
            return QVariant();
        }
    }
    switch (role) {
    case Qt::DecorationRole:
        return QIcon::fromTheme(item->iconName());
    case Qt::DisplayRole:
        return item->displayName();
    default:
        break;
    }
    return QVariant();
}

bool SideBarModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags SideBarModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index); // FIXME: Implement me!
}

bool SideBarModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool SideBarModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return true;
}

bool SideBarModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

bool SideBarModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
    return true;
}
