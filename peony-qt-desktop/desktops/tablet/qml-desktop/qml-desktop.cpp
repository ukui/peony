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
// Created by hxf on 2022/3/1.
//

#include "qml-desktop.h"

#include <QVBoxLayout>

using namespace Peony;

QmlDesktop::QmlDesktop(QWidget *parent) : DesktopWidgetBase(parent)
{
    this->m_exitAnimationType = AnimationType::LeftToRight;

    m_quick = new QQuickView;
    m_quick->setSource(QUrl("/usr/share/ukui/tablet/contents/ui/main.qml"));

    m_container = QWidget::createWindowContainer(m_quick, this);
    m_container->setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_container);

    setLayout(layout);
}

QmlDesktop::~QmlDesktop()
{
    if (m_quick) {
        m_quick->deleteLater();
    }
}

void QmlDesktop::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);
    if (!activated) {
        m_container->move(0, 0);
    }
}

void QmlDesktop::beforeInitDesktop()
{
//    show();
//    m_container->show();
}

DesktopWidgetBase *QmlDesktop::initDesktop(const QRect &rect)
{
    m_container->setGeometry(rect);
    return DesktopWidgetBase::initDesktop(rect);
}

QWidget *QmlDesktop::getRealDesktop()
{
    return m_container;
}
