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
 * Authors: Wenfei He <lanyue@kylinos.cn>
 *
 */

//
// Created by hxf on 2021/8/17.
//

#include "desktop-global-settings.h"

#ifdef KYLIN_COMMON
#include <ukuisdk/kylin-com4cxx.h>
#endif

#include <QDebug>

using namespace Peony;

static DesktopGlobalSettings *g_desktopGlobalSettings = nullptr;

const QString DesktopGlobalSettings::V10SP1           = V10_SP1;
const QString DesktopGlobalSettings::V10SP1Edu        = V10_SP1_EDU;

DesktopGlobalSettings *DesktopGlobalSettings::globalInstance(QObject *parent)
{
    if (!g_desktopGlobalSettings) {
        g_desktopGlobalSettings = new DesktopGlobalSettings(parent);
    }
    return g_desktopGlobalSettings;
}

DesktopGlobalSettings::DesktopGlobalSettings(QObject *parent) : QObject(parent)
{

}

const QString &DesktopGlobalSettings::getCurrentProjectName()
{
#ifdef KYLIN_COMMON
    QString platFromName = QString::fromStdString(KDKGetPrjCodeName());
    qDebug() << "[DesktopGlobalSettings::getCurrentProjectName]" << platFromName;
    if (QString::compare(V10_SP1, platFromName, Qt::CaseInsensitive) == 0) {
        return V10SP1;
    }

    if (QString::compare(V10_SP1_EDU, platFromName, Qt::CaseInsensitive) == 0) {
        return V10SP1Edu;
    }

    return V10SP1;
#else
    return V10SP1;
#endif
}
