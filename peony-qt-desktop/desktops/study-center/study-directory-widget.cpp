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
#include <iterator>
#include <QPainter>
#include "pushbutton.h"
#include "../../tablet/data/tablet-app-manager.h"
#include "study-list-view.h"
#include "common.h"
#include "../../tablet/src/Style/style.h"

using namespace Peony;


StudyDirectoryWidget::StudyDirectoryWidget(QStringList &strListTitleStyle, QList<QPair<QString, QList<TabletAppEntity*>>> &subtitleList, int mode, QWidget *parent)
: QWidget(parent)
{
    m_iMode = mode;
    m_studyCenterDataList = subtitleList;
    m_strListTitleStyle = strListTitleStyle;
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
    this->setStyleSheet("background-color:rgba(255, 255, 255, 0.85);border-radius:24px;");

    //this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    m_mainLayout=new QVBoxLayout(this);
    if(strListTitleStyle.size() >= 4)
    {
        m_titleLabel = new QLabel;
        QFont ft;
        ft.setBold(true);
        m_titleLabel->setFont(ft);
        m_titleLabel->setAttribute(Qt::WA_TranslucentBackground);
        m_titleLabel->setStyleSheet(QString("QLabel{background-color: transparent;color:%1;font-size:32px}").arg(strListTitleStyle.at(1)));
        m_titleLabel->setText( strListTitleStyle.at(0).toLocal8Bit().constData());
        m_mainLayout->addWidget(m_titleLabel);

        QLabel* backGroundLabel = new QLabel(this);
        QFont bft;
        bft.setBold(true);
        bft.setWeight(QFont::Bold);
        backGroundLabel->setFont(bft);
        backGroundLabel->setStyleSheet(QString("QLabel{background-color: transparent;color:%1;font-size:64px}").arg(strListTitleStyle.at(3)));
        backGroundLabel->setText(strListTitleStyle.at(2).toLocal8Bit().constData());
        backGroundLabel->setGeometry(15,-15,500,110);
        backGroundLabel->lower();

        if(1 == m_iMode)
        {
            m_mainLayout->addSpacing(56);
        }
        else
        {
            m_mainLayout->addSpacing(25);
        }
    }

    m_scrollArea = new QScrollArea;
    m_scrollArea->setStyleSheet("background:transparent;");
    m_scrollArea->horizontalScrollBar()->setVisible(false);
    m_scrollArea->verticalScrollBar()->setVisible(true);
    m_scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar{width:12px;height:172px;padding-top:0px;padding-bottom:0px;border-radius:2px;border:2px solid transparent;}"
                                             "QScrollBar::handle{background-color:#B6BDC6; width:12px;height:172px;border-radius:2px;border:2px solid transparent;}"
                                             "QScrollBar::handle:hover{background-color:#CDD2D8;border-radius:2px;height:172px;border:none;}"
                                             "QScrollBar::handle:pressed{background-color:#9CA6B2;border-radius:2px;}"
                                             "QScrollBar::sub-line{background-color:transparent;height:0px;width:0px;}"
                                             "QScrollBar::add-line{background-color:transparent;height:0px;width:0px;}"
                                             );
    QWidget* scrollAreaWid=new QWidget;
    scrollAreaWid->setStyleSheet("background:transparent;");
    scrollAreaWid->setMinimumSize(Style::itemWidth,Style::itemHeight);
    m_scrollArea->setWidget(scrollAreaWid);
    //m_scrollArea->setWidgetResizable(true);
    m_scrollAreaWidLayout = new QVBoxLayout;
    m_scrollAreaWidLayout->setContentsMargins(0,0,0,0);
    m_scrollAreaWidLayout->setSpacing(0);
    scrollAreaWid->setLayout(m_scrollAreaWidLayout);

    m_mainLayout->addWidget(m_scrollArea);
    m_mainLayout->setContentsMargins(48,33,8,0);
