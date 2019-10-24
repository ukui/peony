#include "directory-view-menu.h"

#include "fm-window.h"
#include "directory-view-container.h"

#include "menu-plugin-manager.h"
#include "file-info.h"

#include "directory-view-factory-manager.h"
#include "view-factory-model.h"
#include "view-factory-sort-filter-model.h"

#include "clipboard-utils.h"
#include "file-operation-utils.h"

#include "file-utils.h"

#include "volume-manager.h"

#include "properties-window.h"

#include "file-launch-manager.h"
#include "file-launch-action.h"

#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

#include <QDebug>

using namespace Peony;

DirectoryViewMenu::DirectoryViewMenu(DirectoryViewIface *directoryView, QWidget *parent) : QMenu(parent)
{
    m_top_window = nullptr;
    m_view = directoryView;

    m_directory = directoryView->getDirectoryUri();
    m_selections = directoryView->getSelections();

    fillActions();
}

DirectoryViewMenu::DirectoryViewMenu(FMWindow *window, QWidget *parent) : QMenu(parent)
{
    m_top_window = window;
    m_view = window->getCurrentPage()->getProxy()->getView();
    //setParent(dynamic_cast<QWidget*>(m_view));

    m_directory = window->getCurrentUri();
    m_selections = window->getCurrentSelections();

    fillActions();
}

