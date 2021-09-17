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
