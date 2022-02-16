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

#include "desktop-mode.h"
#include "desktop-menu.h"
#include "peony-desktop-application.h"

#include <QBoxLayout>
#include <QPushButton>

using namespace Peony;

DesktopMode::DesktopMode(QWidget *parent) : DesktopWidgetBase(parent)
{
    //创建桌面时，默认未激活
    m_isActivated = false;

    //设置ui
    this->initUI();
}

DesktopMode::~DesktopMode()
{
    m_view->deleteLater();
}

DesktopWidgetBase *DesktopMode::initDesktop(const QRect &rect)
{
    DesktopWidgetBase::initDesktop(rect);

//    m_view->refresh();

    return this;
}

void DesktopMode::onPrimaryScreenChanged()
{
    if (m_view->isVisible()) {
        m_view->resolutionChange();
    }
}

void DesktopMode::initUI()
{
    this->setContentsMargins(0, 0, 0, 0);

    m_boxLayout = new QVBoxLayout(this);

    m_boxLayout->setContentsMargins(0, 0, 0, 0);
    m_boxLayout->setAlignment(Qt::AlignCenter);

    this->setLayout(m_boxLayout);

    m_view = new DesktopIconView(this);

    m_boxLayout->addWidget(m_view);
}

void DesktopMode::setActivated(bool activated)
{
    DesktopWidgetBase::setActivated(activated);

    //桌面被激活就显示,取消激活就隐藏
    if (m_view) {
        m_view->setHidden(!activated);
    }
}

QWidget *DesktopMode::getRealDesktop()
{
    if (!m_view) {
        m_view = new DesktopIconView(this);
    }

    return m_view;
}
