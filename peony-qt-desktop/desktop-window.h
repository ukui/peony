/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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
 * Authors: Meihong He <hemeihong@kylinos.cn>
 *
 */

#ifndef DESKTOP_WINDOW_H
#define DESKTOP_WINDOW_H

#include <QStackedWidget>
#include <QTimer>
#include <QStackedLayout>

class QLabel;
class QListView;
class QGraphicsOpacityEffect;
class QGSettings;

namespace Peony {

class DesktopIconView;

class DesktopWindow : public QStackedWidget
{
    Q_OBJECT

public:
    DesktopWindow(QScreen *screen, bool is_primary, QWidget *parent = nullptr);
    ~DesktopWindow();

public:
    const QString getCurrentBgPath();
    bool getIsPrimary(){return m_is_primary;}
    void setIsPrimary(bool is_primary);
    QScreen *getScreen(){return m_screen;}
    void setScreen(QScreen *screen);
    DesktopIconView *getView(){return m_view;}

Q_SIGNALS:
    void changeBg(const QString &bgPath);
    void checkWindow();

public Q_SLOTS:
    void setBgPath(const QString &bgPath);
    void availableGeometryChangedProcess(const QRect &geometry);
    void virtualGeometryChangedProcess(const QRect &geometry);
    void geometryChangedProcess(const QRect &geometry);
    void scaleBg(const QRect &geometry);
    void updateView();
    void updateWinGeometry();

    void connectSignal();
    void disconnectSignal();

protected Q_SLOTS:
    void setBg(const QString &bgPath);
    void setBg(const QColor &color);

protected:
    void initShortcut();
    void initGSettings();

private:
    QString m_current_bg_path;

    QLabel *m_bg_font;
    QLabel *m_bg_back;
    DesktopIconView *m_view;

    QPixmap m_bg_font_pixmap;
    QPixmap m_bg_back_pixmap;

    QTimer *m_trans_timer;
    QGraphicsOpacityEffect *m_opacity_effect;

    qreal m_opacity = 1.0;

    QScreen *m_screen;
    QStackedLayout *m_layout;
    bool m_is_primary;

    QGSettings *m_bg_settings;
    QColor m_last_pure_color = Qt::black;
};

}

#endif // DESKTOP-WINDOW_H
