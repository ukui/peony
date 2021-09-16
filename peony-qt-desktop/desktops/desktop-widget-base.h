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
    explicit DesktopWidgetBase(QWidget *parent = nullptr) : QWidget(parent)
    {
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
    virtual DesktopWidgetBase *initDesktop(const QRect &rect)
    {
        this->setGeometry(rect);
        return this;
    };

    /**
     * @brief 返回该模式下封装的更下一级desktop
     * @return
     */
    virtual QWidget *getRealDesktop()
    {
        return this;
    };

    DesktopType getDesktopType() const
    {
        return m_currentDesktopType;
    }

    void setDesktopType(DesktopType desktopType)
    {
        m_currentDesktopType = desktopType;
    }

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
     * \brief 当前的桌面模式，平板模式或者桌面模式
     */
    DesktopType m_currentDesktopType = DesktopType::Desktop;

    /**
     * \brief 当前桌面的退出动画，根据系统的桌面模式设置
     */
    AnimationType m_exitAnimationType = AnimationType::LeftToRight;

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

    /**
     * @brief 请求移动当前桌面，发出信号后，由接收者进行移动
     * @param animationType
     * @param moveLength
     * @param duration 如果不需要动画，则设为0
     */
    void desktopMoveRequest(AnimationType animationType, quint32 moveLength, quint32 duration);

    /**
     * @brief 桌面移动后回弹到原点
     */
    void desktopReboundRequest();
};

}
#endif // DESKTOPWIDGETBASE_H
