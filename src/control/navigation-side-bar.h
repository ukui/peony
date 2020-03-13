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

#ifndef NAVIGATIONSIDEBAR_H
#define NAVIGATIONSIDEBAR_H

#include <QTreeView>
#include <QStyledItemDelegate>

namespace Peony {
class SideBarModel;
class SideBarProxyFilterSortModel;
}

class QPushButton;

class NavigationSideBar : public QTreeView
{
    Q_OBJECT
public:
    explicit NavigationSideBar(QWidget *parent = nullptr);
    bool eventFilter(QObject *obj, QEvent *e);

    void updateGeometries();

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory = true, bool force = false);
    void labelButtonClicked(bool checked);

private:
    Peony::SideBarProxyFilterSortModel *m_proxy_model;
    Peony::SideBarModel *m_model;

    QPushButton *m_label_button;
};

class NavigationSideBarItemDelegate : public QStyledItemDelegate
{
    friend class NavigationSideBar;
    explicit NavigationSideBarItemDelegate(QObject *parent = nullptr);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // NAVIGATIONSIDEBAR_H
