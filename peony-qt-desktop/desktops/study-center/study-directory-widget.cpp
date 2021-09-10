/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "study-directory-widget.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <iterator>
#include "progress-item-delegate.h"
#include "pushbutton.h"
#include "../../tablet/data/tablet-app-manager.h"
#include "../../tablet/src/Style/style.h"

using namespace Peony;


StudyDirectoryWidget::StudyDirectoryWidget(QStringList &strListTitleStyle, QMap<QString, QList<TabletAppEntity*>> &subtitleMap, int mode, QWidget *parent)
: QWidget(parent)
{
    m_iMode = mode;
    m_studyCenterDataMap = subtitleMap;
    initWidget(strListTitleStyle);
}

StudyDirectoryWidget::~StudyDirectoryWidget()
{
    if(nullptr != m_scrollAreaWidLayout)
    {
        delete m_scrollAreaWidLayout;
        m_scrollAreaWidLayout=nullptr;
    }
    if(nullptr != m_scrollArea)
    {
        delete m_scrollArea;
        m_scrollArea=nullptr;
    }

    if(nullptr != m_mainLayout)
    {
        delete m_mainLayout;
        m_mainLayout=nullptr;
    }
}

void StudyDirectoryWidget::initWidget(QStringList &strListTitleStyle)
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);
    this->setStyleSheet("border:0px;background:transparent;\
                        background-color:rgba(255, 255, 255);\
                        border-radius:15px;");

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    m_mainLayout=new QVBoxLayout(this);
    if(strListTitleStyle.size() >= 2)
    {
        QLabel* titleLabel = new QLabel;
        QFont ft;
        ft.setPointSize(18);
        ft.setWeight(70);
        titleLabel->setFont(ft);
        titleLabel->setStyleSheet(strListTitleStyle.at(1).toLocal8Bit().constData());
        titleLabel->setText( strListTitleStyle.at(0).toLocal8Bit().constData());
        m_mainLayout->addWidget(titleLabel);
    }

    m_scrollArea = new QScrollArea;
    m_scrollArea->horizontalScrollBar()->setVisible(false);
    m_scrollArea->verticalScrollBar()->setVisible(true);
    m_scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar{width:3px;padding-top:0px;padding-bottom:0px;background:transparent;border-radius:6px;}"
                                             "QScrollBar::handle{background-color:rgba(0,0,0,0.25); width:3px;border-radius:1.5px;}"
                                             "QScrollBar::handle:hover{background-color:#697883;border-radius:1.5px;}"
                                             "QScrollBar::handle:pressed{background-color:#8897a3;border-radius:1.5px;}"
                                             "QScrollBar::sub-line{background-color:transparent;height:0px;width:0px;}"
                                             "QScrollBar::add-line{background-color:transparent;height:0px;width:0px;}"
                                             );
    QWidget* scrollAreaWid=new QWidget;
    scrollAreaWid->setMinimumSize(this->width()-40,this->height()-60);
    m_scrollArea->setWidget(scrollAreaWid);
    m_scrollArea->setWidgetResizable(true);
    m_scrollAreaWidLayout = new QVBoxLayout;
    m_scrollAreaWidLayout->setContentsMargins(0,0,0,0);
    m_scrollAreaWidLayout->setSpacing(0);
    scrollAreaWid->setLayout(m_scrollAreaWidLayout);

    m_mainLayout->addWidget(m_scrollArea);
    m_mainLayout->setMargin(20);
    m_mainLayout->setSpacing(20);
//    connect(m_scrollArea->verticalScrollBar(),SIGNAL(valueChanged(int)),
//            this,SLOT(valueChangedSlot(int)));

    initAppListWidget();
    //m_mainLayout->setMargin(20);
   // m_mainLayout->setSpacing(20);
    this->setLayout(m_mainLayout);

    //pUkuiMenuInterface=new UkuiMenuInterface;

    //?????????
//    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
//    m_setting=new QSettings(path,QSettings::IniFormat);

//    for(int i = 1; i < strListTitle.size(); i++)
//    {
//        //fillAppList(strListTitle.at(i));
//    }
}

