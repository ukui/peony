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
#include "FMWindowIface.h"
#include "header-bar.h"

class MainWindowPrivate;
class BorderShadowEffect;
class HeaderBar;
class NavigationSideBar;
class TabWidget;
class QStackedWidget;
class QGSettins;

namespace Peony {
class DirectoryViewContainer;
class FileInfo;
class StatusBar;
class SideBar;
}

class MainWindow : public QMainWindow, public Peony::FMWindowIface
{
    friend class HeaderBar;
    Q_OBJECT
public:
    explicit MainWindow(const QString &uri = nullptr, QWidget *parent = nullptr);
    ~MainWindow();

    bool eventFilter(QObject *watched, QEvent *event);

    QSize sizeHint() const;

    Peony::FMWindowIface *create(const QString &uri);
    Peony::FMWindowIface *create(const QStringList &uris);
    Peony::FMWindowIface *createWithZoomLevel(const QString &uri, int zoomLevel);
    Peony::FMWindowIface *createWithZoomLevel(const QStringList &uris, int zoomLevel);

    Peony::FMWindowFactory *getFactory();

    Peony::DirectoryViewContainer *getCurrentPage();

    const QString getCurrentUri();
    const QStringList getCurrentSelections();
    const QStringList getCurrentAllFileUris();
    const QList<std::shared_ptr<Peony::FileInfo>> getCurrentSelectionFileInfos();

    Qt::SortOrder getCurrentSortOrder();
    int getCurrentSortColumn();

    bool getWindowShowHidden() {
        return m_show_hidden_file;
    }
    bool getWindowUseDefaultNameSortOrder() {
        return m_use_default_name_sort_order;
    }
    bool getWindowSortFolderFirst() {
        return m_folder_first;
    }

    int currentViewZoomLevel();
    bool currentViewSupportZoom();

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
    void signal_itemAdded(const QString& uri);/* 新增文件（夹），item创建完成 */


public Q_SLOTS:
    void maximizeOrRestore();

    void syncControlsLocation(const QString &uri);
    void updateHeaderBar();
    void updateWindowIcon();
    void createFolderOperation();
    void goToUri(const QString &uri, bool addHistory = false, bool force = false);

    void addNewTabs(const QStringList &uris);

    void beginSwitchView(const QString &viewId);

    void refresh();
    void forceStopLoading();

    //imigrate from fm-window
    void advanceSearch();
    void clearRecord();
    void searchFilter(QString target_path, QString keyWord, bool search_file_name, bool search_content);
    void filterUpdate(int type_index=0, int time_index=0, int size_index=0);

    void setShowHidden();
    void setUseDefaultNameSortOrder();
    void setSortFolderFirst();

    void setShortCuts();
    void checkSettings();
    void setLabelNameFilter(QString name);
    void updateTabPageTitle();

    //trash quick operations
    void cleanTrash();
    void recoverFromTrash();

    void setCurrentSelectionUris(const QStringList &uris);
    void setCurrentSortOrder (Qt::SortOrder order);
    void setCurrentSortColumn (int sortColumn);

    void editUri(const QString &uri);
    void editUris(const QStringList &uris);

    void setCurrentViewZoomLevel(int zoomLevel);

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

    void validBorder();
    void initUI(const QString &uri);
    void initAdvancePage();

    QRect sideBarRect();

private:
    BorderShadowEffect *m_effect;

    HeaderBar *m_header_bar;
    Peony::SideBar *m_side_bar;
    QWidget *m_transparent_area_widget;
    QStackedWidget *m_side_bar_container;
    TabWidget *m_tab;
    Peony::StatusBar *m_status_bar;

    bool m_is_draging = false;
    bool m_is_search = false;
    bool m_show_hidden_file;
    bool m_use_default_name_sort_order;
    bool m_folder_first;

    bool m_should_save_side_bar_width = false;
    bool m_should_save_window_size = false;
    bool m_is_first_tab =true;

    QPoint m_offset;

    QWidgetList m_focus_list;

    bool m_shortcuts_set = false;

    bool m_do_not_thumbnail = false;

    //change minimum width to avoid bug#90366
    const int WINDOW_MINIMUM_WIDTH = 740;
    QStringList m_uris_to_edit;/* 新建文件/文件夹，可编辑文件名list */
};

#endif // MAINWINDOW_H
