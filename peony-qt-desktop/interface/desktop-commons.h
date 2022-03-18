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

#ifndef DESKTOPCOMMONS_H
#define DESKTOPCOMMONS_H

namespace Peony {
/**
 * @brief 桌面类型
 * 标识当前桌面的类型
 * 如果需要添加新的桌面请添加对应的 mode
 */
typedef enum {
    Desktop = 0, //桌面模式
    Empty, //仅用于停放图标的空桌面
    Tablet, //平板模式
    StudyCenter, //学习中心模式
    Animation //截图动画容器
} DesktopType;

/**
 * @brief 桌面的动画类型
 * 桌面出现或消失时的动画类型
 */
typedef enum {
    LeftToRight = 0,//左边到右边
    RightToLeft,
    EdgeToCenter,//从边缘收缩到中心
    CenterToEdge,//从中心扩散到边缘
    OpacityFull,//不透明度提高
    OpacityLess,//不透明度减少
} AnimationType;

/**
 * @brief QPropertyAnimation的动画类型名称枚举
 */
typedef enum {
    Pos = 0,
    Geometry,
    WindowOpacity,
} PropertyName;

}

#endif // DESKTOPCOMMONS_H
