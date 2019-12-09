/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "desktop-icon-view.h"

#include "icon-view-style.h"
#include "desktop-icon-view-delegate.h"

#include "desktop-item-model.h"
#include "desktop-item-proxy-model.h"

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"
#include "file-trash-operation.h"
#include "clipboard-utils.h"

#include "desktop-index-widget.h"

#include "file-meta-info.h"

#include <QAction>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>

#include <QWheelEvent>
#include <QApplication>

#include <QDebug>

using namespace Peony;

#define ITEM_POS_ATTRIBUTE "metadata::peony-qt-desktop-item-position"

DesktopIconView::DesktopIconView(QWidget *parent) : QListView(parent)
{
    connect(qApp, &QApplication::paletteChanged, this, [=](){
        viewport()->update();
    });

    m_edit_trigger_timer.setSingleShot(true);
    m_last_index = QModelIndex();

    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WA_TranslucentBackground);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //fix rubberband style.
    setStyle(DirectoryView::IconViewStyle::getStyle());

    setItemDelegate(new DesktopIconViewDelegate(this));

    setDefaultDropAction(Qt::MoveAction);

    setSelectionMode(QListView::ExtendedSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setViewMode(QListView::IconMode);
    setMovement(QListView::Snap);
    setFlow(QListView::TopToBottom);
    setResizeMode(QListView::Adjust);
    setWordWrap(true);

    setDragDropMode(QListView::DragDrop);

    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QListView::ExtendedSelection);

    auto zoomLevel = this->zoomLevel();
    setDefaultZoomLevel(zoomLevel);

    QTimer::singleShot(1, this, [=](){
        connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection){
            //qDebug()<<"selection changed";
            this->setIndexWidget(m_last_index, nullptr);
            this->resetEditTriggerTimer();
            auto currentSelections = this->selectionModel()->selection().indexes();

            if (currentSelections.count() == 1) {
                //qDebug()<<"set index widget";
                m_last_index = currentSelections.first();
                auto delegate = qobject_cast<DesktopIconViewDelegate *>(itemDelegate());
                this->setIndexWidget(m_last_index, new DesktopIndexWidget(delegate, viewOptions(), m_last_index, this));
            } else {
                m_last_index = QModelIndex();
                for (auto index : deselection.indexes()) {
                    this->setIndexWidget(index, nullptr);
                }
            }
        });
    });

    m_model = new DesktopItemModel(this);
    m_proxy_model = new DesktopItemProxyModel(m_model);

    m_proxy_model->setSourceModel(m_model);

    connect(m_model, &DesktopItemModel::dataChanged, this, &DesktopIconView::clearAllIndexWidgets);

    connect(m_model, &DesktopItemModel::refreshed, this, [=](){
        this->updateItemPosistions(nullptr);
    });

    connect(m_model, &DesktopItemModel::requestClearIndexWidget, this, &DesktopIconView::clearAllIndexWidgets);

    connect(m_model, &DesktopItemModel::requestLayoutNewItem, this, [=](const QString &uri){
        auto index = m_proxy_model->mapFromSource(m_model->indexFromUri(uri));
        //qDebug()<<"=====================layout new item"<<index.data();
        bool isOverlapping = false;
        for (int i = 0; i < m_proxy_model->rowCount(); i++) {
            auto other = m_proxy_model->index(i, 0);
            if (index == other)
                continue;
            if (this->visualRect(index) == this->visualRect(other)) {
                isOverlapping = true;
                break;
            }
        }

        if (isOverlapping) {
            //qDebug()<<"=====================find a new empty place put new item";
            bool find = false;
            auto rect = this->QListView::visualRect(index);
            auto grid = this->gridSize();
            auto viewRect = this->rect();
            auto next = rect;
            while (!find) {
                next.translate(0, grid.height());
                if (next.bottom() > viewRect.bottom()) {
                    //put item to next column first column
                    next.translate(grid.width(), 0);
                    auto tmp = next;
                    int row_count_to_move = 0;
                    while (tmp.top() > 0) {
                        tmp.translate(0, -grid.height());
                        row_count_to_move++;
                    }
                    next = tmp.translated(0, grid.height());
                }
                if (!this->indexAt(next.center()).isValid()) {
                    find = true;
                    //put it into empty
                    qDebug()<<"put"<<index.data()<<next.topLeft();
                    this->setPositionForIndex(next.topLeft(), index);
                    this->saveItemPositionInfo(uri);
                }
            }
        }
    });

    connect(m_model, &DesktopItemModel::requestUpdateItemPositions, this, &DesktopIconView::updateItemPosistions);

    connect(m_proxy_model, &QSortFilterProxyModel::layoutChanged, this, [=](){
        //qDebug()<<"layout changed=========================\n\n\n\n\n";
        if (m_proxy_model->getSortType() == DesktopItemProxyModel::Other) {
            return;
        }
        if (m_proxy_model->sortColumn() != 0) {
            return;
        }
        //qDebug()<<"save====================================";
        QTimer::singleShot(100, this, [=](){
            this->saveAllItemPosistionInfos();
        });
    });

    connect(this, &QListView::iconSizeChanged, this, [=](){
        //qDebug()<<"save=============";
        QTimer::singleShot(100, this, [=](){
            this->saveAllItemPosistionInfos();
        });
    });

    setModel(m_proxy_model);
    //m_proxy_model->sort(0);

    m_model->refresh();
}

