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
// Created by hxf on 2021/8/25.
//

#include "peony-desktop-dbus-service.h"
#include "window-manager.h"

#include <QDebug>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QApplication>


using namespace Peony;

PeonyDesktopDbusService::PeonyDesktopDbusService(QObject *parent) : QObject(parent)
{
    QDBusConnection::sessionBus().unregisterService("org.ukui.peony.desktop.service");
    QDBusConnection::sessionBus().registerService("org.ukui.peony.desktop.service");
    QDBusConnection::sessionBus().registerObject("/org/ukui/peonyQtDesktop", this, QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);
}

int PeonyDesktopDbusService::getCurrentDesktopType()
{
    auto window = WindowManager::getInstance()->getWindowByScreen(QApplication::primaryScreen());
    if (window) {
        return window->getCurrentDesktop()->getDesktopType();
    }
    return 0;
}
