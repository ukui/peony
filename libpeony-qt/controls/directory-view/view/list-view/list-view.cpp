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

#include "list-view.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"

#include "list-view-delegate.h"

#include "file-item.h"
#include "list-view-style.h"

#include "global-settings.h"

#include <QHeaderView>

#include <QVBoxLayout>
#include <QMouseEvent>

#include <QScrollBar>

#include <QWheelEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDragMoveEvent>

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

ListView::ListView(QWidget *parent) : QTreeView(parent)
{
    setStyle(Peony::DirectoryView::ListViewStyle::getStyle());

    setAlternatingRowColors(true);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    setItemDelegate(new ListViewDelegate(this));

    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setSectionsMovable(true);
    header()->setStretchLastSection(true);

    setExpandsOnDoubleClick(false);
    setSortingEnabled(true);

    setEditTriggers(QTreeView::NoEditTriggers);
    setDragEnabled(true);
    setDragDropMode(QTreeView::DragDrop);
    setSelectionMode(QTreeView::ExtendedSelection);

    //setAlternatingRowColors(true);

    //setContextMenuPolicy(Qt::CustomContextMenu);
    m_renameTimer = new QTimer(this);
    m_renameTimer->setInterval(3000);
    m_editValid = false;
}

void ListView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    // NOTE:
    // scrollTo() is confilcted with updateGeometry(), where it will
    // leave a space for view. So I override this method. However,
    // the fast keyboard locating of default tree view will be disabled
    // due to the function is overrided, too.

    //QTreeView::scrollTo(index, hint);
    //updateGeometries();
}

bool ListView::isDragging()
{
    return state() == QAbstractItemView::DraggingState;
}

void ListView::bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel)
{
    if (!sourceModel || !proxyModel)
        return;
    m_model = sourceModel;
    m_proxy_model = proxyModel;
    m_proxy_model->setSourceModel(m_model);
    setModel(proxyModel);
    //adjust columns layout.
    adjustColumnsSize();

    //fix diffcult to unselect all item issue
    connect(this->selectionModel(), &QItemSelectionModel::currentColumnChanged, [=]
            (const QModelIndex &current, const QModelIndex &previous) {
        qDebug()<<"list view currentColumnChanged changed";
        if (getSelections().count() > 1 && !m_ctrl_key_pressed)
        {
            this->clearSelection();
            if (current.isValid())
                setCurrentIndex(current);
        }
    });

    connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged, [=]
            (const QModelIndex &current, const QModelIndex &previous) {
        qDebug()<<"list view currentRowChanged changed";
        if (getSelections().count() > 1 && !m_ctrl_key_pressed)
        {
            this->clearSelection();
            if (current.isValid())
                setCurrentIndex(current);
        }
    });

    //edit trigger
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection) {
        qDebug()<<"list view selection changed";
        auto currentSelections = selection.indexes();

        for (auto index : deselection.indexes()) {
            this->setIndexWidget(index, nullptr);
        }

        //rename trigger
        if (!currentSelections.isEmpty()) {
            int first_index_row = currentSelections.first().row();
            bool all_index_in_same_row = true;
            for (auto index : currentSelections) {
                if (first_index_row != index.row()) {
                    all_index_in_same_row = false;
                    break;
                }
            }
            if (all_index_in_same_row) {
                if(m_last_index.row() != currentSelections.first().row())
                {
                    m_editValid = false;
                }
                m_last_index = currentSelections.first();
            }
        } else {
            m_last_index = QModelIndex();
            m_editValid = false;
        }
    });
}

void ListView::keyPressEvent(QKeyEvent *e)
{
    QTreeView::keyPressEvent(e);
    if (e->key() == Qt::Key_Control)
        m_ctrl_key_pressed = true;
}

void ListView::keyReleaseEvent(QKeyEvent *e)
{
    QTreeView::keyReleaseEvent(e);
    if (e->key() == Qt::Key_Control)
        m_ctrl_key_pressed = false;
}

void ListView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton) {
        if (this->state() == QTreeView::EditingState) {
            if (indexWidget(indexAt(e->pos())))
                return;
        }
        Q_EMIT customContextMenuRequested(e->pos());
        return;
    }

    m_editValid = true;
    QTreeView::mousePressEvent(e);

    //comment to fix only select when drag file name issue
