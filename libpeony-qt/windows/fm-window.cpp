#include "fm-window.h"

#include "tab-page.h"
#include "side-bar.h"
#include "navigation-bar.h"
#include "tool-bar.h"
#include "search-bar.h"
#include "status-bar.h"
#include "path-completer.h"
#include "path-edit.h"

#include "search-vfs-uri-parser.h"

#include "directory-view-container.h"
#include "directory-view-plugin-iface.h"
#include "directory-view-widget.h"

#include "directory-view-menu.h"

#include "file-utils.h"
#include "file-info.h"

#include "file-operation-manager.h"

#include "preview-page-plugin-iface.h"
#include "preview-page-factory-manager.h"

#include "directory-view-factory-manager.h"

#include <QDockWidget>
#include <QStandardPaths>
#include <QDebug>

#include <QSplitter>
#include <QHBoxLayout>
#include <QPainter>

#include <QMessageBox>
#include <QLabel>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QStringListModel>
#include <QFileDialog>

#include <QLayout>

using namespace Peony;

FMWindow::FMWindow(const QString &uri, QWidget *parent) : QMainWindow (parent)
{
    m_operation_minimum_interval.setSingleShot(true);

    //setStyleSheet(".Peony--FMWindow::separator{border:0; padding:0}");
    setAttribute(Qt::WA_DeleteOnClose);
    //setAnimated(false);
    /*
    QTimer::singleShot(1000, [=](){
        QDockWidget *d = new QDockWidget(this);
        //d->setFeatures(QDockWidget::DockWidgetClosable);
        auto l = new QLabel(tr("test"), d);
        d->setTitleBarWidget(l);
        //d->setWidget(new QLabel(tr("test"), d));
        addDockWidget(Qt::TopDockWidgetArea, d);
        QTimer::singleShot(1000, [=](){
            d->hide();
            QTimer::singleShot(1000, [=](){
                d->show();
            });
        });
    });
     */

    auto location = uri;
    if (uri.isNull()) {
        location = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }
    //init to solve second search in empty path issue
    m_last_non_search_location = location;

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
    m_tab->addPage(location);

    m_side_bar = new SideBar(this);
    m_filter_bar = new AdvanceSearchBar(this);
    m_filter_bar->setMinimumWidth(180);
    m_filter = dynamic_cast<QWidget*>(m_filter_bar);

    m_side_bar_container = new QStackedWidget(this);
    m_side_bar_container->addWidget(m_side_bar);
    m_side_bar_container->addWidget(m_filter);
    m_side_bar_container->setCurrentWidget(m_side_bar);
    m_splitter->addWidget(m_side_bar_container);

    m_splitter->addWidget(m_tab);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);
    m_splitter->setStretchFactor(2, 1);

    m_tool_bar = new ToolBar(this, this);
    m_tool_bar->setContentsMargins(0, 0, 0, 0);

    m_search_bar = new SearchBar(this);
    m_search_bar->setMinimumWidth(200);
    m_advanced_button = new QPushButton(tr("advanced search"), nullptr);
    m_advanced_button->setFixedWidth(110);
    m_advanced_button->setStyleSheet("color: rgb(10,10,255)");
    m_clear_record = new QPushButton(tr("clear record"), nullptr);
    m_clear_record->setFixedWidth(110);
    m_clear_record->setDisabled(true);
    //set button hidden, function not open to public yet
    m_advanced_button->setVisible(false);
    m_clear_record->setVisible(false);

    m_preview_page_container = new PreviewPageContainer(this);
    m_splitter->addWidget(m_preview_page_container);
    m_preview_page_container->hide();

    //put the tool bar and search bar into
    //a hobx-layout widget, and put the widget int a
    //new tool bar.
    QWidget *w1 = new QWidget(this);
    w1->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *l1 = new QHBoxLayout(w1);
    l1->setContentsMargins(5, 0, 0, 0);
    w1->setLayout(l1);
    l1->addWidget(m_tool_bar, Qt::AlignLeft);
    l1->addWidget(m_search_bar, Qt::AlignRight);
    l1->addWidget(m_advanced_button, Qt::AlignRight);
    l1->addWidget(m_clear_record, Qt::AlignRight);

    ToolBarContainer *t1 = new ToolBarContainer(this);
    t1->setContentsMargins(0, 0, 10, 0);
    t1->setMovable(false);
    t1->addWidget(w1);

    addToolBar(Qt::TopToolBarArea, t1);
    addToolBarBreak();

    m_navigation_bar = new NavigationBar(this);
    m_navigation_bar->setMovable(false);
    m_navigation_bar->bindContainer(m_tab->getActivePage());
    m_navigation_bar->updateLocation(location);

    QToolBar *t = new QToolBar(this);
    t->setStyleSheet(".QToolBar{border: 0; padding: 0}");
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
    connect(m_advanced_button, &QPushButton::clicked, this, &FMWindow::advanceSearch);
    connect(m_clear_record, &QPushButton::clicked, this, &FMWindow::clearRecord);

    //tab changed
    connect(m_tab, &TabPage::currentActiveViewChanged, [=](){
        this->m_tool_bar->updateLocation(getCurrentUri());
        this->m_tool_bar->updateStates();
        this->m_navigation_bar->bindContainer(getCurrentPage());
        this->m_navigation_bar->updateLocation(getCurrentUri());
        this->m_status_bar->update();
        Q_EMIT this->tabPageChanged();
        if (m_filter_visible)
        {
            advanceSearch();
            filterUpdate();
        }
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
        m_update_condition = false; //common search, no filter
        auto targetUri = SearchVFSUriParser::parseSearchKey(uri, key);
        this->goToUri(targetUri, true);
        m_clear_record->setDisabled(false); //has record to clear
    });

    //action
    QAction *stopLoadingAction = new QAction(this);
    stopLoadingAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(stopLoadingAction);
    connect(stopLoadingAction, &QAction::triggered, this, &FMWindow::forceStopLoading);

    //show hidden action
    QAction *showHiddenAction = new QAction(this);
    showHiddenAction->setShortcut(QKeySequence(tr("Ctrl+H", "Show|Hidden")));
    addAction(showHiddenAction);
    connect(showHiddenAction, &QAction::triggered, this, &FMWindow::setShowHidden);

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
    m_tab->connect(m_tab, &TabPage::menuRequest, [=](){
        if (m_is_loading)
            return;
        DirectoryViewMenu menu(this, nullptr);
        menu.exec(QCursor::pos());
    });

    //preview page
    connect(m_navigation_bar, &NavigationBar::switchPreviewPageRequest,
            this, &FMWindow::onPreviewPageSwitch);
    connect(m_tab, &TabPage::currentSelectionChanged, [=](){
        if (m_preview_page_container->getCurrentPage()) {
            auto selection = getCurrentSelections();
            if (!selection.isEmpty()) {
                m_preview_page_container->getCurrentPage()->cancel();
                m_preview_page_container->getCurrentPage()->prepare(selection.first());
                m_preview_page_container->getCurrentPage()->startPreview();
            } else {
                m_preview_page_container->getCurrentPage()->cancel();
            }
        }
    });
    connect(m_tab, &TabPage::currentLocationChanged, [=](){
        if (m_preview_page_container->getCurrentPage()) {
            m_preview_page_container->getCurrentPage()->cancel();
        }
    });
    connect(m_tab, &TabPage::currentActiveViewChanged, [=](){
        if (m_preview_page_container->getCurrentPage()) {
            auto selection = getCurrentSelections();
            if (selection.isEmpty()) {
                m_preview_page_container->getCurrentPage()->cancel();
            } else {
                m_preview_page_container->getCurrentPage()->prepare(selection.first());
                m_preview_page_container->getCurrentPage()->startPreview();
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
    if (m_update_condition)
        filterUpdate();
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
    if (m_filter_visible)
    {
        advanceSearch();
        filterUpdate();
    }
    m_tab->getActivePage()->refresh();
}

void FMWindow::advanceSearch()
{
    //qDebug()<<"advanceSearch clicked";
    if (m_side_bar_container->currentWidget() == m_filter)
    {
        //clear filter
        filterUpdate();
        m_filter_bar->clearData();
        m_side_bar_container->setCurrentWidget(m_side_bar);
        m_filter_visible = false;
    }
    else
    {
        m_filter_visible = true;
        //before show, update cur uri
        QString target_path = getCurrentUri();
        if (target_path.contains("file://"))
             m_advance_target_path = target_path;
        else
             m_advance_target_path = "file://" + target_path;

        m_filter_bar->updateLocation();
        m_side_bar_container->setCurrentWidget(m_filter);
    }
    //to solve back up automatic pop up issue
    QTimer::singleShot(100, this, [=](){
        m_search_bar->hideTableView();
    });
}

void FMWindow::clearRecord()
{
    //qDebug()<<"clearRecord clicked";
    m_search_bar->clearSearchRecord();
    m_clear_record->setDisabled(true);
}

void FMWindow::searchFilter(QString target_path, QString keyWord)
{
    auto targetUri = SearchVFSUriParser::parseSearchKey(target_path, keyWord);
    //qDebug()<<"targeturi:"<<targetUri;
    m_update_condition = true;
    this->goToUri(targetUri, true);
}

void FMWindow::filterUpdate(int type_index, int time_index, int size_index)
{
    //qDebug()<<"filterUpdate:";
    m_tab->getActivePage()->setSortFilter(type_index, time_index, size_index);
}

void FMWindow::setShowHidden()
{
    //qDebug()<<"setShowHidden"<<m_show_hidden_file;
    m_show_hidden_file = !m_show_hidden_file;
    m_tab->getActivePage()->setShowHidden(m_show_hidden_file);
}

void FMWindow::forceStopLoading()
{
    m_tab->getActivePage()->stopLoading();
    m_is_loading = false;
}

void FMWindow::onPreviewPageSwitch(const QString &id)
{
    if (id.isNull()) {
        PreviewPageIface *page = m_preview_page_container->getCurrentPage();
        if (page) {
            m_preview_page_container->removePage(page);
        }
    } else {
        auto oldPage = m_preview_page_container->getCurrentPage();
        PreviewPageIface *page = PreviewPageFactoryManager::getInstance()->getPlugin(id)->createPreviewPage();
        m_preview_page_container->setCurrentPage(page);
        m_preview_page_container->removePage(oldPage);
        //emit selection changed signal manually for starting a preview with new page.
        Q_EMIT m_tab->currentSelectionChanged();
    }
}

void FMWindow::setCurrentSelectionUris(const QStringList &uris)
{
    m_tab->getActivePage()->getView()->setSelections(uris);
}

void FMWindow::setCurrentSortOrder(Qt::SortOrder order)
{
    m_tab->getActivePage()->getView()->setSortOrder(order);
}

Qt::SortOrder FMWindow::getCurrentSortOrder()
{
    return Qt::SortOrder(m_tab->getActivePage()->getView()->getSortOrder());
}

void FMWindow::setCurrentSortColumn(int column)
{
    m_tab->getActivePage()->getView()->setSortType(column);
}

int FMWindow::getCurrentSortColumn()
{
    return m_tab->getActivePage()->getView()->getSortType();
}

const QString FMWindow::getCurrentPageViewType()
{
    if (m_tab->getActivePage()->getView()) {
        return m_tab->getActivePage()->getView()->viewId();
    }
    else {
        return DirectoryViewFactoryManager2::getInstance()->getDefaultViewId();
    }
}

//preview page container
PreviewPageContainer::PreviewPageContainer(QWidget *parent) : QStackedWidget (parent)
{
    setMinimumWidth(300);
}

void PreviewPageContainer::setCurrentPage(PreviewPageIface *page)
{
    if (count() > 0) {
        PreviewPageIface *oldPage = getCurrentPage();
        if (oldPage) {
            removePage(oldPage);
        }
    }

    addWidget(dynamic_cast<QWidget*>(page));
    setCurrentWidget(dynamic_cast<QWidget*>(page));
    dynamic_cast<QWidget*>(page)->show();
    show();
}

void PreviewPageContainer::removePage(PreviewPageIface *page)
{
    if (!page)
        return;
    removeWidget(dynamic_cast<QWidget*>(page));
    if (count() == 0) {
        hide();
    }
    page->closePreviewPage();
}

PreviewPageIface *PreviewPageContainer::getCurrentPage()
{
    return dynamic_cast<PreviewPageIface*>(currentWidget());
}

//ToolBarContainer
ToolBarContainer::ToolBarContainer(QWidget *parent) : QToolBar(parent)
{

}

void ToolBarContainer::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(this->palette().dark().color());
    p.drawLine(this->rect().bottomLeft(), this->rect().bottomRight());
}
