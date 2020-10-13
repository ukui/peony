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

#include "directory-view-menu.h"
#include "directory-view-plugin-iface2.h"
#include "directory-view-widget.h"

#include "fm-window.h"
#include "directory-view-container.h"

#include "menu-plugin-manager.h"
#include "file-info-job.h"
#include "file-info.h"

#include "directory-view-factory-manager.h"
#include "view-factory-model.h"
#include "view-factory-sort-filter-model.h"

#include "clipboard-utils.h"
#include "file-operation-utils.h"
#include "file-operation-manager.h" //FileOpInfo

#include "file-utils.h"
#include "bookmark-manager.h"

#include "volume-manager.h"

#include "properties-window.h"

#include "file-launch-manager.h"
#include "file-launch-action.h"
#include "file-lauch-dialog.h"

#include "file-operation-error-dialog.h"

//play audio lib head file
#include <canberra.h>

#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

#include <QLocale>
#include <QStandardPaths>
#include <recent-vfs-manager.h>

#include <QDebug>

using namespace Peony;

DirectoryViewMenu::DirectoryViewMenu(DirectoryViewWidget *directoryView, QWidget *parent) : QMenu(parent)
{
    m_top_window = nullptr;
    m_view = directoryView;

    m_directory = directoryView->getDirectoryUri();
    m_selections = directoryView->getSelections();

    fillActions();
}

