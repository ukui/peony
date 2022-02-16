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

#include "toolbox.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QtSvg/QSvgRenderer>
#include <QProcess>
#include "src/Interface/currenttimeinterface.h"
#include "src/Style/style.h"
#define TIME_FORMAT "org.ukui.control-center.panel.plugins"

ToolBox::ToolBox(QWidget *parent,int w,int h) : QWidget(parent)
{
    m_width=w;
    m_height=h;
    if(QGSettings::isSchemaInstalled("org.ukui.style")){
        themeSetting=new QGSettings("org.ukui.style");
        themeName=themeSetting->get("style-name").toString();
    }
    connect(themeSetting,&QGSettings::changed,this,[=](){
         changeToolBoxBackground();
    });
    initUi();
    if(QGSettings::isSchemaInstalled("org.ukui.menu.plugin")){
        pluginSetting=new QGSettings("org.ukui.menu.plugin");
        isFocusmodeRun=pluginSetting->get("focusmode").toBool();
        drawfocusStatus();
    }
    connect(pluginSetting,&QGSettings::changed,this,[=](){
        isFocusmodeRun=pluginSetting->get("focusmode").toBool();
         drawfocusStatus();
    });
    Time=new CurrentTimeInterface();
    myTimer=new QTimer();
    myTimer->start(10000);
    connect(myTimer,&QTimer::timeout,[this] () {
        hourLabel->setText(Time->currentTime.split(":").first());
        minuteLabel->setText(Time->currentTime.split(":").last());
    });
    timeSetting=new QGSettings(TIME_FORMAT);
    connect(timeSetting,&QGSettings::changed,this, [=](const QString &key)
    {
         if(key=="hoursystem")
         {
             hourLabel->setText(Time->currentTime.split(":").first());
             minuteLabel->setText(Time->currentTime.split(":").last());
         }
    });

}

ToolBox::~ToolBox()
{
    if(pluginSetting)
        delete pluginSetting;
    if(Time)
        delete Time;
    if(myTimer)
        delete myTimer;
    if(themeSetting)
        delete themeSetting;
    if(timeSetting)
        delete timeSetting;
    if(timeWidget)
        delete timeWidget;
    if(timeLayout)
        delete timeLayout;
    if(hourLabel)
        delete hourLabel;
    if(minuteLabel)
        delete minuteLabel;
    if(toolWidget)
        delete toolWidget;
    if(toolLayout)
        delete toolLayout;
    if(searchBtn)
        delete searchBtn;
    if(focusBoxLayout)
        delete focusBoxLayout;
    if(focusBtn)
        delete focusBtn;
    if(focusStatus)
        delete focusStatus;
    if(noteBtn)
        delete noteBtn;
    if(effect)
        delete effect;
    pluginSetting=nullptr;
    Time=nullptr;
    myTimer=nullptr;
    themeSetting=nullptr;
    timeSetting=nullptr;
    timeWidget=nullptr;
    timeLayout=nullptr;
    hourLabel=nullptr;
    minuteLabel=nullptr;
    toolWidget=nullptr;
    toolLayout=nullptr;
    searchBtn=nullptr;
    focusBoxLayout=nullptr;
    focusBtn=nullptr;
    focusStatus=nullptr;
    noteBtn=nullptr;
    effect=nullptr;


}

