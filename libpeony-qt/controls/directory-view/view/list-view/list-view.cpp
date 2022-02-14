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

#include <QApplication>
#include <QStyleHints>
#include <QPainter>

#include <QToolTip>
#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

ListView::ListView(QWidget *parent) : QTreeView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyle(Peony::DirectoryView::ListViewStyle::getStyle());

    setAutoScroll(true);
    setAutoScrollMargin(100);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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

    m_renameTimer = new QTimer(this);
    m_renameTimer->setInterval(3000);
    m_editValid = false;

    setIconSize(QSize(40, 40));

    m_rubberBand = new QRubberBand(QRubberBand::Shape::Rectangle, this);
    setMouseTracking(true);
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
    if(e->key() == Qt::Key_Down||e->key() == Qt::Key_Up)
    {
        QStringList selections = getSelections();
        if(selections.size() == 1)
            this->scrollToSelection(selections.at(0));
    }

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
        m_rubberBand->hide();
        return;
    }

    m_isLeftButtonPressed = true;
    m_rubberBand->hide();
    m_lastPressedLogicPoint = e->pos() + QPoint(horizontalOffset(), verticalOffset());

    auto index = indexAt(e->pos());
    bool isIndexSelected = selectedIndexes().contains(index);

    QPoint p = e->pos();
    auto visualRect = this->visualRect(index);

    if (isEnableMultiSelect()) {
        int selectBoxColumn = getCurrentCheckboxColumn();
        int selectBoxPosion = viewport()->width() + viewport()->x() - header()->sectionViewportPosition(selectBoxColumn) - 48;
        if (index.column() == selectBoxColumn && p.x() > visualRect.x() + selectBoxPosion - 4 &&
            p.x() < visualRect.x() + selectBoxPosion + 24)
        {
            if(!isIndexSelected)
            {
                this->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select|QItemSelectionModel::Rows);
            }
            else
            {
                this->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Deselect|QItemSelectionModel::Rows);
            }
            return;
        }
    }

    m_editValid = true;
    QTreeView::mousePressEvent(e);

    auto sizeHint = itemDelegate()->sizeHint(viewOptions(), index);
    auto validRect = QRect(visualRect.topLeft(), sizeHint);
    if (!validRect.contains(e->pos())) {
        if (isIndexSelected) {
            clearSelection();
            setCurrentIndex(index);
        }

        this->setState(QAbstractItemView::DragSelectingState);
    }

    //if click left button at blank space, it should select nothing
    //qDebug() << "indexAt(e->pos()):" <<indexAt(e->pos()).column() << indexAt(e->pos()).row() <<indexAt(e->pos()).isValid();
    if(e->button() == Qt::LeftButton && (!indexAt(e->pos()).isValid()) )
    {
        this->clearSelection();
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

    if (m_isLeftButtonPressed) {
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
    m_isLeftButtonPressed = false;
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
        default:
            return;
        }
    }
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
        {
            auto uri = m_proxy_model->itemFromIndex(proxy_index)->uri();
            if(!e->mimeData()->urls().contains(uri))
                m_model->dropMimeData(e->mimeData(), action, 0, 0, index);
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

/*void ListView::updateGeometries()
{
    QTreeView::updateGeometries();
    if (!model())
        return;

    if (model()->columnCount() == 0 || model()->rowCount() == 0)
        return;

    header()->setFixedWidth(this->width());

    QStyleOptionViewItem opt = viewOptions();
    int height = itemDelegate()->sizeHint(opt, QModelIndex()).height();
    verticalScrollBar()->setMaximum(verticalScrollBar()->maximum() + 2);
    //setViewportMargins(0, header()->height(), 0, height);
}
*/
void ListView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        zoomLevelChangedRequest(e->delta() > 0);
        return;
    }
    QTreeView::wheelEvent(e);
}

void ListView::paintEvent(QPaintEvent *e)
{
    auto palette = qApp->palette();
    palette.setColor(QPalette::Active, QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Inactive, QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Disabled, QPalette::Base, Qt::transparent);
    viewport()->setPalette(palette);

    QTreeView::paintEvent(e);
}

void ListView::slotRename()
{
    //special path like trash path not allow rename
    if (getDirectoryUri().startsWith("trash://")
        || getDirectoryUri().startsWith("recent://")
        || getDirectoryUri().startsWith("search://"))
        return;

    //standardPaths not allow rename
    auto currentSelections = getSelections();
    bool hasStandardPath = FileUtils::containsStandardPath(currentSelections);
    if (hasStandardPath)
        return;

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
    int columnSize = 0;

    if (!model())
        return;

    if (model()->columnCount() == 0)
        return;

    header()->resizeSections(QHeaderView::ResizeToContents);

    int rightPartsSize = 0;
    for (int column = 1; column < model()->columnCount(); column++) {
        columnSize = header()->sectionSize(column);
        rightPartsSize += columnSize;
    }

    rightPartsSize += columnSize;

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
    header()->resizeSection(model()->columnCount() - 1, columnSize * 2);
}

void ListView::multiSelect()
{
    return;
}

void ListView::disableMultiSelect()
{
    return;
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

const int ListView::getRowcount()
{
    return model()->rowCount();
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

QRect ListView::visualRect(const QModelIndex &index) const
{
    auto rect = QTreeView::visualRect(index);
    //comment to fix rename state not show icon issue
//    if (index.column() == 0) {
//        rect.setX(0);
//    }
    return rect;
}

int ListView::getCurrentCheckboxColumn()
{
    int section =header()->sectionViewportPosition(3);
    int viewportWidth =viewport()->width()+viewport()->x();
    int selectBox = 3;

    for(int i=1;i<=model()->columnCount()-1;i++)
    {

        section =header()->sectionViewportPosition(i);
        if(section+32>=viewportWidth)
        {
            selectBox = i-1;
            break;
        }
    }
    return selectBox;
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

void ListView::invertSelections(bool isInvert)
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    const QItemSelection currentSelection = selectionModel->selection();
    this->selectAll();
    if (isInvert)
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
    setState(QTreeView::NoState);
    auto origin = FileUtils::getOriginalUri(uri);
    setIndexWidget(m_proxy_model->indexFromUri(origin), nullptr);
    QTreeView::scrollTo(m_proxy_model->indexFromUri(origin));
    edit(m_proxy_model->indexFromUri(origin));
}

void ListView::editUris(const QStringList uris)
{
    //FIXME:
    //implement batch rename.
}

bool ListView::isEnableMultiSelect()
{
    return GlobalSettings::getInstance()->getValue(MULTI_SELECT).toBool();
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
    connect(model, &FileItemModel::findChildrenFinished, this, &DirectoryViewWidget::viewDirectoryChanged);
    connect(m_model, &FileItemModel::updated, m_view, &ListView::resort);

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=]() {
        Q_EMIT viewSelectionChanged();
        if (m_view->selectionModel()->selection().isEmpty())
            Q_EMIT viewSelectionStatus(false);
        else
            Q_EMIT viewSelectionStatus(true);
    });

    connect(m_view, &ListView::activated, this, [=](const QModelIndex &index) {
        //when selections is more than 1, let mainwindow to process
        if (getSelections().count() != 1)
            return;
        auto uri = index.data(Qt::UserRole).toString();
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
        QTimer::singleShot(1, this, [=]() {
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
