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

#ifndef OOBE_PLUGIN_INFACE_H
#define OOBE_PLUGIN_INFACE_H

#include <QtPlugin>
#include <functional>
#include <QString>
#include <QObject>

class QWidget;
class kyMenuPluginInterface
{

public:
    virtual ~kyMenuPluginInterface() {}
    virtual const QString name() = 0;
    virtual const QString description() = 0;
    virtual QWidget *createWidget(QWidget *parent)=0;
};
Q_DECLARE_INTERFACE (kyMenuPluginInterface, "org.kyMenuPlugin.AbstractInterface")
#endif