DesktopIconView::~DesktopIconView()
{
    //saveAllItemPosistionInfos();
}

void DesktopIconView::saveAllItemPosistionInfos()
{
    //qDebug()<<"======================save";
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        auto indexRect = QListView::visualRect(index);
        QStringList topLeft;
        topLeft<<QString::number(indexRect.top());
        topLeft<<QString::number(indexRect.left());

        auto metaInfo = FileMetaInfo::fromUri(index.data(Qt::UserRole).toString());
        if (metaInfo) {
            //qDebug()<<"save real"<<index.data()<<topLeft;
            metaInfo->setMetaInfoStringList(ITEM_POS_ATTRIBUTE, topLeft);
        }
    }
    //qDebug()<<"======================save finished";
}

void DesktopIconView::saveItemPositionInfo(const QString &uri)
{
    auto index = m_proxy_model->mapFromSource(m_model->indexFromUri(uri));
    auto indexRect = QListView::visualRect(index);
    QStringList topLeft;
    topLeft<<QString::number(indexRect.top());
    topLeft<<QString::number(indexRect.left());
    auto metaInfo = FileMetaInfo::fromUri(index.data(Qt::UserRole).toString());
    if (metaInfo) {
        //qDebug()<<"save"<<uri<<topLeft;
        metaInfo->setMetaInfoStringList(ITEM_POS_ATTRIBUTE, topLeft);
    }
}

void DesktopIconView::resetAllItemPositionInfos()
{
    for (int i = 0; i < m_proxy_model->rowCount(); i++) {
        auto index = m_proxy_model->index(i, 0);
        auto indexRect = QListView::visualRect(index);
        QStringList topLeft;
        topLeft<<QString::number(indexRect.top());
        topLeft<<QString::number(indexRect.left());
        auto metaInfo = FileMetaInfo::fromUri(index.data(Qt::UserRole).toString());
        if (metaInfo) {
            QStringList tmp;
            tmp<<"-1"<<"-1";
            metaInfo->setMetaInfoStringList(ITEM_POS_ATTRIBUTE, tmp);
        }
    }
}

void DesktopIconView::resetItemPosistionInfo(const QString &uri)
{
    auto metaInfo = FileMetaInfo::fromUri(uri);
    if (metaInfo)
        metaInfo->removeMetaInfo(ITEM_POS_ATTRIBUTE);
}

void DesktopIconView::updateItemPosistions(const QString &uri)
{
    if (uri.isNull()) {
        for (int i = 0; i < m_proxy_model->rowCount(); i++) {
            auto index = m_proxy_model->index(i, 0);
            auto metaInfo = FileMetaInfo::fromUri(index.data(Qt::UserRole).toString());
            if (metaInfo) {
                updateItemPosistions(index.data(Qt::UserRole).toString());
            }
        }
        return;
    }

    auto index = m_proxy_model->mapFromSource(m_model->indexFromUri(uri));
    //qDebug()<<"update"<<uri<<index.data();

    if (!index.isValid()) {
        //qDebug()<<"err: index invalid";
        return;
    }
    auto metaInfo = FileMetaInfo::fromUri(uri);
    if (!metaInfo) {
        //qDebug()<<"err: no meta data";
        return;
    }

    auto list = metaInfo->getMetaInfoStringList(ITEM_POS_ATTRIBUTE);
    if (!list.isEmpty()) {
        if (list.count() == 2) {
            int top = list.first().toInt();
            int left = list.at(1).toInt();
            if (top >= 0 && left >= 0) {
//                auto rect = visualRect(index);
//                auto grid = gridSize();
//                if (abs(rect.top() - top) < grid.width() && abs(rect.left() - left))
//                    return;
                QPoint p(left, top);
                //qDebug()<<"set"<<index.data()<<p;
                setPositionForIndex(QPoint(left, top), index);
            } else {
                saveItemPositionInfo(uri);
            }
        }
    }
}

