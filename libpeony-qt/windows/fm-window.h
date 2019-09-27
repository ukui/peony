#ifndef FMWINDOW_H
#define FMWINDOW_H

#include <QMainWindow>
#include "peony-core_global.h"

class QSplitter;

namespace Peony {

class TabPage;
class SideBar;
class NavigationBar;
class ToolBar;

class DirectoryViewProxyIface;

/*!
 * \brief The FMWindow class, the normal window of peony-qt's file manager.
 * \details
 * This class show the directory as the common window which used by peony-qt.
 * It contains a tool bar, a navigation bar, a side bar, a preview page and
 * a status bar, and a tab widget container which may conatin over one
 * directory views.
 *
 * The tab view's design is improving and refactoring from peony's window-pane-slot
 * window framework. There is no concept pane in peony-qt (stand by preview page).
 * The window can hold over one 'slot' represent a directory view at a FMWindow
 * instance, but there should be only and just one slot is active in this window.
 *
 * \note
 * The tab view's design is not necessary for a file manager, and it might increased
 * the design difficulty. If you plan to develop a file manager application.
 * You should consider wether it is needed.
 */
class PEONYCORESHARED_EXPORT FMWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FMWindow(const QString &uri, QWidget *parent = nullptr);

    const QString getCurrentUri();
    const QStringList getCurrentSelections();

Q_SIGNALS:
    void activeViewChanged(const DirectoryViewProxyIface *view);
    void tabPageChanged();

public Q_SLOTS:
    void goToUri(const QString &uri, bool addHistory);
    void addNewTabs(const QStringList &uris);

    void beginSwitchView(const QString &viewId);

private:
    QSplitter *m_splitter;

    TabPage *m_tab;
    SideBar *m_side_bar;
    NavigationBar *m_navigation_bar;
    ToolBar *m_tool_bar;
};

}

#endif // FMWINDOW_H
