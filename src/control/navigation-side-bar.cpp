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
#include "bookmark-manager.h"
#include "file-info.h"
#include "file-info-job.h"

#include "global-settings.h"

#include <QHeaderView>
#include <QPushButton>

#include <QVBoxLayout>

#include <QEvent>

#include <QPainter>

#include <QScrollBar>

#include <QKeyEvent>
#include <QLabel>

#include <QUrl>
#include <QDropEvent>
#include <QMimeData>

#include <QTimer>

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

    // add by wwn
    this->setIndentation(0);
    // this->setRootIsDecorated(false);
    // this->setLayoutDirection(Qt::RightToLeft);
    // this->resetIndentation();

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
            if (item->isMounted() || item->isEjectable()) {
                auto leftIndex = m_proxy_model->index(index.row(), 0, index.parent());
                this->collapse(leftIndex);
                item->ejectOrUnmount();
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

    // del by wwn
    // expandAll();
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

void NavigationSideBar::dropEvent(QDropEvent *e)
{
    if (dropIndicatorPosition() == QAbstractItemView::AboveItem || dropIndicatorPosition() == QAbstractItemView::BelowItem) {
        // add to bookmark
        e->setAccepted(true);

        auto data = e->mimeData();
        auto bookmark = Peony::BookMarkManager::getInstance();
        if (bookmark->isLoaded()) {
            for (auto url : data->urls()) {
                auto info = Peony::FileInfo::fromUri(url.toDisplayString(), false);
                if (info->displayName().isNull()) {
                    Peony::FileInfoJob j(info);
                    j.querySync();
                }
                if (info->isDir()) {
                    bookmark->addBookMark(url.url());
                }
            }
        }
    }
    QTreeView::dropEvent(e);
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


// add by wwn
void NavigationSideBar::mousePressEvent(QMouseEvent *event)
{
    // x坐标:190-225
    uint a = event->x();
    // 如果不在展开按钮的范围内就调用以前的
    if (event->x() > 250 || event->x() < 195)
        QTreeView::mousePressEvent(event);
    else {
        // 展开children
        QPoint point(event->x(), event->y());
        if (!isExpanded(indexAt(point)))
            expand(indexAt(point));
        else
            collapse(indexAt(QPoint(event->x(), event->y())));
    }
}

NavigationSideBarItemDelegate::NavigationSideBarItemDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QSize NavigationSideBarItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(36);
    return size;
}

// add by wwn
void NavigationSideBarItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    if (!index.isValid() || option.state == QStyle::State_None)
            return;
    painter->setRenderHint(QPainter::Antialiasing, true);

    // for test
    // inter->setPen(QColor(0, 160, 230));
    // painter->drawRect(opt.rect);
    // painter->drawText(rect, QString::number(count++));

    QStyledItemDelegate::paint(painter, option, index);

    NavigationSideBar* view = qobject_cast<NavigationSideBar*>(this->parent());

    if (view == nullptr)
        return;

    if (!index.model()->hasChildren(index))
            return;

    if (view->isExpanded(index)) {
        QRect rect = option.rect;
        rect.setTop(rect.top() + 7);
        rect.setX(rect.x() + 220);
        rect.setSize(QSize(15, 15));
        painter->drawPixmap(rect, QPixmap(":/img/branches2"));
        rect.setX(option.rect.x());
    }
    else {
        QRect rect = option.rect;
        rect.setTop(rect.top() + 7);
        rect.setX(rect.x() + 220);
        rect.setSize(QSize(15, 15));
        painter->drawPixmap(rect, QPixmap(":/img/branches1"));
        rect.setX(option.rect.x());
    }
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

//    QWidget *w = new QWidget(this);
//    QVBoxLayout *l = new QVBoxLayout;
//    l->setContentsMargins(4, 4, 2, 4);

//    m_label_button = new QPushButton(QIcon::fromTheme("emblem-important-symbolic"), tr("All tags..."), this);
//    m_label_button->setProperty("useIconHighlightEffect", true);
//    m_label_button->setProperty("iconHighlightEffectMode", 1);
//    m_label_button->setProperty("fillIconSymbolicColor", true);
//    m_label_button->setCheckable(true);

//    l->addWidget(m_label_button);

//    connect(m_label_button, &QPushButton::clicked, m_sidebar, &NavigationSideBar::labelButtonClicked);

//    w->setLayout(l);

//    m_layout->addWidget(w);

    setLayout(m_layout);
}

QSize NavigationSideBarContainer::sizeHint() const
{
    auto size = QWidget::sizeHint();
    auto width = Peony::GlobalSettings::getInstance()->getValue(DEFAULT_SIDEBAR_WIDTH).toInt();
    size.setWidth(width);
    return size;
}

TitleLabel::TitleLabel(QWidget *parent):QWidget(parent)
{
    this->setFixedHeight(50);
    m_pix_label = new QLabel(this);
    m_pix_label->setPixmap(QIcon::fromTheme("folder").pixmap(32,32));
    m_text_label = new QLabel(tr("Files"),this);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(9);
    l->addWidget(m_pix_label);
    l->addSpacing(8);
    l->addWidget(m_text_label);
    l->addStretch();
}
