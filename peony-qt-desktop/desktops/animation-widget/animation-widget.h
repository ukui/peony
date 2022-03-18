/*
 * Peony-Qt
 *
 * Copyright (C) 2022, KylinSoft Co., Ltd.
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

#ifndef PEONY_ANIMATION_WIDGET_H
#define PEONY_ANIMATION_WIDGET_H

#include "desktop-widget-base.h"

#include <QWidget>
#include <QPixmap>
#include <QRect>
#include <QTimeLine>

namespace Peony {

/**
 * @brief
 * 输入两张图片，
 * 按照顺序对两张图片的大小和透明度进行改变
 * 结束后发出信号
 *
 */
class AnimationWidget : public DesktopWidgetBase
{
Q_OBJECT
public:
    explicit AnimationWidget(QWidget *parent = nullptr);

    void setActivated(bool activated) override;

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

    /**
     * @brief
     * @param frontend
     * @param backend 如果第二张图片为空，那么只用一张图片进行放大和缩小操作
     */
    void initAnimation(QPixmap frontend, QPixmap backend);

    /**
     * @brief 只需要半次循环
     * 时间线 Backward 一次，Forward 一次为一个循环
     * 如果循环次数小于等于0，那么只需要进行一次Backward或Forward，也就是半次循环
     */
    void half();

    /**
     * @brief 时间线默认是从大到小，反转后是从小到大
     */
    void reverse();

    virtual ~AnimationWidget();

    bool start();

Q_SIGNALS:
    void finished();

protected:
    void paintEvent(QPaintEvent *event) override;

private Q_SLOTS:
    void timeFinished();

private:
    void updateSettings();

private:
    QTimeLine *m_timeLine = nullptr;
    int m_loop = 1;

    QPixmap m_frontend;
    QPixmap m_backend;

    qreal m_zoom = 0.1;

};

}

#endif //PEONY_ANIMATION_WIDGET_H
