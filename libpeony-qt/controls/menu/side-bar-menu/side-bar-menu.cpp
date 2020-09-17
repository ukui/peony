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

#include "side-bar-menu.h"
#include "side-bar-abstract-item.h"

#include "bookmark-manager.h"
#include "properties-window.h"

#include "menu-plugin-manager.h"

#include "file-utils.h"
#include "file-info.h"
#include "file-info-job.h"

#include <QAction>
#include <QModelIndex>
#include <format_dialog.h>

#include <QDebug>

using namespace Peony;

SideBarMenu::SideBarMenu(SideBarAbstractItem *item, SideBar *sideBar, QWidget *parent) : QMenu (parent)
{
    m_uri = item->uri();
    m_item = item;
    m_side_bar = sideBar;

    if (!item) {
        auto action = addAction(QIcon::fromTheme("preview-file"), tr("Properties"));
        action->setEnabled(false);
        return;
    }

    switch (item->type()) {
    case SideBarAbstractItem::FavoriteItem: {
        constructFavoriteActions();
        break;
    }
    case SideBarAbstractItem::PersonalItem: {
        constructPersonalActions();
        break;
    }
    case SideBarAbstractItem::FileSystemItem: {
        constructFileSystemItemActions();
        break;
    }
    default: {
        auto action = addAction(QIcon::fromTheme("preview-file"), tr("Properties"));
        action->setEnabled(false);
        break;
    }
    }
}

const QList<QAction *> SideBarMenu::constructFavoriteActions()
{
    QList<QAction *> l;
    l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("&Delete Symbolic"), [=]() {
        BookMarkManager::getInstance()->removeBookMark(m_uri);
    });
    if (!m_item->firstColumnIndex().parent().isValid()) {
        l.last()->setEnabled(false);
    } else if (m_item->firstColumnIndex().row() < 3) {
        l.last()->setEnabled(false);
    }

    l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"), [=]() {
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });
    if (!m_item->firstColumnIndex().parent().isValid()) {
        l.last()->setEnabled(false);
    }

    return l;
}

const QList<QAction *> SideBarMenu::constructPersonalActions()
{
    QList<QAction *> l;

    l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"), [=]() {
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });

    return l;
}

const QList<QAction *> SideBarMenu::constructFileSystemItemActions()
{
    QList<QAction *> l;

    auto info = FileInfo::fromUri(m_uri, false);
    if (info->displayName().isEmpty()) {
        FileInfoJob j(info);
        j.querySync();
    }
    if (info->canUnmount() || info->canMount()) {
        l<<addAction(QIcon::fromTheme("media-eject"), tr("&Unmount"), [=]() {
            m_item->unmount();
        });
        l.last()->setEnabled(m_item->isMounted());
    }
    if (info->canEject()) {
        l<<addAction(QIcon::fromTheme("media-eject"), tr("&Eject"), [=](){
            m_item->eject();
        });
    }

    auto mgr = MenuPluginManager::getInstance();
    auto ids = mgr->getPluginIds();
    for (auto id : ids) {
        auto factory = mgr->getPlugin(id);
        //qDebug()<<id;
        auto tmp = factory->menuActions(MenuPluginInterface::SideBar, m_uri, QStringList()<<m_uri);
        addActions(tmp);
        for (auto action : tmp) {
            action->setParent(this);
        }
        l<<tmp;
    }

    l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"), [=]() {
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });


    /*
     *  add format function
     *  provide option for all mountable device
     *  if can not format, will have prompt
     */

    if(!m_uri.endsWith(".mount") && info->isVolume() && info->canUnmount()) {
        l<<addAction(QIcon::fromTheme("preview-file"), tr("format"), [=]() {
            Format_Dialog *fd  = new Format_Dialog(m_uri,m_item);
            fd->show();
        });
    }

    return l;
}

