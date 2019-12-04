#include "desktop-item-proxy-model.h"
#include "file-info.h"
#include "file-meta-info.h"

#include <QDebug>

using namespace Peony;

bool startWithChinese(const QString &displayName)
{
    //NOTE: a newly created file might could not get display name soon.
    if (displayName.isEmpty()) {
        return false;
    }
    auto firstStrUnicode = displayName.at(0).unicode();
    return (firstStrUnicode <=0x9FA5 && firstStrUnicode >= 0x4E00);
}

DesktopItemProxyModel::DesktopItemProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

bool DesktopItemProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return false;

    auto sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    auto uri = sourceIndex.data(Qt::UserRole).toString();
    auto info = FileInfo::fromUri(uri);
    //qDebug()<<"fiter"<<uri<<info->displayName();
    if (info->displayName().isNull()) {
        return false;
    }
    if (info->displayName().startsWith(".")) {
        return false;
    }
    return true;
}

bool DesktopItemProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    //qDebug()<<"less than";
    if (m_sort_type == Other) {
        return true;
    }

    auto leftUri = source_left.data(Qt::UserRole).toString();
    auto leftInfo = FileInfo::fromUri(leftUri);
    auto leftMetaInfo = FileMetaInfo::fromUri(leftUri);

    auto rightUri = source_right.data(Qt::UserRole).toString();
    auto rightInfo = FileInfo::fromUri(rightUri);
    auto rightMetaInfo = FileMetaInfo::fromUri(rightUri);

    //computer home and trash first
    if (source_left.row() < 3) {
        if (source_right.row() < source_left.row()) {
            return (sortOrder()==Qt::AscendingOrder)? false: true;
        }
        return (sortOrder()==Qt::AscendingOrder)? true: false;
    }
    if (source_right.row() < 3) {
        if (source_left.row() < source_right.row()) {
            return (sortOrder()==Qt::AscendingOrder)? true: false;
        }
        return (sortOrder()==Qt::AscendingOrder)? false: true;
    }

    //dir first
    if (leftInfo->isDir()) {
        if (rightInfo->isDir()) {
            //do nothing
        } else {
            return (sortOrder()==Qt::AscendingOrder)? true: false;
        }
    } else {
        if (rightInfo->isDir()) {
            return (sortOrder()==Qt::AscendingOrder)? false: true;
        }
    }

    //qDebug()<<"sort===================================="<<SortType(m_sort_type)<<m_sort_type;
    switch (m_sort_type) {
    case FileName: {
        if (startWithChinese(leftInfo->displayName())) {
            if (!startWithChinese(rightInfo->displayName())) {
                return (sortOrder()==Qt::AscendingOrder)? true: false;
            } else {
                //chinese pinyin sort order is reversed compared with english.
                return !QSortFilterProxyModel::lessThan(source_left, source_right);
            }
        } else {
            if (startWithChinese(rightInfo->displayName())) {
                return (sortOrder()==Qt::AscendingOrder)? false: true;
            }
        }
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
    case FileType: {
        return leftInfo->type() < rightInfo->type();
    }
    case FileSize: {
        return leftInfo->size() < rightInfo->size();
    }
    case ModifiedDate: {
        return leftInfo->modifiedTime() < rightInfo->modifiedTime();
    }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
