#include "properties-window.h"

#include "properties-window-tab-page-plugin-iface.h"

#include "basic-properties-page-factory.h"

using namespace Peony;

//plugin manager

static PropertiesWindowPluginManager *global_instance = nullptr;

PropertiesWindowPluginManager *PropertiesWindowPluginManager::getInstance()
{
    if (!global_instance)
        global_instance = new PropertiesWindowPluginManager;
    return global_instance;
}

void PropertiesWindowPluginManager::release()
{
    deleteLater();
}

PropertiesWindowPluginManager::PropertiesWindowPluginManager(QObject *parent) : QObject (parent)
{
    //register internal factories.
    registerFactory(BasicPropertiesPageFactory::getInstance());
}

PropertiesWindowPluginManager::~PropertiesWindowPluginManager()
{
    for (auto factory : m_factory_hash) {
        factory->closeFactory();
    }
    m_factory_hash.clear();
}


bool PropertiesWindowPluginManager::registerFactory(PropertiesWindowTabPagePluginIface *factory)
{
    auto id = factory->name();
    if (m_factory_hash.value(id)) {
        return false;
    }
    m_factory_hash.insert(id, factory);
    return true;
}

const QStringList PropertiesWindowPluginManager::getFactoryNames()
{
    return m_factory_hash.keys();
}

PropertiesWindowTabPagePluginIface *PropertiesWindowPluginManager::getFactory(const QString &id)
{
    return m_factory_hash.value(id);
}

//properties window
PropertiesWindow::PropertiesWindow(const QString &uri, QWidget *parent) : QTabWidget(parent)
{
    setTabsClosable(false);
    setMovable(false);

    auto manager = PropertiesWindowPluginManager::getInstance();
    auto names = manager->getFactoryNames();
    for (auto name : names) {
        auto factory = manager->getFactory(name);
        if (factory->supportUri(uri)) {
            auto tabPage = factory->createTabPage(uri);
            tabPage->setParent(this);
            addTab(tabPage, factory->name());
        }
    }
}
