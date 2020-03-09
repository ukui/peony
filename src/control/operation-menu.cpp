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

#include "operation-menu.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidgetAction>

OperationMenu::OperationMenu(MainWindow *window, QWidget *parent) : QMenu(parent)
{
    m_window = window;

    connect(this, &QMenu::aboutToShow, this, &OperationMenu::updateMenu);

    //FIXME: implement all actions.

    auto editWidgetContainer = new QWidgetAction(this);
    auto editWidget = new OperationMenuEditWidget(this);
    editWidgetContainer->setDefaultWidget(editWidget);
    addAction(editWidgetContainer);

    addSeparator();

    addAction(tr("Conditional Filter"));

    addSeparator();

    addAction(tr("Show Hidden"));
    addAction(tr("Forbid thumbnailing"));
    addAction(tr("Resident in Backend"));

    addSeparator();

    addAction(tr("Help"));

    addAction(tr("About"));
}

void OperationMenu::updateMenu()
{

}

OperationMenuEditWidget::OperationMenuEditWidget(QWidget *parent) : QWidget(parent)
{
    auto vbox = new QVBoxLayout;
    setLayout(vbox);

    auto title = new QLabel(this);
    title->setText(tr("Edit"));
    title->setAlignment(Qt::AlignCenter);
    vbox->addWidget(title);

    auto hbox = new QHBoxLayout;
    auto copy = new QToolButton(this);
    copy->setFixedSize(QSize(40, 40));
    copy->setIcon(QIcon::fromTheme("edit-copy-symbolic"));
    copy->setIconSize(QSize(16, 16));
    copy->setAutoRaise(false);
    hbox->addWidget(copy);

    auto paste = new QToolButton(this);
    paste->setFixedSize(QSize(40, 40));
    paste->setIcon(QIcon::fromTheme("edit-paste-symbolic"));
    paste->setIconSize(QSize(16, 16));
    paste->setAutoRaise(false);
    hbox->addWidget(paste);

    auto cut = new QToolButton(this);
    cut->setFixedSize(QSize(40, 40));
    cut->setIcon(QIcon::fromTheme("edit-cut-symbolic"));
    cut->setIconSize(QSize(16, 16));
    cut->setAutoRaise(false);
    hbox->addWidget(cut);

    auto trash = new QToolButton(this);
    trash->setFixedSize(QSize(40, 40));
    trash->setIcon(QIcon::fromTheme("user-trash-symbolic"));
    trash->setIconSize(QSize(16, 16));
    trash->setAutoRaise(false);
    hbox->addWidget(trash);

    vbox->addLayout(hbox);
}
