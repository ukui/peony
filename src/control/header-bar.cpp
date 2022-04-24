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

#include "header-bar.h"
#include "main-window.h"
#include <gio/gio.h>

#include "view-type-menu.h"
#include "sort-type-menu.h"
#include "operation-menu.h"

#include "directory-view-container.h"
#include "directory-view-widget.h"
#include "advanced-location-bar.h"

#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface2.h"
#include "search-vfs-uri-parser.h"
#include "file-info.h"
#include "file-info-job.h"
#include "file-utils.h"
#include "tab-widget.h"
#include "preview-page-factory-manager.h"
#include "preview-page-plugin-iface.h"

#include "clipboard-utils.h"
#include "file-operation-utils.h"

#include <QHBoxLayout>
#include <QUrl>
#include <QMessageBox>
#include <QProcess>
#include <QDir>

#include <QStyleOptionToolButton>

#include <QEvent>
#include <QApplication>
#include <QTimer>


#include <KWindowSystem>
#include "global-settings.h"
#ifdef KYLIN_COMMON
#include <ukuisdk/kylin-com4cxx.h>
#endif

#include <QtConcurrent>
#include <QAction>

#include <QX11Info>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QPainter>

#include <QDebug>

#include "search-widget.h"

#define DBUS_STATUS_MANAGER_IF "com.kylin.statusmanager.interface"

static HeaderBarStyle *global_instance = nullptr;
static QString terminal_cmd = nullptr;
static QDBusInterface *g_statusManagerDBus = nullptr;

HeaderBar::HeaderBar(MainWindow *parent) : QToolBar(parent)
{
    setAttribute(Qt::WA_AcceptTouchEvents);

    setMouseTracking(true);
    setStyle(HeaderBarStyle::getStyle());
    setFocusPolicy(Qt::TabFocus);

    m_window = parent;
    //disable default menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet(".HeaderBar{"
                  "background-color: transparent;"
                  "border: 0px solid transparent;"
                  "};");

    setMovable(false);

//    auto a = addAction(QIcon::fromTheme("folder-new-symbolic"), tr("Create Folder"), [=]() {
//        //use the same function
//        m_window->createFolderOperation();
//    });
//    auto createFolder = qobject_cast<QToolButton *>(widgetForAction(a));
//    createFolder->setAutoRaise(false);
//    createFolder->setFixedSize(QSize(40, 40));
//    createFolder->setIconSize(QSize(16, 16));

//    addSpacing(2);

//    //find a terminal when init
//    findDefaultTerminal();
//    a = addAction(QIcon::fromTheme("terminal-app-symbolic"), tr("Open Terminal"), [=]() {
//        //open the default terminal
//        openDefaultTerminal();
//    });
//    auto openTerminal = qobject_cast<QToolButton *>(widgetForAction(a));
//    openTerminal->setAutoRaise(false);
//    openTerminal->setFixedSize(QSize(40, 40));
//    openTerminal->setIconSize(QSize(16, 16));


    auto goBack = new HeadBarPushButton(this);
    m_go_back = goBack;
    goBack->setEnabled(false);
    goBack->setToolTip(tr("Go Back"));
    goBack->setFixedSize(QSize(36, 28));
    goBack->setIcon(QIcon::fromTheme("go-previous-symbolic"));
