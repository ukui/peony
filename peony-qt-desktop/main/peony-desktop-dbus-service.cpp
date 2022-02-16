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

#include <QDebug>
#include <QDBusConnection>
#include <QDBusConnectionInterface>


using namespace Peony;

PeonyDesktopDbusService::PeonyDesktopDbusService(QObject *parent) : QObject(parent)
{
    QDBusConnection::sessionBus().unregisterService("org.ukui.peony.desktop.service");
    QDBusConnection::sessionBus().registerService("org.ukui.peony.desktop.service");
    QDBusConnection::sessionBus().registerObject("/org/ukui/peonyQtDesktop", this, QDBusConnection::ExportAllSlots);
}

quint32 PeonyDesktopDbusService::blurBackground(quint32 status)
{
    switch (status) {
        case 0:
            //取消模糊背景
            Q_EMIT blurBackGroundSignal(0);
            return 0;
        case 1:
            //激活模糊背景
            Q_EMIT blurBackGroundSignal(1);
            return 1;
        default:
            return 404;
    }
}