//    if (indexAt(e->pos()).column() != 0) {
//        this->setState(QAbstractItemView::DragSelectingState);
//    }

    //if click left button at blank space, it should select nothing
    //qDebug() << "indexAt(e->pos()):" <<indexAt(e->pos()).column() << indexAt(e->pos()).row();
    if(e->button() == Qt::LeftButton && (!indexAt(e->pos()).isValid()) )
    {
        this->clearSelection();
        //this->clearFocus();
        return;
    }

    //m_renameTimer
    if(!m_renameTimer->isActive())
    {
        m_renameTimer->start();
        m_editValid = false;
    }
    else
    {
        //if remain time is between[0.75, 3000],then trigger rename event;
        //to make sure only click one row
        bool all_index_in_same_row = true;
        if (!this->selectedIndexes().isEmpty()) {
            int first_index_row = this->selectedIndexes().first().row();
            for (auto index : this->selectedIndexes()) {
                if (first_index_row != index.row()) {
                    all_index_in_same_row = false;
                    break;
                }
            }
        }
        //qDebug()<<m_renameTimer->remainingTime()<<m_editValid<<all_index_in_same_row;
        if(m_renameTimer->remainingTime()>=0 && m_renameTimer->remainingTime() <= 2250
                && indexAt(e->pos()) == m_last_index && m_last_index.isValid() && m_editValid == true && all_index_in_same_row)
        {
            slotRename();
        } else
        {
            m_editValid = false;
        }
    }


}

void ListView::mouseReleaseEvent(QMouseEvent *e)
{
    QTreeView::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }
}

void ListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_editValid = false;

    QTreeView::mouseDoubleClickEvent(event);
}

void ListView::dragEnterEvent(QDragEnterEvent *e)
{
    m_editValid = false;
    qDebug()<<"dragEnterEvent()";
    //QTreeView::dragEnterEvent(e);
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    qDebug()<<"dragEnterEvent()" <<action <<m_ctrl_key_pressed;
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(action);
        e->accept();
    }
}

void ListView::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    qDebug()<<"list view dragMoveEvent()" <<action <<m_ctrl_key_pressed;
    auto index = indexAt(e->pos());
    if (index.isValid() && index != m_last_index) {
        QHoverEvent he(QHoverEvent::HoverMove, e->posF(), e->posF());
        viewportEvent(&he);
    } else {
        QHoverEvent he(QHoverEvent::HoverLeave, e->posF(), e->posF());
        viewportEvent(&he);
    }
    if (this == e->source()) {
        return QTreeView::dragMoveEvent(e);
    }
    e->setDropAction(action);
    e->accept();
}

void ListView::dropEvent(QDropEvent *e)
{
//    if (e->source() == this) {
//        // only handle the drop event on item.
//        switch (dropIndicatorPosition()) {
//        case QAbstractItemView::DropIndicatorPosition::OnItem: {
//            break;
//        }
//        default:
//            return;
//        }
//    }
//    QTreeView::dropEvent(e);

    m_last_index = QModelIndex();
    //m_edit_trigger_timer.stop();
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    e->setDropAction(action);
    auto proxy_index = indexAt(e->pos());
    auto index = m_proxy_model->mapToSource(proxy_index);
    qDebug()<<"dropEvent" <<action <<m_ctrl_key_pressed <<indexAt(e->pos()).isValid();
    //move in current path, do nothing
    if (e->source() == this)
    {
        if (indexAt(e->pos()).isValid())
            m_model->dropMimeData(e->mimeData(), action, 0, 0, index);
        return;
    }

    m_model->dropMimeData(e->mimeData(), action, 0, 0, index);
}

void ListView::resizeEvent(QResizeEvent *e)
{
    QTreeView::resizeEvent(e);
    if (m_last_size != size()) {
        m_last_size = size();
        adjustColumnsSize();
    }
}