//    goBack->setFlat(true);
    setStyleSheet("HeadBarPushButton{"
                  "background-color: transparent;"
                  "};");
    auto a = addWidget(goBack);
    m_actions.insert(HeaderBarAction::GoBack, a);


    auto goForward = new HeadBarPushButton(this);
    m_go_forward = goForward;
    goForward->setEnabled(false);
    goForward->setToolTip(tr("Go Forward"));
    goForward->setFixedSize(QSize(36, 28));
    goForward->setIcon(QIcon::fromTheme("go-next-symbolic"));
    a = addWidget(goForward);
    m_actions.insert(HeaderBarAction::GoForward, a);
    connect(goForward, &QPushButton::clicked, m_window, [=]() {
        m_window->getCurrentPage()->goForward();
    });

    m_is_intel = (QString::compare("V10SP1-edu", QString::fromStdString(KDKGetPrjCodeName()), Qt::CaseInsensitive) == 0);
    if (! m_is_intel)
    {
        //non intel project, show go up button
        auto goUp = new HeadBarPushButton(this);
        m_go_up = goUp;
        goUp->setEnabled(true);
        goUp->setToolTip(tr("Go Up"));
        goUp->setFixedSize(QSize(36, 28));
        goUp->setIcon(QIcon::fromTheme("go-up-symbolic"));
        a = addWidget(goUp);
        m_actions.insert(HeaderBarAction::GoForward, a);
        connect(goUp, &QPushButton::clicked, m_window, [=]() {
            m_window->getCurrentPage()->cdUp();
        });
    }

    addSpacing(9);

    //task#10993 实现文档管理器路径模式与搜索模式切换动画
    m_searchWidget = new Peony::SearchWidget(this);
    a = addWidget(m_searchWidget);
    m_actions.insert(HeaderBarAction::LocationBar, a);

    connect(goBack, &QPushButton::clicked, m_window, [=]() {
        m_window->getCurrentPage()->goBack();
        Q_EMIT m_searchWidget->clearSearchBox();
    });

    connect(m_searchWidget, &Peony::SearchWidget::refreshRequest, [=]() {
        m_window->updateTabPageTitle();
    });
    connect(m_searchWidget, &Peony::SearchWidget::updateFileTypeFilter, [=](const int &index) {
        m_window->getCurrentPage()->setSortFilter(index);
    });

    connect(this, &HeaderBar::closeSearch, m_searchWidget, &Peony::SearchWidget::closeSearch );
    connect(this, &HeaderBar::setGlobalFlag, m_searchWidget, &Peony::SearchWidget::setGlobalFlag );
    connect(this, &HeaderBar::updateSearchRecursive, m_searchWidget, &Peony::SearchWidget::updateSearchRecursive);
    connect(this, &HeaderBar::setLocation, m_searchWidget, &Peony::SearchWidget::updateLocation);
    connect(this, &HeaderBar::cancelEdit, m_searchWidget, &Peony::SearchWidget::cancelEdit);
    connect(this, &HeaderBar::startEdit, m_searchWidget, &Peony::SearchWidget::startEdit);
    connect(this, &HeaderBar::finishEdit, m_searchWidget, &Peony::SearchWidget::finishEdit);
    connect(m_searchWidget, &Peony::SearchWidget::updateSearchRequest, this, &HeaderBar::updateSearchRequest);
    connect(m_searchWidget, &Peony::SearchWidget::updateLocationRequest, this, &HeaderBar::updateLocationRequest);

    addSpacing(2);

    a = addAction(QIcon::fromTheme("view-grid-symbolic"), tr("View Type"));
    m_actions.insert(HeaderBarAction::ViewType, a);
    auto viewType = qobject_cast<QToolButton *>(widgetForAction(a));
    viewType->setAutoRaise(false);
    viewType->setFixedSize(QSize(57, 40));
    viewType->setIconSize(QSize(16, 16));
    viewType->setPopupMode(QToolButton::InstantPopup);

    m_view_type_menu = new ViewTypeMenu(viewType);
    QAction * preview = new QAction(tr("Details"));
    m_preview_action = preview;
    connect(m_view_type_menu,&QMenu::aboutToShow,m_view_type_menu,[=](){
        m_view_type_menu->addSeparator();
        m_view_type_menu->insertAction(0,preview);
    });
//    m_view_type_menu->insertAction(0,preview);
    preview->setCheckable(true);

    a->setMenu(m_view_type_menu);

    auto manager = Peony::PreviewPageFactoryManager::getInstance();
    auto pluginNames = manager->getPluginNames();

    connect(preview,&QAction::triggered,[=](bool checked){
        m_window->m_tab->setTriggeredPreviewPage(checked);
        for (auto name : pluginNames) {
            if (checked) {
                auto plugin = Peony::PreviewPageFactoryManager::getInstance()->getPlugin(name);
               m_window->m_tab->setPreviewPage(plugin->createPreviewPage());
            } else {
                m_window->m_tab->setPreviewPage(nullptr);
            }

        }

    });
