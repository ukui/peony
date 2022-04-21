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

#include "plugin-manager.h"

#include "menu-plugin-manager.h"
#include "directory-view-factory-manager.h"
#include "preview-page-factory-manager.h"

#include "directory-view-plugin-iface.h"
#include "directory-view-plugin-iface2.h"
#include "preview-page-plugin-iface.h"
#include "style-plugin-iface.h"
#include "vfs-plugin-manager.h"
#include "emblem-plugin-iface.h"

#include "properties-window.h" //properties factory manager define is in this header
#include "properties-window-tab-page-plugin-iface.h"

#include "directory-view-widget.h"

#include "global-settings.h"

#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QApplication>
#include <QProxyStyle>

using namespace Peony;

static PluginManager *global_instance = nullptr;

PluginManager::PluginManager(QObject *parent) : QObject(parent)
{
    //FIXME: we have to ensure that internal factory being registered successfully.
    PropertiesWindowPluginManager::getInstance();
    MenuPluginManager::getInstance();
    DirectoryViewFactoryManager2::getInstance();
    PreviewPageFactoryManager::getInstance();
    VFSPluginManager::getInstance();

    QDir pluginsDir(PLUGIN_INSTALL_DIRS);
//    if (COMMERCIAL_VERSION)
//        pluginsDir = QDir("/usr/lib/peony-qt-extensions");
    pluginsDir.setFilter(QDir::Files);

    qDebug()<<pluginsDir.entryList().count();
    Q_FOREACH(QString fileName, pluginsDir.entryList(QDir::Files)) {
        qDebug()<<fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        qDebug()<<pluginLoader.fileName();
        qDebug()<<pluginLoader.metaData();
        qDebug()<<pluginLoader.load();

        // version check
        if (pluginLoader.metaData().value("MetaData").toObject().value("version").toString() != VERSION)
            continue;

        QObject *plugin = pluginLoader.instance();
        if (!plugin)
            continue;
        qDebug()<<"test start";
        PluginInterface *piface = dynamic_cast<PluginInterface*>(plugin);
        if (!piface)
            continue;
        m_hash.insert(piface->name(), piface);
        switch (piface->pluginType()) {
        case PluginInterface::MenuPlugin: {
            MenuPluginInterface *menuPlugin = dynamic_cast<MenuPluginInterface*>(piface);
            MenuPluginManager::getInstance()->registerPlugin(menuPlugin);
            break;
        }
        case PluginInterface::PreviewPagePlugin: {
            PreviewPagePluginIface *previewPageFactory = dynamic_cast<PreviewPagePluginIface*>(plugin);
            PreviewPageFactoryManager::getInstance()->registerFactory(previewPageFactory->name(), previewPageFactory);
            break;
        }
        case PluginInterface::PropertiesWindowPlugin: {
            PropertiesWindowTabPagePluginIface *propertiesWindowTabPageFactory = dynamic_cast<PropertiesWindowTabPagePluginIface*>(plugin);
            PropertiesWindowPluginManager::getInstance()->registerFactory(propertiesWindowTabPageFactory);
            break;
        }
        case PluginInterface::ColumnProviderPlugin: {
            //FIXME:
            break;
        }
        case  PluginInterface::StylePlugin: {
            /*!
              \todo
              manage the style plugin
              */
            auto styleProvider = dynamic_cast<StylePluginIface*>(plugin);
            QApplication::setStyle(styleProvider->getStyle());
            break;
        }
        case PluginInterface::DirectoryViewPlugin2: {
            auto p = dynamic_cast<DirectoryViewPluginIface2*>(plugin);
            DirectoryViewFactoryManager2::getInstance()->registerFactory(p->viewIdentity(), p);
            break;
        }
        case PluginInterface::VFSPlugin: {
            auto p = dynamic_cast<VFSPluginIface *>(plugin);
            VFSPluginManager::getInstance()->registerPlugin(p);
            break;
        }
        case PluginInterface::EmblemPlugin: {
            auto p = dynamic_cast<EmblemPluginInterface *>(plugin);
            EmblemProviderManager::getInstance()->registerProvider(p->create());
            break;
        }
        default:
            break;
        }
    }
}

PluginManager::~PluginManager()
{
    m_hash.clear();
    MenuPluginManager::getInstance()->close();
    //FIXME: use private deconstructor.
    DirectoryViewFactoryManager2::getInstance()->deleteLater();
    PreviewPageFactoryManager::getInstance()->deleteLater();
}

PluginManager *PluginManager::getInstance()
{
    if (!global_instance) {
        global_instance = new PluginManager;
    }
    return global_instance;
}

void PluginManager::setPluginEnableByName(const QString &name, bool enable)
{
    m_hash.value(name)->setEnable(enable);
}

void PluginManager::close()
{
    if (global_instance)
        global_instance->deleteLater();
}

void PluginManager::init()
{
    PluginManager::getInstance();
}