void ToolBox::initUi()
{
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_StyledBackground,true);

    this->setFixedSize(m_width,m_height);
    this->setStyleSheet("border:0px solid #ff0000;background:transparent;");
    this->setFocusPolicy(Qt::NoFocus);
    QVBoxLayout* mainLayout=new QVBoxLayout;
    mainLayout->setSpacing(23);
    mainLayout->setContentsMargins(Style::ToolWidgetLeft,Style::TimeWidgetTop,0,0);
    this->setLayout(mainLayout);

    //时间+工具箱
    timeWidget=new QWidget();
    timeWidget->setFixedSize(80,168);
    timeLayout=new QVBoxLayout();
    timeWidget->setLayout(timeLayout);
    timeLayout->setContentsMargins(0,0,0,0);
    timeLayout->setSpacing(8);
    hourLabel=new QLabel();
    hourLabel->setFixedSize(80,80);
    minuteLabel=new QLabel();
    minuteLabel->setFixedSize(80,80);
    timeLayout->addWidget(hourLabel);
    timeLayout->addWidget(minuteLabel);

    Time=new CurrentTimeInterface;
    hourLabel->setText(Time->currentTime.split(":").first());
    minuteLabel->setText(Time->currentTime.split(":").last());

    hourLabel->setStyleSheet("border:0px;background:transparent;font-size:72px;color:white;");
    minuteLabel->setStyleSheet("border:0px;background:transparent;font-size:72px;color:white;");

    toolWidget=new QWidget();
    toolWidget->setFixedSize(80,240);
    toolLayout=new QVBoxLayout();
    toolWidget->setLayout(toolLayout);
    changeToolBoxBackground();
    toolLayout->setContentsMargins(0,0,24,0);

    searchBtn= new QPushButton();
    searchBtn->setFocusPolicy(Qt::NoFocus);
    searchBtn->setFixedSize(80,32);
    searchBtn->setStyleSheet("background:transparent;icon-size:32px;");
    searchBtn->setIcon(QIcon(":/img/ukui-search.svg"));
    focusBoxLayout=new QHBoxLayout();
    focusBoxLayout->setAlignment(Qt::AlignLeft);
    focusBoxLayout->setSpacing(16);
    focusBoxLayout->setContentsMargins(0,0,24,0);
    focusBtn= new QPushButton();
    focusBtn->setFocusPolicy(Qt::NoFocus);
    focusBtn->setFixedSize(80,32);
    focusBtn->setLayout(focusBoxLayout);
    focusBtn->setStyleSheet("background:transparent;icon-size:32px;");
    focusBtn->setIcon(QIcon(":/img/ukui-focus.png"));
    focusStatus=new QLabel();
    focusStatus->setFixedSize(8,8);
    focusBoxLayout->addWidget(focusStatus);
    noteBtn=new QPushButton();
    noteBtn->setFocusPolicy(Qt::NoFocus);
    noteBtn->setFixedSize(80,32);
    noteBtn->setStyleSheet("background:transparent;icon-size:32px;");
    noteBtn->setIcon(QIcon(":/img/note-select.png"));

    connect(searchBtn, &QPushButton::clicked, this, &ToolBox::openSearchBox);
    connect(focusBtn, &QPushButton::clicked, this, &ToolBox::openFocusMode);
    connect(noteBtn,&QPushButton::clicked,this,&ToolBox::openNote);

    toolLayout->addWidget(searchBtn);
    toolLayout->addWidget(focusBtn);
    toolLayout->addWidget(noteBtn);

    mainLayout->addWidget(timeWidget);
    mainLayout->addWidget(toolWidget);
    mainLayout->addStretch();

    effect = new QGraphicsDropShadowEffect(this);
    effect->setXOffset(0);
    effect->setYOffset(0);
    effect->setBlurRadius(8);
    effect->setColor(QColor(38, 38, 38, 100));
    timeWidget->setGraphicsEffect(effect);

}

void ToolBox::setToolOpacityEffect(const qreal& num)
{
//    QGraphicsOpacityEffect *m_effect = new QGraphicsOpacityEffect;
//    m_effect->setOpacity(num);
//    toolWidget->setGraphicsEffect(m_effect); //全局搜索框透明度
    opacity = num;
    changeToolBoxBackground();
}

void ToolBox::openSearchBox()
{
    QProcess p;
    p.setProgram(QString("ukui-search"));
    p.setArguments(QStringList());
    p.startDetached(p.program(),p.arguments());
    p.waitForFinished(-1);

}
void ToolBox::openFocusMode()
{
    type=1;
    Q_EMIT pageSpread();

}
void ToolBox::openNote()
{
    type=2;
    Q_EMIT pageSpread();
}
void ToolBox::drawfocusStatus()
{
    if(isFocusmodeRun)
        focusStatus->setStyleSheet("background:#2FB3E8;");
    else
        focusStatus->setStyleSheet("background:transparent;");
}

void ToolBox::changeToolBoxBackground()
{
    QString darkStyleSheet = QString("border-radius:40px;background:rgba(44,50,57,%1);").arg(opacity);
    QString lightStyleSheet = QString("border-radius:40px;background:rgba(255,255,255,%1);").arg(opacity);
    themeName=themeSetting->get("style-name").toString();
    if(themeName=="ukui-dark")
    {
        toolWidget->setStyleSheet(darkStyleSheet);

    }
    else
    {
        toolWidget->setStyleSheet(lightStyleSheet);

    }
}
