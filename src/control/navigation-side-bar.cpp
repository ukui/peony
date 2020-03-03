/*
 * Peony-Qt
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "navigation-side-bar.h"
#include "side-bar-model.h"
#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-abstract-item.h"
#include <QHeaderView>

SideBar::SideBar(QWidget *parent) : QTreeView(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    header()->hide();

    m_model = new Peony::SideBarModel(this);
    m_proxy_model = new Peony::SideBarProxyFilterSortModel(this);
    m_proxy_model->setSourceModel(m_model);

    this->setModel(m_proxy_model);

    connect(this, &QTreeView::expanded, this, [=](const QModelIndex &index){
        auto item = m_proxy_model->itemFromIndex(index);
        item->findChildrenAsync();
    });

    connect(this, &QTreeView::collapsed, this, [=](const QModelIndex &index){
        auto item = m_proxy_model->itemFromIndex(index);
        item->clearChildren();
    });

    connect(this, &QTreeView::clicked, this, [=](const QModelIndex &index){
        if (index.column() == 0) {
            auto uri = index.data(Qt::UserRole).toString();
            if (!uri.isNull())
                Q_EMIT this->updateLocationRequest(uri);
        }
        if (index.column() == 1) {
            auto item = m_proxy_model->itemFromIndex(index);
            if (item->isMounted()) {
                item->unmount();
            }
        }
    });

    expandAll();
}