//    preview->setShortcuts(QList<QKeySequence>()<<Qt::Key_F3<<QKeySequence(Qt::ALT + Qt::Key_P));
    preview->setShortcut(QKeySequence(Qt::ALT + Qt::Key_P));
    preview->setShortcutVisibleInContextMenu(false);

    connect(m_view_type_menu, &ViewTypeMenu::switchViewRequest, this, [=](const QString &id, const QIcon &icon, bool resetToZoomLevel) {
        viewType->setText(id);
        viewType->setIcon(icon);
        this->viewTypeChangeRequest(id);
        if (resetToZoomLevel) {
            auto viewId = m_window->getCurrentPage()->getView()->viewId();
            auto factoryManger = Peony::DirectoryViewFactoryManager2::getInstance();
            auto factory = factoryManger->getFactory(viewId);
            int zoomLevelHint = factory->zoom_level_hint();
            m_window->getCurrentPage()->setZoomLevelRequest(zoomLevelHint);
        }
    });

    connect(m_view_type_menu, &ViewTypeMenu::updateZoomLevelHintRequest, this, &HeaderBar::updateZoomLevelHintRequest);

    addSpacing(2);

    a = addAction(QIcon::fromTheme("view-sort-ascending-symbolic"), tr("Sort Type"));
    m_actions.insert(HeaderBarAction::SortType, a);
    auto sortType = qobject_cast<QToolButton *>(widgetForAction(a));
    sortType->setAutoRaise(false);
    sortType->setFixedSize(QSize(57, 40));
    sortType->setIconSize(QSize(16, 16));
    sortType->setPopupMode(QToolButton::InstantPopup);

    m_sort_type_menu = new SortTypeMenu(this);
    a->setMenu(m_sort_type_menu);

    connect(m_sort_type_menu, &SortTypeMenu::switchSortTypeRequest, m_window, &MainWindow::setCurrentSortColumn);
    connect(m_sort_type_menu, &SortTypeMenu::switchSortOrderRequest, m_window, [=](Qt::SortOrder order) {
        if (order == Qt::AscendingOrder) {
            sortType->setIcon(QIcon::fromTheme("view-sort-ascending-symbolic"));
        } else {
            sortType->setIcon(QIcon::fromTheme("view-sort-descending-symbolic"));
        }
        m_window->setCurrentSortOrder(order);
    });
    connect(m_sort_type_menu, &QMenu::aboutToShow, m_sort_type_menu, [=]() {
        m_sort_type_menu->setSortType(m_window->getCurrentSortColumn());
        m_sort_type_menu->setSortOrder(m_window->getCurrentSortOrder());
    });

    addSpacing(3);

    // Add by wnn, add tool button when select item
    a = addAction(QIcon::fromTheme("edit-copy-symbolic"), tr("&Copy"));
    m_actions.insert(HeaderBarAction::Copy, a);
    a->setVisible(false);
    a->setToolTip(tr("Copy"));
    auto copy = qobject_cast<QToolButton *>(widgetForAction(a));
    copy->setFixedSize(QSize(40, 40));
    connect(a, &QAction::triggered, [=]() {
        if (!m_window->getCurrentSelections().isEmpty()) {
            if (m_window->getCurrentSelections().first().startsWith("trash://", Qt::CaseInsensitive)) {
                return ;
            }

            Peony::ClipboardUtils::setClipboardFiles(m_window->getCurrentSelections(), false);
        }
    });

    a = addAction(QIcon::fromTheme("edit-cut-symbolic"), tr("&Cut"));
    m_actions.insert(HeaderBarAction::Cut, a);
    a->setVisible(false);
    a->setToolTip(tr("Cut"));
    auto cut = qobject_cast<QToolButton *>(widgetForAction(a));
    cut->setFixedSize(QSize(40, 40));
    connect(a, &QAction::triggered, [=]() {
        Peony::ClipboardUtils::setClipboardFiles(m_window->getCurrentSelections(), true);
    });

    a = addAction(tr("&Select All"));
    a->setIcon(QIcon::fromTheme("edit-select-all-symbolic"));
    m_actions.insert(HeaderBarAction::SeletcAll, a);
    a->setVisible(false);
    a->setToolTip(tr("Select All"));
    auto select = qobject_cast<QToolButton *>(widgetForAction(a));
    select->setFixedSize(QSize(40, 40));
    connect(a, &QAction::triggered, [=]() {
        m_window->getCurrentPage()->getView()->selectAll();
    });

    a = addAction(QIcon::fromTheme("edit-delete-symbolic"), tr("&Delete to trash"));
    m_actions.insert(HeaderBarAction::Delete, a);
    a->setVisible(false);
    a->setToolTip(tr("Delete to trash"));
    auto trash = qobject_cast<QToolButton *>(widgetForAction(a));
    trash->setFixedSize(QSize(40, 40));
    connect(a, &QAction::triggered, [=]() {
        if (m_window->getCurrentUri() == "trash:///") {
            Peony::FileOperationUtils::executeRemoveActionWithDialog(m_window->getCurrentSelections());
        } else {
            Peony::FileOperationUtils::trash(m_window->getCurrentSelections(), true);
        }
    });

    for (auto action : actions()) {
        auto w = widgetForAction(action);
        w->setProperty("iconHighlightEffectMode", 1);
        w->setProperty("useIconHighlightEffect", true);
    }
}

