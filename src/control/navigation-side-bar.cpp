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

#include "side-bar-menu.h"
#include "side-bar-abstract-item.h"

#include <QHeaderView>
#include <QPushButton>

#include <QEvent>

#include <QDebug>

NavigationSideBar::NavigationSideBar(QWidget *parent) : QTreeView(parent)
{
    installEventFilter(this);

    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);
    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    header()->hide();

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_model = new Peony::SideBarModel(this);
    m_proxy_model = new Peony::SideBarProxyFilterSortModel(this);
    m_proxy_model->setSourceModel(m_model);

    this->setModel(m_proxy_model);

    connect(this, &QTreeView::expanded, [=](const QModelIndex &index){
        auto item = m_proxy_model->itemFromIndex(index);
        qDebug()<<item->uri();
        /*!
          \bug can not expanded? enumerator can not get prepared signal, why?
          */
        item->findChildrenAsync();
    });

    connect(this, &QTreeView::collapsed, [=](const QModelIndex &index){
        auto item = m_proxy_model->itemFromIndex(index);
        item->clearChildren();
    });

    connect(this, &QTreeView::clicked, [=](const QModelIndex &index){
        switch (index.column()) {
        case 0: {
            auto item = m_proxy_model->itemFromIndex(index);
            //some side bar item doesn't have a uri.
            //do not emit signal with a null uri to window.
            if (!item->uri().isNull())
                Q_EMIT this->updateWindowLocationRequest(item->uri());
            break;
        }
        case 1: {
            auto item = m_proxy_model->itemFromIndex(index);
            if (item->isMounted()) {
                auto leftIndex = m_proxy_model->index(index.row(), 0, index.parent());
                this->collapse(leftIndex);
                item->unmount();
            }
            break;
        }
        default:
            break;
        }
    });

    connect(this, &QTreeView::customContextMenuRequested, this, [=](const QPoint &pos){
        auto index = indexAt(pos);
        auto item = m_proxy_model->itemFromIndex(index);
        if (item) {
            if (item->type() != Peony::SideBarAbstractItem::SeparatorItem) {
                Peony::SideBarMenu menu(item, nullptr);
                menu.exec(QCursor::pos());
            }
        }
    });

    expandAll();

    m_label_button = new QPushButton(QIcon::fromTheme("emblem-important-symbolic"), tr("All tags..."), this);
    m_label_button->setCheckable(true);

    connect(m_label_button, &QPushButton::clicked, this, &NavigationSideBar::labelButtonClicked);
}

bool NavigationSideBar::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::Resize) {
        m_label_button->resize(this->width() - 8, 33);
        m_label_button->move(4, this->height() - 37);
    }
    return false;
}

void NavigationSideBar::updateGeometries()
{
    setViewportMargins(4, 0, 4, 37);
    QTreeView::updateGeometries();
}
