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

#include "main-window.h"
#include "header-bar.h"
#include "global-settings.h"

#include "border-shadow-effect.h"
#include <private/qwidgetresizehandler_p.h>

#include <QVariant>
#include <QMouseEvent>
#include <QX11Info>

#include <QDockWidget>
#include <QTreeView>

#include "side-bar-proxy-filter-sort-model.h"
#include "side-bar-model.h"

#include "directory-view-container.h"
#include "tab-widget.h"
#include "x11-window-manager.h"
#include "properties-window.h"

#include "navigation-side-bar.h"
#include "advance-search-bar.h"

#include "peony-main-window-style.h"

#include "file-label-box.h"
#include <file-operation-manager.h>
#include <file-operation-utils.h>
#include <create-template-operation.h>

#include "directory-view-menu.h"
#include "directory-view-widget.h"
#include "main-window-factory.h"

#include <QSplitter>

#include <QPainter>

#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>
#include <QDesktopServices>

#include <QDebug>

#include <X11/Xlib.h>

MainWindow::MainWindow(const QString &uri, QWidget *parent) : QMainWindow(parent)
{
    auto settings = Peony::GlobalSettings::getInstance();
    m_show_hidden_file = settings->isExist("show-hidden")? settings->getValue("show-hidden").toBool(): false;
    m_use_default_name_sort_order = settings->isExist("chinese-first")? !settings->getValue("chinese-first").toBool(): false;
    m_folder_first = settings->isExist("folder-first")? settings->getValue("folder-first").toBool(): true;

    setStyle(PeonyMainWindowStyle::getStyle());

    setMinimumWidth(600);
    m_effect = new BorderShadowEffect(this);
    m_effect->setPadding(4);
    m_effect->setBorderRadius(6);
    m_effect->setBlurRadius(4);
    //setGraphicsEffect(m_effect);

    setAnimated(false);
    //setAttribute(Qt::WA_DeleteOnClose); //double free, why?
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    setWindowFlag(Qt::FramelessWindowHint);
    setContentsMargins(4, 4, 4, 4);

    //bind resize handler
    auto handler = new QWidgetResizeHandler(this);
    handler->setMovingEnabled(false);
    m_resize_handler = handler;

    //disable style window manager
    setProperty("useStyleWindowManager", false);

    //short cut settings
    setShortCuts();

    //init UI
    initUI(uri);
}

Peony::FMWindowFactory *MainWindow::getFactory()
{
    return MainWindowFactory::getInstance();
}

Peony::DirectoryViewContainer *MainWindow::getCurrentPage()
{
    return m_tab->currentPage();
}

void MainWindow::setShortCuts()
{
    //stop loading action
    QAction *stopLoadingAction = new QAction(this);
    stopLoadingAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(stopLoadingAction);
    connect(stopLoadingAction, &QAction::triggered, this, &MainWindow::forceStopLoading);

    //show hidden action
    QAction *showHiddenAction = new QAction(this);
    showHiddenAction->setShortcut(QKeySequence(tr("Ctrl+H", "Show|Hidden")));
    addAction(showHiddenAction);
    connect(showHiddenAction, &QAction::triggered, this, [=](){
        this->setShowHidden();
    });

    auto undoAction = new QAction(QIcon::fromTheme("edit-undo-symbolic"), tr("Undo"), this);
    undoAction->setShortcut(QKeySequence::Undo);
    addAction(undoAction);
    connect(undoAction, &QAction::triggered, [=](){
        Peony::FileOperationManager::getInstance()->undo();
    });

    auto redoAction = new QAction(QIcon::fromTheme("edit-redo-symbolic"), tr("Redo"), this);
    redoAction->setShortcut(QKeySequence::Redo);
    addAction(redoAction);
    connect(redoAction, &QAction::triggered, [=](){
        Peony::FileOperationManager::getInstance()->redo();
    });

    auto trashAction = new QAction(this);
    trashAction->setShortcut(Qt::Key_Delete);
    connect(trashAction, &QAction::triggered, [=](){
        auto uris = this->getCurrentSelections();
        if (!uris.isEmpty()) {
            bool isTrash = this->getCurrentUri() == "trash:///";
            if (!isTrash) {
                Peony::FileOperationUtils::trash(uris, true);
            } else {
                Peony::FileOperationUtils::executeRemoveActionWithDialog(uris);
            }
        }
    });
    addAction(trashAction);

    auto deleteAction = new QAction(this);
    deleteAction->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Delete));
    addAction(deleteAction);
    connect(deleteAction, &QAction::triggered, [=](){
        auto uris = this->getCurrentSelections();
        Peony::FileOperationUtils::executeRemoveActionWithDialog(uris);
    });

