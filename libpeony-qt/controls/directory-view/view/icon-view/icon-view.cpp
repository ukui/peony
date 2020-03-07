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

#include "icon-view.h"
#include "standard-view-proxy.h"
#include "file-item.h"

#include "icon-view-delegate.h"
#include "icon-view-style.h"

#include "directory-view-menu.h"
#include "file-info.h"

#include <QMouseEvent>

#include <QDragEnterEvent>
#include <QMimeData>
#include <QDragMoveEvent>
#include <QDropEvent>

#include <QPainter>
#include <QPaintEvent>

#include <QApplication>

#include <QVBoxLayout>

#include <QHoverEvent>

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconView::IconView(QWidget *parent) : QListView(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);

    setStyle(IconViewStyle::getStyle());
    //FIXME: do not create proxy in view itself.
    IconViewDelegate *delegate = new IconViewDelegate(this);
    setItemDelegate(delegate);

    setSelectionMode(QListView::ExtendedSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Snap);
    //setWordWrap(true);

    setContextMenuPolicy(Qt::CustomContextMenu);

    setGridSize(QSize(115, 135));
    setIconSize(QSize(64, 64));
}

IconView::~IconView()
{

}

DirectoryViewProxyIface *IconView::getProxy()
{
    return m_proxy;
}