DirectoryViewMenu::DirectoryViewMenu(FMWindowIface *window, QWidget *parent) : QMenu(parent)
{
    m_top_window = window;
    m_view = window->getCurrentPage()->getView();
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

    if (m_directory.startsWith("burn://"))
        m_is_cd = true;

    if (m_directory.startsWith("recent://"))
        m_is_recent = true;

    //add open actions
    auto openActions = constructOpenOpActions();
    if (!openActions.isEmpty())
        addSeparator();

    //create template actions
    auto templateActions = constructCreateTemplateActions();
    if (!templateActions.isEmpty())
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
        connect(l.last(), &QAction::triggered, [=]() {
            auto windowIface = m_top_window->create(m_directory);
            auto newWindow = dynamic_cast<QWidget *>(windowIface);
            newWindow->setAttribute(Qt::WA_DeleteOnClose);
            //FIXME: show when prepared?
            newWindow->show();
        });
        l<<addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open in New &Tab"));
        connect(l.last(), &QAction::triggered, [=]() {
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

            //FIXME: replace BLOCKING api in ui thread.
            if (displayName.isEmpty())
                displayName = FileUtils::getFileDisplayName(info->uri());
            //when name is too long, show elideText
            //qDebug() << "charWidth:" <<charWidth;
            if (displayName.length() > ELIDE_TEXT_LENGTH)
            {
                int  charWidth = fontMetrics().averageCharWidth();
                displayName = fontMetrics().elidedText(displayName, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
            }
            if (info->isDir()) {
                //add to bookmark option
                l<<addAction(QIcon::fromTheme("bookmark-add-symbolic"), tr("Add to bookmark"));
                connect(l.last(), &QAction::triggered, [=]() {
                    //qDebug() <<"add to bookmark:" <<info->uri();
                    auto bookmark = BookMarkManager::getInstance();
                    if (bookmark->isLoaded()) {
                        bookmark->addBookMark(info->uri());
                    }
                });

                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open \"%1\"").arg(displayName));
                connect(l.last(), &QAction::triggered, [=]() {
                    if (!m_top_window)
                        return;
                    m_top_window->goToUri(m_selections.first(), true);
                });
                l<<addAction(QIcon::fromTheme("window-new-symbolic"), tr("Open \"%1\" in &New Window").arg(displayName));
                connect(l.last(), &QAction::triggered, [=]() {
                    auto windowIface = m_top_window->create(m_selections.first());
                    auto newWindow = dynamic_cast<QWidget *>(windowIface);
                    newWindow->setAttribute(Qt::WA_DeleteOnClose);
                    //FIXME: show when prepared?
                    newWindow->show();
                });
                l<<addAction(QIcon::fromTheme("tab-new-symbolic"), tr("Open \"%1\" in New &Tab").arg(displayName));
                connect(l.last(), &QAction::triggered, [=]() {
                    if (!m_top_window)
                        return;
                    m_top_window->addNewTabs(m_selections);
                });
            } else if (!info->isVolume()) {
                l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open \"%1\"").arg(displayName));
                connect(l.last(), &QAction::triggered, [=]() {
                    auto uri = m_selections.first();
                    FileLaunchManager::openAsync(uri, false, false);
                });
                auto openWithAction = addAction(tr("Open \"%1\" with...").arg(displayName));
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
                openWithMenu->addAction(tr("&More applications..."), [=]() {
                    FileLauchDialog d(m_selections.first());
                    d.exec();
                });
                openWithAction->setMenu(openWithMenu);
            } else {
                l<<addAction(tr("&Open"));
                connect(l.last(), &QAction::triggered, [=]() {
                    auto uri = m_selections.first();
                    //FIXME:
                    m_top_window->goToUri(uri, true);
                });
            }
        } else {
            l<<addAction(QIcon::fromTheme("document-open-symbolic"), tr("&Open %1 selected files").arg(m_selections.count()));
            connect(l.last(), &QAction::triggered, [=]() {
                qDebug()<<"triggered";
                QStringList dirs;
                QMap<QString, QStringList> fileMap;
                /**step 1: Categorize files according to type.
                 * step 2: Open files in batches to avoid loss of asynchronous messages due to program startup.
                **/
                for (auto uri : m_selections) {
                    auto info = FileInfo::fromUri(uri);
                    if (info->isDir() || info->isVolume()) {
                        dirs<<uri;
                    } else {
                        QString mimeType = info->mimeType();
                        if (mimeType.isEmpty()) {
                            FileInfoJob job(info);
                            job.querySync();
                            mimeType = info->mimeType();
                        }

                        QStringList list;
                        if (fileMap.contains(mimeType)) {
                            list = fileMap[mimeType];
                            list << uri;
                            fileMap.insert(mimeType, list);
                        } else {
                            list << uri;
                            fileMap.insert(mimeType, list);
                        }
                    }
                }
                if (!dirs.isEmpty())
                    m_top_window->addNewTabs(dirs);

                if(!fileMap.empty()) {
                    QMap<QString, QStringList>::iterator iter = fileMap.begin();
                    while (iter != fileMap.end())
                    {
                        FileLaunchManager::openAsync(iter.value());
                        iter++;
                    }
                }
            });
        }
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructCreateTemplateActions()
{
    QList<QAction *> l;
    if (m_selections.isEmpty()) {
        auto createAction = new QAction(tr("New..."), this);
        if (m_is_cd) {
            createAction->setEnabled(false);
        }
        //fix create folder fail issue in special path
        auto info = FileInfo::fromUri(m_directory, false);
        FileInfoJob job(info);
        job.querySync();
        if (! info->canWrite())
        {
            createAction->setEnabled(false);
        }
        l<<createAction;
        QMenu *subMenu = new QMenu(this);
        createAction->setMenu(subMenu);
        addAction(createAction);

        //enumerate template dir
        QDir templateDir(g_get_user_special_dir(G_USER_DIRECTORY_TEMPLATES));
        auto templates = templateDir.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
        if (!templates.isEmpty()) {
            for (auto t : templates) {
                QFileInfo qinfo(templateDir, t);
                GFile *gtk_file = g_file_new_for_path(qinfo.filePath().toUtf8().data());
                char *uri_str = g_file_get_uri(gtk_file);
                std::shared_ptr<FileInfo> info = FileInfo::fromUri(uri_str);

                QString mimeType = info->mimeType();
                if (mimeType.isEmpty()) {
                    FileInfoJob job(info);
                    job.querySync();
                    mimeType = info->mimeType();
                }

                QIcon tmpIcon;
                GList *app_infos = g_app_info_get_recommended_for_type(mimeType.toUtf8().constData());
                GList *l = app_infos;
                QList<FileLaunchAction *> actions;
                bool isOnlyUnref = false;
                while (l) {
                    auto app_info = static_cast<GAppInfo*>(l->data);
                    if (!isOnlyUnref) {
                        GThemedIcon *icon = G_THEMED_ICON(g_app_info_get_icon(app_info));
                        const char * const * icon_names = g_themed_icon_get_names(icon);
                        if (icon_names)
                            tmpIcon = QIcon::fromTheme(*icon_names);
                        if(!tmpIcon.isNull())
                            isOnlyUnref = true;
                    }
                    g_object_unref(app_infos);
                    l = l->next;
                }

                QAction *action = new QAction(tmpIcon, qinfo.baseName(), this);
                connect(action, &QAction::triggered, [=]() {
                    // automatically check for conficts
                    CreateTemplateOperation op(m_directory, CreateTemplateOperation::Template, t);
                    Peony::FileOperationErrorDialogWarning dlg;
                    connect(&op, &Peony::FileOperation::errored, &dlg, &Peony::FileOperationErrorDialogWarning::handle);
                    op.run();
                    auto target = op.target();
                    m_uris_to_edit<<target;
                });
                subMenu->addAction(action);
                g_free(uri_str);
                g_object_unref(gtk_file);
            }
            subMenu->addSeparator();
        }

        QList<QAction *> actions;
        auto createEmptyFileAction = new QAction(QIcon::fromTheme("document-new-symbolic"), tr("Empty &File"), this);
        actions<<createEmptyFileAction;
        connect(actions.last(), &QAction::triggered, [=]() {
            //FileOperationUtils::create(m_directory);
            CreateTemplateOperation op(m_directory);
            Peony::FileOperationErrorDialogConflict dlg;
            connect(&op, &Peony::FileOperation::errored, &dlg, &Peony::FileOperationErrorDialogConflict::handle);
            op.run();
            auto targetUri = op.target();
            qDebug()<<"target:"<<targetUri;
            m_uris_to_edit<<targetUri;
        });
        auto createFolderActions = new QAction(QIcon::fromTheme("folder-new-symbolic"), tr("&Folder"), this);
        actions<<createFolderActions;
        connect(actions.last(), &QAction::triggered, [=]() {
            //FileOperationUtils::create(m_directory, nullptr, CreateTemplateOperation::EmptyFolder);
            CreateTemplateOperation op(m_directory, CreateTemplateOperation::EmptyFolder, tr("New Folder"));
            Peony::FileOperationErrorDialogConflict dlg;
            connect(&op, &Peony::FileOperation::errored, &dlg, &Peony::FileOperationErrorDialogConflict::handle);
            op.run();
            auto targetUri = op.target();
            qDebug()<<"target:"<<targetUri;
            m_uris_to_edit<<targetUri;
        });
        subMenu->addActions(actions);
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructViewOpActions()
{
    QList<QAction *> l;

    if (m_selections.isEmpty()) {
        ViewFactorySortFilterModel2 model;
        model.setDirectoryUri(m_directory);
        auto viewNames = model.supportViewIds();
        auto viewFactorysManager = DirectoryViewFactoryManager2::getInstance();

        if (!viewNames.isEmpty()) {
            //view type;
            auto viewTypeAction = addAction(tr("View Type..."));
            l<<viewTypeAction;
            QMenu *viewTypeSubMenu = new QMenu(this);
            for (auto viewId : viewNames) {
                auto viewType = viewTypeSubMenu->addAction(viewFactorysManager->getFactory(viewId)->viewIcon(), viewFactorysManager->getFactory(viewId)->viewName());
                viewType->setData(viewId);
                if (m_view->viewId() == viewId) {
                    viewType->setCheckable(true);
                    viewType->setChecked(true);
                } else {
                    connect(viewType, &QAction::triggered, [=]() {
                        m_top_window->beginSwitchView(viewType->data().toString());
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
        tmp<<sortTypeMenu->addAction(tr("Modified Date"));
        tmp<<sortTypeMenu->addAction(tr("File Type"));
        tmp<<sortTypeMenu->addAction(tr("File Size"));
        int sortType = m_view->getSortType();
        if (sortType >= 0) {
            tmp.at(sortType)->setCheckable(true);
            tmp.at(sortType)->setChecked(true);
        }

        for (int i = 0; i < tmp.count(); i++) {
            connect(tmp.at(i), &QAction::triggered, [=]() {
                m_top_window->setCurrentSortColumn(i);
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
            connect(tmp.at(i), &QAction::triggered, [=]() {
                m_top_window->setCurrentSortOrder(Qt::SortOrder(i));
            });
        }

        sortOrderAction->setMenu(sortOrderMenu);

        auto sortPreferencesAction = addAction(tr("Sort Preferences..."));
        l<<sortPreferencesAction;

        auto sortPreferencesMenu = new QMenu(this);
        auto folderFirst = sortPreferencesMenu->addAction(tr("Folder First"));
        folderFirst->setCheckable(true);
        folderFirst->setChecked(m_top_window->getWindowSortFolderFirst());
        connect(folderFirst, &QAction::triggered, this, [=]() {
            m_top_window->setSortFolderFirst();
            folderFirst->setChecked(m_top_window->getWindowSortFolderFirst());
        });

        if (QLocale::system().name().contains("zh")) {
            auto useDefaultNameSortOrder = sortPreferencesMenu->addAction(tr("Chinese First"));
            useDefaultNameSortOrder->setCheckable(true);
            useDefaultNameSortOrder->setChecked(m_top_window->getWindowUseDefaultNameSortOrder());
            connect(useDefaultNameSortOrder, &QAction::triggered, this, [=]() {
                m_top_window->setUseDefaultNameSortOrder();
                bool checked = m_top_window->getWindowUseDefaultNameSortOrder();
                useDefaultNameSortOrder->setChecked(checked);
            });
        }

        auto showHidden = sortPreferencesMenu->addAction(tr("Show Hidden"));
        showHidden->setCheckable(true);
        showHidden->setChecked(m_top_window->getWindowShowHidden());
        connect(showHidden, &QAction::triggered, this, [=]() {
            m_top_window->setShowHidden();
            showHidden->setChecked(m_top_window->getWindowShowHidden());
        });

        sortPreferencesAction->setMenu(sortPreferencesMenu);
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructFileOpActions()
{
    QList<QAction *> l;

    if (!m_is_trash && !m_is_search && !m_is_computer) {
        QString homeUri = "file://" +  QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if (!m_selections.isEmpty() && !m_selections.contains(homeUri)) {
            l<<addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("&Copy"));
            connect(l.last(), &QAction::triggered, [=]() {
                ClipboardUtils::setClipboardFiles(m_selections, false);
            });
            l<<addAction(QIcon::fromTheme("edit-cut-symbolic"), tr("Cut"));
            connect(l.last(), &QAction::triggered, [=]() {
                ClipboardUtils::setClipboardFiles(m_selections, true);
            });
            if (!m_is_recent) {
                l<<addAction(QIcon::fromTheme("edit-delete-symbolic"), tr("&Delete to trash"));
                connect(l.last(), &QAction::triggered, [=]() {
                    FileOperationUtils::trash(m_selections, true);
                });
            }
            //add delete forever option
            l<<addAction(QIcon::fromTheme("edit-clear-symbolic"), tr("Delete forever"));
            connect(l.last(), &QAction::triggered, [=]() {
                FileOperationUtils::executeRemoveActionWithDialog(m_selections);
            });
            if (m_selections.count() == 1) {
                l<<addAction(QIcon::fromTheme("document-edit-symbolic"), tr("Rename"));
                connect(l.last(), &QAction::triggered, [=]() {
                    m_view->editUri(m_selections.first());
                });
            }
        } else {
            auto pasteAction = addAction(QIcon::fromTheme("edit-paste-symbolic"), tr("&Paste"));
            l<<pasteAction;
            pasteAction->setEnabled(ClipboardUtils::isClipboardHasFiles());
            connect(l.last(), &QAction::triggered, [=]() {
                auto op = ClipboardUtils::pasteClipboardFiles(m_directory);
                if (op) {
                    auto window = dynamic_cast<QWidget *>(m_top_window);
                    auto iface = m_top_window;
                    connect(op, &Peony::FileOperation::operationFinished, window, [=](){
                        auto opInfo = op->getOperationInfo();
                        auto targetUirs = opInfo->dests();
                        iface->setCurrentSelectionUris(targetUirs);
                    }, Qt::BlockingQueuedConnection);
                }
            });
            l<<addAction(QIcon::fromTheme("view-refresh-symbolic"), tr("&Refresh"));
            connect(l.last(), &QAction::triggered, [=]() {
                m_top_window->refresh();
            });
        }
    }

    //select all and reverse select
    if (m_selections.isEmpty())
    {
        l<<addAction(tr("Select &All"));
        connect(l.last(), &QAction::triggered, [=]() {
            //qDebug() << "select all";
            m_view->invertSelections();
        });
    }
    else
    {
        l<<addAction(tr("Reverse Select"));
        connect(l.last(), &QAction::triggered, [=]() {
            //qDebug() << "Reverse select";
            m_view->invertSelections();
        });
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructFilePropertiesActions()
{
    QList<QAction *> l;

    if (!m_is_search) {
        l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"));
        connect(l.last(), &QAction::triggered, [=]() {
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
    } else if (m_selections.count() == 1) {
        l<<addAction(QIcon::fromTheme("preview-file"), tr("Properties"));
        connect(l.last(), &QAction::triggered, [=]() {
            PropertiesWindow *p = new PropertiesWindow(m_selections);
            p->setAttribute(Qt::WA_DeleteOnClose);
            p->show();
        });
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructComputerActions()
{
    QList<QAction *> l;

    if (m_is_computer) {
        /*
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
        */
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
            connect(l.last(), &QAction::triggered, [=]() {
                ca_context *caContext;
                ca_context_create(&caContext);
                const gchar* eventId = "dialog-warning";
                //eventid 是/usr/share/sounds音频文件名,不带后缀
                ca_context_play (caContext, 0,
                                 CA_PROP_EVENT_ID, eventId,
                                 CA_PROP_EVENT_DESCRIPTION, tr("Delete file Warning"), NULL);

                auto result = QMessageBox::question(nullptr, tr("Delete Permanently"), tr("Are you sure that you want to delete these files? "
                                                                                          "Once you start a deletion, the files deleting will never be "
                                                                                          "restored again."));
                if (result == QMessageBox::Yes) {
                    auto uris = m_top_window->getCurrentAllFileUris();
                    FileOperationUtils::remove(uris);
                }
            });
        } else {
            l<<addAction(tr("&Restore"));
            connect(l.last(), &QAction::triggered, [=]() {
                if (m_selections.count() == 1) {
                    FileOperationUtils::restore(m_selections.first());
                } else {
                    FileOperationUtils::restore(m_selections);
                }
            });
            l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("&Delete"));
            connect(l.last(), &QAction::triggered, [=]() {
                ca_context *caContext;
                ca_context_create(&caContext);
                const gchar* eventId = "dialog-warning";
                //eventid 是/usr/share/sounds音频文件名,不带后缀
                ca_context_play (caContext, 0,
                                 CA_PROP_EVENT_ID, eventId,
                                 CA_PROP_EVENT_DESCRIPTION, tr("Delete file Warning"), NULL);

                auto result = QMessageBox::question(nullptr, tr("Delete Permanently"), tr("Are you sure that you want to delete these files? "
                                                                                          "Once you start a deletion, the files deleting will never be "
                                                                                          "restored again."));
                if (result == QMessageBox::Yes) {
                    FileOperationUtils::remove(m_selections);
                }
            });
        }
    } else if (m_is_recent && m_selections.isEmpty()) {
        l<<addAction(tr("Clean All"));
        connect(l.last(), &QAction::triggered, [=]() {
            RecentVFSManager::getInstance()->clearAll();
        });
    }

    return l;
}

const QList<QAction *> DirectoryViewMenu::constructSearchActions()
{
    QList<QAction *> l;
    if (m_is_search || m_is_recent) {
        if (m_selections.isEmpty())
            return l;

        l<<addAction(QIcon::fromTheme("new-window-symbolc"), tr("Open Parent Folder in New Window"));
        connect(l.last(), &QAction::triggered, [=]() {
            for (auto uri : m_selections) {
                //FIXME: replace BLOCKING api in ui thread.
                if (m_is_recent)
                    uri = FileUtils::getTargetUri(uri);
                auto parentUri = FileUtils::getParentUri(uri);
                if (!parentUri.isNull()) {
                    auto *windowIface = m_top_window->create(parentUri);
                    auto newWindow = dynamic_cast<QWidget *>(windowIface);
                    auto selection = m_selections;
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
                    QTimer::singleShot(1000, newWindow, [=]() {
                        if (newWindow)
                            windowIface->setCurrentSelectionUris(selection);
                    });
#else
                    QTimer::singleShot(1000, [=]() {
                        if (newWindow)
                            windowIface->setCurrentSelectionUris(selection);
                    });
#endif
                    newWindow->show();
                }
            }
        });
    }
    return l;
}

const QList<QAction *> DirectoryViewMenu::constructMenuPluginActions()
{
    QList<QAction *> l;
    auto pluginIds = MenuPluginManager::getInstance()->getPluginIds();
    //sort plugiins by name, so the menu option orders is relatively fixed
    qSort(pluginIds.begin(), pluginIds.end());

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
