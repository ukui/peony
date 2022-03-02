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
// Created by hxf on 2021/8/10.
//

#include "tablet-mode-factory.h"
#include "tablet-mode.h"
#include "qml-desktop.h"
#include "desktop-global-settings.h"

using namespace Peony;

static TabletModeFactory *g_tabletModeFactory = nullptr;
static DesktopWidgetBase *g_tabletDesktop     = nullptr;


TabletModeFactory *TabletModeFactory::getInstance(QObject *parent)
{
    if (!g_tabletModeFactory) {
        g_tabletModeFactory = new TabletModeFactory(parent);
    }

    return g_tabletModeFactory;
}

Peony::TabletModeFactory::~TabletModeFactory()
{

}

Peony::DesktopType Peony::TabletModeFactory::getDesktopType()
{
    return DesktopType::Tablet;
}

QString Peony::TabletModeFactory::getFactoryName()
{
    return QString("平板模式");
}

bool Peony::TabletModeFactory::isEnable()
{
    return true;
}

Peony::DesktopWidgetBase *Peony::TabletModeFactory::createDesktop(QWidget *parent)
{
    if (!g_tabletDesktop) {
        if (DesktopGlobalSettings::globalInstance()->getCurrentProjectName() == V10_SP1_EDU) {
            //edu系统加载qt平板桌面
            g_tabletDesktop = new TabletMode(parent);
        } else {
            //非ude加载qml平板桌面
            g_tabletDesktop = new QmlDesktop(parent);
        }

        g_tabletDesktop->setDesktopType(DesktopType::Tablet);
    }

    return g_tabletDesktop;
}

Peony::DesktopWidgetBase *Peony::TabletModeFactory::createNewDesktop(QWidget *parent)
{
    return nullptr;
}

bool Peony::TabletModeFactory::closeFactory()
{
    if (g_tabletDesktop) {
        g_tabletDesktop->deleteLater();
    }

    if (g_tabletModeFactory) {
        g_tabletModeFactory->deleteLater();
    }
    return false;
}

TabletModeFactory::TabletModeFactory(QObject *parent) : QObject(parent)
{

}
