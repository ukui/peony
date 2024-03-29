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
#include "file-item.h"

#include "icon-view-delegate.h"
#include "icon-view-style.h"

#include "directory-view-menu.h"
#include "file-info.h"
#include "file-utils.h"

#include "global-settings.h"

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

#include <QScrollBar>

#include <QMouseEvent>
#include <QApplication>

#include <QStringList>
#include <QStyleHints>
#include <QPoint>

#include <QDebug>
#include <QToolTip>

#include <QStandardPaths>

using namespace Peony;
using namespace Peony::DirectoryView;

IconView::IconView(QWidget *parent) : QListView(parent)
{
//    this->verticalScrollBar()->setProperty("drawScrollBarGroove", false);

    setAttribute(Qt::WA_TranslucentBackground);
    viewport()->setAttribute(Qt::WA_TranslucentBackground);

    setAutoScroll(true);
    setAutoScrollMargin(100);

    setStyle(IconViewStyle::getStyle());
    //FIXME: do not create proxy in view itself.
    IconViewDelegate *delegate = new IconViewDelegate(this);
    setItemDelegate(delegate);
    connect(delegate, &IconViewDelegate::isEditing, this, [=](const bool &editing)
    {
        m_delegate_editing = editing;
    });

    //fix long file name not fully painted issue when drag sliderbar
    QScrollBar *verticalBar = verticalScrollBar();
    connect(verticalBar, &QScrollBar::sliderPressed, [=](){
        m_slider_bar_draging = true;
    });
    connect(verticalBar, &QScrollBar::sliderReleased, [=](){
        m_slider_bar_draging = false;
    });
    connect(verticalBar, &QScrollBar::valueChanged, [=](){
        if (m_slider_bar_draging)
            viewport()->update(viewport()->rect());
    });

    setSelectionMode(QListView::ExtendedSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Snap);
    //setWordWrap(true);

    setContextMenuPolicy(Qt::CustomContextMenu);

    setIconSize(QSize(64, 64));
    setGridSize(itemDelegate()->sizeHint(QStyleOptionViewItem(), QModelIndex()) + QSize(20, 20));

    m_renameTimer = new QTimer(this);
    m_renameTimer->setInterval(3000);
    m_editValid = false;

    setMouseTracking(true);
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
    uris.removeDuplicates();
    return uris;
}

const int IconView::getRowcount()
{
    return model()->rowCount();
}

void IconView::invertSelections(bool isInvert)
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    const QItemSelection currentSelection = selectionModel->selection();
    this->selectAll();
    if (isInvert)
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
    m_editValid = false;
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
    m_editValid = false;
    if (e->keyboardModifiers() && Qt::ControlModifier)
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

void IconView::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    //fix can not drag in the second time issue
    if (this->isDraggingState())
    {
        if (m_allow_set_index_widget) {
            m_allow_set_index_widget = false;
            for (auto index : selectedIndexes()) {
                setIndexWidget(index, nullptr);
            }
        }
    }

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    //qDebug()<<"dragMoveEvent()" <<action <<m_ctrl_key_pressed;
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
    e->setDropAction(action);
    e->accept();
}

void IconView::dropEvent(QDropEvent *e)
{
    m_last_index = QModelIndex();
    //m_edit_trigger_timer.stop();
    if (e->keyboardModifiers() && Qt::ControlModifier)
        m_ctrl_key_pressed = true;
    else
        m_ctrl_key_pressed = false;

    auto action = m_ctrl_key_pressed ? Qt::CopyAction : Qt::MoveAction;
    e->setDropAction(action);
    auto proxy_index = indexAt(e->pos());
    auto index = m_sort_filter_proxy_model->mapToSource(proxy_index);
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
            auto uri = m_sort_filter_proxy_model->itemFromIndex(proxy_index)->uri();
            if(!e->mimeData()->urls().contains(uri))
                m_model->dropMimeData(e->mimeData(), action, 0, 0, index);
        }
        return;
    }

    m_model->dropMimeData(e->mimeData(), action, 0, 0, index);
}

