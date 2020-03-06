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

Q_SIGNALS:
    void currentIndexChanged(int index);
    void tabMoved(int from, int to);
    void tabInserted(int index);
    void tabRemoved(int index);

    void activePageChanged();
    void activePageLocationChanged();
    void activePageViewTypeChanged();
    void activePageViewSortTypeChanged();

    void closeWindowRequest();

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setPreviewPage(Peony::PreviewPageIface *previewPage = nullptr);
    void addPage(const QString &uri, bool jumpTo = false);

protected:
    void changeCurrentIndex(int index);
    void moveTab(int from, int to);
    void removeTab(int index);

private:
    NavigationTabBar *m_tab_bar;
    QStackedWidget *m_stack;

    PreviewPageButtonGroups *m_buttons;

    Peony::PreviewPageIface *m_preview_page = nullptr;
    QDockWidget *m_preview_page_container;
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
