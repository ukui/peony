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

#ifndef KYSMALLPLUGININTERFACE_H
#define KYSMALLPLUGININTERFACE_H

#include <QtPlugin>
#include <functional>
#include <QString>
#include <QObject>

class QWidget;
class KySmallPluginInterface
{

public:
    virtual ~KySmallPluginInterface() {}
    virtual const QString name() const= 0;
    virtual const QString nameCN() const= 0;
    virtual const QString description() const= 0;
    virtual int sortNum() const= 0;
    virtual QWidget *createWidget(QWidget *parent)=0;
};
QT_BEGIN_NAMESPACE
/*
 * 声明接口，Q_DECLARE_INTERFACE 宏告诉Qt 这个纯虚类是一个插件接口类
 * Q_DECLARE_INTERFACE(接口类名, 接口标识符)
 * 如果我们稍后更改接口，则必须使用不同的字符串来标识新接口；否则，应用程序可能会崩溃。因此，在字符串中包含一个版本号是一个好主意
*/
#define SP_PLUGIN_IID "org.Kylin.SmallPlugin.widgetInterface/1.0"
Q_DECLARE_INTERFACE (KySmallPluginInterface, SP_PLUGIN_IID)
QT_END_NAMESPACE

#endif // KYSMALLPLUGININTERFACE_H
