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
//
// Created by hxf on 2022/3/9.
//

#include "animation-widget.h"
#include "desktop-global-settings.h"

#include <QPainter>
#include <QDebug>
#include <QApplication>

using namespace Peony;

AnimationWidget::AnimationWidget(QWidget *parent) : DesktopWidgetBase(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);

    setDesktopType(DesktopType::Animation);

    m_timeLine = new QTimeLine(1000, this);
    m_timeLine->setEasingCurve(QEasingCurve::Linear);
    m_timeLine->setFrameRange(0, 1);
    m_timeLine->setDirection(QTimeLine::Backward);

    connect(m_timeLine, &QTimeLine::finished, this, &AnimationWidget::timeFinished);
    connect(m_timeLine, &QTimeLine::valueChanged, this, [=](qreal x) {
        update();
    });
}

void AnimationWidget::initAnimation(QPixmap frontend, QPixmap backend)
{
    m_frontend.swap(frontend);
    m_backend.swap(backend);
}

void AnimationWidget::updateSettings()
{
    if (m_timeLine->state() == QTimeLine::NotRunning) {
        bool isOk = false;

        qint32 duration = DesktopGlobalSettings::globalInstance(this)->getValue(DESKTOP_ANIMATION_DURATION).toInt(&isOk);
        m_timeLine->setDuration(isOk ? duration : DESKTOP_ANI_DEFAULT_DURATION);

        qreal zoom = DesktopGlobalSettings::globalInstance(this)->getValue(DESKTOP_ITEM_ZOOM_RANGE).toReal(&isOk);
        m_zoom = isOk ? zoom : DESKTOP_DEFAULT_ZOOM_RANGE;
    }
}

bool AnimationWidget::start()
{
    if (m_timeLine->state() != QTimeLine::Running) {
        updateSettings();

        m_timeLine->start();

        return true;
    }

    return false;
}

void AnimationWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    qreal currentValue = m_timeLine->currentValue();
    qreal scale = (1 - m_zoom) + m_zoom * currentValue;
    QPoint midpoint = geometry().center();
    qreal zoom = (1 - currentValue) * m_zoom;

    painter.setOpacity(currentValue);
    painter.scale(scale, scale);
    painter.translate(zoom * (midpoint.x() - x()), zoom * (midpoint.y() - y()));

    painter.drawPixmap(geometry(), m_frontend);
}

void AnimationWidget::timeFinished()
{
    if (m_timeLine->direction() == QTimeLine::Forward) {
        m_timeLine->stop();
        m_timeLine->setDirection(QTimeLine::Backward);
        update();
        Q_EMIT this->finished();

    } else {
        if (m_loop <= 0) {
            Q_EMIT this->finished();
            return;
        }
        m_loop--;
        if (!m_backend.isNull()) {
            m_frontend.swap(m_backend);
        }

        m_timeLine->setDirection(QTimeLine::Forward);
        m_timeLine->start();
    }
}

void AnimationWidget::reverse()
{
    if (m_timeLine->state() != QTimeLine::Running) {
        m_timeLine->setDirection(QTimeLine::Forward);
    }
}

void AnimationWidget::half()
{
    m_loop = 0;
}

void AnimationWidget::setActivated(bool activated)
{
    if (!activated) {
        m_timeLine->stop();
    }
    DesktopWidgetBase::setActivated(activated);
}

DesktopWidgetBase *AnimationWidget::initDesktop(const QRect &rect)
{
    return DesktopWidgetBase::initDesktop(rect);
}

AnimationWidget::~AnimationWidget()
{
    qDebug() << "AnimationWidget Destroyed";
}
