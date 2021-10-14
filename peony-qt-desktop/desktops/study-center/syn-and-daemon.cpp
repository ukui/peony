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

#include "syn-and-daemon.h"
#include <QDebug>
#include <QPainter>
#include "../../tablet/data/tablet-app-manager.h"
#include "study-list-view.h"
#include "common.h"
#include "../../tablet/src/Style/style.h"

using namespace Peony;

SynAndDaemon::SynAndDaemon(QStringList &strListTitleStyle, QList<QPair<QString, QList<TabletAppEntity*>>> &subtitleList, QWidget *parent)
: QWidget(parent)
{
    m_studyCenterDataList = subtitleList;
    m_strListTitleStyle = strListTitleStyle;
    initWidget(strListTitleStyle);
}

SynAndDaemon::~SynAndDaemon()
{
    if(nullptr != m_titleLabel)
    {
        delete m_titleLabel;
        m_titleLabel=nullptr;
    }
    if(nullptr != m_mainLayout)
    {
        delete m_mainLayout;
        m_mainLayout=nullptr;
    }
}

void SynAndDaemon::initWidget(QStringList &strListTitleStyle)
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
        backGroundLabel->setGeometry(48,m_titleLabel->pos().y()-3,500,110);
        backGroundLabel->lower();

        m_mainLayout->addSpacing(25);
    }

    m_mainLayout->setContentsMargins(48,33,8,0);
    initAppListWidget();
    this->setLayout(m_mainLayout);
    connect(this, &SynAndDaemon::changeTheme, [=](QString strTheme)
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

void SynAndDaemon::initAppListWidget()
{
    for(int i = 0; i < m_studyCenterDataList.size(); ++i)
    {
        //插入应用列表
        StudyListView* listView = new StudyListView(this);
        listView->setData(m_studyCenterDataList.at(i).second);
        m_mainLayout->addWidget(listView);
        connect(listView,SIGNAL(clicked(QModelIndex)),this,SLOT(execApplication(QModelIndex)));
    }
}
/**
 * 执行应用程序
 */
void SynAndDaemon::execApplication(QModelIndex app)
{
    //  Q_EMIT sendHideMainWindowSignal();
    TABLETAPP tabletApp = app.data(Qt::DisplayRole).value<TABLETAPP>();

    TabletAppEntity appEntity;
    appEntity.execCommand = tabletApp.execCommand;

    TabletAppManager::getInstance()->execApp(&appEntity);
}

void SynAndDaemon::paintEvent(QPaintEvent* event)
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

