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
#include "file-utils.h"
#include "file-info.h"
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
#include <QDrag>
#include <QPainter>
#include <QWindow>

#include <QApplication>
#include <QStyleHints>

#include <QDebug>
#include <QToolTip>

#include <QStyleOptionViewItem>

#include <QStandardPaths>
#include <QMessageBox>

using namespace Peony;
using namespace Peony::DirectoryView;

ListView::ListView(QWidget *parent) : QTreeView(parent)
{
    // use scroll per pixel mode for calculate vertical scroll bar range.
    // see reUpdateScrollBar()
    setVerticalScrollMode(ScrollPerPixel);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyle(Peony::DirectoryView::ListViewStyle::getStyle());

    setAutoScroll(true);
    setAutoScrollMargin(100);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setSelectionBehavior(QTreeView::SelectRows);

    setAlternatingRowColors(true);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    setItemDelegate(new ListViewDelegate(this));

    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setSectionsMovable(true);
    //header()->setStretchLastSection(true);

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

    //use this property to fix bug 44314 and 33558
    //bug#42244 need to find and fix update fail issue
    setUniformRowHeights(true);
    setIconSize(QSize(40, 40));
    setMouseTracking(true);//追踪鼠标

    m_rubberBand = new QRubberBand(QRubberBand::Shape::Rectangle, this);

    //FIXME: do not create proxy in view itself.
    ListViewDelegate *delegate = new ListViewDelegate(this);
    setItemDelegate(delegate);
    connect(delegate, &ListViewDelegate::isEditing, this, [=](const bool &editing)
    {
        m_delegate_editing = editing;
    });

    //fix head indication sort type and order not change in preference file issue, releated to bug#92525,
    connect(header(), &QHeaderView::sortIndicatorChanged, this, [=](int logicalIndex, Qt::SortOrder order)
    {
        //qDebug() << "sortIndicatorChanged:" <<logicalIndex<<order;
        Peony::GlobalSettings::getInstance()->setValue(SORT_COLUMN, logicalIndex);
        Peony::GlobalSettings::getInstance()->setValue(SORT_ORDER, order);
    });
}

void ListView::scrollTo(const QModelIndex &index, QAbstractItemView::ScrollHint hint)
{
    // Note: due to we rewrite the calculation of view scroll area based on current process,
    // we could not use QTreeView::scrollTo in some cases because it still based on old process
    // and will conflict with new calculation.
    Q_UNUSED(index)
    Q_UNUSED(hint)
    reUpdateScrollBar();
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
//    connect(this->selectionModel(), &QItemSelectionModel::currentColumnChanged, [=]
//            (const QModelIndex &current, const QModelIndex &previous) {
//        qDebug()<<"list view currentColumnChanged changed";
//        if (getSelections().count() > 1 && !m_ctrl_key_pressed)
//        {
//            this->clearSelection();
//            if (current.isValid())
//                setCurrentIndex(current);
//        }
//    });

//    connect(this->selectionModel(), &QItemSelectionModel::currentRowChanged, [=]
//            (const QModelIndex &current, const QModelIndex &previous) {
//        qDebug()<<"list view currentRowChanged changed";
//        if (getSelections().count() > 1 && !m_ctrl_key_pressed)
//        {
//            this->clearSelection();
//            if (current.isValid())
//                setCurrentIndex(current);
//        }
//    });

    //edit trigger
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection) {
        qDebug()<<"list view selection changed"<<m_delegate_editing;
        //continue to fix bug#89540，98951
        if (m_delegate_editing)
            return;
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
    switch (e->key()) {
    case Qt::Key_Control:
        m_ctrl_key_pressed = true;
        break;
    case Qt::Key_Up: {
        if (!selectedIndexes().isEmpty()) {
            QTreeView::scrollTo(selectedIndexes().first());
        }
        break;
    }
    case Qt::Key_Down: {
        if (!selectedIndexes().isEmpty()) {
            auto index = selectedIndexes().first();
            if (index.row() + 1 == model()->rowCount()) {
                verticalScrollBar()->setValue(qMin(verticalScrollBar()->value() + iconSize().height(), verticalScrollBar()->maximum()));
            } else {
                QTreeView::scrollTo(selectedIndexes().first());
            }
        }
        break;
    }
    default:
        break;
    }
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
        m_rubberBand->hide();
        return;
    }

    m_isLeftButtonPressed = true;
    m_rubberBand->hide();
    m_lastPressedLogicPoint = e->pos() + QPoint(horizontalOffset(), verticalOffset());

    auto index = indexAt(e->pos());
    bool isIndexSelected = selectedIndexes().contains(index);

    m_editValid = true;
    QTreeView::mousePressEvent(e);

    auto visualRect = this->visualRect(index);
    auto sizeHint = itemDelegate()->sizeHint(viewOptions(), index);
    auto validRect = QRect(visualRect.topLeft(), sizeHint);
    if (!validRect.contains(e->pos())) {
        if (isIndexSelected) {
            clearSelection();
            setCurrentIndex(index);
        }
        this->setState(QAbstractItemView::DragSelectingState);
    }
    //comment to fix can not enter rename issue
