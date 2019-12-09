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

#include "tool-bar.h"
#include "search-bar.h"
#include "fm-window.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include "directory-view-plugin-iface2.h"
#include "directory-view-widget.h"
#include "clipboard-utils.h"
#include "file-operation-utils.h"

#include "view-factory-model.h"
#include "view-factory-sort-filter-model.h"
#include "directory-view-container.h"

#include <QAction>
#include <QComboBox>
#include <QPushButton>

#include <QStandardPaths>

#include <QMessageBox>

#include <QMenu>

#include <QDebug>

using namespace Peony;

ToolBar::ToolBar(FMWindow *window, QWidget *parent) : QToolBar(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setFixedHeight(50);

    m_top_window = window;
    init();
}

void ToolBar::init()
{
    //layout
    QAction *newWindowAction = addAction(QIcon::fromTheme("window-new-symbolic", QIcon::fromTheme("folder")),
                                         tr("Open in new &Window"));
    QAction *newTabActon = addAction(QIcon::fromTheme("tab-new-symbolic", QIcon::fromTheme("folder")),
                                     tr("Open in new &Tab"));

    addSeparator();

    //view switch
    //FIXME: how about support uri?
    auto viewManager = DirectoryViewFactoryManager::getInstance();

    auto defaultViewId = viewManager->getDefaultViewId();

    auto model = new ViewFactorySortFilterModel2(this);
    m_view_factory_model = model;
    model->setDirectoryUri("file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation));

    /*
    QComboBox *viewCombox = new QComboBox(this);
    m_view_option_box = viewCombox;
    viewCombox->setToolTip(tr("Change Directory View"));

    viewCombox->setModel(model);

    addWidget(viewCombox);
    connect(viewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){
        auto viewId = viewCombox->itemData(index, Qt::ToolTipRole).toString();
        m_top_window->beginSwitchView(viewId);
    });

    addSeparator();
    */

    m_view_action = new QAction(m_view_factory_model->iconFromViewId(m_top_window->getCurrentPageViewType()),
                                m_top_window->getCurrentPageViewType(), this);
    m_view_menu = new QMenu(this);
    m_view_action->setMenu(m_view_menu);
    connect(m_view_action, &QAction::triggered, [=](){
        auto point = this->widgetForAction(m_view_action)->geometry().bottomLeft();
        auto global_point = mapToGlobal(point);
        m_view_menu->exec(global_point);
    });
    connect(m_view_menu, &QMenu::aboutToShow, [=](){
        for (auto id : m_view_factory_model->supportViewIds()) {
            auto action = m_view_menu->addAction(m_view_factory_model->iconFromViewId(id), id, [=](){
                m_top_window->getCurrentPage()->switchViewType(id);
            });
            if (id == m_top_window->getCurrentPageViewType()) {
                action->setCheckable(true);
                action->setChecked(true);
            }
        }
    });
    connect(m_view_menu, &QMenu::aboutToHide, [=](){
        for (auto action : m_view_menu->actions()) {
            action->deleteLater();
        }
    });

    addAction(m_view_action);
    addSeparator();

    //sort type
    /*!
      \todo
      make column extensionable.
      */
    m_sort_action = new QAction(QIcon::fromTheme("view-sort-ascending-symbolic"), tr("Sort Type"), this);
    QMenu *sortMenu = new QMenu(this);
    sortMenu->addAction(tr("File Name"), [=](){
        m_top_window->setCurrentSortColumn(0);
    });
    sortMenu->addAction(tr("File Type"), [=](){
        m_top_window->setCurrentSortColumn(1);
    });
    sortMenu->addAction(tr("File Size"), [=](){
        m_top_window->setCurrentSortColumn(2);
    });
    sortMenu->addAction(tr("Modified Date"), [=](){
        m_top_window->setCurrentSortColumn(3);
    });

    sortMenu->addSeparator();

    sortMenu->addAction(tr("Ascending"), [=](){
        m_top_window->setCurrentSortOrder(Qt::AscendingOrder);
        m_sort_action->setIcon(QIcon::fromTheme("view-sort-ascending-symbolic"));
    });
    sortMenu->addAction(tr("Descending"), [=]{
        m_top_window->setCurrentSortOrder(Qt::DescendingOrder);
        m_sort_action->setIcon(QIcon::fromTheme("view-sort-descending-symbolic"));
    });

    m_sort_action->setMenu(sortMenu);
    addAction(m_sort_action);

    connect(m_sort_action, &QAction::triggered, [=](){
        auto point = this->widgetForAction(m_sort_action)->geometry().bottomLeft();
        auto global_point = mapToGlobal(point);
        sortMenu->exec(global_point);
    });

    connect(sortMenu, &QMenu::aboutToShow, [=](){
        for (auto action : sortMenu->actions()) {
            action->setCheckable(false);
            action->setChecked(false);
        }
        int column = m_top_window->getCurrentSortColumn();
        int order = m_top_window->getCurrentSortOrder();
        sortMenu->actions().at(column)->setCheckable(true);
        sortMenu->actions().at(column)->setChecked(true);
        sortMenu->actions().at(order + 5)->setCheckable(true);
        sortMenu->actions().at(order + 5)->setChecked(true);
    });
    addSeparator();

    //file operations
    QAction *copyAction = addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("Copy"));
    copyAction->setShortcut(QKeySequence::Copy);

    QAction *pasteAction = addAction(QIcon::fromTheme("edit-paste-symbolic"), tr("Paste"));
    pasteAction->setShortcut(QKeySequence::Paste);

    QAction *cutAction = addAction(QIcon::fromTheme("edit-cut-symbolic"), tr("Cut"));
    cutAction->setShortcut(QKeySequence::Cut);

    QAction *trashAction = addAction(QIcon::fromTheme("edit-delete-symbolic"), tr("Trash"));
    trashAction->setShortcut(QKeySequence::Delete);

    m_file_op_actions<<copyAction<<pasteAction<<cutAction<<trashAction;

    addSeparator();

    //advance usage
    //...

    //separator widget

    //extension

    //other options?

    //trash
    m_clean_trash_action = addAction(QIcon::fromTheme("edit-clear-symbolic"), tr("Clean Trash"), [=](){
        auto result = QMessageBox::question(nullptr, tr("Delete Permanently"), tr("Are you sure that you want to delete these files? "
                                                                                  "Once you start a deletion, the files deleting will never be "
                                                                                  "restored again."));
        if (result == QMessageBox::Yes) {
            auto uris = m_top_window->getCurrentAllFileUris();
            qDebug()<<uris;
            FileOperationUtils::remove(uris);
        }
    });

    m_restore_action = addAction(QIcon::fromTheme("view-refresh-symbolic"), tr("Restore"), [=](){
        FileOperationUtils::restore(m_top_window->getCurrentSelections());
    });

    //connect signal
    connect(newWindowAction, &QAction::triggered, [=](){
        if (m_top_window->getCurrentSelections().isEmpty()) {
            FMWindow *newWindow = new FMWindow(m_top_window->getCurrentUri());
            newWindow->show();
            //FIXME: show when prepared
        } else {
            for (auto uri : m_top_window->getCurrentSelections()) {
                FMWindow *newWindow = new FMWindow(m_top_window->getCurrentUri());
                newWindow->show();
                //FIXME: show when prepared
            }
        }
    });

    connect(newTabActon, &QAction::triggered, [=](){
        QStringList l;
        if (m_top_window->getCurrentSelections().isEmpty()) {
            l<<m_top_window->getCurrentUri();
        } else {
            l = m_top_window->getCurrentSelections();
        }
        m_top_window->addNewTabs(l);
    });

    /*
    connect(viewCombox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), [=](const QString &text){
        Q_EMIT this->optionRequest(SwitchView);
        //FIXME: i have to add interface to view proxy for view switch.
    });
    */

    connect(copyAction, &QAction::triggered, [=](){
        if (!m_top_window->getCurrentSelections().isEmpty())
            ClipboardUtils::setClipboardFiles(m_top_window->getCurrentSelections(), false);
    });

    connect(pasteAction, &QAction::triggered, [=](){
        if (ClipboardUtils::isClipboardHasFiles()) {
            //FIXME: how about duplicated copy?
            //FIXME: how to deal with a failed move?
            ClipboardUtils::pasteClipboardFiles(m_top_window->getCurrentUri());
        }
    });
    connect(cutAction, &QAction::triggered, [=](){
        if (!m_top_window->getCurrentSelections().isEmpty()) {
            ClipboardUtils::setClipboardFiles(m_top_window->getCurrentSelections(), true);
        }
    });
    connect(trashAction, &QAction::triggered, [=](){
        if (!m_top_window->getCurrentSelections().isEmpty()) {
            FileOperationUtils::trash(m_top_window->getCurrentSelections(), true);
        }
    });

    //extension

    //trash
}