void DirectoryViewMenu::fillActions()
{
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
        connect(l.last(), &QAction::triggered, [=](){
            FMWindow *newWindow = new FMWindow(m_directory);
            newWindow->setAttribute(Qt::WA_DeleteOnClose);
            //FIXME: show when prepared?
            newWindow->show();
        });
        l<<addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open in New &Tab"));
        connect(l.last(), &QAction::triggered, [=](){
            if (!m_top_window)
                return;
            QStringList uris;
            uris<<m_directory;
            m_top_window->addNewTabs(uris);
        });
    } else {
        if (m_selections.count() == 1) {
            auto info = FileInfo::fromUri(m_selections.first());
            auto displayName = info->displayName();
            if (displayName.isEmpty())
                displayName = FileUtils::getFileDisplayName(info->uri());
            if (info->isDir()) {
                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open \"%1\"").arg(displayName));
                connect(l.last(), &QAction::triggered, [=](){
                    if (!m_top_window)
                        return;
                    m_top_window->goToUri(m_selections.first(), true);
                });
                l<<addAction(QIcon::fromTheme("window-new-symbolic"), tr("Open \"%1\" in &New Window").arg(displayName));
                connect(l.last(), &QAction::triggered, [=](){
                    FMWindow *newWindow = new FMWindow(m_selections.first());
                    newWindow->setAttribute(Qt::WA_DeleteOnClose);
                    //FIXME: show when prepared?
                    newWindow->show();
                });
                l<<addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open \"%1\" in New &Tab").arg(displayName));
                connect(l.last(), &QAction::triggered, [=](){
                    if (!m_top_window)
                        return;
                    m_top_window->addNewTabs(m_selections);
                });
            } else if (!info->isVolume()) {
                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&open \"%1\"").arg(displayName));
                connect(l.last(), &QAction::triggered, [=](){
                    auto uri = m_selections.first();
                    FileLaunchManager::openAsync(uri);
                });
                auto openWithAction = addAction(tr("open \"%1\" with...").arg(displayName));
                //FIXME: add sub menu for open with action.
                QMenu *openWithMenu = new QMenu(this);
                auto recommendActions = FileLaunchManager::getRecommendActions(m_selections.first());
                for (auto action : recommendActions) {
                    action->setParent(openWithMenu);
                    openWithMenu->addAction(static_cast<QAction*>(action));
                }
                auto fallbackActions = FileLaunchManager::getFallbackActions(m_selections.first());
                for (auto action : fallbackActions) {
                    action->setParent(openWithMenu);
                    openWithMenu->addAction(static_cast<QAction*>(action));
                }
                openWithMenu->addSeparator();
                openWithMenu->addAction(tr("&More applications..."));
                openWithAction->setMenu(openWithMenu);
            } else {
                l<<addAction(tr("&Open"));
                connect(l.last(), &QAction::triggered, [=](){
                    auto uri = m_selections.first();
                    //FIXME:
                });
            }
        } else {
            l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open %1 selected files").arg(m_selections.count()));
            connect(l.last(), &QAction::triggered, [=](){
                qDebug()<<"triggered";
                QStringList dirs;
                QStringList files;
                for (auto uri : m_selections) {
                    auto info = FileInfo::fromUri(uri);
                    if (info->isDir() || info->isVolume()) {
                        dirs<<uri;
                    } else {
                        files<<uri;
                    }
                }
                if (!dirs.isEmpty())
                    m_top_window->addNewTabs(dirs);
                if (!files.isEmpty()) {
                    for (auto uri : files) {
                        /*!
                         * \bug
                         * open muti-files has bug some times.
                         */
                        FileLaunchManager::openAsync(uri);
                    }
                }
            });
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructViewOpActions()
{
    QList<QAction *> l;

    if (m_selections.isEmpty()) {
        ViewFactorySortFilterModel model;
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
                } else {
                    connect(viewType, &QAction::triggered, [=](){
                        m_top_window->beginSwitchView(viewType->text());
                    });
                }
            }
            viewTypeAction->setMenu(viewTypeSubMenu);
        }

        //sort type
        auto sortTypeAction = addAction(tr("Sort By..."));
        l<<sortTypeAction;
        QMenu *sortTypeMenu = new QMenu(this);

        QList<QAction *> tmp;
        tmp<<sortTypeMenu->addAction(tr("Name"));
        tmp<<sortTypeMenu->addAction(tr("File Type"));
        tmp<<sortTypeMenu->addAction(tr("File Size"));
        tmp<<sortTypeMenu->addAction(tr("Modified Date"));
        int sortType = m_view->getSortType();
        if (sortType >= 0) {
            tmp.at(sortType)->setCheckable(true);
            tmp.at(sortType)->setChecked(true);
        }

        for (int i = 0; i < tmp.count(); i++) {
            connect(tmp.at(i), &QAction::triggered, [=](){
                m_view->setSortType(i);
            });
        }

        sortTypeAction->setMenu(sortTypeMenu);

        //sort order
        auto sortOrderAction = addAction(tr("Sort Order..."));
        l<<sortOrderAction;
        QMenu *sortOrderMenu = new QMenu(this);
        tmp.clear();
        tmp<<sortOrderMenu->addAction(tr("Ascending Order"));
        tmp<<sortOrderMenu->addAction(tr("Descending Order"));
        int sortOrder = m_view->getSortOrder();
        tmp.at(sortOrder)->setCheckable(true);
        tmp.at(sortOrder)->setChecked(true);

        for (int i = 0; i < tmp.count(); i++) {
            connect(tmp.at(i), &QAction::triggered, [=](){
                m_view->setSortOrder(i);
            });
        }

        sortOrderAction->setMenu(sortOrderMenu);
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructFileOpActions()
{
    QList<QAction *> l;

    if (!m_is_trash && !m_is_search && !m_is_computer) {
        if (!m_selections.isEmpty()) {
            l<<addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("&Copy"));
            connect(l.last(), &QAction::triggered, [=](){
                ClipboardUtils::setClipboardFiles(m_selections, false);
            });
            l<<addAction(QIcon::fromTheme("edit-cut-symbolic"), tr("Cu&t"));
            connect(l.last(), &QAction::triggered, [=](){
                ClipboardUtils::setClipboardFiles(m_selections, true);
            });
            l<<addAction(QIcon::fromTheme("edit-delete-symbolic"), tr("&Delete"));
            connect(l.last(), &QAction::triggered, [=](){
                FileOperationUtils::trash(m_selections, true);
            });
            if (m_selections.count() == 1) {
                l<<addAction(QIcon::fromTheme("document-edit-symbolic"), tr("&Rename"));
                connect(l.last(), &QAction::triggered, [=](){
                    m_view->editUri(m_selections.first());
                });
            }
        } else {
            auto pasteAction = addAction(QIcon::fromTheme("edit-paste-symbolic"), tr("&Paste"));
            l<<pasteAction;
            pasteAction->setEnabled(ClipboardUtils::isClipboardHasFiles());
            connect(l.last(), &QAction::triggered, [=](){
                ClipboardUtils::pasteClipboardFiles(m_directory);
            });
            l<<addAction(QIcon::fromTheme("view-refresh-symbolic"), tr("&Refresh"));
            connect(l.last(), &QAction::triggered, [=](){
                m_top_window->refresh();
            });
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructFilePropertiesActions()
{
    QList<QAction *> l;

    if (!m_is_search) {
        l<<addAction(QIcon::fromTheme("preview-file"), tr("&Properties"));
        connect(l.last(), &QAction::triggered, [=](){
            //FIXME:
            if (m_selections.isEmpty()) {
                QStringList uris;
                uris<<m_directory;
                PropertiesWindow *p = new PropertiesWindow(uris);
                p->setAttribute(Qt::WA_DeleteOnClose);
                p->show();
            } else {
                PropertiesWindow *p = new PropertiesWindow(m_selections);
                p->setAttribute(Qt::WA_DeleteOnClose);
                p->show();
            }
        });
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructComputerActions()
{
    QList<QAction *> l;

    if (m_is_computer) {
        //FIXME: get the volume state and add action dynamicly.
        if (m_selections.count() == 1) {
            l<<addAction(tr("&Umount"));
            connect(l.last(), &QAction::triggered, [=](){
                VolumeManager::unmount(m_selections.first());
            });
            l<<addAction(tr("&Format"));
            connect(l.last(), &QAction::triggered, [=](){
                //FIXME:
                //add format function?
                //maybe put it in plugin...
            });
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructTrashActions()
{
    QList<QAction *> l;

    if (m_is_trash) {
        bool isTrashEmpty = m_top_window->getCurrentAllFileUris().isEmpty();

        if (m_selections.isEmpty()) {
            l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("&Clean the Trash"));
            l.last()->setEnabled(!isTrashEmpty);
            connect(l.last(), &QAction::triggered, [=](){
                auto result = QMessageBox::question(nullptr, tr("Delete Permanently"), tr("Are you sure that you want to delete these files? "
                                                                                          "Once you start a deletion, the files deleting will never be "
                                                                                          "restored again."));
                if (result == QMessageBox::Ok) {
                    auto uris = m_top_window->getCurrentAllFileUris();
                }
            });
        } else {
            l<<addAction(tr("&Restore"));
            connect(l.last(), &QAction::triggered, [=](){
                if (m_selections.count() == 1) {
                    FileOperationUtils::restore(m_selections.first());
                } else {
                    FileOperationUtils::restore(m_selections);
                }
            });
            l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("&Delete"));
            connect(l.last(), &QAction::triggered, [=](){
                auto result = QMessageBox::question(nullptr, tr("Delete Permanently"), tr("Are you sure that you want to delete these files? "
                                                                                          "Once you start a deletion, the files deleting will never be "
                                                                                          "restored again."));
                if (result == QMessageBox::Ok) {
                    FileOperationUtils::remove(m_selections);
                }
            });
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
        connect(l.last(), &QAction::triggered, [=](){
            for (auto uri : m_selections) {
                FMWindow *newWindow = new FMWindow(uri);
                newWindow->show();
            }
        });
        l<<addAction(QIcon::fromTheme("new-tab-symbolic"), tr("Open Parent Folder in New Tab"));
        connect(l.last(), &QAction::triggered, [=](){
            m_top_window->addNewTabs(m_selections);
        });
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
