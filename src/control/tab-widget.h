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

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QMainWindow>
#include <QButtonGroup>
#include "navigation-tab-bar.h"

class NavigationTabBar;
class QStackedWidget;
class PreviewPageButtonGroups;

namespace Peony {
class PreviewPageIface;
class DirectoryViewContainer;
}

/*!
 * \brief The TabWidget class
 * \details
 * TabWidget is different with QTabWidget, it contains 4 parts.
 * 1. tabbar
 * 2. stackwidget
 * 3. preview button group
 * 4. preview page
 * <br>
 * Tabbar and statck is simmilar to QTabWidget's. Preview page is a docked page
 * which can be added or removed by preview button group.
 */
class TabWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = nullptr);

    QTabBar *tabBar() {return m_tab_bar;}

    Peony::DirectoryViewContainer *currentPage();

    const QString getCurrentUri();
    const QStringList getCurrentSelections();

    const QStringList getAllFileUris();

    const QStringList getBackList();
    const QStringList getForwardList();

    bool canGoBack();
    bool canGoForward();
    bool canCdUp();

    int getSortType();
    Qt::SortOrder getSortOrder();

Q_SIGNALS:
    void currentIndexChanged(int index);
    void tabMoved(int from, int to);
    void tabInserted(int index);
    void tabRemoved(int index);

    void activePageSelectionChanged();
    void activePageChanged();
    void activePageLocationChanged();
    void activePageViewTypeChanged();
    void activePageViewSortTypeChanged();

    void selectionChanged();
    void viewDoubleClicked(const QString &uri);
    void updateWindowLocationRequest(const QString &uri, bool addHistory, bool forceUpdate = false);

    void menuRequest(const QPoint &pos);

    void closeWindowRequest();

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setPreviewPage(Peony::PreviewPageIface *previewPage = nullptr);
    void addPage(const QString &uri, bool jumpTo = false);

    void goToUri(const QString &uri, bool addHistory, bool forceUpdate = false);
    void switchViewType(const QString &viewId);

    void goBack();
    void goForward();
    void cdUp();

    void refresh();
    void stopLoading();

    void tryJump(int index);
    void clearHistory();

    void setSortType(int type);
    void setSortOrder(Qt::SortOrder order);

    void setSortFilter(int FileTypeIndex, int FileMTimeIndex, int FileSizeIndex);
    void setShowHidden(bool showHidden = false);
    void setUseDefaultNameSortOrder(bool use);
    void setSortFolderFirst(bool folderFirst);

    void setCurrentSelections(const QStringList &uris);

    void editUri(const QString &uri);
    void editUris(const QStringList &uris);

    void onViewDoubleClicked(const QString &uri);

    int count();
    int currentIndex();
    void removeTab(int index);

protected:
    void changeCurrentIndex(int index);
    void moveTab(int from, int to);

    void bindContainerSignal(Peony::DirectoryViewContainer *container);

private:
    NavigationTabBar *m_tab_bar;
    QStackedWidget *m_stack;

    PreviewPageButtonGroups *m_buttons;

    Peony::PreviewPageIface *m_preview_page = nullptr;
    QDockWidget *m_preview_page_container;

    QAction *m_current_preview_action = nullptr;
};

class PreviewPageButtonGroups : public QButtonGroup
{
    Q_OBJECT
public:
    explicit PreviewPageButtonGroups(QWidget *parent = nullptr);

Q_SIGNALS:
    void previewPageButtonTrigger(bool trigger, const QString &pluginId);
};

#endif // TABWIDGET_H