void ToolBar::updateLocation(const QString &uri)
{
    if (uri.isNull())
        return;

    bool isFileOpDisable = uri.startsWith("trash://") || uri.startsWith("search://");
    for (auto action : m_file_op_actions) {
        action->setEnabled(!isFileOpDisable);
        if (uri.startsWith("search://")) {
            if (action->text() == tr("Copy")) {
                action->setEnabled(true);
            }
        }
    }

    m_view_factory_model->setDirectoryUri(uri);

    auto viewId = m_top_window->getCurrentPage()->getView()->viewId();

    m_view_action->setIcon(m_view_factory_model->iconFromViewId(viewId));
    m_view_action->setText(m_top_window->getCurrentPageViewType());

    /*
    auto index = m_view_factory_model->getIndexFromViewId(viewId);
    if (index.isValid())
        m_view_option_box->setCurrentIndex(index.row());
    else {
        m_view_option_box->setCurrentIndex(0);
    }
    */

    m_clean_trash_action->setVisible(uri.startsWith("trash://"));
    m_restore_action->setVisible(uri.startsWith("trash://"));
}

void ToolBar::updateStates()
{
    if (!m_top_window)
        return;

    auto directory = m_top_window->getCurrentUri();
    auto selection = m_top_window->getCurrentSelections();

    if (directory.startsWith("trash://")) {
        auto files = m_top_window->getCurrentAllFileUris();
        m_clean_trash_action->setEnabled(!files.isEmpty());
        m_restore_action->setEnabled(!selection.isEmpty());
    }
}
