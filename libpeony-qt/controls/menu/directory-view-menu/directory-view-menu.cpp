#include "directory-view-menu.h"

#include "menu-plugin-manager.h"
#include "file-info.h"

#include "directory-view-factory-manager.h"
#include "view-factory-model.h"

#include "clipboard-utils.h"

using namespace Peony;

DirectoryViewMenu::DirectoryViewMenu(DirectoryViewIface *directoryView, QWidget *parent) : QMenu(parent)
{
    m_view = directoryView;

    m_directory = directoryView->getDirectoryUri();
    m_selections = directoryView->getSelections();

    if (m_directory == "computer:///")
        m_is_computer = true;

    if (m_directory == "trash:///")
        m_is_trash = true;

    if (m_directory.startsWith("search://"))
        m_is_search = true;

    //add open actions
    auto openActions = constructOpenOpActions();
    if (!openActions.isEmpty())
        addSeparator();

    //add view actions
    auto viewActions = constructViewOpActions();
    if (!viewActions.isEmpty())
        addSeparator();

    //add operation actions
    auto fileOpActions = constructFileOpActions();
    if (!fileOpActions.isEmpty())
        addSeparator();

    //add plugin actions
    auto pluginActions = constructMenuPluginActions();
    if (!pluginActions.isEmpty())
        addSeparator();

    //add propertries actions
    auto propertiesAction = constructFilePropertiesActions();
    if (!propertiesAction.isEmpty())
        addSeparator();

    //add actions in computer:///
    auto computerActions = constructComputerActions();
    if (!computerActions.isEmpty())
        addSeparator();

    //add actions in trash:///
    auto trashActions = constructTrashActions();
    if (!trashActions.isEmpty())
        addSeparator();

    //add actions in search:///
    auto searchActions = constructSearchActions();
}

const QList<QAction *> DirectoryViewMenu::constructOpenOpActions()
{
    QList<QAction *> l;
    if (m_is_trash)
        return l;

    bool isBackgroundMenu = m_selections.isEmpty();
    if (isBackgroundMenu) {
        l<<addAction(QIcon::fromTheme("window-new-symbolic"), tr("Open in &New Window"));
        l<<addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open in New &Tab"));
    } else {
        if (m_selections.count() == 1) {
            auto info = FileInfo::fromUri(m_selections.first());
            if (info->isDir()) {
                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open \"%1\"").arg(info->displayName()));
                l<<addAction(QIcon::fromTheme("window-new-symbolic"), tr("Open \"%1\" in &New Window").arg(info->displayName()));
                l<<addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open \"%1\" in New &Tab").arg(info->displayName()));
            } else {
                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&open \"%1\"").arg(info->displayName()));
                auto openWithAction = addAction(tr("open \"%1\" with...").arg(info->displayName()));
                //FIXME: add sub menu for open with action.
                QMenu *openWithMenu = new QMenu(this);
                openWithMenu->addAction("app1");
                openWithMenu->addSeparator();
                openWithMenu->addAction(tr("&More applications..."));
                openWithAction->setMenu(openWithMenu);
            }
        } else {
            l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open %1 selected files").arg(m_selections.count()));
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructViewOpActions()
{
    QList<QAction *> l;

    if (m_selections.isEmpty()) {
        ViewFactoryModel model;
        model.setDirectoryUri(m_directory);
        auto viewNames = model.supportViewIds();

        auto viewFactorysManager = DirectoryViewFactoryManager::getInstance();

        if (!viewNames.isEmpty()) {
            //view type;
            auto viewTypeAction = addAction(tr("View Type..."));
            l<<viewTypeAction;
            QMenu *viewTypeSubMenu = new QMenu(this);
            for (auto viewId : viewNames) {
                auto viewType = viewTypeSubMenu->addAction(viewFactorysManager->getFactory(viewId)->viewIcon(), viewId);
                if (m_view->viewId() == viewId) {
                    viewType->setCheckable(true);
                    viewType->setChecked(true);
                }
            }
            viewTypeAction->setMenu(viewTypeSubMenu);
        }

        //sort type
        auto sortTypeAction = addAction(tr("Sort By..."));
        l<<sortTypeAction;
        QMenu *sortTypeMenu = new QMenu(this);
        //FIXME: add check for current type.
        sortTypeMenu->addAction(tr("Name"));
        sortTypeMenu->addAction(tr("File Type"));
        sortTypeMenu->addAction(tr("File Size"));
        sortTypeMenu->addAction(tr("Modified Date"));
        sortTypeAction->setMenu(sortTypeMenu);

        //sort order
        auto sortOrderAction = addAction(tr("Sort Order..."));
        l<<sortOrderAction;
        //FIXME: add check for current order
        QMenu *sortOrderMenu = new QMenu(this);
        sortOrderMenu->addAction(tr("Ascending Order"));
        sortOrderMenu->addAction(tr("Descending Order"));
        sortOrderAction->setMenu(sortOrderMenu);
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructFileOpActions()
{
    QList<QAction *> l;

    if (!m_is_trash && !m_is_search && !m_is_computer) {
        if (!m_selections.isEmpty()) {
            l<<addAction(QIcon::fromTheme("gtk-copy"), tr("&Copy"));
            l<<addAction(QIcon::fromTheme("gtk-cut"), tr("Cu&t"));
            //FIXME: force delete?
            l<<addAction(QIcon::fromTheme("user-trash"), tr("&Delete"));
        } else {
            auto pasteAction = addAction(QIcon::fromTheme("gtk-paste"), tr("&Paste"));
            l<<pasteAction;
            pasteAction->setEnabled(ClipboardUtils::isClipboardHasFiles());
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructFilePropertiesActions()
{
    QList<QAction *> l;

    if (!m_is_search)
        l<<addAction(QIcon::fromTheme("preview-file"), tr("&Properties"));

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructComputerActions()
{
    QList<QAction *> l;

    if (m_is_computer) {
        //FIXME: get the volume state and add action dynamicly.
        if (m_selections.count() == 1) {
            l<<addAction(tr("&Umount"));
            l<<addAction(tr("&Format"));
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructTrashActions()
{
    QList<QAction *> l;

    if (m_is_trash) {
        if (m_selections.isEmpty()) {
            l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("&Clean the Trash"));
        } else {
            l<<addAction(tr("&Restore"));
            l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("&Delete"));
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructSearchActions()
{
    QList<QAction *> l;
    if (m_is_search) {
        if (m_selections.isEmpty())
            return l;

        l<<addAction(QIcon::fromTheme("new-window-symbolc"), tr("Open Parent Folder in New Window"));
        l<<addAction(QIcon::fromTheme("new-tab-symbolic"), tr("Open Parent Folder in New Tab"));
    }
    return l;
}

const QList<QAction *> DirectoryViewMenu::constructMenuPluginActions()
{
    QList<QAction *> l;
    auto pluginIds = MenuPluginManager::getInstance()->getPluginIds();
    for (auto id : pluginIds) {
        auto plugin = MenuPluginManager::getInstance()->getPlugin(id);
        auto actions = plugin->menuActions(MenuPluginInterface::DirectoryView, m_directory, m_selections);
        l<<actions;
        for (auto action : actions) {
            action->setParent(this);
            addAction(action);
        }
    }
    return l;
}
