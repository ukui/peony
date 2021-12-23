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

#ifndef TABLET_MENU_H
#define TABLET_MENU_H

#include <QMenu>
#include <QProxyStyle>
#include "src/UtilityFunction/fulllistview.h"

class TabletMenu : public QMenu
{
    Q_OBJECT
public:
    TabletMenu(FullListView* view, QPoint pos);

private:
    void fillUninstallAction();

protected Q_SLOTS:
    void uninstallActionTriggerSlot();

private:
    FullListView *m_mainView = nullptr;
    QPoint m_pos;
};


#endif //TABLET_MENU_H
