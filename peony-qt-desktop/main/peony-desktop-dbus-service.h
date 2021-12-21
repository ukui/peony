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
    explicit PeonyDesktopDbusService(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * @brief 获取当前桌面的类型 id。
     * @return 桌面的类型id
     */
    int getCurrentDesktopType();

Q_SIGNALS:
    /**
    * @param desktopType {
         0: 普通PC桌面
         1: 空白桌面
         2: 平板桌面
         3: 学习专区
     }
    */
    void desktopChangedSignal(int desktopType);
};

}

#endif //PEONY_PEONY_DESKTOP_DBUS_SERVICE_H
