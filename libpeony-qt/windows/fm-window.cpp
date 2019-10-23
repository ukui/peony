#include "fm-window.h"

#include "tab-page.h"
#include "side-bar.h"
#include "navigation-bar.h"
#include "tool-bar.h"
#include "search-bar.h"
#include "status-bar.h"

#include "search-vfs-uri-parser.h"

#include "directory-view-container.h"
#include "directory-view-plugin-iface.h"

#include "directory-view-menu.h"

#include "file-utils.h"
#include "file-info.h"

#include "file-operation-manager.h"

#include "preview-page-plugin-iface.h"
#include "preview-page-factory-manager.h"

#include <QDockWidget>
#include <QStandardPaths>
#include <QDebug>

#include <QSplitter>
#include <QVBoxLayout>

#include <QPainter>

#include <QMessageBox>

using namespace Peony;

FMWindow::FMWindow(const QString &uri, QWidget *parent) : QMainWindow (parent)
{
    m_operation_minimum_interval.setSingleShot(true);

    setAttribute(Qt::WA_DeleteOnClose);
    setAnimated(false);

    auto location = uri;
    if (uri.isEmpty()) {
        location = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setChildrenCollapsible(false);
    m_splitter->setLayoutDirection(Qt::LeftToRight);
    m_splitter->setHandleWidth(2);
    m_splitter->setStyleSheet("QSplitter"
                              "{"
                              "border: 0;"
                              "padding: 0;"
                              "margin: 0;"
                              "}");

    setCentralWidget(m_splitter);

    m_tab = new TabPage(this);
    m_tab->addPage(uri);

    m_side_bar = new SideBar(this);

    m_splitter->addWidget(m_side_bar);
    m_splitter->addWidget(m_tab);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setStretchFactor(2, 1);

    m_tool_bar = new ToolBar(this, this);
    m_tool_bar->setContentsMargins(0, 0, 0, 0);

    m_search_bar = new SearchBar(this);

    //put the tool bar and search bar into
    //a hobx-layout widget, and put the widget int a
    //new tool bar.
    QWidget *w1 = new QWidget(this);
    w1->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *l1 = new QHBoxLayout(w1);
    l1->setContentsMargins(0, 0, 0, 0);
    w1->setLayout(l1);
    l1->addWidget(m_tool_bar, Qt::AlignLeft);
    l1->addWidget(m_search_bar, Qt::AlignRight);

    QToolBar *t1 = new QToolBar(this);
    t1->setContentsMargins(0, 0, 10, 0);
    t1->setMovable(false);
    t1->addWidget(w1);

    addToolBar(Qt::TopToolBarArea, t1);
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

    m_status_bar = new StatusBar(this, this);
    setStatusBar(m_status_bar);

    m_navigation_bar->bindContainer(m_tab->getActivePage());

    //connect signals
    connect(m_side_bar, &SideBar::updateWindowLocationRequest, this, &FMWindow::goToUri);
    connect(m_tab, &TabPage::updateWindowLocationRequest, this, &FMWindow::goToUri);
    connect(m_navigation_bar, &NavigationBar::updateWindowLocationRequest, this, &FMWindow::goToUri);
    connect(m_navigation_bar, &NavigationBar::refreshRequest, this, &FMWindow::refresh);

    //tab changed
    connect(m_tab, &TabPage::currentActiveViewChanged, [=](){
        this->m_tool_bar->updateLocation(getCurrentUri());
        this->m_tool_bar->updateStates();
        this->m_navigation_bar->bindContainer(getCurrentPage());
        this->m_navigation_bar->updateLocation(getCurrentUri());
        this->m_status_bar->update();
        Q_EMIT this->tabPageChanged();
    });

    //location change
    connect(m_tab, &TabPage::currentLocationChanged,
            this, &FMWindow::locationChangeEnd);
    connect(this, &FMWindow::locationChangeStart, [=](){
        m_is_loading = true;
        m_side_bar->blockSignals(true);
        m_tool_bar->blockSignals(true);
        m_navigation_bar->setBlock(true);
    });
    connect(this, &FMWindow::locationChangeEnd, [=](){
        //qDebug()<<this->getCurrentAllFileUris();
        m_is_loading = false;
        m_side_bar->blockSignals(false);
        m_tool_bar->blockSignals(false);
        m_navigation_bar->setBlock(false);
        qDebug()<<this->getCurrentUri();
        m_navigation_bar->updateLocation(getCurrentUri());
        m_tool_bar->updateLocation(getCurrentUri());
        m_tool_bar->updateStates();
    });

    //selection changed
    connect(m_tab, &TabPage::currentSelectionChanged, [=](){
        m_status_bar->update();
        m_tool_bar->updateStates();
        Q_EMIT this->windowSelectionChanged();
    });

    //location change
    connect(this, &FMWindow::locationChangeStart, [this](){
        QCursor c;
        c.setShape(Qt::WaitCursor);
        this->setCursor(c);
        m_status_bar->update(tr("Loaing... Press Esc to stop a loading."));
    });

    connect(this, &FMWindow::locationChangeEnd, [this](){
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        this->setCursor(c);
        m_status_bar->update();
    });

    //view switched
    connect(m_tab, &TabPage::viewTypeChanged, [=](){
        m_tool_bar->updateLocation(getCurrentUri());
        m_tool_bar->updateStates();
    });

    //search
    connect(m_search_bar, &SearchBar::searchKeyChanged, [=](){
        //FIXME: filter the current directory
    });
    connect(m_search_bar, &SearchBar::searchRequest, [=](const QString &key){
        QString uri = this->getCurrentUri();
        if (uri.startsWith("search:///")) {
            uri = m_last_non_search_location;
        }
        auto targetUri = SearchVFSUriParser::parseSearchKey(uri, key);
        this->goToUri(targetUri, true);
    });

    //action
    QAction *stopLoadingAction = new QAction(this);
    stopLoadingAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(stopLoadingAction);
    connect(stopLoadingAction, &QAction::triggered, this, &FMWindow::forceStopLoading);

    auto undoAction = new QAction(QIcon::fromTheme("edit-undo-symbolic"), tr("Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    addAction(undoAction);
    connect(undoAction, &QAction::triggered, [=](){
        FileOperationManager::getInstance()->undo();
    });

    auto redoAction = new QAction(QIcon::fromTheme("edit-redo-symbolic"), tr("Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    addAction(redoAction);
    connect(redoAction, &QAction::triggered, [=](){
        FileOperationManager::getInstance()->redo();
    });

    //menu
    m_tab->connect(m_tab, &TabPage::menuRequest, [=](const QPoint &pos){
        if (m_is_loading)
            return;
        DirectoryViewMenu menu(this, nullptr);
        menu.exec(pos);
    });

    //preview page
    connect(m_navigation_bar, &NavigationBar::switchPreviewPageRequest,
            this, &FMWindow::onPreviewPageSwitch);
    connect(m_tab, &TabPage::currentSelectionChanged, [=](){
        if (m_preview_page) {
            auto selection = getCurrentSelections();
            if (!selection.isEmpty()) {
                m_preview_page->cancel();
                m_preview_page->prepare(selection.first());
                m_preview_page->startPreview();
            } else {
                m_preview_page->cancel();
            }
        }
    });
    connect(m_tab, &TabPage::currentLocationChanged, [=](){
        if (m_preview_page) {
            m_preview_page->cancel();
        }
    });
    connect(m_tab, &TabPage::currentActiveViewChanged, [=](){
        if (m_preview_page) {
            auto selection = getCurrentSelections();
            if (selection.isEmpty()) {
                m_preview_page->cancel();
            } else {
                m_preview_page->prepare(selection.first());
                m_preview_page->startPreview();
            }
        }
    });
}

const QString FMWindow::getCurrentUri()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getCurrentUri();
    }
    return nullptr;
}

const QStringList FMWindow::getCurrentAllFileUris()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getAllFileUris();
    }
    return QStringList();
}

const QStringList FMWindow::getCurrentSelections()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getCurrentSelections();
    }
    return QStringList();
}

