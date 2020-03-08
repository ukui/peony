/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "mate-terminal-menu-plugin.h"
#include <file-info.h>
#include <gio/gio.h>

#include <QAction>
#include <QUrl>

#include <QTranslator>
#include <QLocale>
#include <QApplication>

#include <QtConcurrent>

#include <QDebug>

using namespace Peony;

static QString terminal_cmd = nullptr;

MateTerminalMenuPlugin::MateTerminalMenuPlugin(QObject *parent) : QObject (parent)
{
    QTranslator *t = new QTranslator(this);
    qDebug()<<"\n\n\n\n\n\n\ntranslate:"<<t->load(":/translations/peony-qt-mate-terminal-extension_"+QLocale::system().name());
    QApplication::installTranslator(t);

    QtConcurrent::run([=]{
        GList *infos = g_app_info_get_all();
        GList *l = infos;
        while (l) {
            const char *cmd = g_app_info_get_executable(static_cast<GAppInfo*>(l->data));
            QString tmp = cmd;
            if (tmp.contains("terminal")) {
                terminal_cmd = tmp;
                if (tmp == "mate-terminal") {
                    break;
                }
            }
            l = l->next;
        }
        g_list_free_full(infos, g_object_unref);
    });
}

void openTerminal(const QString &uri){
    //QString test_path = "/develop";
    QUrl url = uri;
    auto directory = url.path().toUtf8().constData();
    qDebug() <<"triggered" <<uri << url.path();
    //qDebug()<< "openTerminal directory" << directory;
    gchar **argv = nullptr;
    g_shell_parse_argv (terminal_cmd.toUtf8().constData(), nullptr, &argv, nullptr);
    GError *err = nullptr;
    QString path, dir;
    path.prepend(directory);
    dir.append(directory);
    qDebug() << "terminal_cmd:" << terminal_cmd <<"directory" << path <<dir;
    g_spawn_async (directory,
                   argv,
                   nullptr,
                   G_SPAWN_SEARCH_PATH,
                   nullptr,
                   nullptr,
                   nullptr,
                   &err);
    if (err) {
        qDebug()<<err->message;
        g_error_free(err);
        err = nullptr;
    }
    g_strfreev (argv);
}

QList<QAction *> MateTerminalMenuPlugin::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> actions;
    if (terminal_cmd.isNull()) {
        return actions;
    }
    if (types == MenuPluginInterface::DirectoryView || types == MenuPluginInterface::DesktopWindow) {
        if (selectionUris.isEmpty()) {
            QAction *dirAction = new QAction(QIcon::fromTheme("utilities-terminal-symbolic"), tr("Open Directory in Terminal"));
            dirAction->connect(dirAction, &QAction::triggered, [=](){
                openTerminal(uri);
            });
            actions<<dirAction;
        }
        if (selectionUris.count() == 1) {
            auto info = FileInfo::fromUri(selectionUris.first(), false);
            if (info->isDir()) {
                QAction *dirAction = new QAction(QIcon::fromTheme("utilities-terminal-symbolic"), tr("Open Directory in Terminal"));
                dirAction->connect(dirAction, &QAction::triggered, [=](){
                    openTerminal(selectionUris.first());
                });
                actions<<dirAction;
            }
        }
    }
    return actions;
}
