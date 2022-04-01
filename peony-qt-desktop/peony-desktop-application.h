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

#include <QScreen>
#include <QWindow>

class DesktopBackgroundWindow;

namespace Peony {
class DesktopIconView;
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
    static void gotoSetResolution();

    static qint64 peony_desktop_start_time;

Q_SIGNALS:
    void requestSetUKUIOutputEnable(bool enable);

protected Q_SLOTS:
    void parseCmd(QString msg, bool isPrimary);
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
    void updateVirtualDesktopGeometryByWindows();

    void addBgWindow(QScreen *screen);
    void relocateIconView();

private:
    void setupDesktop();
    void setupBgAndDesktop();

    bool m_first_parse = true;

    QList<DesktopBackgroundWindow *> m_bg_windows;

    QTimeLine *m_primaryScreenSettingsTimeLine = nullptr;
};

#endif // PEONYDESKTOPAPPLICATION_H
