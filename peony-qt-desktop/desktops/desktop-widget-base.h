#ifndef DESKTOPWIDGETBASE_H
#define DESKTOPWIDGETBASE_H

#include "desktop-commons.h"

#include <QWidget>

namespace Peony {

/**
 * \brief 桌面插件抽象基类
 */
class DesktopWidgetBase : public QWidget
{
Q_OBJECT
public:
    explicit DesktopWidgetBase(QWidget *parent = nullptr) : QWidget(parent) {
//        setAttribute(Qt::WA_TranslucentBackground, true);
        setWindowFlags(Qt::FramelessWindowHint);
    }

    ~DesktopWidgetBase()
    {}

    /**
     * \brief 当前组件是否激活
     * \return 返回true表示组件已经被激活（当前桌面），被激活后不允许再激活
     */
    bool isActivated()
    {
        return m_isActivated;
    }

    /**
     * \brief 当前组件是否激活
     * \return 返回true表示组件已经被激活（当前桌面），被激活后不允许再激活
     */
    virtual void setActivated(bool activated)
    {
        this->setHidden(!activated);
        m_isActivated = activated;
    }

    /**
     * \brief 当前桌面的退出动画类型
     * \return
     */
    AnimationType getExitAnimationType()
    {
        return m_exitAnimationType;
    }

    /**
     * @brief initDesktop
     * @return
     * 初始化桌面,对桌面的坐标，大小等属性进行设置,
     * 在对应组件的构造方法中，只对界面进行初始化，显示一些没有数据的布局等。
     * 在组件创建完成后，调用initDesktop方法加载数据。
     */
    virtual DesktopWidgetBase *initDesktop(const QRect &rect) {
        this->setGeometry(rect);
        return this;
    };

protected:

    /**
     * \brief 当前桌面是否激活,已经激活的桌面不允许再激活
     */
    bool m_isActivated = false;

    /**
     * \brief 当前桌面排序索引
     */
    //quint32 m_index = 0;

    /**
     * \brief 系统当前的桌面模式，平板模式或者桌面模式
     * 可以在打开开始菜单时判断是否展示学习中心，
     * 根据当前系统的模式判断是否启用滑动切换桌面。
     */
    DesktopType m_systemDesktopType = DesktopType::Desktop;

    /**
     * \brief 当前桌面的退出动画，根据系统的桌面模式设置
     */
    AnimationType m_exitAnimationType = AnimationType::LeftToRight;

public Q_SLOTS:

    /**
     * \brief 设置当前的系统桌面模式
     * \param systemDesktopMode 系统模式枚举变量
     * 根据系统的桌面模式显示部分组件
     */
    void setSystemDesktopMode(DesktopType systemDesktopType)
    {
        //（系统桌面模式没有学习中心模式）
        if (systemDesktopType == DesktopType::StudyCenter) {
            this->m_systemDesktopType = DesktopType::Desktop;
        } else {
            this->m_systemDesktopType = systemDesktopType;
        }
    }

Q_SIGNALS:

    /**
     * \brief 请求切换到别的桌面
     * \param targetType 目标桌面类型
     * \param targetAnimation 目标切换动画类型
     */
    void moveToOtherDesktop(DesktopType targetType, AnimationType targetAnimation);

    /**
     * \brief 打开设置背景界面
     */
    void gotoSetBackground();

};

}
#endif // DESKTOPWIDGETBASE_H
