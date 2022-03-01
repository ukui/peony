/*
 * Copyright: 2021, KylinSoft Co., Ltd.
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
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

#include "src/menu/tablet-menu.h"
#include <QIcon>
#include <QPalette>

TabletMenu::TabletMenu(FullListView* view, QPoint pos) : QMenu(nullptr), m_mainView(view), m_pos(pos)
{
    fillUninstallAction();
}

void TabletMenu::fillUninstallAction()
{
    auto uninstallAction = addAction(tr("Uninstall"),
                                     this, &TabletMenu::uninstallActionTriggerSlot);

    QIcon icon(":/img/uninstall.svg");
    uninstallAction->setIcon(icon);

    if (!m_mainView) {
        uninstallAction->setDisabled(true);
        return;
    }

    QModelIndex index = m_mainView->indexAt(m_pos);
    QString appDesktopName = m_mainView->listmodel->data(index, Qt::DisplayRole).toString();
    appDesktopName = appDesktopName.split("/").last();

    m_mainView->syssetting->beginGroup("ukui-menu-sysapplist");
    if (m_mainView->syssetting->contains(appDesktopName)) {
        uninstallAction->setDisabled(true);
    }
    m_mainView->syssetting->endGroup();
}

void TabletMenu::uninstallActionTriggerSlot()
{
    QString cmd = QString("kylin-software-center");
    QProcess::startDetached(cmd);
}
