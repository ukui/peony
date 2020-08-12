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

#include "global-settings.h"

#include <QHeaderView>
#include <QPushButton>

#include <QVBoxLayout>

#include <QEvent>

#include <QPainter>

#include <QScrollBar>

#include <QKeyEvent>

#include <QDebug>

NavigationSideBar::NavigationSideBar(QWidget *parent) : QTreeView(parent)
{
    setIconSize(QSize(16, 16));

    setProperty("useIconHighlightEffect", true);
    //both default and highlight.
    setProperty("iconHighlightEffectMode", 1);

    this->verticalScrollBar()->setProperty("drawScrollBarGroove", false);

    setDragDropMode(QTreeView::DropOnly);

    setProperty("doNotBlur", true);
    viewport()->setProperty("doNotBlur", true);

    auto delegate = new NavigationSideBarItemDelegate(this);
    setItemDelegate(delegate);

    installEventFilter(this);

    setStyleSheet(".NavigationSideBar"
                  "{"
                  "border: 0px solid transparent"
                  "}");
    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);
    header()->setSectionResizeMode(QHeaderView::Custom);
    header()->hide();

    setContextMenuPolicy(Qt::CustomContextMenu);

    setExpandsOnDoubleClick(false);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_model = new Peony::SideBarModel(this);
    m_proxy_model = new Peony::SideBarProxyFilterSortModel(this);
    m_proxy_model->setSourceModel(m_model);

    this->setModel(m_proxy_model);

    connect(this, &QTreeView::expanded, [=](const QModelIndex &index) {
        auto item = m_proxy_model->itemFromIndex(index);
        qDebug()<<item->uri();
        /*!
          \bug can not expanded? enumerator can not get prepared signal, why?
          */
        item->findChildrenAsync();
    });

    connect(this, &QTreeView::collapsed, [=](const QModelIndex &index) {
        auto item = m_proxy_model->itemFromIndex(index);
        item->clearChildren();
    });

    connect(this, &QTreeView::clicked, [=](const QModelIndex &index) {
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

    connect(this, &QTreeView::customContextMenuRequested, this, [=](const QPoint &pos) {
        auto index = indexAt(pos);
        auto item = m_proxy_model->itemFromIndex(index);
        if (item) {
            if (item->type() != Peony::SideBarAbstractItem::SeparatorItem) {
                Peony::SideBarMenu menu(item, nullptr);
                menu.exec(QCursor::pos());
            }
        }
    });

    connect(m_model, &QAbstractItemModel::dataChanged, this, [=](){
        this->viewport()->update();
    });

    expandAll();
}

bool NavigationSideBar::eventFilter(QObject *obj, QEvent *e)
{
    return false;
}

void NavigationSideBar::updateGeometries()
{
    setViewportMargins(4, 0, 4, 0);
    QTreeView::updateGeometries();
}

void NavigationSideBar::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    //skip unmount indicator index
    if (index.isValid()) {
        if (index.column() == 0) {
            QTreeView::scrollTo(index, hint);
        }
    }
}

void NavigationSideBar::paintEvent(QPaintEvent *event)
{
    QTreeView::paintEvent(event);
}

void NavigationSideBar::resizeEvent(QResizeEvent *e)
{
    QTreeView::resizeEvent(e);
    if (header()->count() > 0)
        header()->resizeSection(0, this->viewport()->width() - 30);
}

QSize NavigationSideBar::sizeHint() const
{
    return QTreeView::sizeHint();
}

void NavigationSideBar::keyPressEvent(QKeyEvent *event)
{
    QTreeView::keyPressEvent(event);

    if (event->key() == Qt::Key_Return) {
        if (!selectedIndexes().isEmpty()) {
            auto index = selectedIndexes().first();
            auto uri = index.data(Qt::UserRole).toString();
            Q_EMIT this->updateWindowLocationRequest(uri, true);
        }
    }
}

NavigationSideBarItemDelegate::NavigationSideBarItemDelegate(QObject *parent)
{

}

QSize NavigationSideBarItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(36);
    return size;
}

NavigationSideBarContainer::NavigationSideBarContainer(QWidget *parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    m_layout = new QVBoxLayout;
    m_layout->setContentsMargins(0, 4, 0, 0);
    m_layout->setSpacing(0);
}

void NavigationSideBarContainer::addSideBar(NavigationSideBar *sidebar)
{
    if (m_sidebar)
        return;

    m_sidebar = sidebar;
    m_layout->addWidget(sidebar);

    QWidget *w = new QWidget(this);
    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(4, 4, 2, 4);

    m_label_button = new QPushButton(QIcon::fromTheme("emblem-important-symbolic"), tr("All tags..."), this);
    m_label_button->setProperty("useIconHighlightEffect", true);
    m_label_button->setProperty("iconHighlightEffectMode", 1);
    m_label_button->setProperty("fillIconSymbolicColor", true);
    m_label_button->setCheckable(true);

    l->addWidget(m_label_button);

    connect(m_label_button, &QPushButton::clicked, m_sidebar, &NavigationSideBar::labelButtonClicked);

    w->setLayout(l);

    m_layout->addWidget(w);

    setLayout(m_layout);
}

QSize NavigationSideBarContainer::sizeHint() const
{
    auto size = QWidget::sizeHint();
    auto width = Peony::GlobalSettings::getInstance()->getValue(DEFAULT_SIDEBAR_WIDTH).toInt();
    size.setWidth(width);
    return size;
}