void HeaderBar::findDefaultTerminal()
{
    QtConcurrent::run([](){
        GList *infos = g_app_info_get_all();
        GList *l = infos;
        while (l) {
            const char *cmd = g_app_info_get_executable(static_cast<GAppInfo*>(l->data));
            QString tmp = cmd;
            if (tmp.contains("terminal")) {
                terminal_cmd = tmp;
                if (tmp == "mate-terminal") {
                    break;
                }
            }
            l = l->next;
        }
        g_list_free_full(infos, g_object_unref);
    });
}

void HeaderBar::openDefaultTerminal()
{
    //don't find any terminal
    if (terminal_cmd == nullptr)
    {
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle(tr("Operate Tips"));
        msgBox->setText(tr("Don't find any terminal, please install at least one terminal!"));
        msgBox->exec();
        return;
    }

    QUrl url = m_window->getCurrentUri();
    auto directory = url.path().toUtf8().constData();
    gchar **argv = nullptr;
    g_shell_parse_argv (terminal_cmd.toUtf8().constData(), nullptr, &argv, nullptr);
    GError *err = nullptr;
    g_spawn_async (directory,
                   argv,
                   nullptr,
                   G_SPAWN_SEARCH_PATH,
                   nullptr,
                   nullptr,
                   nullptr,
                   &err);
    if (err) {
        qDebug()<<err->message;
        g_error_free(err);
        err = nullptr;
        tryOpenAgain();
    }
    g_strfreev (argv);
}

void HeaderBar::tryOpenAgain()
{
    QUrl url = m_window->getCurrentUri();
    auto absPath = url.path();
    qDebug() << "tryOpenAgain url" <<url<<absPath<<terminal_cmd;
    QProcess p;
    p.setProgram(terminal_cmd);
    p.setArguments(QStringList()<<"--working-directory"<<absPath);
    p.startDetached(p.program(), p.arguments());
    p.waitForFinished(-1);
}

