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
 */

//
// Created by hxf on 2021/8/31.
//

#include "study-center-factory.h"
#include "study-center-mode.h"

using namespace Peony;

static StudyCenterFactory *g_studyCenterModeFactory = nullptr;
static StudyCenterMode    *g_studyCenterMode        = nullptr;

StudyCenterFactory *StudyCenterFactory::getInstance(QObject *parent)
{
    if (!g_studyCenterModeFactory) {
        g_studyCenterModeFactory = new StudyCenterFactory(parent);
    }

    return g_studyCenterModeFactory;
}


StudyCenterFactory::StudyCenterFactory(QObject *parent) : QObject(parent)
{

}

StudyCenterFactory::~StudyCenterFactory()
{

}

DesktopType StudyCenterFactory::getDesktopType()
{
    return DesktopType::StudyCenter;
}

QString StudyCenterFactory::getFactoryName()
{
    return QString("study center mode");
}

bool StudyCenterFactory::isEnable()
{
    return true;
}

DesktopWidgetBase *StudyCenterFactory::createDesktop(QWidget *parent)
{
    if (!g_studyCenterMode) {
        g_studyCenterMode = new StudyCenterMode(parent);
        g_studyCenterMode->setDesktopType(DesktopType::StudyCenter);
    }

    return g_studyCenterMode;
}

DesktopWidgetBase *StudyCenterFactory::createNewDesktop(QWidget *parent)
{
    return new StudyCenterMode(parent);
}

bool StudyCenterFactory::closeFactory()
{
    if (g_studyCenterMode) {
        g_studyCenterMode->deleteLater();
    }

    if (g_studyCenterModeFactory) {
        g_studyCenterModeFactory->deleteLater();
    }
    return false;
}
