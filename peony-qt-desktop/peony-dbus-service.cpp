/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
 * Authors: renpeijia <renpeijia@kylinos.cn>
 *
 */

#include "desktop-icon-view.h"
#include "peony-dbus-service.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>

#include <QDebug>
using namespace Peony;

PeonyDbusService::PeonyDbusService(DesktopIconView *view, QObject *parent) : QObject(parent)
{
    m_desktopIconView = view;
}

PeonyDbusService::~PeonyDbusService()
{
    m_desktopIconView = nullptr;
}

void PeonyDbusService::DbusServerRegister()
{
    QDBusConnection::sessionBus().unregisterService("org.ukui.peony");
    QDBusConnection::sessionBus().registerService("org.ukui.peony");
    QDBusConnection::sessionBus().registerObject("/org/ukui/peony", this, QDBusConnection :: ExportAllSlots | QDBusConnection::ExportAllSignals);
}

QString PeonyDbusService::GetSecurityConfigPath()
{
    QString jsonPath=QDir::homePath()+"/.config/peony-security-config.json";

    return jsonPath;
}

int PeonyDbusService::ReloadSecurityConfig()
{
    m_desktopIconView->updateBWList();

    return 0;
}

void PeonyDbusService::receiveSrcAndDestUrisOfCopy(const QStringList& sourceUris, const QStringList& destUris)
{
    Q_EMIT sendSrcAndDestUrisOfCopyDspsFiles(sourceUris, destUris);
}