const QStringList DesktopIconView::getSelections()
{
    QStringList uris;
    auto indexes = selectionModel()->selection().indexes();
    for (auto index : indexes) {
        uris<<index.data(Qt::UserRole).toString();
    }
    return uris;
}

const QStringList DesktopIconView::getAllFileUris()
{

}

void DesktopIconView::setSelections(const QStringList &uris)
{

}

void DesktopIconView::invertSelections()
{

}

void DesktopIconView::scrollToSelection(const QString &uri)
{

}

int DesktopIconView::getSortType()
{
    return m_proxy_model->getSortType();
}

void DesktopIconView::setSortType(int sortType)
{
    //resetAllItemPositionInfos();
    m_proxy_model->setSortType(sortType);
    m_proxy_model->sort(1);
    m_proxy_model->sort(0, m_proxy_model->sortOrder());
}

int DesktopIconView::getSortOrder()
{
    return m_proxy_model->sortOrder();
}

void DesktopIconView::setSortOrder(int sortOrder)
{
    m_proxy_model->sort(0, Qt::SortOrder(sortOrder));
}

void DesktopIconView::editUri(const QString &uri)
{
    clearAllIndexWidgets();
    QTimer::singleShot(100, this, [=](){
        edit(m_proxy_model->mapFromSource(m_model->indexFromUri(uri)));
    });
}

void DesktopIconView::editUris(const QStringList uris)
{

}

void DesktopIconView::setCutFiles(const QStringList &uris)
{
    ClipboardUtils::setClipboardFiles(uris, true);
}

void DesktopIconView::closeView()
{
    deleteLater();
}

void DesktopIconView::wheelEvent(QWheelEvent *e)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier)
    {
       if (e->delta() > 0) {
           zoomIn();
       } else {
           zoomOut();
       }
       resetAllItemPositionInfos();
    }
}

void DesktopIconView::zoomOut()
{
    switch (zoomLevel()) {
    case Huge:
        setDefaultZoomLevel(Large);
        break;
    case Large:
        setDefaultZoomLevel(Normal);
        break;
    case Normal:
        setDefaultZoomLevel(Small);
        break;
    default:
        setDefaultZoomLevel(zoomLevel());
        break;
    }
}

void DesktopIconView::zoomIn()
{
    switch (zoomLevel()) {
    case Small:
        setDefaultZoomLevel(Normal);
        break;
    case Normal:
        setDefaultZoomLevel(Large);
        break;
    case Large:
        setDefaultZoomLevel(Huge);
        break;
    default:
        setDefaultZoomLevel(zoomLevel());
        break;
    }
}

/*
Small, //icon: 32x32; grid: 56x64; hover rect: 40x56; font: system*0.8
Normal, //icon: 48x48; grid: 64x72; hover rect = 56x64; font: system
Large, //icon: 64x64; grid: 115x135; hover rect = 105x118; font: system*1.2
Huge //icon: 96x96; grid: 130x150; hover rect = 120x140; font: system*1.4
*/
void DesktopIconView::setDefaultZoomLevel(ZoomLevel level)
{
    //qDebug()<<"set default zoom level:"<<level;
    m_zoom_level = level;
    switch (level) {
    case Small:
        setIconSize(QSize(24, 24));
        setGridSize(QSize(64, 64));
        break;
    case Large:
        setIconSize(QSize(64, 64));
        setGridSize(QSize(115, 135));
        break;
    case Huge:
        setIconSize(QSize(96, 96));
        setGridSize(QSize(140, 170));
        break;
    default:
        m_zoom_level = Normal;
        setIconSize(QSize(48, 48));
        setGridSize(QSize(96, 96));
        break;
    }
    clearAllIndexWidgets();
    auto metaInfo = FileMetaInfo::fromUri("computer:///");
    if (metaInfo) {
        qDebug()<<"set zoom level"<<m_zoom_level;
        metaInfo->setMetaInfoInt("peony-qt-desktop-zoom-level", int(m_zoom_level));
    } else {

    }
}