//selection
//FIXME: implement the selection functions.
void IconView::setSelections(const QStringList &uris)
{
    clearSelection();
    for (auto uri: uris) {
        const QModelIndex index = m_sort_filter_proxy_model->indexFromUri(uri);
        if (index.isValid()) {
            selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

const QStringList IconView::getSelections()
{
    QStringList uris;
    QModelIndexList selections = selectedIndexes();
    for (auto index : selections) {
        auto item = m_sort_filter_proxy_model->itemFromIndex(index);
        uris<<item->uri();
    }
    return uris;
}

void IconView::invertSelections()
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    const QItemSelection currentSelection = selectionModel->selection();
    this->selectAll();
    selectionModel->select(currentSelection, QItemSelectionModel::Deselect);
}

void IconView::scrollToSelection(const QString &uri)
{
    auto index = m_sort_filter_proxy_model->indexFromUri(uri);
    scrollTo(index);
}

//clipboard
void IconView::setCutFiles(const QStringList &uris)
{
    //let delegate and model know how to deal with cut files.
}

//location
//FIXME: implement location functions.
void IconView::setDirectoryUri(const QString &uri)
{
    m_current_uri = uri;
}

const QString IconView::getDirectoryUri()
{
    return m_model->getRootUri();
}

void IconView::beginLocationChange()
{
    m_model->setRootUri(m_current_uri);
}

void IconView::stopLocationChange()
{
    m_model->cancelFindChildren();
}

//other
void IconView::open(const QStringList &uris, bool newWindow)
{

}

void IconView::closeView()
{
    this->deleteLater();
}

void IconView::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
}

void IconView::dragMoveEvent(QDragMoveEvent *e)
{
    auto index = indexAt(e->pos());
    if (index.isValid() && index != m_last_index) {
        QHoverEvent he(QHoverEvent::HoverMove, e->posF(), e->posF());
        viewportEvent(&he);
    } else {
        QHoverEvent he(QHoverEvent::HoverLeave, e->posF(), e->posF());
        viewportEvent(&he);
    }
    if (this == e->source()) {
        return QListView::dragMoveEvent(e);
    }
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void IconView::dropEvent(QDropEvent *e)
{
    m_last_index = QModelIndex();
    m_edit_trigger_timer.stop();
    e->setDropAction(Qt::MoveAction);
    auto proxy_index = indexAt(e->pos());
    auto index = m_sort_filter_proxy_model->mapToSource(proxy_index);
    qDebug()<<"dropEvent";
    if (e->source() == this) {
        if (indexAt(e->pos()).isValid()) {
            m_model->dropMimeData(e->mimeData(), Qt::MoveAction, 0, 0, index);
            return;
        }
        else {
            return;
        }
    }
    m_model->dropMimeData(e->mimeData(), Qt::MoveAction, 0, 0, index);
}

void IconView::mousePressEvent(QMouseEvent *e)
{
    QListView::mousePressEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    qDebug()<<m_edit_trigger_timer.isActive()<<m_edit_trigger_timer.interval();
    if (indexAt(e->pos()) == m_last_index && m_last_index.isValid()) {
        if (m_edit_trigger_timer.isActive()) {
            setIndexWidget(m_last_index, nullptr);
            //edit(m_last_index);
        }
    }
}

void IconView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    if (!m_edit_trigger_timer.isActive() && indexAt(e->pos()).isValid() && this->selectedIndexes().count() == 1) {
        resetEditTriggerTimer();
    }
}

void IconView::resetEditTriggerTimer()
{
    m_edit_trigger_timer.disconnect();
    m_edit_trigger_timer.stop();
    QTimer::singleShot(750, [&](){
        qDebug()<<"start";
        m_edit_trigger_timer.setSingleShot(true);
        m_edit_trigger_timer.start(1000);
    });
}

void IconView::paintEvent(QPaintEvent *e)
{
    QPainter p(this->viewport());
    p.setRenderHint(QPainter::Antialiasing);
    //p.fillRect(this->geometry(), this->palette().base());
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(this->rect().adjusted(0, 0, -1, -1), 6, 6);
    path.addRect(0, 0, this->width(), 6);
    path.addRect(0, 0, 6, this->height());
    p.fillPath(path, this->palette().base());
    if (m_repaint_timer.isActive()) {
        m_repaint_timer.stop();
        QTimer::singleShot(100, [this](){
            this->repaint();
        });
    }
    QListView::paintEvent(e);
}

void IconView::resizeEvent(QResizeEvent *e)
{
    //FIXME: first resize is disfluency.
    //but I have to reset the index widget in view's resize.
    QListView::resizeEvent(e);
    setIndexWidget(m_last_index, nullptr);
}

void IconView::wheelEvent(QWheelEvent *e)
{
    QListView::wheelEvent(e);
    if (e->buttons() == Qt::LeftButton)
        this->viewport()->update();
}

void IconView::bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel)
{
    m_model = sourceModel;
    m_sort_filter_proxy_model = proxyModel;

    setModel(m_sort_filter_proxy_model);

    //edit trigger
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection){
        qDebug()<<"selection changed";
        auto currentSelections = selection.indexes();

        for (auto index : deselection.indexes()) {
            this->setIndexWidget(index, nullptr);
        }

        //Q_EMIT m_proxy->viewSelectionChanged();
        if (currentSelections.count() == 1) {
            m_last_index = currentSelections.first();
            this->resetEditTriggerTimer();
        } else {
            m_last_index = QModelIndex();
        }
    });
}

void IconView::setProxy(DirectoryViewProxyIface *proxy)
{
    if (!proxy)
        return;

    m_proxy = proxy;
    if (!m_proxy) {
        return;
    }

    //connect(m_model, &FileItemModel::dataChanged, this, &IconView::clearIndexWidget);
    connect(m_model, &FileItemModel::updated, this, &IconView::resort);

    connect(m_model, &FileItemModel::findChildrenFinished,
            this, &IconView::reportViewDirectoryChanged);

    connect(this, &IconView::doubleClicked, [=](const QModelIndex &index){
        qDebug()<<"double click"<<index.data(FileItemModel::UriRole);
        Q_EMIT m_proxy->viewDoubleClicked(index.data(FileItemModel::UriRole).toString());
    });



    //menu
//    connect(this, &IconView::customContextMenuRequested, [=](const QPoint &pos){
//        if (!indexAt(pos).isValid())
//            this->clearSelection();

//        //NOTE: we have to ensure that we have cleared the
//        //selection if menu request at blank pos.
//        QTimer::singleShot(1, [=](){
//            Q_EMIT this->getProxy()->menuRequest(QCursor::pos());
//        });
//    });
}

