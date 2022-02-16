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

#ifndef DESKTOPMODE_H
#define DESKTOPMODE_H

#include "desktop-widget-base.h"
#include "desktop-icon-view.h"

#include <QVBoxLayout>

namespace Peony {

class DesktopMode : public DesktopWidgetBase
{
Q_OBJECT
public:
    explicit DesktopMode(QWidget *parent = nullptr);

    ~DesktopMode() override;

    void setActivated(bool activated) override;

    DesktopWidgetBase *initDesktop(const QRect &rect) override;

    void onPrimaryScreenChanged() override;

    void initUI();

    void initMenu();

    QWidget *getRealDesktop() override;

//    QWidget *getRealDesktop() override;

private:
    DesktopIconView *m_view = nullptr;
    QVBoxLayout *m_boxLayout = nullptr;
};

}

#endif // DESKTOPMODE_H