void IconView::mouseMoveEvent(QMouseEvent *e)
{
    QModelIndex itemIndex = indexAt(e->pos());
    if (!itemIndex.isValid()) {
        if (QToolTip::isVisible()) {
            QToolTip::hideText();
        }
    }

    if (m_ignore_mouse_move_event) {
        return;
    }
    QListView::mouseMoveEvent(e);
    if(getSelections().count()>1)
        multiSelect();
    viewport()->update(viewport()->rect());
}

void IconView::mousePressEvent(QMouseEvent *e)
{
    m_allow_set_index_widget = true;

    qDebug()<<"moursePressEvent";
    QModelIndex itemIndex = indexAt(e->pos());
    if (itemIndex.isValid() && true == m_multi_select) {
        m_mouse_release_unselect = selectedIndexes().contains(itemIndex);
    } else {
        m_mouse_release_unselect = false;
    }

    QListView::mousePressEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    if (true == m_mouse_release_unselect) {
        selectionModel()->setCurrentIndex(itemIndex, QItemSelectionModel::Select|QItemSelectionModel::Rows);
    }

    if(getSelections().count()>1)
        multiSelect();

    viewport()->update(viewport()->rect());

    if (!itemIndex.isValid()) {
        disableMultiSelect();
    }
    //FIXME: Modify the icon style, only click on the text to respond, click on the icon to not respond
    QRect rect = visualRect(indexAt(e->pos()));
    QSize iconExpectedSize = iconSize();

    QRect testRect(rect.x(), rect.y()+ iconExpectedSize.height(),rect.width(),rect.height()-iconExpectedSize.height());
    if(!testRect.contains(e->pos()))
    {
        m_editValid = false;
        m_renameTimer->start();
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
        if(m_renameTimer->remainingTime()>=0 && m_renameTimer->remainingTime() <= 3000 - qApp->styleHints()->mouseDoubleClickInterval()
                && indexAt(e->pos()) == m_last_index && m_last_index.isValid() && m_editValid == true)
        {
            slotRename();
        } else
        {
            m_editValid = false;
        }
    }
}

void IconView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    if (true == m_mouse_release_unselect) {
        QModelIndex itemIndex = indexAt(e->pos());
        if (itemIndex.isValid()) {
            selectionModel()->setCurrentIndex(itemIndex, QItemSelectionModel::Deselect|QItemSelectionModel::Rows);
        }
    }
}

void IconView::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_editValid = false;
    QListView::mouseDoubleClickEvent(event);
}

void IconView::paintEvent(QPaintEvent *e)
{
    QPainter p(this->viewport());
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::transparent);
    p.setBrush(this->palette().base());
    p.drawRect(this->rect());

    QListView::paintEvent(e);
}

void IconView::resizeEvent(QResizeEvent *e)
{
    QListView::resizeEvent(e);
    // fix 85058
    updateEditorGeometries();
}

void IconView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        if (e->delta() > 0) {
            zoomLevelChangedRequest(true);
        } else {
            zoomLevelChangedRequest(false);
        }
        return;
    }

    QListView::wheelEvent(e);
    if (e->buttons() == Qt::LeftButton)
        this->viewport()->update();
}

void IconView::updateGeometries()
{
    QListView::updateGeometries();

    if (!model())
        return;

    if (model()->columnCount() == 0 || model()->rowCount() == 0)
        return;

    verticalScrollBar()->setMaximum(verticalScrollBar()->maximum() + BOTTOM_STATUS_MARGIN);
}

void IconView::focusInEvent(QFocusEvent *e)
{
    QListView::focusInEvent(e);
    if (e->reason() == Qt::TabFocusReason) {
        if (selectedIndexes().isEmpty()) {
            selectionModel()->select(model()->index(0, 0), QItemSelectionModel::SelectCurrent|QItemSelectionModel::Rows);
        } else {
            scrollTo(selectedIndexes().first(), QListView::PositionAtCenter);
            //auto selections = selectedIndexes();
            clearSelection();
            //added for tab key to focus button issue
            //comment to fix crash bug#68788
//            QTimer::singleShot(100, this, [=](){
//                for (auto index : selections) {
//                    selectionModel()->select(index, QItemSelectionModel::Select);
//                }
//            });
        }
    }
}

void IconView::slotRename()
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

bool IconView::getIgnore_mouse_move_event() const
{
    return m_ignore_mouse_move_event;
}

