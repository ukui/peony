#include "file-item-model.h"
#include "file-item.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-info.h"

#include <QDebug>
#include <QMessageBox>

using namespace Peony;

FileItemProxyFilterSortModel::FileItemProxyFilterSortModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void FileItemProxyFilterSortModel::setSourceModel(QAbstractItemModel *model)
{
    if (sourceModel())
        disconnect(sourceModel());
    QSortFilterProxyModel::setSourceModel(model);
    FileItemModel *file_item_model = static_cast<FileItemModel*>(model);
    connect(file_item_model, &FileItemModel::updated, this, &FileItemProxyFilterSortModel::update);
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

const QModelIndex FileItemProxyFilterSortModel::indexFromUri(const QString &uri)
{
    FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
    const QModelIndex sourceIndex = model->indexFromUri(uri);
    return mapFromSource(sourceIndex);
}

bool FileItemProxyFilterSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    //qDebug()<<left<<right;
    if (left.isValid() && right.isValid()) {
        FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
        auto leftItem = model->itemFromIndex(left);
        auto rightItem = model->itemFromIndex(right);
        if (!(leftItem->hasChildren() && rightItem->hasChildren())) {
            //make folder always has a higher order.
            if (!leftItem->hasChildren() && !rightItem->hasChildren()) {
                goto default_sort;
            }
            bool lesser = leftItem->hasChildren();
            if (sortOrder() == Qt::AscendingOrder)
                return lesser;
            return !lesser;
        }

default_sort:
        switch (sortColumn()) {
        case FileItemModel::FileName: {
            QString leftDisplayName = leftItem->m_info->displayName();
            QString rightDisplayName = rightItem->m_info->displayName();
            bool leftStartWithChinese = startWithChinese(leftDisplayName);
            bool rightStartWithChinese = startWithChinese(rightDisplayName);
            if (!(!leftStartWithChinese && !rightStartWithChinese)) {
                if (sortOrder() == Qt::AscendingOrder) {
                    return leftStartWithChinese;
                }
                return rightStartWithChinese;
            }
            return leftItem->m_info->displayName().toLower() < rightItem->m_info->displayName().toLower();
        }
        case FileItemModel::FileSize: {
            return leftItem->m_info->size() < rightItem->m_info->size();
        }
        case FileItemModel::FileType: {
            return leftItem->m_info->fileType() < rightItem->m_info->fileType();
        }
        case FileItemModel::ModifiedDate: {
            return leftItem->m_info->modifiedTime() < rightItem->m_info->modifiedTime();
        }
        default:
            break;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

bool FileItemProxyFilterSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    //FIXME:
    FileItemModel *model = static_cast<FileItemModel*>(sourceModel());
    //root
    auto childIndex = model->index(sourceRow, 0, sourceParent);
    if (childIndex.isValid()) {
        if (!m_show_hidden) {
            auto item = static_cast<FileItem*>(childIndex.internalPointer());
            qDebug()<<sourceRow<<item->m_info->displayName()<<model->rowCount(sourceParent);
            //QMessageBox::warning(nullptr, "filter", item->m_info->displayName());
            if (item->m_info->displayName() != nullptr) {
                if (item->m_info->displayName().at(0) == '.')
                    //qDebug()<<sourceRow<<item->m_info->displayName()<<model->rowCount(sourceParent);
                    return false;
            }
        }
        //regExp
    }
    return true;
}

void FileItemProxyFilterSortModel::update()
{
    invalidateFilter();
}

void FileItemProxyFilterSortModel::setShowHidden(bool showHidden)
{
    m_show_hidden = showHidden;
}

bool FileItemProxyFilterSortModel::startWithChinese(const QString &displayName) const
{
    //NOTE: a newly created file might could not get display name soon.
    if (displayName.isEmpty()) {
        return false;
    }
    auto firstStrUnicode = displayName.at(0).unicode();
    return (firstStrUnicode <=0x9FA5 && firstStrUnicode >= 0x4E00);
}