void HeaderBar::switchSelectStatus(bool select)
{
    if (select) {
        m_actions.find(HeaderBarAction::SortType).value()->setVisible(false);
        m_actions.find(HeaderBarAction::ViewType).value()->setVisible(false);
        m_actions.find(HeaderBarAction::Copy).value()->setVisible(true);
        m_actions.find(HeaderBarAction::Cut).value()->setVisible(true);
        m_actions.find(HeaderBarAction::SeletcAll).value()->setVisible(true);
        m_actions.find(HeaderBarAction::Delete).value()->setVisible(true);
    }
    else {
        m_actions.find(HeaderBarAction::SortType).value()->setVisible(true);
        m_actions.find(HeaderBarAction::ViewType).value()->setVisible(true);
        m_actions.find(HeaderBarAction::Copy).value()->setVisible(false);
        m_actions.find(HeaderBarAction::Cut).value()->setVisible(false);
        m_actions.find(HeaderBarAction::SeletcAll).value()->setVisible(false);
        m_actions.find(HeaderBarAction::Delete).value()->setVisible(false);
    }
    //fix bug#100105 After the selected status changes, the view type is grayed out.
    updateViewTypeEnable();
    updateSortTypeEnable();
}

void HeaderBar::addSpacing(int pixel)
{
    for (int i = 0; i < pixel; i++) {
        addSeparator();
    }
}

void HeaderBar::mouseMoveEvent(QMouseEvent *e)
{
    QToolBar::mouseMoveEvent(e);
    QCursor c;
    c.setShape(Qt::ArrowCursor);
    this->topLevelWidget()->setCursor(c);
}

void HeaderBar::mouseDoubleClickEvent(QMouseEvent *e)
{
    QToolBar::mouseDoubleClickEvent(e);
    if(e->button() == Qt::LeftButton || e->button() == Qt::RightButton){
        m_window->maximizeOrRestore();
    }
}

void HeaderBar::updatePreviewPageVisible()
{
    auto manager = Peony::PreviewPageFactoryManager::getInstance();
    auto pluginNames = manager->getPluginNames();
    for (auto name : pluginNames) {
        if (m_view_type_menu->menuAction()->isVisible() && m_preview_action->isChecked()) {
            auto plugin = Peony::PreviewPageFactoryManager::getInstance()->getPlugin(name);
            m_window->m_tab->setPreviewPage(plugin->createPreviewPage());
        } else {
            m_window->m_tab->setPreviewPage(nullptr);
        }
    }
}

void HeaderBar::updateIcons()
{
    if(!m_window)
        return;

    if (!m_window->getCurrentPage())
        return;

    if (!m_window->getCurrentPage()->getView())
        return;

    qDebug()<<"updateIcons:" <<m_window->getCurrentUri();
    qDebug()<<"updateIcons:" <<m_window->getCurrentSortColumn();
    qDebug()<<"updateIcons:" <<m_window->getCurrentSortOrder();
    m_view_type_menu->setCurrentDirectory(m_window->getCurrentUri());
    m_view_type_menu->setCurrentView(m_window->getCurrentPage()->getView()->viewId(), true);
    m_sort_type_menu->switchSortTypeRequest(m_window->getCurrentSortColumn());
    m_sort_type_menu->switchSortOrderRequest(m_window->getCurrentSortOrder());
    m_searchWidget->updateCloseSearch(Peony::FileUtils::getFileIconName(m_window->getCurrentUri()));
    //go back & go forward
    if (m_window->getCurrentPage()) {
        m_go_back->setEnabled(m_window->getCurrentPage()->canGoBack());
        m_go_forward->setEnabled(m_window->getCurrentPage()->canGoForward());
        if (! m_is_intel)
        {
            m_go_up->setEnabled(m_window->getCurrentPage()->canCdUp());
            m_go_up->setProperty("useIconHighlightEffect", 0x2);
            m_go_up->setProperty("isWindowButton", 1);
        }
    }

    //fix create folder fail issue in special path
//    auto curUri = m_window->getCurrentUri();
//    auto info = Peony::FileInfo::fromUri(curUri, false);
//    Peony::FileInfoJob job(info);
//    job.querySync();
//    if (info->canWrite())
//        m_create_folder->setEnabled(true);
//    else
//        m_create_folder->setEnabled(false);

    m_go_back->setProperty("useIconHighlightEffect", 0x2);
    m_go_back->setProperty("isWindowButton", 1);
    m_go_forward->setProperty("useIconHighlightEffect", 0x2);
    m_go_forward->setProperty("isWindowButton", 1);

    //maximize & restore
    //updateMaximizeState();
}

