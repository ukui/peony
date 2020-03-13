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

#include <QHeaderView>

#include <QVBoxLayout>
#include <QMouseEvent>

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

ListView::ListView(QWidget *parent) : QTreeView(parent)
{
    setItemDelegate(new ListViewDelegate(this));

    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setSectionsMovable(true);
    header()->setDefaultSectionSize(200);
    //header()->setStretchLastSection(false);

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

void ListView::bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel)
{
    if (!sourceModel || !proxyModel)
        return;
    m_model = sourceModel;
    m_proxy_model = proxyModel;
    m_proxy_model->setSourceModel(m_model);
    setModel(proxyModel);

    //edit trigger
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection){
        qDebug()<<"selection changed";
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

    if (indexAt(e->pos()).column() != 0) {
        this->setState(QAbstractItemView::DragSelectingState);
    }

    //if click left button at blank space, it should select nothing
    if(e->button() == Qt::LeftButton && (!indexAt(e->pos()).isValid()) )
    {
        this->clearSelection();
        this->clearFocus();
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
        if(m_renameTimer->remainingTime()>=750 && m_renameTimer->remainingTime() <= 3000
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
    QTreeView::dragEnterEvent(e);
}

void ListView::slotRename()
{
    //delay edit action to avoid doubleClick or dragEvent
    qDebug()<<"slotRename"<<m_editValid;
    QTimer::singleShot(300, [&](){
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
    scrollTo(index);
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
    layout->addWidget(m_view);

    setLayout(layout);
}

ListView2::~ListView2()
{

}

void ListView2::bindModel(FileItemModel *model, FileItemProxyFilterSortModel *proxyModel)
{
    disconnect(m_model);
    disconnect(m_proxy_model);
    m_model = model;
    m_proxy_model = proxyModel;

    m_view->bindModel(model, proxyModel);
    connect(model, &FileItemModel::findChildrenFinished, this, &DirectoryViewWidget::viewDirectoryChanged);
    connect(m_model, &FileItemModel::updated, m_view, &ListView::resort);

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DirectoryViewWidget::viewSelectionChanged);

    connect(m_view, &ListView::doubleClicked, this, [=](const QModelIndex &index){
        qDebug()<<index.data(Qt::UserRole).toString();
        Q_EMIT this->viewDoubleClicked(index.data(Qt::UserRole).toString());
    });

    //FIXME: how about multi-selection?
    //menu
    connect(m_view, &ListView::customContextMenuRequested, this, [=](const QPoint &pos){
        qDebug()<<"menu request";
        if (!m_view->indexAt(pos).isValid())
        {
            m_view->clearSelection();
            m_view->clearFocus();
        }

        auto index = m_view->indexAt(pos);
        auto selectedIndexes = m_view->selectionModel()->selection().indexes();
        auto visualRect = m_view->visualRect(index);
        auto sizeHint = m_view->itemDelegate()->sizeHint(m_view->viewOptions(), index);
        auto validRect = QRect(visualRect.topLeft(), sizeHint);
        if (!selectedIndexes.contains(index)) {
            if (!validRect.contains(pos)) {
                m_view->clearSelection();
                m_view->clearFocus();
            } else {
                auto flags = QItemSelectionModel::Select|QItemSelectionModel::Rows;
                m_view->clearSelection();
                m_view->clearFocus();
                m_view->selectionModel()->select(m_view->indexAt(pos), flags);
            }
        }

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