// NOTE: When icon view was resorted,
// index widget would deviated from its normal position by somehow.
// So, do not set any index widget when the resorting.
void IconView::resort()
{
    if (m_last_index.isValid()) {
        this->setIndexWidget(m_last_index, nullptr);
    }

    if (m_sort_filter_proxy_model)
        m_sort_filter_proxy_model->sort(getSortType(), Qt::SortOrder(getSortOrder()));
}

void IconView::reportViewDirectoryChanged()
{
    if (m_proxy)
        Q_EMIT m_proxy->viewDirectoryChanged();
}

QRect IconView::visualRect(const QModelIndex &index) const
{
    auto rect = QListView::visualRect(index);
    rect.setX(rect.x() + 10);
    rect.setY(rect.y() + 15);
    auto size = itemDelegate()->sizeHint(QStyleOptionViewItem(), index);
    rect.setSize(size);
    return rect;
}

int IconView::getSortType()
{
    int type = m_sort_filter_proxy_model->sortColumn();
    return type<0? 0: type;
}

void IconView::setSortType(int sortType)
{
    m_sort_filter_proxy_model->sort(sortType, Qt::SortOrder(getSortOrder()));
}

int IconView::getSortOrder()
{
    return m_sort_filter_proxy_model->sortOrder();
}

void IconView::setSortOrder(int sortOrder)
{
    m_sort_filter_proxy_model->sort(getSortType(), Qt::SortOrder(sortOrder));
}

const QStringList IconView::getAllFileUris()
{
    return m_sort_filter_proxy_model->getAllFileUris();
}

void IconView::editUri(const QString &uri)
{
    setIndexWidget(m_sort_filter_proxy_model->indexFromUri(uri), nullptr);
    edit(m_sort_filter_proxy_model->indexFromUri(uri));
}

void IconView::editUris(const QStringList uris)
{
    //FIXME:
    //implement batch rename.
}

void IconView::clearIndexWidget()
{
    for (int i = 0; i < m_sort_filter_proxy_model->rowCount(); i++) {
        auto index = m_sort_filter_proxy_model->index(i, 0);
        setIndexWidget(index, nullptr);
    }
}

//Icon View 2
IconView2::IconView2(QWidget *parent) : DirectoryViewWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    m_view = new IconView(this);
    layout->addWidget(m_view);

    setLayout(layout);
}

IconView2::~IconView2()
{

}

void IconView2::bindModel(FileItemModel *model, FileItemProxyFilterSortModel *proxyModel)
{
    disconnect(m_model);
    disconnect(m_proxy_model);
    m_model = model;
    m_proxy_model = proxyModel;

    m_view->bindModel(model, proxyModel);
    connect(model, &FileItemModel::findChildrenFinished, this, &DirectoryViewWidget::viewDirectoryChanged);
    //connect(m_model, &FileItemModel::dataChanged, m_view, &IconView::clearIndexWidget);
    connect(m_model, &FileItemModel::updated, m_view, &IconView::resort);

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DirectoryViewWidget::viewSelectionChanged);

    connect(m_view, &IconView::doubleClicked, this, [=](const QModelIndex &index){
        Q_EMIT this->viewDoubleClicked(index.data(Qt::UserRole).toString());
    });

    connect(m_view, &IconView::customContextMenuRequested, this, [=](const QPoint &pos){
        if (!m_view->indexAt(pos).isValid())
            m_view->clearSelection();

        //NOTE: we have to ensure that we have cleared the
        //selection if menu request at blank pos.
        QTimer::singleShot(1, [=](){
            Q_EMIT this->menuRequest(QCursor::pos());
        });
    });

    connect(m_proxy_model, &FileItemProxyFilterSortModel::layoutChanged, this, [=](){
        Q_EMIT this->sortOrderChanged(Qt::SortOrder(getSortOrder()));
    });
    connect(m_proxy_model, &FileItemProxyFilterSortModel::layoutChanged, this, [=](){
        Q_EMIT this->sortTypeChanged(getSortType());
    });
}

void IconView2::repaintView()
{
    m_view->update();
    m_view->viewport()->update();
}


