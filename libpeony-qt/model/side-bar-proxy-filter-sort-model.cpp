#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-abstract-item.h"
#include "side-bar-model.h"

#include <QDebug>

using namespace Peony;

SideBarProxyFilterSortModel::SideBarProxyFilterSortModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

bool SideBarProxyFilterSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    auto index = sourceModel()->index(sourceRow, 0, sourceParent);
    auto item = static_cast<SideBarAbstractItem*>(index.internalPointer());
    if (item) {
        if (!item->displayName().isEmpty()) {
            if (item->displayName().at(0) == ".") {
                return false;
            }
        }
    }
    return true;
}

bool SideBarProxyFilterSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    qDebug()<<"less than";
    if (!(left.isValid() && right.isValid())) {
        return QSortFilterProxyModel::lessThan(left, right);
    }
    auto leftItem = static_cast<SideBarAbstractItem*>(left.internalPointer());
    auto rightItem = static_cast<SideBarAbstractItem*>(right.internalPointer());
    if (leftItem->type() != SideBarAbstractItem::FileSystemItem || rightItem->type() != SideBarAbstractItem::FileSystemItem) {
        return false;
    }
    qDebug()<<leftItem->displayName()<<rightItem->displayName();
    return leftItem->displayName().compare(rightItem->displayName()) > 0;
}

SideBarAbstractItem *SideBarProxyFilterSortModel::itemFromIndex(const QModelIndex &proxy_index)
{
    SideBarModel *model = static_cast<SideBarModel*>(sourceModel());
    auto index = mapToSource(proxy_index);
    return model->itemFromIndex(index);
}
