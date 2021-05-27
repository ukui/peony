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

#ifndef X11WINDOWMANAGER_H
#define X11WINDOWMANAGER_H

#include <QObject>
#include <QPoint>

class X11WindowManager : public QObject
{
    Q_OBJECT
public:
    static X11WindowManager *getInstance();

    bool eventFilter(QObject *watched, QEvent *event) override;

    void registerWidget(QWidget *widget);

private:
    explicit X11WindowManager(QObject *parent = nullptr);

    bool m_is_draging = false;
    ulong m_prepare_drag_time = 0;

    QWidget *m_current_widget = nullptr;

    QPoint m_press_pos;
    QPoint m_toplevel_offset;
};

#endif // X11WINDOWMANAGER_H