void HeaderBar::updateViewTypeEnable()
{
    auto url = m_window->getCurrentUri();
    //qDebug() << "updateViewTypeEnable url:" << url;
    if(url == "computer:///"){
        m_view_type_menu->setEnabled(false);
        m_view_type_menu->menuAction()->setVisible(false);
    }else{
        m_view_type_menu->setEnabled(true);
        m_view_type_menu->menuAction()->setVisible(true);
    }
}

void HeaderBar::updateSortTypeEnable()
{
    auto url = m_window->getCurrentUri();
    qDebug() << "url:" << url;
    if(url == "computer:///"){
        m_sort_type_menu->setEnabled(false);
        m_sort_type_menu->menuAction()->setVisible(false);
    }else{
        m_sort_type_menu->setEnabled(true);
        m_sort_type_menu->menuAction()->setVisible(true);
    }
}

void HeaderBar::updateMaximizeState()
{
    //maximize & restore
    //do it in container
    bool maximized = m_window->isMaximized();
    if (maximized) {
        m_maximize_restore_button->setIcon(QIcon::fromTheme("window-restore-symbolic"));
        m_maximize_restore_button->setToolTip(tr("Restore"));
    } else {
        m_maximize_restore_button->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        m_maximize_restore_button->setToolTip(tr("Maximize"));
    }
}

void HeaderBar::cancleSelect() {
    switchSelectStatus(false);
}

//HeaderBarToolButton
HeaderBarToolButton::HeaderBarToolButton(QWidget *parent) : QToolButton(parent)
{
    setAutoRaise(false);
    setIconSize(QSize(16, 16));
}

//HeadBarPushButton
HeadBarPushButton::HeadBarPushButton(QWidget *parent) : QPushButton(parent)
{
    setIconSize(QSize(16, 16));
}

//HeaderBarStyle
HeaderBarStyle *HeaderBarStyle::getStyle()
{
    if (!global_instance) {
        global_instance = new HeaderBarStyle;
    }
    return global_instance;
}

int HeaderBarStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    if (qobject_cast<const HeaderBarContainer *>(widget))
        return 0;

    switch (metric) {
    case PM_ToolBarIconSize:
        return 16;
    case PM_ToolBarSeparatorExtent:
        return 1;
    case PM_ToolBarItemSpacing: {
        return 1;
    }
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

void HeaderBarStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    //This is a "lie". We want to use instant popup menu for tool button, and we aslo
    //want use popup menu style with this tool button, so we change the related flags
    //to draw in our expected.
    if (control == CC_ToolButton) {
        QStyleOptionToolButton button = *qstyleoption_cast<const QStyleOptionToolButton *>(option);
        if (button.features.testFlag(QStyleOptionToolButton::HasMenu)) {
            button.features = QStyleOptionToolButton::None;
            if (!widget->property("isOptionButton").toBool()) {
                button.features |= QStyleOptionToolButton::HasMenu;
                button.features |= QStyleOptionToolButton::MenuButtonPopup;
                button.subControls |= QStyle::SC_ToolButtonMenu;
            }
            button.palette.setColor(QPalette::Button,Qt::transparent);
            return QProxyStyle::drawComplexControl(control, &button, painter, widget);
        }
    }
    return QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void HeaderBarStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == PE_IndicatorToolBarSeparator) {
        return;
    }
    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}

