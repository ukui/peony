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

#ifndef PEONY_DESKTOP_GLOBAL_SETTINGS_H
#define PEONY_DESKTOP_GLOBAL_SETTINGS_H
//dbus
#define DBUS_STATUS_MANAGER_IF "com.kylin.statusmanager.interface"
//schemas
#define TABLED_ROTATION_SCHEMA "org.ukui.SettingsDaemon.plugins.xrandr"
#define TABLET_SCHEMA "org.ukui.SettingsDaemon.plugins.tablet-mode"

#define DESKTOP_SETTINGS_SCHEMA      "org.ukui.peony.desktop.settings"

//keys
#define TABLET_MODE "tabletMode"
#define DESKTOP_ANIMATION_DURATION   "desktopAnimationDuration"
#define DESKTOP_ITEM_ZOOM_RANGE      "desktopItemZoomRange"
#define ENABLE_SMALL_PLUGIN          "enableSmallPlugin"

#include <QObject>
#include <QMap>
#include <QVariant>

namespace Peony {

class DesktopGlobalSettings : public QObject
{
    Q_OBJECT
private:
    static const QString V10SP1;
    static const QString V10SP1Edu;

public:
    static DesktopGlobalSettings *globalInstance(QObject *parent = nullptr);

    const QString &getCurrentProjectName();

    bool allowSwitchDesktop();

    /**
     * 用于在非 edu系统上判断是否支持平板特性<br/>
     * 1是只支持pc特性，<br/>
     * 2是只支持平板特性，<br/>
     * 3是平板和pc都支持<br/>
     */
    int getProductFeatures();

    QVariant getValue(const QString &key) const;
    void setValue(const QString &key, const QVariant &value);

Q_SIGNALS:
    void valueChanged(const QString &key, const QVariant &value);

private:
    explicit DesktopGlobalSettings(QObject *parent = nullptr);
    void initDesktopSetting();

private:
    QMap<QString, QVariant> m_cache;
};

}

#endif //PEONY_DESKTOP_GLOBAL_SETTINGS_H
