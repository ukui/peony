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

#include "pushbutton.h"
#include <QFont>
#include <QDebug>

TitleWidget::TitleWidget(QWidget *parent, QString name, int width, int height):
    QWidget(parent)
{
    qDebug()<<"PushButton::PushButton name:"<<name;
    this->name=name;
    this->width=width;
    this->height=height;
    initAppBtn();
}

TitleWidget::~TitleWidget()
{
}

void TitleWidget::initAppBtn()
{

    this->setFixedSize(this->width,this->height);
    this->setFocusPolicy(Qt::NoFocus);
    QHBoxLayout* layout=new QHBoxLayout(this);
   // layout->setSpacing(6);

    QFont font;
    font.setBold(true);
    QLabel* textlabel=new QLabel(this);
    textlabel->setAttribute(Qt::WA_TranslucentBackground);
    textlabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    textlabel->setStyleSheet("QLabel{color: palette(text);font-size:24px}");
    textlabel->setFont(font);
    qDebug()<<"PushButton::initAppBtn name:"<<name;
    textlabel->setText(tr(name.toLocal8Bit().data()));
    textlabel->adjustSize();

    QFrame* line=new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);


    layout->setSizeConstraint(QLayout::SetMaximumSize);
    layout->addWidget(textlabel);
    layout->addWidget(line);

    layout->setStretch(0, 0);
    layout->setStretch(1, 1);

    layout->setSpacing(28);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);
    connect(this, &TitleWidget::changeTheme,  [=](QString strTheme)
    {
        if (strTheme == "ukui-dark")
        {
            //深色主题
           line->setStyleSheet("background-color:rgba(255, 255, 255, 0.05)");
        }
        else
        {
            //浅色主题
           line->setStyleSheet("background-color:rgba(38, 38, 40, 0.05)");
        }
    });
}
