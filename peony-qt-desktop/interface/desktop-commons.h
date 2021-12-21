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
    StudyCenter //学习中心模式
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
