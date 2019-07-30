#include "file-item-proxy-filter-sort-model.h"
#include "file-info.h"

#include <QDebug>

using namespace Peony;

FileItemProxyFilterSortModel::FileItemProxyFilterSortModel(QSortFilterProxyModel *parent) : QSortFilterProxyModel(parent)
{

}

FileItem *FileItemProxyFilterSortModel::itemFromIndex(const QModelIndex &proxyIndex)
{
    FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
    QModelIndex index = mapToSource(proxyIndex);
    return model->itemFromIndex(index);
}

QModelIndex FileItemProxyFilterSortModel::getSourceIndex(const QModelIndex &proxyIndex)
{
    return mapToSource(proxyIndex);
}

bool FileItemProxyFilterSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    //qDebug()<<left<<right;
    if (left.isValid() && right.isValid()) {
        FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
        auto leftItem = model->itemFromIndex(left);
        auto rightItem = model->itemFromIndex(right);
        if (!(leftItem->hasChildren() && rightItem->hasChildren())) {
            return leftItem->hasChildren()? false: true;
        }
        //sort by column type.
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

bool FileItemProxyFilterSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    return true;
    FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
    //root
    auto childIndex = model->index(sourceRow, 0, sourceParent);
    if (childIndex.isValid()) {
        auto item = static_cast<FileItem*>(childIndex.internalPointer());
        if (item->m_info->displayName() != nullptr) {
            if (item->m_info->displayName().at(0) == '.')
                return false;
        }
    }
    return true;
}
