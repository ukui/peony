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

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QTreeView>
#include "peony-core_global.h"

namespace Peony {

class SideBarDelegate;

class PEONYCORESHARED_EXPORT SideBar : public QTreeView
{
    friend class SideBarDelegate;
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = nullptr);

    QSize sizeHint() const override;

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory = true, bool forceUpdate = false);

protected:
    void paintEvent(QPaintEvent *e) override;
    QRect visualRect(const QModelIndex &index) const override;
    //int horizontalOffset() const override {return 100;}

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
};

}

#endif // SIDEBAR_H
