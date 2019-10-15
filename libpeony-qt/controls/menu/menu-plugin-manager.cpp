#include "menu-plugin-manager.h"

#include <QDebug>

using namespace Peony;

static MenuPluginManager *global_instance = nullptr;

MenuPluginManager::MenuPluginManager(QObject *parent) : QObject(parent)
{

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
