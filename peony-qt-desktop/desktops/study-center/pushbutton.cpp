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

#include "pushbutton.h"
#include <QFont>
#include <QDebug>

PushButton::PushButton(QWidget *parent, QString name, int width, int height):
    QPushButton(parent)
{
    qDebug()<<"PushButton::PushButton name:"<<name;
    this->name=name;
    this->width=width;
    this->height=height;
    initAppBtn();
}

PushButton::~PushButton()
{
}

void PushButton::initAppBtn()
{
    char btnstyle[300];
    sprintf(btnstyle,"QPushButton{background:transparent;border:0px;color:#ffffff;font-size:14px;padding-left:0px;text-align: left center;}\
            QPushButton:hover{background-color:rgba(255, 255 ,255, 0.14);}\
            QPushButton:pressed{background-color:rgba(255, 255, 255, 0.14);}");

    this->setFixedSize(this->width,this->height);
    this->setStyleSheet(btnstyle);
    this->setFocusPolicy(Qt::NoFocus);
    QHBoxLayout* layout=new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
   // layout->setSpacing(6);

    char style[100];
    sprintf(style,"color:#000000;");
    QFont font;
    font.setWeight(70);
    QLabel* textlabel=new QLabel(this);
    textlabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    textlabel->setStyleSheet(style);
    textlabel->setFont(font);
    qDebug()<<"PushButton::initAppBtn name:"<<name;
    textlabel->setText(tr(name.toLocal8Bit().data()));
    textlabel->adjustSize();

    QFrame* line=new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background-color:rgba(0, 0, 0, 0.06)");
    line->setFixedSize(this->width-textlabel->width()-15,1);

    layout->addWidget(textlabel);
    layout->addWidget(line);
    this->setLayout(layout);
    this->setEnabled(false);
}