//    else if (isIndexSelected) {
//        return;
//    }

    //if click left button at blank space, it should select nothing
    //qDebug() << "indexAt(e->pos()):" <<indexAt(e->pos()).column() << indexAt(e->pos()).row() <<indexAt(e->pos()).isValid();
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
        //qDebug()<<m_renameTimer->remainingTime()<<m_editValid<<all_index_in_same_row<<qApp->styleHints()->mouseDoubleClickInterval();
        if(m_renameTimer->remainingTime()>=0 && m_renameTimer->remainingTime() <= 3000 - qApp->styleHints()->mouseDoubleClickInterval()
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
    m_rubberBand->hide();
    m_isLeftButtonPressed = false;
}

void ListView::mouseMoveEvent(QMouseEvent *e)
{
    QModelIndex itemIndex = indexAt(e->pos());
    if (!itemIndex.isValid()) {
        if (QToolTip::isVisible()) {
             QToolTip::hideText();
         }
    } else {
        if (0 != itemIndex.column() && QToolTip::isVisible()) {
            QToolTip::hideText();
        }
    }

    QTreeView::mouseMoveEvent(e);

    if (e->buttons() & Qt::LeftButton) {
        auto pos = e->pos();
        auto offset = QPoint(horizontalOffset(), verticalOffset());
        auto logicPos = pos + offset;
        QRect logicRect = QRect(logicPos, m_lastPressedLogicPoint);
        m_logicRect = logicRect.normalized();

        int dx = -horizontalOffset();
        int dy = -verticalOffset() + this->header()->height();
        auto realRect = m_logicRect.adjusted(dx, dy, dx ,dy);

        if (!m_rubberBand->isVisible())
            m_rubberBand->show();
        m_rubberBand->setGeometry(realRect);
    } else {
        m_rubberBand->hide();
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
    if (e->keyboardModifiers() & Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    qDebug()<<"dragEnterEvent()" <<action <<m_ctrl_key_pressed;
    if (e->mimeData()->hasUrls()) {
        if (FileUtils::containsStandardPath(e->mimeData()->urls())) {
            e->ignore();
            if (this == e->source()) {
                clearSelection();
            }
            return;
        }
        e->setDropAction(action);
        e->accept();
    }
}

void ListView::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->keyboardModifiers() & Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    //qDebug()<<"list view dragMoveEvent()" <<action <<m_ctrl_key_pressed;
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
    // do not comment this code.
    if (e->source() == this) {
        // only handle the drop event on item.
        switch (dropIndicatorPosition()) {
        case QAbstractItemView::DropIndicatorPosition::OnItem: {
            break;
        }
        case QAbstractItemView::DropIndicatorPosition::OnViewport: {
            if (e->keyboardModifiers() & Qt::ControlModifier) {
                break;
            } else {
                return;
            }
        }
        default:
            return;
        }
    }
//    QTreeView::dropEvent(e);

    m_last_index = QModelIndex();
    //m_edit_trigger_timer.stop();
    if (e->keyboardModifiers() & Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    e->setDropAction(action);
    if (e->keyboardModifiers() & Qt::ShiftModifier) {
        action = Qt::TargetMoveAction;
    }

    //Do not allow dragging files to file manager when searching
    //related to bug#107063,118004
    if (m_current_uri.startsWith("search://") || m_current_uri.startsWith("favorite://")) {
        QMessageBox::warning(this, tr("warn"), tr("This operation is not supported."));
        return;
    }

    auto proxy_index = indexAt(e->pos());
    auto index = m_proxy_model->mapToSource(proxy_index);
    qDebug()<<"dropEvent" <<action <<m_ctrl_key_pressed <<indexAt(e->pos()).isValid();

    QString username = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).split("/").last();
    QString boxpath = "file://"+QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.box";
    QString oldboxpath = "file://box/"+username;

    if(m_current_uri == boxpath || m_current_uri == oldboxpath || m_current_uri == "filesafe:///"){
        return;
    }

    //move in current path, do nothing
    if (e->source() == this)
    {
        if (indexAt(e->pos()).isValid())
        {
            auto uri = m_proxy_model->itemFromIndex(proxy_index)->uri();
            if(!e->mimeData()->urls().contains(uri))
                m_model->dropMimeData(e->mimeData(), action, 0, 0, index);
        } else {
            if (m_ctrl_key_pressed) {
                m_model->dropMimeData(e->mimeData(), Qt::CopyAction, 0, 0, QModelIndex());
            }
        }
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

/*!
 * \brief ListView::reUpdateScrollBar
 * \details
 * tree view use QTreeViewPrivate::updateScrollBars() for reset scrollbar range.
 * there are 3 parts for that method called.
 *
 * 1. QTreeView::scrollTo()
 * 2. QTreeView::dataChanged()
 * 3. QTreeView::updateGeometries()
 *
 * we have to override all of them to make sure that our custom scrollbar range
 * set correctly.
 */
void ListView::reUpdateScrollBar()
{
    if (!model())
        return;

    if (model()->rowCount() == 0) {
        return;
    }

    int totalHeight = 0;
    int rowCount = model()->rowCount();
    for (int row = 0; row < rowCount; row++) {
        auto index = model()->index(row, 0);
        totalHeight += sizeHintForIndex(index).height();
    }

    verticalScrollBar()->setSingleStep(iconSize().height());
    verticalScrollBar()->setPageStep(viewport()->height() - header()->height());
    verticalScrollBar()->setRange(0, totalHeight + header()->height() + 100 - viewport()->height());
}

void ListView::updateGeometries()
{
    QTreeView::updateGeometries();
    reUpdateScrollBar();
}

void ListView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        zoomLevelChangedRequest(e->delta() > 0);
        return;
    }
    QTreeView::wheelEvent(e);
}

void ListView::focusInEvent(QFocusEvent *e)
{
    QTreeView::focusInEvent(e);
    if (e->reason() == Qt::TabFocus) {
        if (selectedIndexes().isEmpty()) {
            selectionModel()->select(model()->index(0, 0), QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
        } else {
            QTreeView::scrollTo(selectedIndexes().first(), QTreeView::EnsureVisible);
            reUpdateScrollBar();
            auto selections = getSelections();
            clearSelection();
            //use uri rather than index, to fix crash bug#68788, 96145
            QTimer::singleShot(100, this, [=](){
                setSelections(selections);
            });
        }
    }
}

void ListView::startDrag(Qt::DropActions flags)
{
    auto indexes = selectedIndexes();
    if (indexes.count() > 0) {
        auto pos = mapFromGlobal(QCursor::pos());
        qreal scale = 1.0;
        QWidget *window = this->window();
        if (window) {
            auto windowHandle = window->windowHandle();
            if (windowHandle) {
                scale = windowHandle->devicePixelRatio();
            }
        }

        auto drag = new QDrag(this);
        drag->setMimeData(model()->mimeData(indexes));

        QRegion rect;
        QHash<QModelIndex, QRect> indexRectHash;
        for (auto index : indexes) {
            rect += (visualRect(index));
            indexRectHash.insert(index, visualRect(index));
        }

        QRect realRect = rect.boundingRect();
        QPixmap pixmap(realRect.size() * scale);
        pixmap.fill(Qt::transparent);
        pixmap.setDevicePixelRatio(scale);
        QPainter painter(&pixmap);
        for (auto index : indexes) {
            painter.save();
            painter.translate(indexRectHash.value(index).topLeft() - rect.boundingRect().topLeft());
            //painter.translate(-rect.boundingRect().topLeft());
            QStyleOptionViewItem opt = viewOptions();
            auto viewItemDelegate = static_cast<ListViewDelegate *>(itemDelegate());
            viewItemDelegate->initIndexOption(&opt, index);
            opt.displayAlignment = Qt::Alignment(Qt::AlignLeft|Qt::AlignVCenter);
            opt.rect.setSize(indexRectHash.value(index).size());
            opt.rect.moveTo(0, 0);
            opt.state |= QStyle::State_Selected;
            painter.setOpacity(0.8);
            QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, &painter);
            painter.restore();
        }

        drag->setPixmap(pixmap);
        drag->setHotSpot(pos - rect.boundingRect().topLeft() - QPoint(0, header()->height()));
        drag->setDragCursor(QPixmap(), m_ctrl_key_pressed? Qt::CopyAction: Qt::MoveAction);
        drag->exec(m_ctrl_key_pressed? Qt::CopyAction: Qt::MoveAction);
    }
}

void ListView::slotRename()
{
    //special path like trash path not allow rename
    if (getDirectoryUri().startsWith("trash://")
        || getDirectoryUri().startsWith("recent://")
        || getDirectoryUri().startsWith("favorite://")
        || getDirectoryUri().startsWith("search://"))
        return;

    //standardPaths not allow rename
    auto currentSelections = getSelections();
    bool hasStandardPath = FileUtils::containsStandardPath(currentSelections);
    if (hasStandardPath)
        return;

    //delay edit action to avoid doubleClick or dragEvent
    qDebug()<<"slotRename"<<m_editValid;
    QTimer::singleShot(300, this, [&]() {
        qDebug()<<"singleshot"<<m_editValid;
        //fix bug#98951, click edit box boarder will reenter edit issue
        if(m_editValid &&  ! m_delegate_editing) {
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

    header()->resizeSections(QHeaderView::ResizeToContents);

    int rightPartsSize = 0;
    for (int column = 1; column < model()->columnCount(); column++) {
        int columnSize = header()->sectionSize(column);
        rightPartsSize += columnSize;
    }

    //set column 0 minimum width, fix header icon overlap with name issue
    if(columnWidth(0) < columnWidth(1))
        setColumnWidth(0, columnWidth(1));

    if (this->width() - rightPartsSize < BOTTOM_STATUS_MARGIN) {
        int size = width() - BOTTOM_STATUS_MARGIN;
        size /= header()->count() - 1;
        setColumnWidth(0, BOTTOM_STATUS_MARGIN);
        for (int column = 1; column < model()->columnCount(); column++) {
            setColumnWidth(column, size);
        }
        return;
    }

    header()->resizeSection(0, this->viewport()->width() - rightPartsSize);
}

void ListView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QTreeView::dataChanged(topLeft, bottomRight, roles);
    reUpdateScrollBar();
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
    QString uri;
    QModelIndexList selections = selectedIndexes();
    for (auto index : selections) {
        if (index.column() == 0)
            uris<<index.data(FileItemModel::UriRole).toString();
    }
    uris.removeDuplicates();
    return uris;
}

void ListView::setSelections(const QStringList &uris)
{
    clearSelection();
    QItemSelection selection;
    for (auto uri: uris) {
        const QModelIndex index = m_proxy_model->indexFromUri(uri);
        if (index.isValid()) {
            QItemSelection selectionToBeMerged(index, index);
            selection.merge(selectionToBeMerged, QItemSelectionModel::Select);
        }
    }
    auto flags = QItemSelectionModel::Select|QItemSelectionModel::Rows;
    selectionModel()->select(selection, flags);
}

const QStringList ListView::getAllFileUris()
{
    return m_proxy_model->getAllFileUris();
}

QRect ListView::visualRect(const QModelIndex &index) const
{
    auto rect = QTreeView::visualRect(index);
    //comment to fix rename state not show icon issue
//    if (index.column() == 0) {
//        rect.setX(0);
//    }

    return rect;
}

void ListView::open(const QStringList &uris, bool newWindow)
{
    return;
}

void ListView::beginLocationChange()
{
    m_editValid = false;
    m_last_index = QModelIndex();
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
    reUpdateScrollBar();
}

void ListView::setCutFiles(const QStringList &uris)
{
    return;
}

bool ListView::getDelegateEditFlag()
{
    return m_delegate_editing;
}

int ListView::getSortType()
{
    int type = m_proxy_model->sortColumn();
    return type<0? 0: type;
}

void ListView::setSortType(int sortType)
{
    //fix indicator not agree with actual sort order issue, link to bug#71475
    header()->setSortIndicator(sortType, Qt::SortOrder(getSortOrder()));
    m_proxy_model->sort(sortType, Qt::SortOrder(getSortOrder()));
}

int ListView::getSortOrder()
{
    return m_proxy_model->sortOrder();
}

void ListView::setSortOrder(int sortOrder)
{
    //fix indicator not agree with actual sort order issue, link to bug#71475
    header()->setSortIndicator(getSortType(), Qt::SortOrder(sortOrder));
    m_proxy_model->sort(getSortType(), Qt::SortOrder(sortOrder));
}

void ListView::editUri(const QString &uri)
{
    setState(QTreeView::NoState);
    auto origin = FileUtils::getOriginalUri(uri);
    if(uri.startsWith("mtp://"))/* Fixbug#82649:在手机内部存储里新建文件/文件夹时，名称不是可编辑状态,都是默认文件名/文件夹名 */
        origin = uri;
    QModelIndex index =m_proxy_model->indexFromUri(origin);
    setIndexWidget(index, nullptr);
    //注释该行以修复bug:#60474
//    QTreeView::scrollTo(m_proxy_model->indexFromUri(origin));
    edit(index);
    //fix bug#70769, edit box overlapped with status bar issue
    //qDebug() <<"editUri row"<<m_proxy_model->rowCount()<<index.row();
    if(index.row() >= m_proxy_model->rowCount()-1)
       QTreeView::scrollToBottom();
}

void ListView::editUris(const QStringList uris)
{
    //FIXME:
    //implement batch rename.
}

void ListView::keyboardSearch(const QString &key)
{
    // ensure current index is index in display name column
    if (currentIndex().column() != 0) {
        selectionModel()->setCurrentIndex(m_model->index(currentIndex().row(), 0, currentIndex().parent()), QItemSelectionModel::SelectCurrent);
    }

    // note: checking qtreeview.cpp we can find that the keyboard search only select rows
    // while selection mode is single selection. so we have a trick here for trigger that
    // action.
    setSelectionMode(QTreeView::SingleSelection);
    QAbstractItemView::keyboardSearch(key);
    setSelectionMode(QTreeView::ExtendedSelection);

    auto indexes = selectedIndexes();
    if (!indexes.isEmpty()) {
        QTreeView::scrollTo(indexes.first(), QTreeView::PositionAtCenter);
        reUpdateScrollBar();
        if (verticalScrollBar()->value() < viewport()->height()) {
            return;
        }
        verticalScrollBar()->setValue(qMin(verticalScrollBar()->value() + iconSize().height(), verticalScrollBar()->maximum()));
    }
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

    //m_model->setPositiveResponse(false);

    m_view->bindModel(model, proxyModel);
    connect(m_model, &FileItemModel::selectRequest, this, &DirectoryViewWidget::updateWindowSelectionRequest);
    connect(m_model,&FileItemModel::signal_itemAdded, this, [=](const QString& uri){
        Q_EMIT this->signal_itemAdded(uri);
    });
    connect(model, &FileItemModel::findChildrenFinished, this, &DirectoryViewWidget::viewDirectoryChanged);
    //connect(m_model, &FileItemModel::updated, m_view, &ListView::resort);
    connect(m_model, &FileItemModel::updated, m_view->viewport(), QOverload<>::of(&QWidget::update));

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DirectoryViewWidget::viewSelectionChanged);

    connect(m_view, &ListView::activated, this, [=](const QModelIndex &index) {
        //when selections is more than 1, let mainwindow to process
        if (getSelections().count() != 1)
            return;
        auto uri = getSelections().first();
        Q_EMIT this->viewDoubleClicked(uri);
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
        if (m_need_resize_header) {
            m_view->adjustColumnsSize();
        }
        m_need_resize_header = false;
    });
}

void ListView2::repaintView()
{
    m_view->update();
    m_view->viewport()->update();
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
    for (auto index : m_proxy_model->getAllFileIndexes()) {
        m_view->setIndexWidget(index, nullptr);
        m_view->closePersistentEditor(index);
    }
}
