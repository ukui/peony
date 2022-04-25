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

#include "study-directory-widget.h"
#include <QDesktopWidget>
#include <QDebug>
#include <iterator>
#include <QPainter>
#include "../../tablet/data/tablet-app-manager.h"
#include "study-list-view.h"
#include "common.h"
#include "../../tablet/src/Style/style.h"
#include "desktop-background-manager.h"

#include <QPainterPath>

using namespace Peony;

//qt's global function
extern void qt_blurImage(QImage &blurImage, qreal radius, bool quality, int transposed);

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
        backGroundLabel->setGeometry(48,m_titleLabel->pos().y()-3,500,110);
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
    //bug#106918 滚动区域填满精准练习
    scrollAreaWid->adjustSize();
    m_scrollArea->setWidget(scrollAreaWid);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_scrollAreaWidLayout = new QVBoxLayout;
    m_scrollAreaWidLayout->setContentsMargins(0,0,0,0);
    m_scrollAreaWidLayout->setSpacing(0);
    scrollAreaWid->setLayout(m_scrollAreaWidLayout);
    m_mainLayout->addWidget(m_scrollArea);
    m_mainLayout->setContentsMargins(48,33,8,0);

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
           m_colorMask = QColor(38,38,40);
        }
        else
        {
            //浅色主题
           this->setStyleSheet(styleSheetLight);
            m_colorMask = QColor(255,255,255);
        }
    });

    resizeScrollAreaControls();
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
            m_titleMap.insert(strTitle,titleWid);
            m_scrollAreaWidLayout->addWidget(titleWid);
            connect(this, SIGNAL(changeTheme( QString)), titleWid, SIGNAL(changeTheme( QString)));
            if(m_studyCenterDataList.at(i).second.size() < 1)
            {
                titleWid->hide();
            }
        }

        //插入应用列表
        StudyListView* listView = new StudyListView(this);
        listView->setData(m_studyCenterDataList.at(i).second);
        m_viewMap.insert(m_studyCenterDataList.at(i).first, listView);
        m_scrollAreaWidLayout->addWidget(listView);
        connect(listView,SIGNAL(clicked(QModelIndex)),this,SLOT(execApplication(QModelIndex)));
    }
    m_scrollAreaWidLayout->addStretch();
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
 * 设置scrollarea所填充控件大小
 */
void StudyDirectoryWidget::resizeScrollAreaControls()
{
    for (int i = 0; i < m_studyCenterDataList.size(); ++i)
    {
        StudyListView* listview = m_viewMap.value(m_studyCenterDataList.at(i).first);

        if(nullptr == listview)
        {
            continue;
        }
        listview->adjustSize();
        int iDividend = (m_scrollArea->width()-16)/Style::GridWidth;
        qDebug()<<"标题: " <<m_studyCenterDataList.at(i).first <<" 每行应用个数:"<<iDividend;
        int iRowCount = 0;
        if(listview->model()->rowCount()%iDividend>0)
        {
            iRowCount=listview->model()->rowCount()/iDividend+1;
        }
        else
        {
            iRowCount=listview->model()->rowCount()/iDividend;

        }

        listview->setFixedHeight(listview->gridSize().height()*iRowCount);
    }

    m_scrollArea->widget()->adjustSize();
}

void StudyDirectoryWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    auto manager = DesktopBackgroundManager::globalInstance();
    QSize size = this->size();
    QPoint point = this->pos();
    QRect widgetRect = this->rect();

    //把需要模糊的图片区域放大，再模糊处理
    QImage img = manager->getBlurImage();
    //img = img.copy(point.x(),point.y(),size.width(),size.height());
    QRect source(point.x(),point.y(),size.width(),size.height());

    QPainterPath roundPath;
    roundPath.addRoundedRect(widgetRect,24,24);
    painter.setClipPath(roundPath);
    //painter.drawImage(0,0,img);
    painter.drawImage(widgetRect,img,source);

//    auto colorMask = QColor(255,255,255);
    m_colorMask.setAlphaF(0.85);
    painter.fillRect(widgetRect, m_colorMask);
    painter.restore();

    painter.save();
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

    painter.setPen(QPen(Qt::NoPen));
    QColor color(m_strListTitleStyle.at(1).toLocal8Bit().constData());
    painter.setBrush(QBrush(color));

    painter.drawPath(path);

    painter.restore();
}


/**
 * 更新应用列表
 */
void StudyDirectoryWidget::updateAppData(QList<QPair<QString, QList<TabletAppEntity*>>> &subAppMap)
{
    m_studyCenterDataList.clear();
    m_studyCenterDataList = subAppMap;

    for (int i = 0; i < m_studyCenterDataList.size(); ++i) {
        m_viewMap.value(m_studyCenterDataList.at(i).first)->setData(m_studyCenterDataList.at(i).second);
        TitleWidget* titleWid =  m_titleMap.value(m_studyCenterDataList.at(i).first);
        if(nullptr != titleWid)
        {
            if(m_studyCenterDataList.at(i).second.size() < 1 )
            {
                titleWid->hide();
            }
            else
            {
                titleWid->show();
            }
        }
        else
        {
             qDebug()<<"get title fail";
        }

    }

    resizeScrollAreaControls();
}
