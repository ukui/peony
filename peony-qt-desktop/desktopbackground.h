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

#ifndef DESKTOPBACKGROUND_H
#define DESKTOPBACKGROUND_H

#include <QWidget>

class QGSettings;
class QVariantAnimation;
class QTimeLine;

class DesktopBackground : public QWidget
{
    Q_OBJECT
public:
    explicit DesktopBackground(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *e);

    void updateScreens();
    void initBackground();
    void setBackground();
    QString getAccountBackground();
    void setAccountBackground();
    void initGSettings();

protected:
    void switchBackground();
    void connectScreensChangement();

private:
    QPixmap m_frontPixmap;
    QPixmap m_backPixmap;
    QPixmap m_pendingPixmap;

    QGSettings *m_backgroundSettings = nullptr;
    QVariantAnimation *m_animation = nullptr;

    bool m_usePureColor = false;
    QColor m_color = Qt::black;

    bool m_paintBackground = false;

    QTimeLine *m_timeLine = nullptr;

    QString m_current_bg_path;
};

#endif // DESKTOPBACKGROUND_H
