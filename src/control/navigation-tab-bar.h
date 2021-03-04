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

#ifndef NAVIGATIONTABBAR_H
#define NAVIGATIONTABBAR_H

#include <QTabBar>
#include <QProxyStyle>
#include <QTimer>
#include <memory>

class QToolButton;
class QDrag;
namespace Peony {
class FileInfo;
}

class NavigationTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit NavigationTabBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void pageAdded(const QString &uri);
    void closeWindowRequest();
    void addPageRequest(const QString &uri, bool jumpTo);
    void locationUpdated(const QString &uri);
    void floatButtonVisibleChanged(bool visible, int yoffset);

public Q_SLOTS:
    void addPage(const QString &uri = nullptr, bool jumpToNewTab = false);
    void addPages(const QStringList &uri);
    void updateLocation(int index, const QString &uri);

protected:
    void tabRemoved(int index) override;
    void tabInserted(int index) override;
    void relayoutFloatButton(bool insterted);

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

    void resizeEvent(QResizeEvent *e) override;

private:
    QToolButton *m_float_button;

    QTimer m_drag_timer;
    bool m_start_drag = false;
    QPoint m_press_pos;
    QDrag *m_drag = nullptr;
    bool m_should_trigger_drop = false;

    const int ELIDE_TEXT_LENGTH = 16;

    std::shared_ptr<Peony::FileInfo> m_info;
};

class TabBarStyle : public QProxyStyle
{
    friend class NavigationTabBar;
    friend class TabWidget;
    static TabBarStyle *getStyle();
    TabBarStyle() {}

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
};

#endif // NAVIGATIONTABBAR_H
