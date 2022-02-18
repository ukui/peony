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

#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"

#include "icon-view-factory.h"
#include "list-view-factory.h"

#include "global-settings.h"

#include <QDebug>

using namespace Peony;

static DirectoryViewFactoryManager2 *globalInstance2 = nullptr;

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
    m_settings = GlobalSettings::getInstance();
    m_hash = new QHash<QString, DirectoryViewPluginIface2*>();

    //register icon view and list view
    auto iconViewFactory2 = IconViewFactory2::getInstance();
    registerFactory(iconViewFactory2->viewIdentity(), iconViewFactory2);
    m_internal_views<<"Icon View";

    auto listViewFactory2 = ListViewFactory2::getInstance();
    registerFactory(listViewFactory2->viewIdentity(), listViewFactory2);
    m_internal_views<<"List View";
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

const QString DirectoryViewFactoryManager2::getDefaultViewId(const QString &uri)
{
    if (m_default_view_id_cache.isNull()) {
        auto string = m_settings->getValue(DEFAULT_VIEW_ID).toString();
        if (string.isEmpty()) {
            string = "Icon View";
        } else {
            if (!m_hash->contains(string))
                string = "Icon View";
        }
        m_default_view_id_cache = string;
    }
    return m_default_view_id_cache;
}

const QString DirectoryViewFactoryManager2::getDefaultViewId(int zoomLevel, const QString &uri)
{
    auto factorys = m_hash->values();

    auto defaultFactory = getFactory(getDefaultViewId());
    int priorty = 0;

    for (auto factory : factorys) {
        if (factory->supportUri(uri)) {
            if (factory->priority(uri) > priorty) {
                defaultFactory = factory;
                priorty = factory->priority(uri);
                continue;
            }
        }
    }

    if (!defaultFactory->supportZoom())
        return defaultFactory->viewIdentity();

    if (zoomLevel < 0)
        return getDefaultViewId(uri);

    if (defaultFactory->supportZoom()) {
        if (zoomLevel <= 40 && zoomLevel >=0) {
            defaultFactory = getFactory("List View");
        }
        if (zoomLevel >= 41) {
            defaultFactory = getFactory("Icon View");
        }
    }

    return defaultFactory->viewIdentity();
}

void DirectoryViewFactoryManager2::setDefaultViewId(const QString &viewId)
{
    if (!m_internal_views.contains(viewId))
        return;

    if (getFactoryNames().contains(viewId)) {
        m_default_view_id_cache = viewId;
        saveDefaultViewOption();
    }
}

void DirectoryViewFactoryManager2::saveDefaultViewOption()
{
    m_settings->setValue(DEFAULT_VIEW_ID, m_default_view_id_cache);
}