DesktopIconView::ZoomLevel DesktopIconView::zoomLevel() const
{
    //FIXME:
    if (m_zoom_level != Invalid)
        return m_zoom_level;

    auto metaInfo = FileMetaInfo::fromUri("computer:///");
    if (metaInfo) {
        auto i = metaInfo->getMetaInfoInt("peony-qt-desktop-zoom-level");
        return ZoomLevel(i);
    }

    GFile *computer = g_file_new_for_uri("computer:///");
    GFileInfo *info = g_file_query_info(computer,
                                        "metadata::peony-qt-desktop-zoom-level",
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    char* zoom_level = g_file_info_get_attribute_as_string(info, "metadata::peony-qt-desktop-zoom-level");
    if (!zoom_level) {
        //qDebug()<<"======================no zoom level meta info\n\n\n";
        g_object_unref(info);
        g_object_unref(computer);
        return Normal;
    }
    g_object_unref(info);
    g_object_unref(computer);
    QString zoomLevel = zoom_level;
    g_free(zoom_level);
    //qDebug()<<ZoomLevel(QString(zoomLevel).toInt())<<"\n\n\n\n\n\n\n\n";
    return ZoomLevel(zoomLevel.toInt()) == Invalid? Normal: ZoomLevel(QString(zoomLevel).toInt());
}

void DesktopIconView::mousePressEvent(QMouseEvent *e)
{
    if (!indexAt(e->pos()).isValid()) {
        clearAllIndexWidgets();
        clearSelection();
    }

    QListView::mousePressEvent(e);

    //qDebug()<<m_last_index.data();
    if (e->button() != Qt::LeftButton) {
        return;
    }

    if (indexAt(e->pos()) == m_last_index && m_last_index.isValid()) {
        //qDebug()<<"check";
        if (m_edit_trigger_timer.isActive()) {
            //qDebug()<<"edit";
            setIndexWidget(m_last_index, nullptr);
            edit(m_last_index);
        }
    }
}

void DesktopIconView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton) {
        return;
    }

    if (!m_edit_trigger_timer.isActive() && indexAt(e->pos()).isValid() && this->selectedIndexes().count() == 1) {
        resetEditTriggerTimer();
    }
}

void DesktopIconView::resetEditTriggerTimer()
{
    m_edit_trigger_timer.disconnect();
    m_edit_trigger_timer.stop();
    QTimer::singleShot(750, [&](){
        //qDebug()<<"start";
        m_edit_trigger_timer.setSingleShot(true);
        m_edit_trigger_timer.start(1000);
    });
}

void DesktopIconView::dragEnterEvent(QDragEnterEvent *e)
{
    //qDebug()<<"drag enter event";
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(Qt::MoveAction);
        e->acceptProposedAction();
    }
}

void DesktopIconView::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->isAccepted())
        return;
    //qDebug()<<"drag move event";
    if (this == e->source()) {
        e->accept();
        return QListView::dragMoveEvent(e);
    }
    e->setDropAction(Qt::CopyAction);
    e->accept();
}

void DesktopIconView::dropEvent(QDropEvent *e)
{
    //qDebug()<<"drop event";
    /*!
      \todo
      fix the bug that move drop action can not move the desktop
      item to correct position.

      i use copy action to avoid this bug, but the drop indicator
      is incorrect.
      */
    m_edit_trigger_timer.stop();
    if (this == e->source()) {

        auto index = indexAt(e->pos());
        if (index.isValid()) {
            auto info = FileInfo::fromUri(index.data(Qt::UserRole).toString());
            if (!info->isDir())
                return;
        }

        QListView::dropEvent(e);

        auto urls = e->mimeData()->urls();
        for (auto url : urls) {
//            if (url.path() == QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
//                continue;
            saveItemPositionInfo(url.toDisplayString());
        }
        return;
    }
    m_model->dropMimeData(e->mimeData(), Qt::MoveAction, -1, -1, this->indexAt(e->pos()));
    //FIXME: save item position
}

const QFont DesktopIconView::getViewItemFont(QStyleOptionViewItem *item)
{
    auto font = item->font;
    switch (zoomLevel()) {
    case DesktopIconView::Small:
        font.setPointSizeF(font.pointSizeF() * 0.8);
        break;
    case DesktopIconView::Large:
        font.setPointSizeF(font.pointSizeF() * 1.2);
        break;
    case DesktopIconView::Huge:
        font.setPointSizeF(font.pointSizeF() * 1.4);
        break;
    default:
        break;
    }
    return font;
}

void DesktopIconView::clearAllIndexWidgets()
{
    if (!model())
        return;

    int row = 0;
    auto index = model()->index(row, 0);
    while (index.isValid()) {
        setIndexWidget(index, nullptr);
        row++;
        index = model()->index(row, 0);
    }
}

void DesktopIconView::refresh()
{
    m_model->refresh();
}

QRect DesktopIconView::visualRect(const QModelIndex &index) const
{
    auto rect = QListView::visualRect(index);
    QPoint p(10, 5);

    switch (zoomLevel()) {
    case Small:
        p *= 0.8;
        break;
    case Large:
        p *= 1.2;
        break;
    case Huge:
        p *= 1.4;
        break;
    default:
        break;
    }
    rect.moveTo(rect.topLeft() + p);
    return rect;
}
