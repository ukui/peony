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
 *
 */

#include "desktop-window.h"
#include "peony-desktop-application.h"
#include "desktop-screen.h"
#include "peony-log.h"

#include <stdio.h>
#include <stdlib.h>
#include <QTime>
#include <QFile>

#include <QStandardPaths>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>

#define UKUI_SERVICE    "org.gnome.SessionManager"
#define UKUI_PATH   "/org/gnome/SessionManager"
#define UKUI_INTERFACE    "org.gnome.SessionManager"

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/peony-desktop.log";
    bool showDebug = true;
    //屏蔽代码，自动生成日志，无需手动创建
//    if (!QFile::exists(logFilePath)) {
//        showDebug = false;
//    }
    FILE *log_file = nullptr;

    if (showDebug) {
        log_file = fopen(logFilePath.toLocal8Bit().constData(), "a+");
    }

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        if (!log_file) {
            break;
        }
        fprintf(log_file, "Debug: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(log_file? log_file: stdout, "Info: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(log_file? log_file: stderr, "Warning: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(log_file? log_file: stderr, "Critical: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(log_file? log_file: stderr, "Fatal: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    }

    if (log_file)
        fclose(log_file);
}

int main(int argc, char *argv[])
{
    PeonyDesktopApplication::peony_desktop_start_time = QDateTime::currentMSecsSinceEpoch();
    qInstallMessageHandler(messageOutput);

    qDebug() << "desktop start time in main:" <<PeonyDesktopApplication::peony_desktop_start_time;
    PEONY_DESKTOP_LOG_WARN("peony desktop start#######################");

    DesktopScreen *screen = new DesktopScreen();

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    delete screen;

    PeonyDesktopApplication a(argc, argv);
    if (a.isSecondary())
        return 0;

    PEONY_DESKTOP_LOG_WARN("peony desktop start before dbus interface#######################");
    QDBusInterface interface(UKUI_SERVICE,
                                 UKUI_PATH,
                                 UKUI_INTERFACE,
                                 QDBusConnection::sessionBus());

    interface.call("startupfinished","peony-qt-desktop","finish");
    PEONY_DESKTOP_LOG_WARN("peony desktop end#######################");

    return a.exec();
}