//    auto searchAction = new QAction(this);
//    searchAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::CTRL + Qt::Key_F)<<QKeySequence(Qt::CTRL + Qt::Key_E)<<Qt::Key_F3);
//    connect(searchAction, &QAction::triggered, this, [=](){
//        m_search_bar->setFocus();
//    });
//    addAction(searchAction);

    auto locationAction = new QAction(this);
    locationAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::CTRL + Qt::Key_D));
    connect(locationAction, &QAction::triggered, this, [=](){
        m_header_bar->startEdit();
    });
    addAction(locationAction);

    auto newWindowAction = new QAction(this);
    newWindowAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    connect(newWindowAction, &QAction::triggered, this, [=](){
        MainWindow *newWindow = new MainWindow(getCurrentUri());
        newWindow->show();
    });
    addAction(newWindowAction);

    auto closeWindowAction = new QAction(this);
    closeWindowAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_F4));
    connect(closeWindowAction, &QAction::triggered, this, [=](){
        this->close();
    });
    addAction(closeWindowAction);

    auto aboutAction = new QAction(this);
    aboutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F2));
    connect(aboutAction, &QAction::triggered, this, [=](){
        QMessageBox::about(this,
                           tr("Peony Qt"),
                           tr("Authour: \n"
                              "\tYue Lan <lanyue@kylinos.cn>\n"
                              "\tMeihong He <hemeihong@kylinos.cn>\n"
                              "\n"
                              "Copyright (C): 2019, Tianjin KYLIN Information Technology Co., Ltd."));
    });
    addAction(aboutAction);

    auto newTabAction = new QAction(this);
    newTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    connect(newTabAction, &QAction::triggered, this, [=](){
        this->addNewTabs(QStringList()<<this->getCurrentUri());
    });
    addAction(newTabAction);

    auto closeTabAction = new QAction(this);
    closeTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    connect(closeTabAction, &QAction::triggered, this, [=](){
        if (m_tab->count() <= 1) {
            this->close();
        } else {
            m_tab->removeTab(m_tab->currentIndex());
        }
    });
    addAction(closeTabAction);

    auto nextTabAction = new QAction(this);
    nextTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab));
    connect(nextTabAction, &QAction::triggered, this, [=](){
        int currentIndex = m_tab->currentIndex();
        if (currentIndex + 1 < m_tab->count()) {
            m_tab->setCurrentIndex(currentIndex + 1);
        } else {
            m_tab->setCurrentIndex(0);
        }
    });
    addAction(nextTabAction);

    auto previousTabAction = new QAction(this);
    previousTabAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Tab));
    connect(previousTabAction, &QAction::triggered, this, [=](){
        int currentIndex = m_tab->currentIndex();
        if (currentIndex > 0) {
            m_tab->setCurrentIndex(currentIndex - 1);
        } else {
            m_tab->setCurrentIndex(m_tab->count() - 1);
        }
    });
    addAction(previousTabAction);

    auto newFolderAction = new QAction(this);
    newFolderAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    connect(newFolderAction, &QAction::triggered, this, [=](){
        Peony::CreateTemplateOperation op(getCurrentUri(), Peony::CreateTemplateOperation::EmptyFolder, tr("New Folder"));
        op.run();
        auto targetUri = op.target();
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
            QTimer::singleShot(500, this, [=](){
#else
            QTimer::singleShot(500, [=](){
#endif
            this->getCurrentPage()->getView()->scrollToSelection(targetUri);
            this->editUri(targetUri);
        });
    });
    addAction(newFolderAction);

    //show selected item's properties
    auto propertiesWindowAction = new QAction(this);
    propertiesWindowAction->setShortcut(QKeySequence(Qt::ALT + Qt::Key_Return));
    connect(propertiesWindowAction, &QAction::triggered, this, [=](){
        //Fixed issue:when use this shortcut without any selections, this will crash
        if (getCurrentSelections().count() > 0)
        {
            Peony::PropertiesWindow *w = new Peony::PropertiesWindow(getCurrentSelections());
            w->show();
        }
    });
    addAction(propertiesWindowAction);

    auto helpAction = new QAction(this);
    helpAction->setShortcut(QKeySequence(Qt::Key_F1));
    connect(helpAction, &QAction::triggered, this, [=](){
        QUrl url = QUrl("help:ubuntu-kylin-help/files", QUrl::TolerantMode);
        QDesktopServices::openUrl(url);
    });
    addAction(helpAction);

    auto maxAction = new QAction(this);
    maxAction->setShortcut(QKeySequence(Qt::Key_F11));
    connect(maxAction, &QAction::triggered, this, [=](){
        if (!this->isFullScreen()) {
            this->showFullScreen();
        } else {
            this->showMaximized();
        }
    });
    addAction(maxAction);

//    auto previewPageAction = new QAction(this);
//    previewPageAction->setShortcuts(QList<QKeySequence>()<<QKeySequence(Qt::ALT + Qt::Key_P));
//    connect(previewPageAction, &QAction::triggered, this, [=](){
//        auto lastPreviewPageId = m_navigation_bar->getLastPreviewPageId();
//        m_navigation_bar->triggerAction(lastPreviewPageId);
//    });
//    addAction(previewPageAction);

    auto refreshAction = new QAction(this);
    refreshAction->setShortcut(Qt::Key_F5);
    connect(refreshAction, &QAction::triggered, this, [=](){
        this->refresh();
    });
    addAction(refreshAction);
}

const QString MainWindow::getCurrentUri()
{
    //qDebug() << "getCurrentUri in Main-window";
    return m_tab->getCurrentUri();
}

const QStringList MainWindow::getCurrentSelections()
{
    return m_tab->getCurrentSelections();
}

const QStringList MainWindow::getCurrentAllFileUris()
{
    return m_tab->getAllFileUris();
}

Qt::SortOrder MainWindow::getCurrentSortOrder()
{
    return m_tab->getSortOrder();
}

int MainWindow::getCurrentSortColumn()
{
    return m_tab->getSortType();
}

void MainWindow::maximizeOrRestore()
{
    if (!this->isMaximized()) {
        this->showMaximized();
    } else {
        this->showNormal();
    }
    m_header_bar->updateIcons();
}

void MainWindow::syncControlsLocation(const QString &uri)
{
    m_tab->goToUri(uri, false, false);
}

void MainWindow::updateHeaderBar()
{
    m_header_bar->setLocation(getCurrentUri());
    m_header_bar->updateIcons();
}

void MainWindow::goToUri(const QString &uri, bool addHistory, bool force)
{
    QUrl url(uri);
    auto realUri = uri;
    if (url.scheme().isEmpty()) {
        if (uri.startsWith("/")) {
            realUri = "file://" + uri;
        } else {
            QDir currentDir = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
            currentDir.cd(uri);
            auto absPath = currentDir.absoluteFilePath(uri);
            url = QUrl::fromLocalFile(absPath);

            realUri = url.toDisplayString();
        }
    }

    if (getCurrentUri() == realUri) {
        if (!force)
            return;
    }

    locationChangeStart();
    m_tab->goToUri(realUri, addHistory, force);
    m_header_bar->setLocation(uri);
}

void MainWindow::addNewTabs(const QStringList &uris)
{
    for (auto uri : uris) {
        m_tab->addPage(uri, false);
    }
}

void MainWindow::beginSwitchView(const QString &viewId)
{
    auto selection = getCurrentSelections();
//    int sortType = getCurrentSortColumn();
//    Qt::SortOrder sortOrder = getCurrentSortOrder();
    m_tab->switchViewType(viewId);
    m_tab->setCurrentSelections(selection);
}

void MainWindow::refresh()
{
    goToUri(getCurrentUri(), false, true);
}

void MainWindow::advanceSearch()
{
    qDebug()<<"advanceSearch clicked";
    initAdvancePage();
}

void MainWindow::clearRecord()
{
    //qDebug()<<"clearRecord clicked";
//    m_search_bar->clearSearchRecord();
//    m_clear_record->setDisabled(true);
}

void MainWindow::searchFilter(QString target_path, QString keyWord, bool search_file_name, bool search_content)
{
//    auto targetUri = SearchVFSUriParser::parseSearchKey(target_path, keyWord, search_file_name, search_content);
//    //qDebug()<<"targeturi:"<<targetUri;
//    m_update_condition = true;
//    this->goToUri(targetUri, true);
}

void MainWindow::filterUpdate(int type_index, int time_index, int size_index)
{
    //qDebug()<<"filterUpdate:";
    //m_tab->getActivePage()->setSortFilter(type_index, time_index, size_index);
}

void MainWindow::setShowHidden(bool showHidden)
{
    //qDebug()<<"setShowHidden"<<m_show_hidden_file;
    m_show_hidden_file = showHidden;
    getCurrentPage()->setShowHidden(showHidden);
}

void MainWindow::setShowHidden()
{
    //qDebug()<<"setShowHidden"<<m_show_hidden_file;
    m_show_hidden_file = !m_show_hidden_file;
    getCurrentPage()->setShowHidden(m_show_hidden_file);
}

void MainWindow::setUseDefaultNameSortOrder(bool use)
{
    m_use_default_name_sort_order = use;
    getCurrentPage()->setUseDefaultNameSortOrder(use);
}

void MainWindow::setSortFolderFirst(bool folderFirst)
{
    m_folder_first = folderFirst;
    getCurrentPage()->setSortFolderFirst(folderFirst);
}

void MainWindow::forceStopLoading()
{
    m_tab->stopLoading();
}

void MainWindow::setCurrentSelectionUris(const QStringList &uris)
{
    m_tab->setCurrentSelections(uris);
}

void MainWindow::setCurrentSortOrder(Qt::SortOrder order)
{
    m_tab->setSortOrder(order);
}

void MainWindow::setCurrentSortColumn(int sortColumn)
{
    m_tab->setSortType(sortColumn);
}

void MainWindow::editUri(const QString &uri)
{
    m_tab->editUri(uri);
}

void MainWindow::editUris(const QStringList &uris)
{
    m_tab->editUris(uris);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    m_header_bar->updateMaximizeState();
    validBorder();
    update();
    QMainWindow::resizeEvent(e);
}

/*!
 * \note
 * The window has a noticeable tearing effect due to the drawing of shadow effects.
 * I should consider do not painting a shadow when resizing.
 */
void MainWindow::paintEvent(QPaintEvent *e)
{
    validBorder();
    QColor color = this->palette().window().color();

    QPainterPath sidebarPath;
    sidebarPath.addRect(sideBarRect());
    m_effect->setTransParentPath(sidebarPath);

    //color.setAlphaF(0.5);
    m_effect->setWindowBackground(color);
    QPainter p(this);

    m_effect->drawWindowShadowManually(&p, this->rect(), m_resize_handler->isButtonDown());
    QMainWindow::paintEvent(e);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    //qDebug()<<"mouse pressed"<<e;
    QMainWindow::mousePressEvent(e);
    if (e->button() == Qt::LeftButton && !e->isAccepted())
        m_is_draging = true;
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    //NOTE: when starting a X11 window move, the mouse move event
    //will unreachable when draging, and after draging we could not
    //get the release event correctly.
    //qDebug()<<"mouse move";
    QMainWindow::mouseMoveEvent(e);
    if (!m_is_draging)
        return;
    Display *display = QX11Info::display();
    Atom netMoveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
    XEvent xEvent;
    const auto pos = QCursor::pos();

    memset(&xEvent, 0, sizeof(XEvent));
    xEvent.xclient.type = ClientMessage;
    xEvent.xclient.message_type = netMoveResize;
    xEvent.xclient.display = display;
    xEvent.xclient.window = this->winId();
    xEvent.xclient.format = 32;
    xEvent.xclient.data.l[0] = pos.x();
    xEvent.xclient.data.l[1] = pos.y();
    xEvent.xclient.data.l[2] = 8;
    xEvent.xclient.data.l[3] = Button1;
    xEvent.xclient.data.l[4] = 0;

    XUngrabPointer(display, CurrentTime);
    XSendEvent(display, QX11Info::appRootWindow(QX11Info::appScreen()),
               False, SubstructureNotifyMask | SubstructureRedirectMask,
               &xEvent);
    XFlush(display);

    m_is_draging = false;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    /*!
     * \bug
     * release event sometimes "disappear" when we request
     * X11 window manager for movement.
     */
    QMainWindow::mouseReleaseEvent(e);
    //qDebug()<<"mouse released";
    m_is_draging = false;
}

void MainWindow::validBorder()
{
    if (this->isMaximized()) {
        setContentsMargins(0, 0, 0, 0);
        m_effect->setPadding(0);
        setProperty("blurRegion", QVariant());
    } else {
        setContentsMargins(4, 4, 4, 4);
        m_effect->setPadding(4);
        QPainterPath path;
        auto rect = this->rect();
        rect.adjust(4, 4, -4, -4);
        path.addRoundedRect(rect, 6, 6);
        setProperty("blurRegion", QRegion(path.toFillPolygon().toPolygon()));
    }
}

void MainWindow::initUI(const QString &uri)
{
    connect(this, &MainWindow::locationChangeStart, this, [=](){
        QCursor c;
        c.setShape(Qt::WaitCursor);
        this->setCursor(c);
        m_tab->setCursor(c);
    });

    connect(this, &MainWindow::locationChangeEnd, this, [=](){
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        this->setCursor(c);
        m_tab->setCursor(c);
        updateHeaderBar();
    });

    //HeaderBar
    auto headerBar = new HeaderBar(this);
    m_header_bar = headerBar;
    addToolBar(headerBar);

    connect(m_header_bar, &HeaderBar::updateLocationRequest, this, &MainWindow::goToUri);
    connect(m_header_bar, &HeaderBar::viewTypeChangeRequest, this, &MainWindow::beginSwitchView);

    //SideBar
    QDockWidget *sidebarContainer = new QDockWidget(this);

    sidebarContainer->setFeatures(QDockWidget::NoDockWidgetFeatures);
    auto palette = sidebarContainer->palette();
    palette.setColor(QPalette::Window, Qt::transparent);
    sidebarContainer->setPalette(palette);
//    sidebarContainer->setStyleSheet("{"
//                                    "background-color: transparent;"
//                                    "border: 0px solid transparent"
//                                    "}");
    sidebarContainer->setTitleBarWidget(new QWidget(this));
    sidebarContainer->titleBarWidget()->setFixedHeight(0);
    sidebarContainer->setAttribute(Qt::WA_TranslucentBackground);
    sidebarContainer->setContentsMargins(0, 0, 0, 0);

    NavigationSideBar *sidebar = new NavigationSideBar(this);
    m_side_bar = sidebar;
    connect(m_side_bar, &NavigationSideBar::updateWindowLocationRequest, this, &MainWindow::goToUri);

    auto labelDialog = new FileLabelBox(this);
    labelDialog->hide();

    auto splitter = new QSplitter(this);
    splitter->setHandleWidth(0);
    splitter->addWidget(sidebar);
    splitter->addWidget(labelDialog);

    connect(sidebar, &NavigationSideBar::labelButtonClicked, labelDialog, &QWidget::setVisible);

    sidebarContainer->setWidget(splitter);
    addDockWidget(Qt::LeftDockWidgetArea, sidebarContainer);

    auto views = new TabWidget;
    m_tab = views;
    if (uri.isNull()) {
        auto home = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        m_tab->addPage(home);
    } else {
        m_tab->addPage(uri, true);
    }

    connect(views->tabBar(), &QTabBar::tabBarDoubleClicked, this, [=](int index){
        if (index == -1)
            maximizeOrRestore();
    });
    connect(views, &TabWidget::closeWindowRequest, this, &QWidget::close);

    X11WindowManager *tabBarHandler = X11WindowManager::getInstance();
    tabBarHandler->registerWidget(views->tabBar());

    setCentralWidget(views);

    //bind signals
    connect(m_tab, &TabWidget::updateWindowLocationRequest, this, &MainWindow::goToUri);
    connect(m_tab, &TabWidget::activePageLocationChanged, this, &MainWindow::locationChangeEnd);
    connect(m_tab, &TabWidget::activePageViewTypeChanged, this, &MainWindow::updateHeaderBar);
    connect(m_tab, &TabWidget::activePageChanged, this, &MainWindow::updateHeaderBar);
    connect(m_tab, &TabWidget::menuRequest, this, [=](){
        Peony::DirectoryViewMenu menu(this);
        menu.exec(QCursor::pos());
    });
}

void MainWindow::initAdvancePage()
{
    //Fix me: advance search page, need the new design to develop new UI
    //auto filterBar = new Peony::AdvanceSearchBar(this);
}

QRect MainWindow::sideBarRect()
{
    auto pos = m_side_bar->mapTo(this, QPoint());
    return QRect(pos, m_side_bar->size());
}
