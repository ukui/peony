/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "icon-container.h"
#include <QPaintEvent>
#include <QPainter>

#include <QStyleOptionButton>

using namespace Peony;

IconContainer::IconContainer(QWidget *parent) : QPushButton(parent)
{
    setEnabled(true);
    setCheckable(false);
    setDefault(true);
    setFlat(true);

    m_style = new IconContainerStyle;
    setStyle(m_style);
}

IconContainer::~IconContainer()
{
    m_style->deleteLater();
}

void IconContainer::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.fillRect(this->rect(), this->palette().base());
    p.setPen(this->palette().dark().color());
    //p.drawRect(this->rect().adjusted(0, 0, -1, -1));
    QPushButton::paintEvent(e);
}

IconContainerStyle::IconContainerStyle() : QProxyStyle()
{

}

void IconContainerStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    QStyleOptionButton opt = *qstyleoption_cast<const QStyleOptionButton *>(option);
    opt.palette.setColor(QPalette::Highlight, Qt::transparent);
    return QProxyStyle::drawControl(element, &opt, painter, widget);
}
