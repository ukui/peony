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
#include "properties-window-tab-iface.h"

#include <QMap>
#include <QMutex>
#include <QSize>
#include <QProxyStyle>
#include <QtConcurrent>
#include <file-info.h>

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

    QHash<QString, PropertiesWindowTabPagePluginIface *> m_factory_hash;
    QMap<int, QString> m_sorted_factory_map;

    QMutex m_mutex;
};

class PEONYCORESHARED_EXPORT PropertiesWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit PropertiesWindow(const QStringList &uris, QWidget *parent = nullptr);

    void setWindowTitleTextAndIcon();

    /*!
     * 判断当前的uris中是否'不存在'目录
     * \brief notDir
     * \return
     */
    void notDir();

    void gotoAboutComputer();

    void show();

    void init();

    /*!
     * 初始化底部按钮栏
     * \brief initStatusBar
     */
    void initStatusBar();

    /*!
     * 设置tab栏的样式。
     * FIX:目前tab样式不能跟随主题
     * \brief initTabPage
     * \param uris
     */
    void initTabPage(const QStringList &uris);

    QStringList &getUris()
    {
        return m_uris;
    }

    /*!
     * 检查当前的uris是否已经在窗口中打开
     * 如果已经打开，那么返回true,
     * 如果没有打开，那么返回false
     * \brief checkUriIsOpen
     * \param uris
     * \return
     */
    static bool checkUriIsOpen(QStringList &uris, PropertiesWindow *newWindow);

    /*!
     * 返回指定uris在 openPropertiesWindows 中的索引，如果不存在，返回 -1
     * Returns the index of the specified uris in openPropertiesWindows, if it does not exist, returns -1
     * \brief getOpenUriIndex
     * \return
     */
    static qint64 getOpenUriIndex(QStringList &uris);

    /*!
     * 从已打开窗口列表中删除索引的窗口
     * \brief removeThisWindow
     * \param index
     */
    static void removeThisWindow(qint64 index);

    /*!
     * 存放当前窗口的所有tab页面
     * \brief addTabPage
     * \param tabPage
     */
    void addTabPage(PropertiesWindowTabIface *tabPage)
    {

        if (tabPage) {
            m_openTabPage.append(tabPage);
            connect(tabPage, &PropertiesWindowTabIface::requestCloseMainWindow, this, [=]() {
                Q_ASSERT(tabPage);
                this->close();
            });
        }
    }

    /*!
     * 响应确认按钮
     * \brief saveAllChanged
     */
    void saveAllChanged();

protected:
    /**
     * 在窗口关闭时，将存储的窗口指针从openPropertiesWindows中删除
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent *event);

private:
    bool m_notDir = true;
    std::shared_ptr<FileInfo> m_fileInfo = nullptr;

    bool m_destroyThis = false;
    QStringList m_uris;
    QList<PropertiesWindowTabIface *> m_openTabPage;

public:
    //init in properties-window.cpp
    static const qint32 s_windowWidth;
    static const qint32 s_windowHeightFolder;
    static const qint32 s_windowHeightOther;
    static const QSize s_bottomButtonSize;

    // QWidget interface

};

class tabStyle : public QProxyStyle
{
    // QStyle interface
public:
    void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter,
                     const QWidget *widget) const;

    QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize,
                           const QWidget *w) const;
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