HeaderBarContainer::HeaderBarContainer(QWidget *parent) : QToolBar(parent)
{
    setStyle(HeaderBarStyle::getStyle());

    setContextMenuPolicy(Qt::CustomContextMenu);

    setStyleSheet(".HeaderBarContainer"
                  "{"
                  "background-color: transparent;"
                  "border: 0px solid transparent"
                  "}");

    setFixedHeight(80);
    setMovable(false);

    m_layout = new QHBoxLayout;
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_internal_widget = new QWidget(this);
    m_internal_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

TopMenuBar::TopMenuBar(HeaderBar *headerBar, MainWindow *parent) : QMenuBar(parent)
{
    m_window = parent;
    m_header_bar = headerBar;
    setContextMenuPolicy(Qt::CustomContextMenu);

    setStyleSheet(".TopMenuBar"
                  "{"
                  "background-color: transparent;"
                  "border: 0px solid transparent"
                  "}");

    setFixedHeight(48);

    m_top_menu_layout = new QHBoxLayout(this);
    m_top_menu_layout->setSpacing(0);
    m_top_menu_layout->setContentsMargins(0, 0, 0, 0);

    m_top_menu_internal_widget = new QWidget(this);
    m_top_menu_internal_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addWindowButtons();
    m_window->installEventFilter(this);
}

bool TopMenuBar::eventFilter(QObject *obj, QEvent *e)
{

    Q_UNUSED(obj)
    if (m_window) {
        //use updateMaximizeState function, comment those code
        if (e->type() == QEvent::Resize || QEvent::WindowStateChange == e->type()) {
            //fix bug#95419, drag change window to restore not change state issue
            m_header_bar->updateMaximizeState();
        }
        //fix double click space window has no change issue, bug#38499
//        if (e->type() == QEvent::MouseButtonDblClick)
//        {
//            m_header_bar->m_window->maximizeOrRestore();
//        }
        return false;
    } else {
        if (e->type() == QEvent::MouseMove) {
            //auto w = qobject_cast<QWidget *>(obj);
            QCursor c;
            c.setShape(Qt::ArrowCursor);
            //this->setCursor(c);
            //w->setCursor(c);
            this->topLevelWidget()->setCursor(c);
        }

    }

    return false;

}

void TopMenuBar::addWindowButtons()
{
    m_window->installEventFilter(this);
    auto layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto optionButton = new QToolButton(m_top_menu_internal_widget);
    optionButton->setIcon(QIcon::fromTheme("open-menu-symbolic"));
    optionButton->setToolTip(tr("Option"));
    optionButton->setAutoRaise(true);
    optionButton->setFixedSize(QSize(48, 48));
    optionButton->setIconSize(QSize(16, 16));
    optionButton->setPopupMode(QToolButton::InstantPopup);
    optionButton->setProperty("isOptionButton", true);
    optionButton->setProperty("isWindowButton", 1);

    OperationMenu *operationMenu = new OperationMenu(m_window, optionButton);
    optionButton->setMenu(operationMenu);

    //minimize, maximize and close
    //  最小化，最大化，关闭
    auto minimize = new QToolButton(m_top_menu_internal_widget);
    minimize->setIcon(QIcon::fromTheme("window-minimize-symbolic"));
    minimize->setToolTip(tr("Minimize"));
    minimize->setAutoRaise(true);
    minimize->setFixedSize(QSize(48, 48));
    minimize->setIconSize(QSize(16, 16));

    connect(minimize, &QToolButton::clicked, this, [=]() {
        KWindowSystem::minimizeWindow(m_window->winId());
        m_window->showMinimized();
    });

    //window-maximize-symbolic
    //window-restore-symbolic
    auto maximizeAndRestore = new QToolButton(m_top_menu_internal_widget);
    m_header_bar->m_maximize_restore_button = maximizeAndRestore;
    //switch tips with button status, fix bug#77604
    m_header_bar->updateMaximizeState();
    maximizeAndRestore->setAutoRaise(true);
    maximizeAndRestore->setFixedSize(QSize(48, 48));
    maximizeAndRestore->setIconSize(QSize(16, 16));

    connect(maximizeAndRestore, &QToolButton::clicked, this, [=]() {
        m_window->maximizeOrRestore();
    });
    m_max_or_restore = maximizeAndRestore;

    auto close = new QToolButton(m_top_menu_internal_widget);
    close->setIcon(QIcon::fromTheme("window-close-symbolic"));
    close->setToolTip(tr("Close"));
    close->setAutoRaise(true);
    close->setFixedSize(QSize(48, 48));
    close->setIconSize(QSize(16, 16));

    connect(close, &QToolButton::clicked, this, [=]() {
        m_window->close();
    });

    auto palette = qApp->palette();
    palette.setColor(QPalette::Highlight, QColor("#E54A50"));
    close->setPalette(palette);

    m_minimize = minimize;
    m_close = close;

    if (!g_statusManagerDBus) {
        g_statusManagerDBus = new QDBusInterface(DBUS_STATUS_MANAGER_IF, "/" ,DBUS_STATUS_MANAGER_IF,QDBusConnection::sessionBus(),this);
    }
    QDBusReply<bool> message_a = g_statusManagerDBus->call("get_current_tabletmode");
    if (message_a.isValid()) {
        m_tablet_mode = message_a.value();
    }
    updateTabletMode(m_tablet_mode);

    connect(g_statusManagerDBus, SIGNAL(mode_change_signal(bool)), this, SLOT(updateTabletMode(bool)));

    layout->addWidget(optionButton);
    layout->addWidget(minimize);
    layout->addWidget(maximizeAndRestore);
    layout->addWidget(close);

    m_top_menu_internal_widget->setLayout(layout);
    QSpacerItem *spacer = new QSpacerItem(4000, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_top_menu_layout->addSpacerItem(spacer);
    m_top_menu_layout->addWidget(m_top_menu_internal_widget);

    optionButton->setMouseTracking(true);
    optionButton->installEventFilter(this);
    minimize->setMouseTracking(true);
    minimize->installEventFilter(this);
    maximizeAndRestore->setMouseTracking(true);
    maximizeAndRestore->installEventFilter(this);
    close->setMouseTracking(true);
    close->installEventFilter(this);

    for (int i = 0; i < layout->count(); i++) {
        auto w = layout->itemAt(i)->widget();
        w->setProperty("isWindowButton", 1);
        w->setProperty("useIconHighlightEffect", 0x2);
    }
    close->setProperty("isWindowButton", 2);
    close->setProperty("useIconHighlightEffect", 0x8);

    optionButton->setVisible((Peony::GlobalSettings::getInstance()->getProjectName() != V10_SP1_EDU));
}

void TopMenuBar::updateTabletMode(bool isTabletMode)
{
    m_tablet_mode = isTabletMode;
    if(m_tablet_mode)
    {
        m_minimize->hide();
        m_max_or_restore->hide();
        m_close->hide();
    }
    else
    {
        m_minimize->setVisible(true);
        m_max_or_restore->setVisible(true);
        m_close->setVisible(true);
    }
    QTimer::singleShot(100, this, [=](){
        auto palette = qApp->palette();
        palette.setColor(QPalette::Highlight, QColor("#E54A50"));
        m_close->setPalette(palette);
    });
}

bool HeaderBarContainer::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj)
    auto window = qobject_cast<MainWindow *>(obj);
    if (window) {
//        if (e->type() == QEvent::Resize) {
//            if (window->isMaximized()) {
//                m_max_or_restore->setIcon(QIcon::fromTheme("window-restore-symbolic"));
//                //m_max_or_restore->setToolTip(tr("Restore"));
//            } else {
//                m_max_or_restore->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
//                //m_max_or_restore->setToolTip(tr("Maximize"));
//            }
//        }
        return false;
    } else {
        if (e->type() == QEvent::MouseMove) {
            //auto w = qobject_cast<QWidget *>(obj);
            QCursor c;
            c.setShape(Qt::ArrowCursor);
            //this->setCursor(c);
            //w->setCursor(c);
            this->topLevelWidget()->setCursor(c);
        }
    }

    return false;
}

void HeaderBarContainer::addHeaderBar(HeaderBar *headerBar)
{
    if (m_header_bar)
        return;
    m_header_bar = headerBar;

    headerBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout->addWidget(headerBar);

    m_internal_widget->setLayout(m_layout);
    addWidget(m_internal_widget);

//    m_header_bar->m_window->installEventFilter(this);
}


