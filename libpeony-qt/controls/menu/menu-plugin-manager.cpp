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

#include "menu-plugin-manager.h"

//create link
#include <file-operation-manager.h>
#include <file-link-operation.h>
#include <file-info.h>
#include <gio/gio.h>

#include <QStandardPaths>
#include <QFileDialog>
#include <QApplication>
//create link

//tag file
#include "file-label-model.h"
#include <QMenu>

#include <QAction>
#include <QIcon>

#include <QDebug>

using namespace Peony;

static MenuPluginManager *global_instance = nullptr;

MenuPluginManager::MenuPluginManager(QObject *parent) : QObject(parent)
{
    registerPlugin(new CreateLinkInternalPlugin(this));
    registerPlugin(new FileLabelInternalMenuPlugin(this));
}

MenuPluginManager::~MenuPluginManager()
{

}

bool MenuPluginManager::registerPlugin(MenuPluginInterface *plugin)
{
    if (m_hash.value(plugin->name())) {
        return false;
    }
    m_hash.insert(plugin->name(), plugin);
    return true;
}

MenuPluginManager *MenuPluginManager::getInstance()
{
    if (!global_instance) {
        global_instance = new MenuPluginManager;
    }
    return global_instance;
}

void MenuPluginManager::close()
{
    this->deleteLater();
}

const QStringList MenuPluginManager::getPluginIds()
{
    return m_hash.keys();
}

MenuPluginInterface *MenuPluginManager::getPlugin(const QString &pluginId)
{
    return m_hash.value(pluginId);
}

//CreateLinkInternalPlugin
CreateLinkInternalPlugin::CreateLinkInternalPlugin(QObject *parent) : QObject (parent)
{

}

QList<QAction *> CreateLinkInternalPlugin::menuActions(MenuPluginInterface::Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> l;
    if (types == MenuPluginInterface::DesktopWindow || types == MenuPluginInterface::DirectoryView) {
        if (selectionUris.count() == 1) {
            auto createLinkToDesktop = new QAction(QIcon::fromTheme("emblem-link-symbolic"), tr("Create Link to Desktop"), nullptr);
            auto info = FileInfo::fromUri(selectionUris.first());
            QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            QString originPath = QUrl(selectionUris.first()).path();
            //special type mountable, or isVirtual then return
            if (selectionUris.first().startsWith("computer:///") || info->isVirtual()
                || selectionUris.first().startsWith("trash:///")
                || selectionUris.first().startsWith("recent:///")
                || originPath.startsWith(desktopPath))
                return l;

            connect(createLinkToDesktop, &QAction::triggered, [=]() {
                //QUrl src = selectionUris.first();
                QString desktopUri = "file://" + desktopPath;
                FileLinkOperation *op = new FileLinkOperation(selectionUris.first(), desktopUri);
                op->setAutoDelete(true);
                FileOperationManager::getInstance()->startOperation(op, true);
            });
            l<<createLinkToDesktop;

            auto createLinkTo = new QAction(tr("Create Link to..."), nullptr);
            connect(createLinkTo, &QAction::triggered, [=]() {
                QUrl targetDir = QFileDialog::getExistingDirectoryUrl(nullptr,
                                 tr("Choose a Directory to Create Link"),
                                 uri);
                if (!targetDir.isEmpty()) {
                    //QUrl src = selectionUris.first();
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

//FileLabelInternalMenuPlugin
FileLabelInternalMenuPlugin::FileLabelInternalMenuPlugin(QObject *parent)
{

}

QList<QAction *> FileLabelInternalMenuPlugin::menuActions(MenuPluginInterface::Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> l;
    //fix virtual path add label fail issue
    auto info = FileInfo::fromUri(uri);
    if (info->isVirtual())
        return l;

    if (types == DirectoryView) {
        if (selectionUris.count() == 1) {
            //not allow in trash path
            if (uri.startsWith("trash://") || uri.startsWith("smb://")
                || uri.startsWith("recent://") || uri.startsWith("computer://"))
                return l;
            auto action = new QAction(tr("Add File Label..."), nullptr);
            auto uri = selectionUris.first();
            auto menu = new QMenu();
            auto items = FileLabelModel::getGlobalModel()->getAllFileLabelItems();
            for (auto item : items) {
                auto ids = FileLabelModel::getGlobalModel()->getFileLabelIds(uri);
                bool checked = ids.contains(item->id());
                auto a = menu->addAction(item->name(), [=]() {
                    if (!checked) {
                        // note: while add label to file at first time (usually new user created),
                        // it might fail to add a label correctly, but second time will work.
                        // it might be a bug of gvfsd-metadata. anyway we should to avoid this
                        // situation.
                        FileLabelModel::getGlobalModel()->addLabelToFile(uri, item->id());
                        FileLabelModel::getGlobalModel()->addLabelToFile(uri, item->id());
                    } else {
                        FileLabelModel::getGlobalModel()->removeFileLabel(uri, item->id());
                    }
                });
                a->setCheckable(true);
                a->setChecked(checked);
            }
            menu->addSeparator();
            menu->addAction(tr("Delete All Label"), [=]() {
                FileLabelModel::getGlobalModel()->removeFileLabel(uri);
            });
            action->setMenu(menu);
            l<<action;
        }
    }
    return l;
}
