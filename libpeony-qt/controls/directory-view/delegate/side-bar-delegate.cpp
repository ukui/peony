/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "side-bar-delegate.h"
#include "side-bar.h"

#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-abstract-item.h"
#include "side-bar-separator-item.h"

#include <QPainter>

#include <QPushButton>

#include <QDebug>

using namespace Peony;

SideBarDelegate::SideBarDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_styled_button = new QPushButton;
}

SideBarDelegate::~SideBarDelegate()
{
    m_styled_button->deleteLater();
}

void SideBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //return QStyledItemDelegate::paint(painter, option, index);
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    //draw separator
    SideBar *sideBar = qobject_cast<SideBar*>(this->parent());
    SideBarProxyFilterSortModel *model = qobject_cast<SideBarProxyFilterSortModel*>(sideBar->model());
    auto item = model->itemFromIndex(index);
    if (item->type() == SideBarAbstractItem::SeparatorItem) {
        SideBarSeparatorItem *separator = qobject_cast<SideBarSeparatorItem*>(item);
        if  (separator->separatorType() != SideBarSeparatorItem::EmptyFile) {
            auto visualRect = sideBar->visualRect(index);
            visualRect.setX(0);
            painter->fillRect(visualRect, opt.widget->palette().brush(QPalette::Base));
            return;
        }
    }

    //FIXME: maybe i should use qss "show-decoration-selected" instead

    if (index.column() != 0 || !index.isValid()) {
        if (opt.state.testFlag(QStyle::State_Selected)) {
            painter->fillRect(opt.rect, m_styled_button->palette().highlight());
        } else if (opt.state.testFlag(QStyle::State_MouseOver)) {
            QColor color = m_styled_button->palette().highlight().color();
            color.setAlpha(127);
            painter->fillRect(opt.rect, color);
        }
        return QStyledItemDelegate::paint(painter, option, index);
    }

    auto sideBarView = sideBar;

    auto visualRect = sideBarView->visualRect(index);
    visualRect.setX(0);
    painter->fillRect(visualRect, opt.widget->palette().brush(QPalette::Base));

    int x = opt.rect.x();
    opt.rect.setX(0);
    if (opt.state.testFlag(QStyle::State_Selected)) {
        painter->fillRect(opt.rect, m_styled_button->palette().highlight());
    } else if (opt.state.testFlag(QStyle::State_MouseOver)) {
        QColor color = m_styled_button->palette().highlight().color();
        color.setAlpha(127);
        painter->fillRect(opt.rect, color);
    }
    //sideBarView->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, sideBarView);

    opt.rect.setX(x + 7);

    auto iconRect = QRect(opt.rect.topLeft() - QPoint(opt.rect.height(), 0), QSize(opt.rect.height(), opt.rect.height()));
    iconRect.adjust(8, 8, -8, -8);
    iconRect.moveTo(iconRect.topLeft() + QPoint(6, 0));
    if (sideBarView->model()->hasChildren(index)) {
        if (sideBarView->isExpanded(index)) {
            auto icon = QIcon::fromTheme("pan-down-symbolic", QIcon::fromTheme("go-down"));
            icon.paint(painter, iconRect, Qt::AlignCenter);
        } else {
            auto icon = QIcon::fromTheme("pan-end-symbolic",QIcon::fromTheme("go-next"));
            icon.paint(painter, iconRect);
        }
    }

    sideBarView->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, sideBarView);
}

QSize SideBarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //separator
    SideBar *sideBar = qobject_cast<SideBar*>(this->parent());
    SideBarProxyFilterSortModel *model = qobject_cast<SideBarProxyFilterSortModel*>(sideBar->model());
    auto item = model->itemFromIndex(index);
    if (item->type() == SideBarAbstractItem::SeparatorItem) {
        auto separatorItem = static_cast<SideBarSeparatorItem*>(item);
        int height = 0;
        auto size = QStyledItemDelegate::sizeHint(option, index);
        switch (separatorItem->separatorType()) {
        case SideBarSeparatorItem::Large: {
            height = 15;
            break;
        }
        case SideBarSeparatorItem::Small: {
            height = 12;
            break;
        }
        case SideBarSeparatorItem::EmptyFile: {
            height = 28;
            break;
        }
        }
        size.setHeight(height);
        return size;
    }

    if (index.column() != 0)
        return QStyledItemDelegate::sizeHint(option, index);

    auto size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(28);
    size.setWidth(size.width() + 10);
    return size;
}