void ListView::updateGeometries()
{
    QTreeView::updateGeometries();
    if (!model())
        return;

    if (model()->columnCount() == 0 || model()->rowCount() == 0)
        return;

    QStyleOptionViewItem opt = viewOptions();
    int height = itemDelegate()->sizeHint(opt, QModelIndex()).height();
    verticalScrollBar()->setMaximum(verticalScrollBar()->maximum() + 1);
    setViewportMargins(0, header()->height(), 0, height);
}

void ListView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        zoomLevelChangedRequest(e->delta() > 0);
        return;
    }
    QTreeView::wheelEvent(e);
}

void ListView::slotRename()
{
    //delay edit action to avoid doubleClick or dragEvent
    qDebug()<<"slotRename"<<m_editValid;
    QTimer::singleShot(300, m_renameTimer, [&]() {
        qDebug()<<"singleshot"<<m_editValid;
        if(m_editValid) {
            m_renameTimer->stop();
            setIndexWidget(m_last_index, nullptr);
            edit(m_last_index);
            m_editValid = false;
        }
    });

}

void ListView::setProxy(DirectoryViewProxyIface *proxy)
{

}

void ListView::resort()
{
    m_proxy_model->sort(getSortType(), Qt::SortOrder(getSortOrder()));
}

void ListView::reportViewDirectoryChanged()
{
    Q_EMIT m_proxy->viewDirectoryChanged();
}

void ListView::adjustColumnsSize()
{
    if (!model())
        return;

    if (model()->columnCount() == 0)
        return;

    resizeColumnToContents(0);

    int rightPartsSize = 0;
    for (int column = 1; column < model()->columnCount(); column++) {
        resizeColumnToContents(column);
        int columnSize = header()->sectionSize(column);
        rightPartsSize += columnSize;
    }

    if (this->width() - rightPartsSize < BOTTOM_STATUS_MARGIN)
        return;

    //resizeColumnToContents(0);
    header()->resizeSection(0, this->viewport()->width() - rightPartsSize);
}

DirectoryViewProxyIface *ListView::getProxy()
{
    return m_proxy;
}

const QString ListView::getDirectoryUri()
{
    if (!m_model)
        return nullptr;
    return m_model->getRootUri();
}

void ListView::setDirectoryUri(const QString &uri)
{
    m_current_uri = uri;
}

const QStringList ListView::getSelections()
{
    QStringList uris;
    QModelIndexList selections = selectedIndexes();
    for (auto index : selections) {
        if (index.column() == 0)
            uris<<index.data(FileItemModel::UriRole).toString();
    }
    return uris;
}

void ListView::setSelections(const QStringList &uris)
{
    clearSelection();
    for (auto uri: uris) {
        const QModelIndex index = m_proxy_model->indexFromUri(uri);
        if (index.isValid()) {
            auto flags = QItemSelectionModel::Select|QItemSelectionModel::Rows;
            selectionModel()->select(index, flags);
        }
    }
}

const QStringList ListView::getAllFileUris()
{
    return m_proxy_model->getAllFileUris();
}

void ListView::open(const QStringList &uris, bool newWindow)
{
    return;
}

void ListView::beginLocationChange()
{
    m_editValid = false;
    //setModel(nullptr);
    m_model->setRootUri(m_current_uri);
}

void ListView::stopLocationChange()
{
    m_model->cancelFindChildren();
}

void ListView::closeView()
{
    this->deleteLater();
}

void ListView::invertSelections()
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    const QItemSelection currentSelection = selectionModel->selection();
    this->selectAll();
    selectionModel->select(currentSelection, QItemSelectionModel::Deselect);
}

void ListView::scrollToSelection(const QString &uri)
{
    auto index = m_proxy_model->indexFromUri(uri);
    QTreeView::scrollTo(index);
}

void ListView::setCutFiles(const QStringList &uris)
{
    return;
}

int ListView::getSortType()
{
    int type = m_proxy_model->sortColumn();
    return type<0? 0: type;
}

void ListView::setSortType(int sortType)
{
    m_proxy_model->sort(sortType, Qt::SortOrder(getSortOrder()));
}

int ListView::getSortOrder()
{
    return m_proxy_model->sortOrder();
}

void ListView::setSortOrder(int sortOrder)
{
    m_proxy_model->sort(getSortType(), Qt::SortOrder(sortOrder));
}

