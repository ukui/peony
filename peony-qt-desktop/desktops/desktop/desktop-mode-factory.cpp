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
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

//
// Created by hxf on 2021/8/6.
//

#include "desktop-mode-factory.h"
#include "desktop-mode.h"

using namespace Peony;

static DesktopModeFactory *g_desktopModeFactory = nullptr;
static DesktopMode        *g_desktopMode        = nullptr;

DesktopModeFactory *DesktopModeFactory::getInstance(QObject *parent)
{
    if (!g_desktopModeFactory) {
        g_desktopModeFactory = new DesktopModeFactory(parent);
    }

    return g_desktopModeFactory;
}

DesktopModeFactory::DesktopModeFactory(QObject *parent) : QObject(parent)
{

}

DesktopModeFactory::~DesktopModeFactory()
{

}

DesktopType DesktopModeFactory::getDesktopType()
{
    return DesktopType::Desktop;
}

QString DesktopModeFactory::getFactoryName()
{
    return QString(tr("desktop icon mode"));
}

bool DesktopModeFactory::isEnable()
{
    //桌面模式默认为启用状态
    return true;
}

DesktopWidgetBase *DesktopModeFactory::createDesktop(QWidget *parent)
{
    if (!g_desktopMode) {
        g_desktopMode = new DesktopMode(parent);
        g_desktopMode->setDesktopType(DesktopType::Desktop);
    }

    return g_desktopMode;
}

DesktopWidgetBase *DesktopModeFactory::createNewDesktop(QWidget *parent)
{
    return new DesktopMode(parent);
}

bool DesktopModeFactory::closeFactory()
{
    if (g_desktopMode) {
        g_desktopMode->deleteLater();
    }

    if (g_desktopModeFactory) {
        g_desktopModeFactory->deleteLater();
    }

    return true;
}


