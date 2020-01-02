/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#include <QScreen>
#include <QWindow>

class PeonyDesktopApplication : public SingleApplication
{
    Q_OBJECT
public:
    explicit PeonyDesktopApplication(int &argc, char *argv[], const char *applicationName = "peony-qt-desktop");

protected Q_SLOTS:
    void parseCmd(quint32 id, QByteArray msg, bool isPrimary);
    bool isPrimaryScreen(QScreen *screen);

public Q_SLOTS:
    void layoutDirectionChangedProcess(Qt::LayoutDirection direction);
    void primaryScreenChangedProcess(QScreen *screen);
    void screenAddedProcess(QScreen *screen);
    void screenRemovedProcess(QScreen *screen);

    void addWindow(QScreen *screen, bool checkPrimay = true);
    void changeBgProcess(const QString& bgPath);
    void checkWindowProcess();

private:
    bool m_first_parse = true;
    QList<Peony::DesktopWindow*> m_window_list;
};

#endif // PEONYDESKTOPAPPLICATION_H
