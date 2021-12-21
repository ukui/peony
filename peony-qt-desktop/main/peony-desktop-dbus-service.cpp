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