void IconView::setIgnore_mouse_move_event(bool ignore_mouse_move_event)
{
    m_ignore_mouse_move_event = ignore_mouse_move_event;
}

void IconView::bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel)
{
    m_model = sourceModel;
    m_sort_filter_proxy_model = proxyModel;

    setModel(m_sort_filter_proxy_model);

    //edit trigger
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection) {
        qDebug()<<"selection changed";
        auto currentSelections = selection.indexes();

        for (auto index : deselection.indexes()) {
            this->setIndexWidget(index, nullptr);
        }

        if (state() == QListView::DragSelectingState)
            m_allow_set_index_widget = false;

        //Q_EMIT m_proxy->viewSelectionChanged();
        if (currentSelections.count() == 1) {
            qDebug()<<"m_last_index  "<<(m_last_index == currentSelections.first())<<currentSelections.first();
            if(m_last_index != currentSelections.first())
            {
                m_editValid = false;
            }
            m_last_index = currentSelections.first();
            //qDebug()<<"IconView::bindModel:"<<"selection changed: "<<"resetEditTriggerTimer";
            //this->resetEditTriggerTimer();
        } else {
            m_last_index = QModelIndex();
            m_editValid = false;
        }


        qDebug()<<"selection changed2"<<m_editValid;
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

    connect(this, &IconView::activated, [=](const QModelIndex &index) {
        qDebug()<<"double click"<<index.data(FileItemModel::UriRole);
        //when selections is more than 1, let mainwindow to process
        if (getSelections().count() != 1)
            return;
        auto uri = index.data(FileItemModel::UriRole).toString();
        //process open symbolic link
        auto info = FileInfo::fromUri(uri);
        if (info->isSymbolLink() && uri.startsWith("file://") && info->isValid())
            uri = "file://" + FileUtils::getSymbolicTarget(uri);
        if(!m_multi_select)
            Q_EMIT m_proxy->viewDoubleClicked(uri);
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
    //fix uncompress selected file will cover file before it issue
    clearIndexWidget();
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

bool IconView::getDelegateEditFlag()
{
    return m_delegate_editing;
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
    setState(QListView::NoState);
    auto origin = FileUtils::getOriginalUri(uri);
    if(uri.startsWith("mtp://"))/* Fixbug#82649:在手机内部存储里新建文件/文件夹时，名称不是可编辑状态,都是默认文件名/文件夹名 */
        origin = uri;
    QModelIndex index = m_sort_filter_proxy_model->indexFromUri(origin);
    setIndexWidget(index, nullptr);
    qDebug() <<"editUri:" <<uri <<origin;
    QListView::scrollTo(index);
    edit(index);
//    if (! m_delegate_editing)
//        edit(m_sort_filter_proxy_model->indexFromUri(origin));
}

void IconView::editUris(const QStringList uris)
{
    //FIXME:
    //implement batch rename.
}

void IconView::selectAll()
{
    // fix: #62397
//    for (int i = 0; i < model()->rowCount(); i++) {
//        selectionModel()->select(model()->index(i, 0), QItemSelectionModel::Select);
//    }
    // optimize selectAll(). do not trigger selection changed signal to many times.
    QItemSelection selection(model()->index(0, 0), model()->index(model()->rowCount() - 1, 0));
    selectionModel()->select(selection, QItemSelectionModel::Select);
}

void IconView::clearIndexWidget()
{
    for (int i = 0; i < m_sort_filter_proxy_model->rowCount(); i++) {
        auto index = m_sort_filter_proxy_model->index(i, 0);
        setIndexWidget(index, nullptr);
        closePersistentEditor(index);
    }
}

void IconView::multiSelect()
{
    if (GlobalSettings::getInstance()->getValue(MULTI_SELECT).toBool()) {
        m_multi_select = true;
    }
    setSelectionMode(MultiSelection);
}

void IconView::disableMultiSelect()
{
    m_multi_select = false;
    setSelectionMode(ExtendedSelection);
    viewport()->update(viewport()->rect());
}

//Icon View 2
IconView2::IconView2(QWidget *parent) : DirectoryViewWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    m_view = new IconView(this);

    int defaultZoomLevel = GlobalSettings::getInstance()->getValue(DEFAULT_VIEW_ZOOM_LEVEL).toInt();
    if (defaultZoomLevel >= minimumZoomLevel() && defaultZoomLevel <= maximumZoomLevel())
        m_zoom_level = defaultZoomLevel;

    connect(m_view, &IconView::zoomLevelChangedRequest, this, &IconView2::zoomRequest);

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
    connect(m_model, &FileItemModel::selectRequest, this, &DirectoryViewWidget::updateWindowSelectionRequest);
    connect(m_model,&FileItemModel::signal_itemAdded, this, [=](const QString& uri){
        Q_EMIT this->signal_itemAdded(uri);
    });
    connect(model, &FileItemModel::findChildrenFinished, this, &DirectoryViewWidget::viewDirectoryChanged);
    //connect(m_model, &FileItemModel::dataChanged, m_view, &IconView::clearIndexWidget);
    connect(m_model, &FileItemModel::updated, m_view, &IconView::resort);
    connect(m_model, &FileItemModel::updated, m_view->viewport(), QOverload<>::of(&QWidget::update));

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=]() {
        Q_EMIT viewSelectionChanged();

        if (m_view->selectionModel()->selection().isEmpty())
            Q_EMIT viewSelectionStatus(false);
        else
            Q_EMIT viewSelectionStatus(true);
    });

    connect(m_view, &IconView::activated, this, [=](const QModelIndex &index) {
        //when selections is more than 1, let mainwindow to process
        if (getSelections().count() != 1)
            return;
        auto uri = index.data(Qt::UserRole).toString();
        //process open symbolic link
        auto info = FileInfo::fromUri(uri);
        if (info->isSymbolLink() && uri.startsWith("file://") && info->isValid())
            uri = "file://" +  FileUtils::getSymbolicTarget(uri);
        if(!m_view->m_multi_select)
            Q_EMIT this->viewDoubleClicked(uri);
    });

    connect(m_view, &IconView::customContextMenuRequested, this, [=](const QPoint &pos) {
        // we should clear the dirty rubber band due to call context menu.
        bool isDragSelecting = m_view->isDraggingState();
        if (isDragSelecting) {
            // send a fake mouse release event for clear the rubber band.
            // note that we should pass mouse move event durring delaying requesting
            // directory view menu, otherwize both dnd and menu action will be triggered.
            m_view->setIgnore_mouse_move_event(true);
            QMouseEvent fakeEvent(QMouseEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            qApp->sendEvent(m_view, &fakeEvent);
            m_view->viewport()->repaint();
        }

        if (!m_view->indexAt(pos).isValid())
            m_view->clearSelection();

        //NOTE: we have to ensure that we have cleared the
        //selection if menu request at blank pos.
        QTimer::singleShot(isDragSelecting? 300: 1, this, [=]() {
            m_view->setIgnore_mouse_move_event(false);
            Q_EMIT this->menuRequest(QCursor::pos());
        });
    });

    connect(m_proxy_model, &FileItemProxyFilterSortModel::layoutChanged, this, [=]() {
        Q_EMIT this->sortOrderChanged(Qt::SortOrder(getSortOrder()));
    });
    connect(m_proxy_model, &FileItemProxyFilterSortModel::layoutChanged, this, [=]() {
        Q_EMIT this->sortTypeChanged(getSortType());
    });
}

void IconView2::repaintView()
{
    m_view->update();
    m_view->viewport()->update();
}

void IconView2::setCurrentZoomLevel(int zoomLevel)
{
    if (zoomLevel <= maximumZoomLevel() && zoomLevel >= minimumZoomLevel()) {
        m_zoom_level = zoomLevel;
        //FIXME: implement zoom
        int base = 16; //50
        int adjusted = base + zoomLevel;
        m_view->setIconSize(QSize(adjusted, adjusted));
        m_view->setGridSize(m_view->itemDelegate()->sizeHint(QStyleOptionViewItem(), QModelIndex()) + QSize(20, 20));
    }
}

void IconView2::clearIndexWidget()
{
    for (auto index : m_proxy_model->getAllFileIndexes()) {
        m_view->closePersistentEditor(index);
        m_view->setIndexWidget(index, nullptr);
    }
}