void ListView::editUri(const QString &uri)
{
    setIndexWidget(m_proxy_model->indexFromUri(uri), nullptr);
    edit(m_proxy_model->indexFromUri(uri));
}

void ListView::editUris(const QStringList uris)
{
    //FIXME:
    //implement batch rename.
}

//List View 2
ListView2::ListView2(QWidget *parent) : DirectoryViewWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    m_view = new ListView(this);

    int defaultZoomLevel = GlobalSettings::getInstance()->getValue(DEFAULT_VIEW_ZOOM_LEVEL).toInt();
    if (defaultZoomLevel >= minimumZoomLevel() && defaultZoomLevel <= maximumZoomLevel())
        m_zoom_level = defaultZoomLevel;

    connect(m_view, &ListView::zoomLevelChangedRequest, this, &ListView2::zoomRequest);
    layout->addWidget(m_view);

    setLayout(layout);
}

ListView2::~ListView2()
{
    m_model->setPositiveResponse(true);
}

void ListView2::bindModel(FileItemModel *model, FileItemProxyFilterSortModel *proxyModel)
{
    disconnect(m_model);
    disconnect(m_proxy_model);
    m_model = model;
    m_proxy_model = proxyModel;

    m_model->setPositiveResponse(false);

    m_view->bindModel(model, proxyModel);
    connect(model, &FileItemModel::findChildrenFinished, this, &DirectoryViewWidget::viewDirectoryChanged);
    connect(m_model, &FileItemModel::updated, m_view, &ListView::resort);

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DirectoryViewWidget::viewSelectionChanged);

    connect(m_view, &ListView::doubleClicked, this, [=](const QModelIndex &index) {
        qDebug()<<index.data(Qt::UserRole).toString();
        Q_EMIT this->viewDoubleClicked(index.data(Qt::UserRole).toString());
    });

    //FIXME: how about multi-selection?
    //menu
    connect(m_view, &ListView::customContextMenuRequested, this, [=](const QPoint &pos) {
        qDebug()<<"menu request";
        if (!m_view->indexAt(pos).isValid())
        {
            m_view->clearSelection();
            //m_view->clearFocus();
        }

        auto index = m_view->indexAt(pos);
        auto selectedIndexes = m_view->selectionModel()->selection().indexes();
        auto visualRect = m_view->visualRect(index);
        auto sizeHint = m_view->itemDelegate()->sizeHint(m_view->viewOptions(), index);
        auto validRect = QRect(visualRect.topLeft(), sizeHint);
        if (!selectedIndexes.contains(index)) {
            if (!validRect.contains(pos)) {
                m_view->clearSelection();
                //m_view->clearFocus();
            } else {
                auto flags = QItemSelectionModel::Select|QItemSelectionModel::Rows;
                m_view->clearSelection();
                //m_view->clearFocus();
                m_view->selectionModel()->select(m_view->indexAt(pos), flags);
            }
        }

        //NOTE: we have to ensure that we have cleared the
        //selection if menu request at blank pos.
        QTimer::singleShot(1, [=]() {
            Q_EMIT this->menuRequest(QCursor::pos());
        });
    });

    connect(m_proxy_model, &FileItemProxyFilterSortModel::layoutChanged, this, [=]() {
        Q_EMIT this->sortOrderChanged(Qt::SortOrder(getSortOrder()));
    });
    connect(m_proxy_model, &FileItemProxyFilterSortModel::layoutChanged, this, [=]() {
        Q_EMIT this->sortTypeChanged(getSortType());
    });

    connect(m_model, &FileItemModel::findChildrenFinished, this, [=]() {
        //delay a while for proxy model sorting.
        QTimer::singleShot(100, this, [=]() {
            //m_view->setModel(m_proxy_model);
            //adjust columns layout.
            m_view->adjustColumnsSize();
        });
    });
}

void ListView2::setCurrentZoomLevel(int zoomLevel)
{
    int base = 16;
    int adjusted = base + zoomLevel;
    m_view->setIconSize(QSize(adjusted, adjusted));
    m_zoom_level = zoomLevel;
}

void ListView2::clearIndexWidget()
{
    for (auto index : m_view->selectedIndexes()) {
        m_view->setIndexWidget(index, nullptr);
    }
}