void StudyDirectoryWidget::initAppListWidget()
{
    QMap<QString, QList<TabletAppEntity*>>::const_iterator ite = m_studyCenterDataMap.begin();
    for(; ite != m_studyCenterDataMap.constEnd(); ++ite)
    {     
        if(1 == m_iMode)
        {
            //设置子标题
            QString  strTitle = ite.key();
            PushButton* titleBtn = new PushButton(this,strTitle,m_scrollArea->width()-12,20);
            m_scrollAreaWidLayout->addWidget(titleBtn);
        }

        //插入应用列表
        QListView* listView = new QListView;
        listView->setStyleSheet("QListView{border:0px;}\
                            QListView:Item{background:transparent;border:0px;color:#ffffff;font-size:14px;padding-left:0px;}\
                            QListView:Item:hover{background:transparent;}\
                            QListView:Item:pressed{background:transparent;}");
        listView->setSelectionMode(QAbstractItemView::SingleSelection);
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
       // listView->setViewMode(QListView::IconMode);
        listView->setContextMenuPolicy(Qt::CustomContextMenu);
        listView->setResizeMode(QListView::Adjust);
        listView->setTextElideMode(Qt::ElideRight);
        listView->setMouseTracking(true);
        listView->setFocusPolicy(Qt::NoFocus);
    //    this->setAcceptDrops(false);
        listView->setMovement(QListView::Static);
    //    this->setUpdatesEnabled(true);
        listView->setFlow(QListView::LeftToRight);
        listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        listView->setGridSize(QSize(Style::AppListIconSize,Style::AppListIconSize));
        QStandardItemModel* listmodel=new QStandardItemModel;
        listView->setModel(listmodel);

        QList<TabletAppEntity*> tabletAppList = ite.value();
        for(int i = 0; i < tabletAppList.size(); ++i)
        {
            QStandardItem* item=new QStandardItem;
            //item->setText(desktopfp);
            //item->setIcon(icon);
            //item->setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);
            TABLETAPP stuApp;
            stuApp.serialNumber = tabletAppList.at(i)->serialNumber;
            stuApp.desktopName = tabletAppList.at(i)->desktopName;
            stuApp.appName = tabletAppList.at(i)->appName;
            stuApp.appIcon = tabletAppList.at(i)->appIcon;
            stuApp.execCommand = tabletAppList.at(i)->execCommand;

            item->setData(QVariant::fromValue<TABLETAPP>(stuApp),Qt::DisplayRole);
            listmodel->appendRow(item);
        }

        FullItemDelegate* delegate= new FullItemDelegate(listView,0);
        listView->setItemDelegate(delegate);
        m_scrollAreaWidLayout->addWidget(listView);
        connect(listView,SIGNAL(clicked(QModelIndex)),this,SLOT(execApplication(QModelIndex)));

       // connect(listView,SIGNAL(sendItemClickedSignal(QString)),this,SLOT(execApplication(QString)));
       // connect(listView,SIGNAL(sendUpdateAppListSignal(QString,int)),this,SIGNAL(sendUpdateAppListSignal(QString,int)));
       // connect(listView,SIGNAL(removeListItemSignal(QString)),this,SIGNAL(removeListItemSignal(QString)));

    }
    //m_mainLayout->setContentsMargins(Style::LeftWidWidth,0,0,0);

}
/**
 * 执行应用程序
 */
void StudyDirectoryWidget::execApplication(QModelIndex app)
{
    //  Q_EMIT sendHideMainWindowSignal();
    TABLETAPP tabletApp = app.data(Qt::DisplayRole).value<TABLETAPP>();
    QString execpath=tabletApp.execCommand;
    //移除启动参数%u或者%U
    if(execpath.contains("%"))
    {
        int index=execpath.indexOf(QString("%").at(0));
        execpath.remove(index-1,3);
    }
    QProcess::startDetached(execpath);


//    QStringList parameters;
//    if(exe.indexOf("%") != -1)
//    {
//         exe = exe.left(exe.indexOf("%") - 1);
//         //qDebug()<<"=====dd====="<<exe;
//    }
//    if(exe.indexOf("-") != -1)
//    {
//         parameters = exe.split(" ");
//         exe = parameters[0];
//         parameters.removeAt(0);
//         //qDebug()<<"===qqq==="<<exe;
//    }

//     qDebug()<<"5 exe"<<exe<<parameters;
//     QDBusInterface session("org.gnome.SessionManager", "/com/ukui/app", "com.ukui.app");
//     if (parameters.isEmpty())
//     {
//         session.call("app_open", exe, parameters);
//     }
//     else
//     {
//         session.call("app_open", exe, parameters);
//     }

     //Q_EMIT sendHideMainWindowSignal();

}


/**
 * 更新应用列表
 */
//void StudyDirectoryWidget::updateListViewSlot(QString desktopfp, int type)
//{
//    for(int i=0;i<listview->model()->rowCount();i++)
//    {
//        QVariant var=listview->model()->index(i,0).m_strListData(Qt::DisplayRole);
//        QString path=var.value<QString>();
//        if(QString::compare(path,desktopfp)==0)
//        {
//            listview->model()->removeRow(i);
//            break;
//        }
//    }
//    setting->beginGroup("lockapplication");
//    QStandardItem* item=new QStandardItem;
//    item->setData(QVariant::fromValue<QString>(desktopfp),Qt::DisplayRole);
//    QStandardItemModel* listmodel=qobject_cast<QStandardItemModel*>(listview->model());
//    if(type==0)
//        listmodel->insertRow(setting->allKeys().size()-1,item);
//    else
//        listmodel->insertRow(setting->allKeys().size(),item);
//    setting->endGroup();
//}

