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
 * Authors: Meihong <hemeihong@kylinos.cn>
 */

#include "create-link-menu-plugin.h"

#include <file-operation-manager.h>
#include <file-link-operation.h>
#include <file-info.h>
#include <gio/gio.h>

#include <QStandardPaths>
#include <QFileDialog>
#include <QTranslator>
#include <QApplication>

using namespace Peony;

CreateLinkMenuPlugin::CreateLinkMenuPlugin(QObject *parent) : QObject (parent)
{
    QTranslator *t = new QTranslator(this);
    qDebug()<<"system().name:"<<QLocale::system().name();
    qDebug()<<"\n\n\n\n\n\n\ntranslate:"<<t->load(":/translations/peony-qt-create-link-extension_"+QLocale::system().name());
    QApplication::installTranslator(t);
}

QList<QAction *> CreateLinkMenuPlugin::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> l;
    if (types == MenuPluginInterface::DesktopWindow || types == MenuPluginInterface::DirectoryView) {
        if (selectionUris.count() == 1) {
            auto createLinkToDesktop = new QAction(QIcon::fromTheme("emblem-symbolic-link"), tr("Create Link to Desktop"));
            auto info = FileInfo::fromUri(selectionUris.first(), false);
            //special type mountable, or isVirtual then return
            if (selectionUris.first().startsWith("computer:///") || info->isVirtual())
                return l;

            connect(createLinkToDesktop, &QAction::triggered, [=](){
                QUrl src = selectionUris.first();
                QString desktopUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
                FileLinkOperation *op = new FileLinkOperation(selectionUris.first(), desktopUri);
                op->setAutoDelete(true);
                FileOperationManager::getInstance()->startOperation(op, true);
            });
            l<<createLinkToDesktop;

            auto createLinkTo = new QAction(tr("Create Link to..."));
            connect(createLinkTo, &QAction::triggered, [=](){
                QUrl targetDir = QFileDialog::getExistingDirectoryUrl(nullptr,
                                                                      tr("Choose a Directory to Create Link"),
                                                                      uri);
                if (!targetDir.isEmpty()) {
                    QUrl src = selectionUris.first();
                    QString target = targetDir.url();
                    FileLinkOperation *op = new FileLinkOperation(selectionUris.first(), target);
                    op->setAutoDelete(true);
                    FileOperationManager::getInstance()->startOperation(op, true);
                }
            });
            l<<createLinkTo;
        }
    }
    return l;
}
