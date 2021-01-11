/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#ifndef PROPERTIESWINDOW_H
#define PROPERTIESWINDOW_H

#include <QTabWidget>
#include <QMainWindow>
#include "peony-core_global.h"

#include <QMap>
#include <QMutex>
#include <QSize>
#include <QProxyStyle>

namespace Peony {

class PropertiesWindowTabPagePluginIface;
class PropertiesWindowPluginManager : public QObject
{
    friend class PropertiesWindow;
    Q_OBJECT

public:
    static PropertiesWindowPluginManager *getInstance();
    void release();

    bool registerFactory(PropertiesWindowTabPagePluginIface *factory);
    const QStringList getFactoryNames();
    PropertiesWindowTabPagePluginIface *getFactory(const QString &id);

private:
    explicit PropertiesWindowPluginManager(QObject *parent = nullptr);
    ~PropertiesWindowPluginManager() override;

    QHash<QString, PropertiesWindowTabPagePluginIface*> m_factory_hash;
    QMap<int, QString> m_sorted_factory_map;

    QMutex m_mutex;
};

class PEONYCORESHARED_EXPORT PropertiesWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PropertiesWindow(const QStringList &uris, QWidget *parent = nullptr);

    void setWindowTitleTextAndIcon();
    bool notDir();
    void gotoAboutComputer();
    void saveAllChanged();
    void show();
    void initStatusBar();
    void initTabPage(const QStringList &uris);

public:
    QStringList m_uris;

public:
    //init in properties-window.cpp
    static const qint32 s_windowWidth;
    static const qint32 s_windowHeightFolder;
    static const qint32 s_windowHeightOther;
    static const QSize  s_bottomButtonSize;
    static const QSize  s_topButtonSize;
};

class tabStyle : public QProxyStyle {
    // QStyle interface
public:
    void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
    QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *w) const;
};

class PropertiesWindowPrivate : public QTabWidget
{
    friend class PropertiesWindow;
    Q_OBJECT
private:
    explicit PropertiesWindowPrivate(const QStringList &uris, QWidget *parent = nullptr);

};

}

#endif // PROPERTIESWINDOW_H
