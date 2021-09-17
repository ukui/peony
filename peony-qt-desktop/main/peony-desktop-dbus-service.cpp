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
