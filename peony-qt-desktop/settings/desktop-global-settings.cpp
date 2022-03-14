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
#include <QGSettings>

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
    initDesktopSetting();
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

int DesktopGlobalSettings::getProductFeatures()
{
    QString features = QString::fromStdString(KDKGetOSRelease("PRODUCT_FEATURES"));
    qDebug() << "[DesktopGlobalSettings::getProductFeatures]" << features;
    if (!features.isEmpty()) {
        bool isOk = false;
        int tmp = features.toInt(&isOk);
        if (isOk) {
            return tmp;
        }
    }

    return 1;
}

bool DesktopGlobalSettings::allowSwitchDesktop()
{
    //edu系统允许切换
    if (getCurrentProjectName() == V10_SP1_EDU) {
        return true;
    }

    //同时支持平板和pc特性的系统允许切换
    if (getProductFeatures() == 3) {
        return true;
    }

    return false;
}

void DesktopGlobalSettings::initDesktopSetting()
{
    //设置默认值
    setValue(DESKTOP_ANIMATION_DURATION, 1000);
    setValue(ENABLE_SMALL_PLUGIN, true);
    setValue(DESKTOP_ITEM_ZOOM_RANGE, 0.1);

    if (QGSettings::isSchemaInstalled(DESKTOP_SETTINGS_SCHEMA)) {
        QGSettings *settings = new QGSettings(DESKTOP_SETTINGS_SCHEMA, "/", this);
        connect(settings, &QGSettings::changed, this, [=](const QString &key) {
            setValue(key, settings->get(key));
        });

        if (settings->keys().contains(DESKTOP_ANIMATION_DURATION)) {
            setValue(DESKTOP_ANIMATION_DURATION, settings->get(DESKTOP_ANIMATION_DURATION));
        }
        if (settings->keys().contains(ENABLE_SMALL_PLUGIN)) {
            setValue(ENABLE_SMALL_PLUGIN, settings->get(ENABLE_SMALL_PLUGIN));
        }
        if (settings->keys().contains(DESKTOP_ITEM_ZOOM_RANGE)) {
            setValue(DESKTOP_ITEM_ZOOM_RANGE, settings->get(DESKTOP_ITEM_ZOOM_RANGE));
        }
    }
}

void DesktopGlobalSettings::setValue(const QString &key, const QVariant &value)
{
    m_cache.remove(key);
    m_cache.insert(key, value);

    Q_EMIT valueChanged(key, value);
}

QVariant DesktopGlobalSettings::getValue(const QString &key) const
{
    if (m_cache.contains(key)) {
        return m_cache.value(key);
    }

    return {};
}
