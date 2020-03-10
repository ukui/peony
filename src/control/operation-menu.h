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

#ifndef OPERATIONMENU_H
#define OPERATIONMENU_H

#include <QMenu>

class MainWindow;

class OperationMenu : public QMenu
{
    Q_OBJECT
public:
    explicit OperationMenu(MainWindow *window, QWidget *parent = nullptr);

public Q_SLOTS:
    void updateMenu();

private:
    QAction *m_show_hidden;
    QAction *m_forbid_thumbnailing;
    QAction *m_resident_in_backend;

private:
    MainWindow *m_window;
};

class OperationMenuEditWidget : public QWidget
{
    friend class OperationMenu;
    Q_OBJECT
    explicit OperationMenuEditWidget(QWidget *parent = nullptr);

    void updateActions(const QString &currentDirUri, const QStringList &selections);

    QAction *m_copy;
    QAction *m_paste;
    QAction *m_cut;
    QAction *m_trash;
};

#endif // OPERATIONMENU_H