//void StudyDirectoryWidget::updateListViewAllSlot()
//{
//    getCommonUseAppList();
//    listview->updateData(m_strListData);
//}

//void StudyDirectoryWidget::removeListItemSlot(QString desktopfp)
//{
//    for(int i=0;i<listview->model()->rowCount();i++)
//    {
//        QVariant var=listview->model()->index(i,0).m_strListData(Qt::DisplayRole);
//        QString path=var.value<QString>();
//        if(QString::compare(path,desktopfp)==0)
//        {
//            listview->model()->removeRow(i);
//            break;
//        }
//    }
//}

//void StudyDirectoryWidget::removeListAllItemSlot()
//{
//    setting->beginGroup("lockapplication");
//    for(int i=listview->model()->rowCount()-1;i>=0;i--)
//    {
//        QVariant var=listview->model()->index(i,0).m_strListData(Qt::DisplayRole);
//        QString desktopfp=var.value<QString>();
//        QFileInfo fileInfo(desktopfp);
//        QString desktopfn=fileInfo.fileName();
//        if(!setting->contains(desktopfn))
//            listview->model()->removeRow(i);
//    }
//    setting->endGroup();
//}

//void StudyDirectoryWidget::getUseAppList(QString &strModuleName)
//{
//    m_setting->beginGroup(strModuleName);
//    QStringList lockdesktopfnList=m_setting->allKeys();
//    m_setting->endGroup();

//    m_strListData.clear();
//    Q_FOREACH(QString desktopfn,lockdesktopfnList)
//    {
//        QString desktopfp=QString("/usr/share/applications/"+desktopfn);
//        m_strListData.append(desktopfp);
//    }
//}
/**
 * 设置scrollarea所填充控件大小
 */
void StudyDirectoryWidget::resizeScrollAreaControls()
{
//    int row = 0;
//    int temp = 0;
//    temp = this->width()-40/Style::;
//   QMap<QString, QList<TabletAppEntity*>> m_studyCenterDataMap;
//    if(1 == m_iMode)
//    {
//        for(int i = 0; i < m_studyCenterDataMap.size(); ++i)
//        {
//            int temp2 = m_studyCenterDataMap[i].size()%temp;
//            row += (temp2==0? m_studyCenterDataMap[i].size()/temp:m_studyCenterDataMap[i].size()/temp+1);
//        }
//        row*Style::
//    }
//    for()
//    {
//        //应用界面
//        QLayoutItem* widItem=m_scrollAreaWidLayout->itemAt(row*2+1);
//        QWidget* wid=widItem->widget();
//        QListView* listview=qobject_cast<QListView*>(wid);
//        listview->adjustSize();
//        int dividend=(m_scrollArea->width()-5)/this->width();

//        int rowcount=0;
//        if(listview->model()->rowCount()%dividend>0)
//        {
//            rowcount=listview->model()->rowCount()/dividend+1;
//        }
//        else
//        {
//            rowcount=listview->model()->rowCount()/dividend;

//        }

//        listview->setFixedSize(m_scrollArea->width()-5+1,listview->gridSize().height()*rowcount);
//        row++;
//    }
//    m_scrollArea->widget()->adjustSize();
}
//void StudyDirectoryWidget::valueChangedSlot(int value)
//{
//    int index=0;
//    while(index<=m_letterList.count()-1)
//    {
//        int min=m_scrollAreaWidLayout->itemAt(2*index)->widget()->y();
//        int max=0;
//        if(index==m_letterList.count()-1)
//            max=m_scrollAreaWid->height();
//        else
//            max=m_scrollAreaWidLayout->itemAt(2*(index+1))->widget()->y();

//        if(value>=min && value <max)
//        {
//            Q_FOREACH (QAbstractButton* button, m_buttonList) {
//                LetterClassifyButton* letterbtn=qobject_cast<LetterClassifyButton*>(button);
//                if(index==m_buttonList.indexOf(button))
//                {
//                    letterbtn->setChecked(true);
//                }
//                else
//                {
//                    letterbtn->setChecked(false);
//                }
//            }
//            break;
//        }
//        else
//            index++;
//    }

////    //向下滚动
////    if((m_buttonList.at(index)->pos().y()+m_buttonList.at(index)->height()+m_letterListScrollArea->widget()->pos().y()) >= m_letterListScrollArea->height())
////    {
////        int val=m_letterListScrollArea->verticalScrollBar()->sliderPosition()+m_buttonList.at(index)->height();
////        m_letterListScrollArea->verticalScrollBar()->setSliderPosition(val);
////    }

////    //向上滚动
////    if((m_buttonList.at(index)->pos().y()+m_letterListScrollArea->widget()->pos().y()) <= 0)
////    {

////        int val=m_letterListScrollArea->verticalScrollBar()->value()-m_buttonList.at(index)->height();
////        m_letterListScrollArea->verticalScrollBar()->setSliderPosition(val);
////    }

//}
