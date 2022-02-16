/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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

#ifndef DESKTOPBACKGROUNDWINDOW_H
#define DESKTOPBACKGROUNDWINDOW_H

#include <QMainWindow>

class DesktopBackgroundWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DesktopBackgroundWindow(QScreen *screen, QWidget *parent = nullptr);

    int id() const;

    QScreen *screen() const;

public Q_SLOTS:
    void setWindowGeometry(const QRect &geometry);

protected Q_SLOTS:
    void updateWindowGeometry();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_id = -1;
    QScreen *m_screen = nullptr;
};

#endif // DESKTOPBACKGROUNDWINDOW_H
