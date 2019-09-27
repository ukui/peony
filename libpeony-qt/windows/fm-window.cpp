#include "fm-window.h"

#include "tab-page.h"
#include "side-bar.h"
#include "navigation-bar.h"
#include "tool-bar.h"

#include "directory-view-container.h"
#include "directory-view-plugin-iface.h"

#include "file-utils.h"

#include <QDockWidget>
#include <QStandardPaths>
#include <QDebug>

#include <QSplitter>
#include <QVBoxLayout>

using namespace Peony;

FMWindow::FMWindow(const QString &uri, QWidget *parent) : QMainWindow (parent)
{
    setAnimated(false);

    auto location = uri;
    if (uri.isEmpty()) {
        location = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setChildrenCollapsible(false);
    m_splitter->setLayoutDirection(Qt::LeftToRight);

    setCentralWidget(m_splitter);

    m_tab = new TabPage(this);
    m_tab->addPage(uri);

    m_side_bar = new SideBar(this);
    m_side_bar->resize(150, 0);

    m_splitter->addWidget(m_side_bar);
    m_splitter->addWidget(m_tab);
    m_splitter->setStretchFactor(1, 1);

    m_tool_bar = new ToolBar(nullptr, this);
    m_tool_bar->setContentsMargins(0, 0, 0, 0);
    m_tool_bar->setMovable(false);

    addToolBar(Qt::TopToolBarArea, m_tool_bar);
    addToolBarBreak();

    m_navigation_bar = new NavigationBar(this);
    m_navigation_bar->setMovable(false);
    m_navigation_bar->bindContainer(m_tab->getActivePage());
    m_navigation_bar->updateLocation(uri);

    QToolBar *t = new QToolBar(this);
    t->setMovable(false);
    t->addWidget(m_navigation_bar);
    t->setContentsMargins(0, 0, 0, 0);
    addToolBar(t);
}

const QString FMWindow::getCurrentUri()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getCurrentUri();
    }
    return nullptr;
}

const QStringList FMWindow::getCurrentSelections()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getCurrentSelections();
    }
    return QStringList();
}

void FMWindow::addNewTabs(const QStringList &uris)
{
    for (auto uri : uris) {
        m_tab->addPage(uri);
    }
}

void FMWindow::goToUri(const QString &uri, bool addHistory)
{
    m_tab->getActivePage()->goToUri(uri, addHistory);
}

void FMWindow::beginSwitchView(const QString &viewId)
{
    m_tab->getActivePage()->switchViewType(viewId);
}