//    connect(m_scrollArea->verticalScrollBar(),SIGNAL(valueChanged(int)),
//            this,SLOT(valueChangedSlot(int)));

    initAppListWidget();
    this->setLayout(m_mainLayout);
    connect(this, &StudyDirectoryWidget::changeTheme, [=](QString strTheme)
    {
        QString styleSheetDark = QString("QWidget{border-radius:24px;background-color:rgba(38, 38, 40,0.85)}"
                                         "QLabel,QListView,QPushButton,QScrollArea{background:transparent}");

        QString styleSheetLight = QString("QWidget{border-radius:24px;background:rgba(255, 255, 255,0.85)}"
                                          "QLabel,QListView,QPushButton,QScrollArea{background:transparent");

        if (strTheme == "ukui-dark")
        {
            //深色主题
           this->setStyleSheet(styleSheetDark);
        }
        else
        {
            //浅色主题
           this->setStyleSheet(styleSheetLight);
        }
    });
}

void StudyDirectoryWidget::initAppListWidget()
{
    for(int i = 0; i < m_studyCenterDataList.size(); ++i)
    {
        if(1 == m_iMode)
        {
            //设置子标题
            QString  strTitle = m_studyCenterDataList.at(i).first;
            TitleWidget* titleWid = new TitleWidget(this,strTitle,m_scrollArea->width()-12,40);
            m_scrollAreaWidLayout->addWidget(titleWid);
            connect(this, SIGNAL(changeTheme( QString)), titleWid, SIGNAL(changeTheme( QString)));
        }

        //插入应用列表
        StudyListView* listView = new StudyListView(this);
        listView->setData(m_studyCenterDataList.at(i).second);
        m_scrollAreaWidLayout->addWidget(listView);
        connect(listView,SIGNAL(clicked(QModelIndex)),this,SLOT(execApplication(QModelIndex)));
    }

    resizeScrollAreaControls();

}
/**
 * 执行应用程序
 */
void StudyDirectoryWidget::execApplication(QModelIndex app)
{
    //  Q_EMIT sendHideMainWindowSignal();
    TABLETAPP tabletApp = app.data(Qt::DisplayRole).value<TABLETAPP>();

    TabletAppEntity appEntity;
    appEntity.execCommand = tabletApp.execCommand;

    TabletAppManager::getInstance()->execApp(&appEntity);
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
    int iIndex = 0;
    int iCount = m_iMode == 1? m_scrollAreaWidLayout->count()/2:m_scrollAreaWidLayout->count();
    qDebug()<<"mode: " <<m_iMode<<" count: "<<iCount;
    while(iIndex<iCount)
    {
        //应用界面
        int iRow = m_iMode == 1? iIndex*2+1:iIndex;

        QLayoutItem* widItem=m_scrollAreaWidLayout->itemAt(iRow);
        QWidget* wid=widItem->widget();
        StudyListView* listview=qobject_cast<StudyListView*>(wid);
        listview->adjustSize();
        int iDividend=(m_scrollArea->width())/Style::GridSize;
        qDebug()<<"resizeScrollAreaControls iRow: " <<iRow <<" iDividend:"<<iDividend;
        int iRowCount=0;
        if(listview->model()->rowCount()%iDividend>0)
        {
            iRowCount = listview->model()->rowCount()/iDividend+1;
        }
        else
        {
            iRowCount = listview->model()->rowCount()/iDividend;

        }

        listview->setFixedSize(m_scrollArea->width(),listview->gridSize().height()*iRowCount);

        iIndex++;
    }
    m_scrollArea->widget()->adjustSize();
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
void StudyDirectoryWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.setRenderHint(QPainter::Antialiasing,true);
    //QPainterPath画圆角矩形
    const qreal radius = 8;
    QPainterPath path;
    QRect rect(0,m_titleLabel->pos().y()+7,8,32);
    path.moveTo(rect.topRight() - QPointF(radius, 0));
    path.lineTo(rect.topLeft());
    path.lineTo(rect.bottomLeft());
    path.lineTo(rect.bottomRight() - QPointF(radius, 0));
    path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
    path.lineTo(rect.topRight() + QPointF(0, radius));
    path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

    painter.setRenderHint(QPainter::Antialiasing);
    painter.begin(this);
    painter.setPen(QPen(Qt::NoPen));
    QColor color(m_strListTitleStyle.at(1).toLocal8Bit().constData());
    painter.setBrush(QBrush(color));

    painter.drawPath(path);

    painter.save();
    painter.restore();

}

