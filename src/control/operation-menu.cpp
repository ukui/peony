/*
 * Peony-Qt
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "peony-application.h"

#include "main-window.h"
#include "operation-menu.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidgetAction>

#include "global-settings.h"
#include "clipboard-utils.h"
#include "file-operation-utils.h"

OperationMenu::OperationMenu(MainWindow *window, QWidget *parent) : QMenu(parent)
{
    m_window = window;

    connect(this, &QMenu::aboutToShow, this, &OperationMenu::updateMenu);

    //FIXME: implement all actions.

    auto editWidgetContainer = new QWidgetAction(this);
    auto editWidget = new OperationMenuEditWidget(window, this);
    m_edit_widget = editWidget;
    editWidgetContainer->setDefaultWidget(editWidget);
    addAction(editWidgetContainer);

    connect(m_edit_widget, &OperationMenuEditWidget::operationAccepted, this, &QMenu::hide);

    addSeparator();

    //addAction(tr("Conditional Filter"));
    auto advanceSearch = addAction(tr("Advance Search"), this, [=]()
    {
       m_window->advanceSearch();
    });

    addSeparator();

    auto keepAllow = addAction(tr("Keep Allow"), this, [=](bool checked){
        m_window->setWindowFlag(Qt::WindowStaysOnTopHint, checked);
        m_window->show();
    });
    keepAllow->setCheckable(true);

    auto showHidden = addAction(tr("Show Hidden"), this, [=](bool checked){
        //window set show hidden
        m_window->setShowHidden();
    });
    m_show_hidden = showHidden;
    showHidden->setCheckable(true);

    auto forbidThumbnailing = addAction(tr("Forbid thumbnailing"), this, [=](bool checked){
        //FIXME:
        Peony::GlobalSettings::getInstance()->setValue("do-not-thumbnail", checked);
    });
    m_forbid_thumbnailing = forbidThumbnailing;
    forbidThumbnailing->setCheckable(true);
    forbidThumbnailing->setChecked(Peony::GlobalSettings::getInstance()->getValue("do-not-thumbnail").toBool());

    auto residentInBackend = addAction(tr("Resident in Backend"), this, [=](bool checked){
        //FIXME:
        Peony::GlobalSettings::getInstance()->setValue("resident", checked);
        qApp->setQuitOnLastWindowClosed(!checked);
    });
    m_resident_in_backend = residentInBackend;
    residentInBackend->setCheckable(true);
    residentInBackend->setChecked(Peony::GlobalSettings::getInstance()->getValue("resident").toBool());

    addSeparator();

    addAction(QIcon::fromTheme("help-app-symbolic"), tr("Help"), this, [=](){
        PeonyApplication::help();
    });

    addAction(QIcon::fromTheme("help-about", QIcon::fromTheme("gtk-about-symbolic")), tr("About"), this, [=](){
        PeonyApplication::about();
    });
}

void OperationMenu::updateMenu()
{
    m_show_hidden->setChecked(Peony::GlobalSettings::getInstance()->isExist("show-hidden")?
                                  Peony::GlobalSettings::getInstance()->getValue("show-hidden").toBool():
                                  false);

    m_forbid_thumbnailing->setChecked(Peony::GlobalSettings::getInstance()->isExist("do-not-thumbnail")?
                                          Peony::GlobalSettings::getInstance()->getValue("do-not-thumbnail").toBool():
                                          false);

    m_resident_in_backend->setChecked(Peony::GlobalSettings::getInstance()->isExist("resident")?
                                          Peony::GlobalSettings::getInstance()->getValue("resident").toBool():
                                          false);

    //get window current directory and selections, then update ohter actions.
    m_edit_widget->updateActions(m_window->getCurrentUri(), m_window->getCurrentSelections());
}

OperationMenuEditWidget::OperationMenuEditWidget(MainWindow *window, QWidget *parent) : QWidget(parent)
{
    auto vbox = new QVBoxLayout;
    setLayout(vbox);

    auto title = new QLabel(this);
    title->setText(tr("Edit"));
    title->setAlignment(Qt::AlignCenter);
    vbox->addWidget(title);

    auto hbox = new QHBoxLayout;
    auto copy = new QToolButton(this);
    m_copy = copy;
    copy->setFixedSize(QSize(40, 40));
    copy->setIcon(QIcon::fromTheme("edit-copy-symbolic"));
    copy->setIconSize(QSize(16, 16));
    copy->setAutoRaise(false);
    hbox->addWidget(copy);

    auto paste = new QToolButton(this);
    m_paste = paste;
    paste->setFixedSize(QSize(40, 40));
    paste->setIcon(QIcon::fromTheme("edit-paste-symbolic"));
    paste->setIconSize(QSize(16, 16));
    paste->setAutoRaise(false);
    hbox->addWidget(paste);

    auto cut = new QToolButton(this);
    m_cut = cut;
    cut->setFixedSize(QSize(40, 40));
    cut->setIcon(QIcon::fromTheme("edit-cut-symbolic"));
    cut->setIconSize(QSize(16, 16));
    cut->setAutoRaise(false);
    hbox->addWidget(cut);

    auto trash = new QToolButton(this);
    m_trash = trash;
    trash->setFixedSize(QSize(40, 40));
    trash->setIcon(QIcon::fromTheme("user-trash-symbolic"));
    trash->setIconSize(QSize(16, 16));
    trash->setAutoRaise(false);
    hbox->addWidget(trash);

    vbox->addLayout(hbox);

    connect(m_copy, &QToolButton::clicked, this, [=](){
        Peony::ClipboardUtils::setClipboardFiles(window->getCurrentSelections(), false);
        Q_EMIT operationAccepted();
    });

    connect(m_cut, &QToolButton::clicked, this, [=](){
        Peony::ClipboardUtils::setClipboardFiles(window->getCurrentSelections(), true);
        Q_EMIT operationAccepted();
    });

    connect(m_paste, &QToolButton::clicked, this, [=](){
        Peony::ClipboardUtils::pasteClipboardFiles(window->getCurrentUri());
        Q_EMIT operationAccepted();
    });

    connect(m_trash, &QToolButton::clicked, this, [=](){
        if (window->getCurrentUri() == "trash:///") {
            Peony::FileOperationUtils::executeRemoveActionWithDialog(window->getCurrentSelections());
        } else {
            Peony::FileOperationUtils::trash(window->getCurrentSelections(), true);
        }
        Q_EMIT operationAccepted();
    });
}

void OperationMenuEditWidget::updateActions(const QString &currentDirUri, const QStringList &selections)
{
    //FIXME:
    bool isSelectionEmpty = selections.isEmpty();
    m_copy->setEnabled(!isSelectionEmpty);
    m_cut->setEnabled(!isSelectionEmpty);
    m_trash->setEnabled(!isSelectionEmpty);

    if (isSelectionEmpty) {
        bool isClipboradHasFile = Peony::ClipboardUtils::isClipboardHasFiles();
        m_paste->setEnabled(isClipboradHasFile);
    } else {
        m_paste->setEnabled(false);
    }
}
