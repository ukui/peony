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

#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-abstract-item.h"
#include "side-bar-model.h"

#include "file-utils.h"
#include "file-info.h"
#include "file-info-job.h"

#include <QUrl>
#include <QDir>
#include <QDebug>

using namespace Peony;

SideBarProxyFilterSortModel::SideBarProxyFilterSortModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

bool SideBarProxyFilterSortModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    auto index = sourceModel()->index(sourceRow, 0, sourceParent);
    auto item = static_cast<SideBarAbstractItem*>(index.internalPointer());

    if (item->type() != SideBarAbstractItem::SeparatorItem) {
        if (item->displayName().isNull() && item->type() == SideBarAbstractItem::FileSystemItem)
            return false;

        //not exist path filter
        if (item->type() == SideBarAbstractItem::FavoriteItem && ! item->uri().isEmpty())
        {
            QDir dir(QUrl(item->uri()).path());
            if (! dir.exists())
                return false;
        }
    }
    //comment to fix bug 41426, user add .config file to bookmark for convinient accesss
//    if (item) {
//        if (!item->displayName().isEmpty()) {
//            if (QString(item->displayName().at(0)) == ".") {
//                return false;
//            }
//        }
//    }
    if (item->type() == SideBarAbstractItem::FileSystemItem) {
        if (sourceParent.data(Qt::UserRole).toString() == "computer:///") {
            if (item->uri() != "computer:///root.link") {

                //FIXME: replace BLOCKING api in ui thread.
                auto gvfsFileInfo = FileInfo::fromUri(item->uri());
                if (gvfsFileInfo->displayName().isEmpty() || gvfsFileInfo->targetUri().isEmpty()) {
                        FileInfoJob j(gvfsFileInfo);
                        j.querySync();
                }
                QString gvfsDisplayName = gvfsFileInfo->displayName();
                QString gvfsUnixDevice = gvfsFileInfo->unixDeviceFile();

                // FIX bug 43701 This filters out mounted FTP file systems
                if (!gvfsUnixDevice.isNull()
                        && (gvfsFileInfo->targetUri().startsWith("ftp://")
                            || gvfsFileInfo->targetUri().startsWith("ftp://")))
                    return true;

                if(!gvfsUnixDevice.isNull() && !gvfsDisplayName.contains(":"))
                    return false;//Filter some non-mountable drive items

                if (item->isMounted())
                    return true;
                if (item->isRemoveable() && item->isMountable()) {
                    return true;
                }
                if (!item->isRemoveable() && !item->isEjectable())
                    return true;
                return false;
            }
        }
    }

    //special Volumn of 839 M upgrade part not show process
    if (item->uri().startsWith("file:///media/") && item->uri().endsWith("/2691-6AB8"))
        return false;

    return true;
}

bool SideBarProxyFilterSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    //qDebug()<<"less than";
    if (!(left.isValid() && right.isValid())) {
        return QSortFilterProxyModel::lessThan(left, right);
    }
    auto leftItem = static_cast<SideBarAbstractItem*>(left.internalPointer());
    auto rightItem = static_cast<SideBarAbstractItem*>(right.internalPointer());
    if (leftItem->type() != SideBarAbstractItem::FileSystemItem || rightItem->type() != SideBarAbstractItem::FileSystemItem) {
        return false;
    }
    //qDebug()<<leftItem->displayName()<<rightItem->displayName();
    return leftItem->displayName().compare(rightItem->displayName()) > 0;
}

SideBarAbstractItem *SideBarProxyFilterSortModel::itemFromIndex(const QModelIndex &proxy_index)
{
    SideBarModel *model = static_cast<SideBarModel*>(sourceModel());
    auto index = mapToSource(proxy_index);
    return model->itemFromIndex(index);
}
