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

#ifndef DESKTOPMANAGER_H
#define DESKTOPMANAGER_H

#include "desktop-widget-base.h"
#include "desktop-factory-i-face.h"

#include <QMutex>
#include <QMap>

namespace Peony {

class DesktopManager : public QObject
{
Q_OBJECT
public:
    /**
     * @brief getInstance
     * 创建一个桌面组件管理器
     */
    static DesktopManager *getInstance(bool enablePreloading = false, QObject *parent = nullptr);

    /**
     * @brief registerPlugin
     * 注册桌面组件工厂
     */
    bool registerPlugin(DesktopFactoryIFace *factory);

    /**
     * @brief 通过桌面类型获取对应的工厂实例
     * @param desktopType
     * @return
     */
    DesktopFactoryIFace *getFactoryByType(DesktopType desktopType);

    /**
     * @brief 通过桌面类型获取一个桌面
     * @param desktopType 桌面类型，枚举值
     * @return
     */
    DesktopWidgetBase *getDesktopByType(DesktopType desktopType, QWidget *parent = nullptr);

    ~DesktopManager() override;

private:
    explicit DesktopManager(bool enablePreloading = false, QObject *parent = nullptr);

private:
    /**
     * @brief 工厂实例
     */
    QMap<DesktopType, DesktopFactoryIFace *> m_pluginMap;

    /**
     * @brief 是否在启动程序时同时创建全部桌面（预加载）
     */
    bool m_enablePreloading = false;

    QMutex m_mutex;

};

}

#endif // DESKTOPMANAGER_H
