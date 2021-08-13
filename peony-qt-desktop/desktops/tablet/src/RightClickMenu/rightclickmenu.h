/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef RIGHTCLICKMENU_H
#define RIGHTCLICKMENU_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QSvgRenderer>
#include <QPainter>
#include <QWidgetAction>
#include <QLabel>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDBusInterface>
#include <QProcess>
#include <QEvent>
//#include <QPaintDevice>
#include "src/Interface/ukuimenuinterface.h"
#include "src/Style/style.h"

class RightClickMenu : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Right-click menu
     * @param module: Right-click menu type, Only the following parameters can be entered:
     *  0: application
     *  1: power
     *  2: others
     */
    explicit RightClickMenu(QWidget *parent = nullptr);
    virtual ~RightClickMenu();
    /**
     * @brief Displays the application right-click menu
     * @param desktopfp: Application desktop file
     * @return return action number
     */
    int showAppBtnMenu(QString desktopfp,bool isinstall);
    /**
     * @brief Displays the power right-click menu
     * @return return action number
     */
    //int showShutdownMenu();
    /**
     * @brief Displays other application right-click menus
     * @param desktopfp: Application desktop file
     * @return return action number
     */
    //int showOtherMenu(QString desktopfp);

private:
    QString m_desktopfp;
    int m_actionNumber;
    QSettings *m_setting=nullptr;

    char m_style[400];

    QProcess *m_cmdProc=nullptr;

protected:
    /**
     * @brief Get icon pixmap
     * @param iconstr: Icon names
     * @param type: Icon type, Only the following parameters can be entered:
     *  0: Resource icon
     *  1: System icon
     * @return
     */
    QPixmap getIconPixmap(QString iconstr, int type);
    /**
     * @brief Process image color
     * @param source: QPixmap icon
     * @return return QPixmap icon
     */
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source);

private Q_SLOTS:
    /**
     * @brief Fixed to all software
     */
    void fixToAllActionTriggerSlot();
    /**
     * @brief Unfix from all software
     */
    void unfixedFromAllActionTriggerSlot();
    /**
     * @brief Fixed to the taskbar
     */
    void fixToTaskbarActionTriggerSlot();
    /**
     * @brief Unfix from the taskbar
     */
    void unfixedFromTaskbarActionTriggerSlot();
    /**
     * @brief Add to the desktop shortcut
     */
    void addToDesktopActionTriggerSlot();
    /**
     * @brief Uninstall
     */
    void uninstallActionTriggerSlot();
    /**
     * @brief Attribute
     */
    void attributeActionTriggerSlot();
    /**
     * @brief Lock Screen
     */
    //void lockScreenActionTriggerSlot();
    /**
     * @brief Switch user
     */
    //void switchUserActionTriggerSlot();
    /**
     * @brief Log out
     */
    //void logoutActionTriggerSlot();
    /**
     * @brief Reboot
     */
    //void rebootActionTriggerSlot();
    /**
     * @brief Shut down
     */
    //void shutdownActionTriggerSlot();
    //void hibernateActionTriggerSlot();
    //void sleepActionTriggerSlot();
    /**
     * @brief Personalize this list
     */
    //void otherListActionTriggerSlot();
    /**
     * @brief Read command output
     */
    void onReadOutput();
};

#endif // RIGHTCLICKMENU_H
