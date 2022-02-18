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

//
// Created by hxf on 2021/8/10.
//

#ifndef PEONY_TABLET_MODE_FACTORY_H
#define PEONY_TABLET_MODE_FACTORY_H

#include "desktop-factory-i-face.h"

namespace Peony {

class TabletModeFactory : public QObject, public DesktopFactoryIFace
{
    Q_OBJECT
public:
    static TabletModeFactory *getInstance(QObject *parent = nullptr);

    ~TabletModeFactory() override;

    DesktopType getDesktopType() override;

    QString getFactoryName() override;

    bool isEnable() override;

    DesktopWidgetBase *createDesktop(QWidget *parent) override;

    DesktopWidgetBase *createNewDesktop(QWidget *parent) override;

    bool closeFactory() override;

private:
    explicit TabletModeFactory(QObject *parent = nullptr);
};

}

#endif //PEONY_TABLET_MODE_FACTORY_H
