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
 */

#ifndef STATUSPUSHBUTTON_H
#define STATUSPUSHBUTTON_H
#include <QPushButton>
#include <QPixmap>

class StatusPushButton : public QPushButton
{
    Q_OBJECT
public:
    StatusPushButton(QWidget *parent);
    ~StatusPushButton();
    void paintEvent(QPaintEvent *);
    void setValue(double value);
private:
    double m_val = 0.0;
    QPixmap disc;

public Q_SLOTS:
    void changeTheme(QString);
};

#endif // STATUSPUSHBUTTON_H
