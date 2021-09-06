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
#include "desktop-window.h"
#include "volume-manager.h"
#include "window-manager.h"
#include "desktop-manager.h"

#include <QScreen>
#include <QWindow>
#include <QPropertyAnimation>
#include <QDBusInterface>

using namespace Peony;

class PeonyDesktopApplication : public SingleApplication
{
    Q_OBJECT
public:
    explicit PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName = "peony-qt-desktop");

    void initManager();
    static bool userGuideDaemonRunning();
    static void showGuide(const QString &appName = "");

    static qint64 peony_desktop_start_time;

    void initGSettings();

    void updateGSettingValues();

    QRect createRectForAnimation(QRect &screenRect, QRect &currentRect, AnimationType animationType, bool isExit);

    QPropertyAnimation *createPropertyAnimation(AnimationType animationType, DesktopWidgetBase *object, QRect &startRect, QRect &endRect);

    PropertyName getPropertyNameByAnimation(AnimationType animationType);

private:
    //切换桌面专用函数，请勿乱调
    Peony::DesktopWidgetBase *getNextDesktop(DesktopType targetType, DesktopWindow *parentWindow);
    /**
     * @brief 将target组件的Effect(GraphicsEffect)暂时保存下来，以便在动画结束后恢复
     * @param target
     * @return
     */
    QWidget *saveEffectWidget(QWidget *target);

    /**
     * @brief 切换桌面
     */
    void changeDesktop();

protected Q_SLOTS:
    void parseCmd(quint32 id, QByteArray msg, bool isPrimary);
    bool isPrimaryScreen(QScreen *screen);

public Q_SLOTS:
    void layoutDirectionChangedProcess(Qt::LayoutDirection direction);
    void primaryScreenChangedProcess(QScreen *screen);
    void screenAddedProcess(QScreen *screen);
    void screenRemovedProcess(QScreen *screen);
    void volumeRemovedProcess(const std::shared_ptr<Peony::Volume> &volume);

    void addWindow(QScreen *screen, bool checkPrimay = true);
    void changeBgProcess(const QString& bgPath);
    void checkWindowProcess();

    void changePrimaryWindowDesktop(DesktopType targetType, AnimationType targetAnimation);

private Q_SLOTS:
    void updateTabletModeValue(bool mode);

private:
    bool m_firstParse = true;

    bool m_animationIsRunning = false;

    QGSettings *m_tabletModeGSettings = nullptr;
    QDBusInterface *m_statusManagerDBus = nullptr;

    bool m_isTabletMode = false;
    bool m_learningCenterActivated = false;

    QMutex m_mutex;
    //打开开始菜单后，将当前桌面暂时保存，以便在关闭开始菜单后进行显示
    Peony::DesktopWidgetBase *m_lastDesktop = nullptr;

    //当前主屏幕或者主屏切换后的上一个主屏幕。
    QScreen *m_primaryScreen = nullptr;
    //窗口管理器
    Peony::WindowManager *m_windowManager = nullptr;
    //桌面管理器
    Peony::DesktopManager *m_desktopManager = nullptr;
};

#endif // PEONYDESKTOPAPPLICATION_H
