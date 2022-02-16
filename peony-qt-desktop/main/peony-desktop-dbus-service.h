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

#ifndef PEONY_PEONY_DESKTOP_DBUS_SERVICE_H
#define PEONY_PEONY_DESKTOP_DBUS_SERVICE_H

#include <QObject>

namespace Peony {

class PeonyDesktopDbusService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.peonyQtDesktop")

public:
    PeonyDesktopDbusService(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * @brief 窗管未实现小窗模糊背景功能，由桌面暂时代替实现。
     * @param status
     * @return
     */
    quint32 blurBackground(quint32 status);

Q_SIGNALS:
    void blurBackGroundSignal(quint32 status);
};

}

#endif //PEONY_PEONY_DESKTOP_DBUS_SERVICE_H
