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

#include "peony-application.h"

#include "main-window.h"

#include <stdio.h>
#include <stdlib.h>
#include <QTime>
#include <QFile>

#include <QStandardPaths>

#include "navigation-tab-bar.h"
#include "tab-widget.h"

void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    bool showDebug = true;
    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/peony-qt.log";
    if (!QFile::exists(logFilePath)) {
        showDebug = false;
    }
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

int main(int argc, char *argv[]) {
    PeonyApplication::peony_start_time = QDateTime::currentMSecsSinceEpoch();
    qInstallMessageHandler(messageOutput);
    qDebug() << "peony start in main time:" <<PeonyApplication::peony_start_time ;

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    PeonyApplication app(argc, argv, "peony-qt");
    if (app.isSecondary())
        return 0;

    return app.exec();
}
