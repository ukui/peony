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

#ifndef PEONY_QML_DESKTOP_H
#define PEONY_QML_DESKTOP_H

#include "desktop-widget-base.h"

#include <QQuickView>

namespace Peony {

class QmlDesktop : public DesktopWidgetBase
{
public:
    explicit QmlDesktop(QWidget *parent = nullptr);

    ~QmlDesktop() override;

    void setActivated(bool activated) override;

    void beforeInitDesktop() override;

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

    QWidget *getRealDesktop() override;

private:
    QQuickView *m_quick = nullptr;
    QWidget *m_container = nullptr;
};

}

#endif //PEONY_QML_DESKTOP_H
