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

#ifndef PEONYDBUSSERVICE_H
#define PEONYDBUSSERVICE_H

#include "desktop-icon-view.h"
#include <QCoreApplication>
#include <QTimer>
#include <QtDBus>
#include <QDebug>
#include <QObject>

/*
名称：org.ukui.peony
对象路径：/org/ukui/peony
接口：org.ukui.peony
方法：GetSecurityConfigPath()//获取安全配置文件存放路径
     ReloadSecurityConfig()// 重新加载安全配置
*/

namespace Peony {

class DesktopIconView;

class PeonyDbusService:public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.ukui.peony")

public:    
    explicit PeonyDbusService(DesktopIconView *view, QObject *parent=nullptr);
    ~PeonyDbusService();

   void DbusServerRegister();

public Q_SLOTS:
    QString GetSecurityConfigPath();
    int ReloadSecurityConfig();

private:
    DesktopIconView *m_desktopIconView = nullptr;
};

}
#endif // PEONYDBUSSERVICE_H
