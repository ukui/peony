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
