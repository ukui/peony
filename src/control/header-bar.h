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
#include <QMenuBar>

class MainWindow;
class ViewTypeMenu;
class SortTypeMenu;
class OperationMenu;

namespace Peony {
class AdvancedLocationBar;
}

class HeaderBar;
class QHBoxLayout;

class HeaderBarContainer : public QToolBar
{
    Q_OBJECT
public:
    explicit HeaderBarContainer(QWidget *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *e);

    void addHeaderBar(HeaderBar *headerBar);

private:
    QWidget *m_internal_widget;
    QHBoxLayout *m_layout;

    HeaderBar *m_header_bar = nullptr;

    QToolButton *m_max_or_restore = nullptr;
};

class HeaderBar : public QToolBar
{
    friend class HeaderBarContainer;
    friend class MainWindow;
    Q_OBJECT
    enum HeaderBarAction {
        GoBack,
        GoForward,
        LocationBar,
        Search,
        ViewType,
        SortType,
        Option,
        Copy,
        Cut,
        SeletcAll,
        Delete
    };

private:
    explicit HeaderBar(MainWindow *parent = nullptr);

Q_SIGNALS:
    void updateLocationRequest(const QString &uri, bool addHistory = true, bool force = true);
    void viewTypeChangeRequest(const QString &viewId);
    void updateZoomLevelHintRequest(int zoomLevelHint);
    void updateSearchRequest(bool showSearch);

protected:
    void addSpacing(int pixel);
    void mouseMoveEvent(QMouseEvent *e);

private Q_SLOTS:
    void setLocation(const QString &uri);
    void updateIcons();
    void updateSearchRecursive(bool recursive);
    void updateMaximizeState();
    void startEdit(bool bSearch = false);
    void finishEdit();
    void searchButtonClicked();
    void openDefaultTerminal();
    void findDefaultTerminal();
    void tryOpenAgain();
    void setSearchMode(bool mode);
    void closeSearch();
    void switchSelectStatus(bool select);

private:
    const QString m_uri;
    MainWindow *m_window;

    Peony::AdvancedLocationBar *m_location_bar;

    ViewTypeMenu *m_view_type_menu;
    SortTypeMenu *m_sort_type_menu;
    OperationMenu *m_operation_menu;

    QToolButton *m_create_folder;
    QPushButton *m_go_back;
    QPushButton *m_go_forward;
    //QToolButton *m_maximize_restore_button;
    QToolButton *m_search_button;
    QAction *m_search_action;
    QAction *m_close_search_action;

    bool m_search_mode = false;
    bool m_search_recursive = true;

    // save the actions to show or hide
    QHash<HeaderBarAction, QAction*> m_actions;
};

class HeaderBarToolButton : public QToolButton
{
    friend class HeaderBar;
    friend class MainWindow;
    Q_OBJECT
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
    friend class HeaderBarContainer;
    static HeaderBarStyle *getStyle();

    HeaderBarStyle() {}

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
};

class TopMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit TopMenuBar(MainWindow *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *e);

protected:
    void addWindowButtons();

private:
    QWidget *m_top_menu_internal_widget;
    QHBoxLayout *m_top_menu_layout;
    MainWindow *m_window;
    QToolButton *m_max_or_restore = nullptr;
    bool m_tablet_mode;
};

#endif // HEADERBAR_H