const QList<std::shared_ptr<FileInfo>> FMWindow::getCurrentSelectionFileInfos()
{
    const QStringList uris = getCurrentSelections();
    QList<std::shared_ptr<FileInfo>> infos;
    for(auto uri : uris) {
        auto info = FileInfo::fromUri(uri);
        infos<<info;
    }
    return infos;
}

void FMWindow::addNewTabs(const QStringList &uris)
{
    for (auto uri : uris) {
        m_tab->addPage(uri);
    }
}

void FMWindow::goToUri(const QString &uri, bool addHistory, bool forceUpdate)
{
    if (forceUpdate)
        goto update;

    if (getCurrentUri() == uri)
        return;

update:
    if (!uri.startsWith("search://")) {
        m_last_non_search_location = uri;
    }
    Q_EMIT locationChangeStart();
    m_tab->getActivePage()->goToUri(uri, addHistory, forceUpdate);
    m_tab->refreshCurrentTabText();
    m_navigation_bar->updateLocation(uri);
    m_tool_bar->updateLocation(uri);
}

void FMWindow::beginSwitchView(const QString &viewId)
{
    m_tab->getActivePage()->switchViewType(viewId);
}

void FMWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
}

DirectoryViewContainer *FMWindow::getCurrentPage()
{
    return m_tab->getActivePage();
}

void FMWindow::refresh()
{
    if (m_operation_minimum_interval.isActive()) {
        return;
    }
    m_operation_minimum_interval.start(500);
    m_tab->getActivePage()->refresh();
}

void FMWindow::forceStopLoading()
{
    m_tab->getActivePage()->stopLoading();
    m_is_loading = false;
}

void FMWindow::onPreviewPageSwitch(const QString &id)
{
    if (id.isNull()) {
        PreviewPageIface *page = dynamic_cast<PreviewPageIface*>(m_splitter->widget(2));
        if (page) {
            page->closePreviewPage();
            m_preview_page = nullptr;
        }
    } else {
        PreviewPageIface *page = dynamic_cast<PreviewPageIface*>(m_splitter->widget(2));
        if (page) {
            page->closePreviewPage();
        }
        page = PreviewPageFactoryManager::getInstance()->getPlugin(id)->createPreviewPage();
        m_splitter->addWidget(dynamic_cast<QWidget*>(page));
        if (!getCurrentSelections().isEmpty()) {
            page->prepare(getCurrentSelections().first());
            page->startPreview();
        }
        m_preview_page = page;
    }
}
