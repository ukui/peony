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

#include "side-bar.h"

using namespace Peony;

SideBar::SideBar(QWidget *parent) : QDockWidget(parent)
{
    setFeatures(QDockWidget::NoDockWidgetFeatures);
//    auto palette = palette();
//    palette.setColor(QPalette::Window, Qt::transparent);
//    setPalette(palette);
    setTitleBarWidget(new QWidget(this));
    titleBarWidget()->setFixedHeight(0);
    setAttribute(Qt::WA_TranslucentBackground);
    setContentsMargins(0, 0, 0, 0);
}

SideBar::~SideBar()
{

}

QSize SideBar::sizeHint() const
{
    auto size = QWidget::sizeHint();
    size.setWidth(180);
    return size;
}

FMWindowIface *SideBar::getWindowIface()
{
    return dynamic_cast<FMWindowIface *>(topLevelWidget());
}
