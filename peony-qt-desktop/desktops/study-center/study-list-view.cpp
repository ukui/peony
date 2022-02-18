/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 */

#include "study-list-view.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMouseEvent>
#include <QDebug>
#include <QList>
#include "progress-item-delegate.h"
#include "../../tablet/src/Style/style.h"

using namespace Peony;

StudyListView::StudyListView(QWidget *parent): QListView(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("QListView{border:0px;}\
                        QListView:Item{background:transparent;border:0px;color:#ffffff;font-size:14px;padding-left:0px;}\
                        QListView:Item:hover{background:transparent;}\
                        QListView:Item:pressed{background:transparent;}");
    this->setProperty("notUseSlideGesture", true);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   // listView->setViewMode(QListView::IconMode);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setResizeMode(QListView::Adjust);
    this->setTextElideMode(Qt::ElideRight);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::NoFocus);
//    this->setAcceptDrops(false);
    this->setMovement(QListView::Static);
//    this->setUpdatesEnabled(true);
    this->setFlow(QListView::LeftToRight);
    this->setViewMode(QListView::IconMode);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setGridSize(QSize(Style::GridWidth,Style::GridHeight));

    m_listmodel=new QStandardItemModel;
    this->setModel(m_listmodel);
}
void StudyListView::setData(QList<TabletAppEntity*> tabletAppList)
{
    m_listmodel->clear();
    for(int i = 0; i < tabletAppList.size(); ++i)
    {
        QStandardItem* item=new QStandardItem;
        TABLETAPP stuApp;
        stuApp.serialNumber = tabletAppList.at(i)->serialNumber;
        stuApp.desktopName = tabletAppList.at(i)->desktopName;
        stuApp.appName = tabletAppList.at(i)->appName;
        stuApp.appIcon = tabletAppList.at(i)->appIcon;
        stuApp.execCommand = tabletAppList.at(i)->execCommand;
        stuApp.iTime = 0;

        item->setData(QVariant::fromValue<TABLETAPP>(stuApp),Qt::DisplayRole);
        m_listmodel->appendRow(item);
    }

    if (!m_delegate) {
        m_delegate = new FullItemDelegate(this, 0);
    }
    this->setItemDelegate(m_delegate);

}

void StudyListView::mousePressEvent(QMouseEvent *event)
{
    QPoint pressedpos = event->pos();
    m_pressedPos = QPoint(0, 0);
    m_iconClicked = false;
    if(event->button() == Qt::LeftButton) {//左键
        m_listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(1),Qt::UserRole);
        if (this->indexAt(pressedpos).isValid()) {
            //滑动的起点在图标上
            m_iconClicked = true;
        }
    } else if(event->button() == Qt::RightButton) {//右键
        m_listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
    }
    if (m_iconClicked) {
        m_pressedPos = pressedpos;
        event->accept();
    } else {
        //让父组件获取按下事件
        event->ignore();
    }
}
void StudyListView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_iconClicked) {
        //没有按在图标上，忽略事件
        event->ignore();
        return;
    }
    //去除按下图标的放大效果
    m_listmodel->setData(this->indexAt(m_pressedPos),QVariant::fromValue<bool>(0),Qt::UserRole);

    QPoint pressedpos = event->pos();
    //QPoint pressedGlobalPos = event->globalPos();
    if (event->button() == Qt::LeftButton) {
        m_listmodel->setData(this->indexAt(pressedpos), QVariant::fromValue<bool>(0), Qt::UserRole);
        Q_EMIT(clicked(this->indexAt(pressedpos)));

    } else if (event->button() == Qt::RightButton) {
        m_listmodel->setData(this->indexAt(pressedpos), QVariant::fromValue<bool>(0), Qt::UserRole);
    }
    event->accept();
}
void StudyListView::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pressedpos = event->pos();
    this->model()->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
    QListView::mouseMoveEvent(event);
}
void StudyListView::mouseDoubleClickEvent(QMouseEvent *event)
{
     QListView::mouseDoubleClickEvent(event);
}

int StudyListView::horizontalOffset() const
{
    return 15;
}
