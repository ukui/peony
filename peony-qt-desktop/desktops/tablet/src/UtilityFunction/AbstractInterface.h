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

#ifndef ABSTRACTINTERFACE_H
#define ABSTRACTINTERFACE_H
#include <QObject>
class QWidget;
class AbstractInterface
{
public:
    virtual ~AbstractInterface() {}
    virtual QWidget *createPluginWidget(QWidget *parent,bool type) = 0;
};
#define AbstractInterface_iid "Welcome to use focusmode"
Q_DECLARE_INTERFACE(AbstractInterface,AbstractInterface_iid)
#endif // ABSTRACTINTERFACE_H
