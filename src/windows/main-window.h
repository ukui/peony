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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class MainWindowPrivate;
class BorderShadowEffect;
class HeaderBar;
class DirectoryViewContainter;
class SideBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &uri = nullptr, QWidget *parent = nullptr);

    QSize sizeHint() const {return QSize(800, 600);}

Q_SIGNALS:
    void locationChanged(const QString &uri);
    void viewLoaded(bool successed = true);

public Q_SLOTS:
    void syncControlsLocation(const QString &uri);
    void goToUri(const QString &uri, bool addHistory = false, bool force = false);

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

    void validBorder();
    void initUI();

private:
    BorderShadowEffect *m_effect;
};

#endif // MAINWINDOW_H
