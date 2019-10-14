#include "directory-view-menu.h"

#include "menu-plugin-manager.h"

using namespace Peony;

DirectoryViewMenu::DirectoryViewMenu(DirectoryViewIface *directoryView, QWidget *parent) : QMenu(parent)
{
    auto directory = directoryView->getDirectoryUri();
    auto selection = directoryView->getSelections();

    bool isTrash = directory.startsWith("trash://");
    bool isComputer = directory.startsWith("computer://");
    bool isSearch = directory.startsWith("search://");
    bool isBlank = selection.isEmpty();

    //add open actions

    //add view actions

    //add operation actions

    //add plugin actions
    auto pluginIds = MenuPluginManager::getInstance()->getPluginIds();
    for (auto id : pluginIds) {
        auto plugin = MenuPluginManager::getInstance()->getPlugin(id);
        auto actions = plugin->menuActions(MenuPluginInterface::DirectoryView, directory, selection);
        for (auto action : actions) {
            action->setParent(this);
            addAction(action);
        }
    }

    //add propertries actions

    //add search
}
