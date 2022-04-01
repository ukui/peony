/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 * Authors: Meihong He <hemeihong@kylinos.cn>
 *
 */

#ifndef PEONYDESKTOPAPPLICATION_H
#define PEONYDESKTOPAPPLICATION_H

#include "singleapplication.h"
#include "qtsingleapplication.h"
#include "volume-manager.h"

#include "window-manager.h"
#include "desktop-manager.h"
#include "peony-desktop-dbus-service.h"
#include "animation-widget.h"

#include <QScreen>
#include <QWindow>
#include <QMutex>
#include <QPropertyAnimation>
#include <QDBusInterface>

namespace Peony {
class WindowManager;
class DesktopManager;
class DesktopIconView;
class DesktopBackgroundWindow;
}

using namespace Peony;

class QTimeLine;

class PeonyDesktopApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit PeonyDesktopApplication(int &argc, char *argv[], const QString &applicationName = "peony-qt-desktop");

    static Peony::DesktopIconView *getIconView();
    static bool userGuideDaemonRunning();
    static void showGuide(const QString &appName = "");
    static void gotoSetBackground();

    static qint64 peony_desktop_start_time;

private:
    void setupBgAndDesktop();

    void setupDesktop();

    void addBgWindow(QScreen *screen);

    /**
     * @brief 初始化时，为全部窗口添加桌面
     */
    void initWindowDesktop();

    void updateDesktop();

    /**
     * @brief 唤起全部窗口，保证主屏不被覆盖
     */
    void raiseWindows();

    void initManager();

    void initGSettings();

    void initDBusService();

    void updateGSettingValues();

    QRect createRectForAnimation(QRect &screenRect, QRect &currentRect, AnimationType animationType, bool isExit);

    QPropertyAnimation *createPropertyAnimation(AnimationType animationType, DesktopWidgetBase *object, QRect &startRect, QRect &endRect);

    Peony::PropertyName getPropertyNameByAnimation(AnimationType animationType);

    Peony::DesktopWidgetBase *getNextDesktop(DesktopType targetType, Peony::DesktopBackgroundWindow *parentWindow);

    /**
     * @brief 切换桌面
     */
    void changeDesktop();

protected Q_SLOTS:
    void parseCmd(QString msg, bool isPrimary);

public Q_SLOTS:
    void layoutDirectionChangedProcess(Qt::LayoutDirection direction);
    void primaryScreenChangedProcess(QScreen *screen);
    void screenAddedProcess(QScreen *screen);
    void screenRemovedProcess(QScreen *screen);
    void volumeRemovedProcess(const std::shared_ptr<Peony::Volume> &volume);

private Q_SLOTS:
    void updateTabletModeValue(bool mode);

    void changePrimaryWindowDesktop(DesktopType targetType, AnimationType targetAnimation);

private:
    //截图动画不需要知道目标动画类型
    void startScreenshotAnimation(Peony::DesktopBackgroundWindow*, Peony::DesktopWidgetBase*, Peony::DesktopWidgetBase*);
    void startPropertyAnimation(Peony::DesktopBackgroundWindow*, Peony::DesktopWidgetBase*, Peony::DesktopWidgetBase*, AnimationType);

private:
    QList<Peony::DesktopBackgroundWindow *> m_bg_windows;

    QMutex m_mutex;

    bool m_firstParse = true;
    bool m_isTabletMode = false;
    bool m_animationIsRunning = false;
    bool m_learningCenterActivated = false;

    //QGSettings     *m_tabletModeGSettings = nullptr;
    QDBusInterface *m_statusManagerDBus   = nullptr;

    //当前主屏幕或者主屏切换后的上一个主屏幕。
    QScreen               *m_primaryScreen  = nullptr;
    //窗口管理器
    Peony::WindowManager  *m_windowManager  = nullptr;
    //桌面管理器
    Peony::DesktopManager *m_desktopManager = nullptr;
    //intel dbus service
    Peony::PeonyDesktopDbusService *m_desktopDbusService = nullptr;
};

#endif // PEONYDESKTOPAPPLICATION_H
