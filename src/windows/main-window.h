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
class NavigationSideBar;
class TabWidget;

namespace Peony {
class DirectoryViewContainer;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &uri = nullptr, QWidget *parent = nullptr);

    QSize sizeHint() const {return QSize(800, 600);}

    Peony::DirectoryViewContainer *getCurrentPage();

    const QString getCurrentUri();
    const QStringList getCurrentSelections();
    const QStringList getCurrentAllFileUris();

    Qt::SortOrder getCurrentSortOrder();
    int getCurrentSortColumn();

Q_SIGNALS:
    void windowSelectionChanged();
    void locationChanged(const QString &uri);
    void viewLoaded(bool successed = true);

    /*!
     * \brief locationChangeStart
     * \details
     * This signal is used to tell the window doing a location change.
     * When a window is excuting a location change, it should not excute another
     * one util the location change finished.
     */
    void locationChangeStart();
    /*!
     * \brief endLocationChange
     * \details
     * This signal is used to tell the window that a location change finished.
     * Once a location change finished, we can start a new location change.
     */
    void locationChangeEnd();

public Q_SLOTS:
    void syncControlsLocation(const QString &uri);
    void updateHeaderBar();
    void goToUri(const QString &uri, bool addHistory = false, bool force = false);

    void addNewTabs(const QStringList &uris);

    void beginSwitchView(const QString &viewId);

    void refresh();
    void forceStopLoading();

    void setCurrentSelectionUris(const QStringList &uris);
    void setCurrentSortOrder (Qt::SortOrder order);
    void setCurrentSortColumn (int sortColumn);

    void editUri(const QString &uri);
    void editUris(const QStringList &uris);

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void validBorder();
    void initUI();

    QRect sideBarRect();

private:
    BorderShadowEffect *m_effect;

    HeaderBar *m_header_bar;
    NavigationSideBar *m_side_bar;
    TabWidget *m_tab;
    bool m_is_draging = false;
};

#endif // MAINWINDOW_H
