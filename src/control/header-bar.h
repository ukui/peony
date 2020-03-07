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

#ifndef HEADERBAR_H
#define HEADERBAR_H

#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QProxyStyle>

class MainWindow;

class HeaderBar : public QToolBar
{
    friend class MainWindow;
    Q_OBJECT
private:
    explicit HeaderBar(MainWindow *parent = nullptr);

Q_SIGNALS:
    void updateLocationRequest(const QString &uri);

protected:
    void addSpacing(int pixel);

private Q_SLOTS:
    void setLocation(const QString &uri);

private:
    const QString m_uri;
    MainWindow *m_window;
};

class HeaderBarToolButton : public QToolButton
{
    friend class HeaderBar;
    friend class MainWindow;
    Q_OBJECT;
    explicit HeaderBarToolButton(QWidget *parent = nullptr);
};

class HeadBarPushButton : public QPushButton
{
    friend class HeaderBar;
    friend class MainWindow;
    Q_OBJECT
    explicit HeadBarPushButton(QWidget *parent = nullptr);
};

class HeaderBarStyle : public QProxyStyle
{
    friend class HeaderBar;
    static HeaderBarStyle *getStyle();

    HeaderBarStyle() {}

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
};

#endif // HEADERBAR_H
