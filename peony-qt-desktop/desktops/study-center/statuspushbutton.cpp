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

#include "statuspushbutton.h"
#include <QDebug>
#include <QPainter>
#include <QString>

StatusPushButton::StatusPushButton(QWidget *parent)
{
    disc = QPixmap(":/img/refresh.svg");
    this->setFixedSize(disc.width(),disc.height());
}

StatusPushButton::~StatusPushButton()
{

}

void StatusPushButton::paintEvent(QPaintEvent *)
{
    if(m_val == 360){
        m_val = 0;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    //QPixmap disc("/home/user_0ec802/refresh.svg");
    qDebug() << "m_val" << m_val;
    painter.save();
    painter.translate(disc.width()/2,disc.height()/2);
    /* 旋转的角度 */
    painter.rotate(m_val);
    /* 恢复中心点 */
    painter.translate(-disc.width()/2,-disc.height()/2);
    /* 画图操作 */
    painter.drawPixmap(0,0,disc.width(),disc.height(), disc);
    painter.restore();
}

void StatusPushButton::setValue(double value)
{
    m_val = value;
}

void StatusPushButton::changeTheme(QString strTheme)
{
    if (strTheme == "ukui-dark")
    {
        disc.load(":/img/white-refresh.svg");
    }
    else
    {
        disc.load(":/img/refresh.svg");
    }
}
