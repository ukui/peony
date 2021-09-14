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
    this->setStyleSheet("QListView{border:0px;}\
                        QListView:Item{background:transparent;border:0px;color:#ffffff;font-size:14px;padding-left:0px;}\
                        QListView:Item:hover{background:transparent;}\
                        QListView:Item:pressed{background:transparent;}");
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
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setGridSize(QSize(Style::AppListIconSize,Style::AppListIconSize));

    m_listmodel=new QStandardItemModel;
    this->setModel(m_listmodel);
}
void StudyListView::setData(QList<TabletAppEntity*> tabletAppList)
{
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

    FullItemDelegate* delegate= new FullItemDelegate(this,0);
    this->setItemDelegate(delegate);

}

void StudyListView::mousePressEvent(QMouseEvent *event)
{
    QPoint pressedpos = event->pos();
    QPoint pressedGlobalPos = event->globalPos();
    if(event->button() == Qt::LeftButton)
    {//左键
        m_listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(1),Qt::UserRole);
    }
    else if(event->button() == Qt::RightButton)
    {//右键
        m_listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
    }
    event->accept();

}
void StudyListView::mouseReleaseEvent(QMouseEvent *event)
{
 //  QAbstractItemModel* listmodel = this->model();

   QPoint pressedpos = event->pos();
   QPoint pressedGlobalPos = event->globalPos();
   if(event->button() == Qt::LeftButton)
   {//左键
       m_listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
       Q_EMIT(clicked(this->indexAt(pressedpos)));
   }
   else if(event->button() == Qt::RightButton)
   {//右键
       m_listmodel->setData(this->indexAt(pressedpos),QVariant::fromValue<bool>(0),Qt::UserRole);
   }
   event->accept();

}
void StudyListView::mouseMoveEvent(QMouseEvent *event)
{
     QListView::mouseReleaseEvent(event);
}
void StudyListView::mouseDoubleClickEvent(QMouseEvent *event)
{
     QListView::mouseDoubleClickEvent(event);
}

