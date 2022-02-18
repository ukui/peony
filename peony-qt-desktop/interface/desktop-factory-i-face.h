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

#ifndef DESKTOPFACTORYIFACE_H
#define DESKTOPFACTORYIFACE_H

#include "desktop-widget-base.h"
#include "desktop-commons.h"

namespace Peony {

class DesktopFactoryIFace
{
public:
    virtual ~DesktopFactoryIFace() {}

    /**
     * @brief getDesktopType
     * @return
     * 当前工厂创建的桌面的类型
     */
    virtual DesktopType getDesktopType() = 0;

    /**
     * @brief getFactoryName
     * @return
     * 工厂的名称信息
     */
    virtual QString getFactoryName() = 0;

    /**
     * @brief isEnable
     * @return
     * 当前工厂是否启用
     * 禁用后不加载工厂插件
     */
    virtual bool isEnable() = 0;

    /**
     * @brief
     * @param parent
     * @return
     * 创建并返回一个桌面组件
     * 桌面组件应该是以单例的形式存在与工厂中的
     */
    virtual DesktopWidgetBase *createDesktop(QWidget *parent = nullptr) = 0;

    /**
     * @brief 创建一个新的桌面，不是唯一的单例桌面
     * @param parent
     * @return
     */
    virtual DesktopWidgetBase *createNewDesktop(QWidget *parent = nullptr) = 0;

    /**
     * @brief closeFactory
     * 销毁工厂实例
     */
    virtual bool closeFactory() = 0;

};

}

#endif // DESKTOPFACTORYIFACE_H
