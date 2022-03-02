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

#ifndef DESKTOPWIDGETBASE_H
#define DESKTOPWIDGETBASE_H

#include "desktop-commons.h"

#include <QWidget>

namespace Peony {

/**
 * \brief 桌面插件抽象基类
 * 生命周期：
 *               beforeInitDesktop                setPause(false)    initDesktop     setActivated(true)
 * |-未激活-| ---> |-预加载数据-| ------------------> |-取消暂停-| ---> |-初始化桌面-| ---> |-激活桌面-|
 *                          \ (可选)                 /
 *                           \ --> |-显示桌面-| --> /
 *                                   show()
 */
class DesktopWidgetBase : public QWidget
{
Q_OBJECT
public:
    explicit DesktopWidgetBase(QWidget *parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
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
        this->getRealDesktop()->setHidden(!activated);
        this->setPause(!activated);
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
     * @brief beforeInitDesktop
     * @return
     * 桌面生命周期函数。
     * 在初始化桌面之前做一些处理，该方法会在桌面显示出来之前被调用。
     * 一些需要在桌面可见之前就完成的操作应该在该方法中调用。
     */
    virtual void beforeInitDesktop() {}

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

    virtual void onPrimaryScreenChanged() {}

    DesktopType getDesktopType() const
    {
        return m_currentDesktopType;
    }

    void setDesktopType(DesktopType desktopType)
    {
        m_currentDesktopType = desktopType;
    }

    void setPause(bool isPause) {
        m_isPause = isPause;
    }

    bool isPause() {
        return m_isPause;
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

    /**
     * @brief 发送切换桌面请求后，进入暂停状态
     *        通过该属性屏蔽一些在切换过程中的动作
     */
    bool m_isPause = false;

    /**
     * @brief 发送切换桌面请求，并设置暂停状态，在1000ms后自动恢复
     * @param targetType
     * @param targetAnimation
     */
    virtual void requestMoveToOtherDesktop(DesktopType targetType, AnimationType targetAnimation) {
        setPause(true);
        Q_EMIT moveToOtherDesktop(targetType, targetAnimation);
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
