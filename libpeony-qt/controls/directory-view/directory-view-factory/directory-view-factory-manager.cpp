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

#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"

#include "icon-view-factory.h"
#include "list-view-factory.h"

#include <QSettings>

#include <QDebug>

using namespace Peony;

static DirectoryViewFactoryManager *globalInstance = nullptr;
static DirectoryViewFactoryManager2 *globalInstance2 = nullptr;

DirectoryViewFactoryManager* DirectoryViewFactoryManager::getInstance()
{
    if (!globalInstance) {
        globalInstance = new DirectoryViewFactoryManager;
    }
    return globalInstance;
}

DirectoryViewFactoryManager::DirectoryViewFactoryManager(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings("UbuntuKylin Team", "Peony Qt", this);
    m_hash = new QHash<QString, DirectoryViewPluginIface*>();
    //register icon view and list view
    auto iconViewFactory = IconViewFactory::getInstance();
    registerFactory(iconViewFactory->viewIdentity(), iconViewFactory);

    auto listViewFactory = ListViewFactory::getInstance();
    registerFactory(listViewFactory->viewIdentity(), listViewFactory);
}

DirectoryViewFactoryManager::~DirectoryViewFactoryManager()
{

}

void DirectoryViewFactoryManager::registerFactory(const QString &name, DirectoryViewPluginIface *factory)
{
    if (m_hash->value(name)) {
        return;
    }
    m_hash->insert(name, factory);
}

QStringList DirectoryViewFactoryManager::getFactoryNames()
{
    return m_hash->keys();
}

DirectoryViewPluginIface *DirectoryViewFactoryManager::getFactory(const QString &name)
{
    return m_hash->value(name);
}

const QString DirectoryViewFactoryManager::getDefaultViewId()
{
    if (m_default_view_id_cache.isNull()) {
        auto string = m_settings->value("directory-view/default-view-id").toString();
        if (string.isEmpty()) {
            string = tr("Icon View");
        }
        m_default_view_id_cache = string;
    }
    return m_default_view_id_cache;
}

void DirectoryViewFactoryManager::setDefaultViewId(const QString &viewId)
{
    if (getFactoryNames().contains(viewId)) {
        m_default_view_id_cache = viewId;
    }
}

void DirectoryViewFactoryManager::saveDefaultViewOption()
{
    m_settings->setValue("directory-view/default-view-id", m_default_view_id_cache);
}

//2

DirectoryViewFactoryManager2* DirectoryViewFactoryManager2::getInstance()
{
    if (!globalInstance2) {
        globalInstance2 = new DirectoryViewFactoryManager2;
    }
    return globalInstance2;
}

DirectoryViewFactoryManager2::DirectoryViewFactoryManager2(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings("UbuntuKylin Team", "Peony Qt", this);
    m_hash = new QHash<QString, DirectoryViewPluginIface2*>();

    //register icon view and list view
    auto iconViewFactory2 = IconViewFactory2::getInstance();
    registerFactory(iconViewFactory2->viewIdentity(), iconViewFactory2);

    auto listViewFactory2 = ListViewFactory2::getInstance();
    registerFactory(listViewFactory2->viewIdentity(), listViewFactory2);
}

DirectoryViewFactoryManager2::~DirectoryViewFactoryManager2()
{

}

void DirectoryViewFactoryManager2::registerFactory(const QString &name, DirectoryViewPluginIface2 *factory)
{
    if (m_hash->value(name)) {
        return;
    }
    m_hash->insert(name, factory);
}

QStringList DirectoryViewFactoryManager2::getFactoryNames()
{
    return m_hash->keys();
}

DirectoryViewPluginIface2 *DirectoryViewFactoryManager2::getFactory(const QString &name)
{
    return m_hash->value(name);
}

const QString DirectoryViewFactoryManager2::getDefaultViewId()
{
    if (m_default_view_id_cache.isNull()) {
        auto string = m_settings->value("directory-view/default-view-id").toString();
        if (string.isEmpty()) {
            string = "Icon View";
        }
        m_default_view_id_cache = string;
    }
    return m_default_view_id_cache;
}

void DirectoryViewFactoryManager2::setDefaultViewId(const QString &viewId)
{
    if (getFactoryNames().contains(viewId)) {
        m_default_view_id_cache = viewId;
    }
}

void DirectoryViewFactoryManager2::saveDefaultViewOption()
{
    m_settings->setValue("directory-view/default-view-id", m_default_view_id_cache);
}
