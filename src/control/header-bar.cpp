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

#include <QHBoxLayout>
#include <QUrl>
#include <QMessageBox>

#include <QStyleOptionToolButton>

#include <QEvent>

#include <QDebug>

static HeaderBarStyle *global_instance = nullptr;
static QString terminal_cmd = nullptr;

HeaderBar::HeaderBar(MainWindow *parent) : QToolBar(parent)
{
    setMouseTracking(true);
    setStyle(HeaderBarStyle::getStyle());

    m_window = parent;
    //disable default menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    //setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet(".HeaderBar{"
                  "background-color: transparent;"
                  "border: 0px solid transparent;"
                  "margin: 4px 5px 4px 5px;"
                  "};");

    setMovable(false);

    auto a = addAction(QIcon::fromTheme("folder-new-symbolic"), tr("Create Folder"), [=](){
        //use the same function
        m_window->createFolderOperation();
    });
    auto createFolder = qobject_cast<QToolButton *>(widgetForAction(a));
    createFolder->setAutoRaise(false);
    createFolder->setFixedSize(QSize(40, 40));
    createFolder->setIconSize(QSize(16, 16));

    addSpacing(2);

    //find a terminal when init
    findDefaultTerminal();
    a = addAction(QIcon::fromTheme("terminal-app-symbolic"), tr("Open Terminal"), [=](){
        //open the default terminal
        openDefaultTerminal();
    });
    auto openTerminal = qobject_cast<QToolButton *>(widgetForAction(a));
    openTerminal->setAutoRaise(false);
    openTerminal->setFixedSize(QSize(40, 40));
    openTerminal->setIconSize(QSize(16, 16));

    addSpacing(9);

    auto goBack = new HeadBarPushButton(this);
    m_go_back = goBack;
    goBack->setEnabled(false);
    goBack->setToolTip(tr("Go Back"));
    goBack->setFixedSize(QSize(36, 28));
    goBack->setIcon(QIcon::fromTheme("go-previous-symbolic"));
    addWidget(goBack);
    connect(goBack, &QPushButton::clicked, m_window, [=](){
        m_window->getCurrentPage()->goBack();
    });

    auto goForward = new HeadBarPushButton(this);
    m_go_forward = goForward;
    goForward->setEnabled(false);
    goForward->setToolTip(tr("Go Forward"));
    goForward->setFixedSize(QSize(36, 28));
    goForward->setIcon(QIcon::fromTheme("go-next-symbolic"));
    addWidget(goForward);
    connect(goForward, &QPushButton::clicked, m_window, [=](){
        m_window->getCurrentPage()->goForward();
    });

    addSpacing(9);

    auto locationBar = new Peony::AdvancedLocationBar(this);
    m_location_bar = locationBar;
    addWidget(locationBar);

    connect(m_location_bar, &Peony::AdvancedLocationBar::updateWindowLocationRequest, this, &HeaderBar::updateLocationRequest);

    addSpacing(9);
    a = addAction(QIcon::fromTheme("edit-find-symbolic"), tr("Search"));
    connect(a, &QAction::triggered, this, &HeaderBar::searchButtonClicked);
    auto search = qobject_cast<QToolButton *>(widgetForAction(a));
    search->setAutoRaise(false);
    search->setFixedSize(QSize(40, 40));
    setIconSize(QSize(16, 16));
    m_search_button = search;

    addSpacing(9);

    a = addAction(QIcon::fromTheme("view-grid-symbolic"), tr("View Type"));
    auto viewType = qobject_cast<QToolButton *>(widgetForAction(a));
    viewType->setAutoRaise(false);
    viewType->setFixedSize(QSize(57, 40));
    viewType->setIconSize(QSize(16, 16));
    viewType->setPopupMode(QToolButton::InstantPopup);

    m_view_type_menu = new ViewTypeMenu(viewType);
    viewType->setMenu(m_view_type_menu);

    connect(m_view_type_menu, &ViewTypeMenu::switchViewRequest, this, [=](const QString &id, const QIcon &icon){
        viewType->setText(id);
        viewType->setIcon(icon);
        this->viewTypeChangeRequest(id);
    });

    addSpacing(2);

    a = addAction(QIcon::fromTheme("view-sort-ascending-symbolic"), tr("Sort Type"));
    auto sortType = qobject_cast<QToolButton *>(widgetForAction(a));
    sortType->setAutoRaise(false);
    sortType->setFixedSize(QSize(57, 40));
    sortType->setIconSize(QSize(16, 16));
    sortType->setPopupMode(QToolButton::InstantPopup);

    m_sort_type_menu = new SortTypeMenu(this);
    sortType->setMenu(m_sort_type_menu);

    connect(m_sort_type_menu, &SortTypeMenu::switchSortTypeRequest, m_window, &MainWindow::setCurrentSortColumn);
    connect(m_sort_type_menu, &SortTypeMenu::switchSortOrderRequest, m_window, [=](Qt::SortOrder order){
        if (order == Qt::AscendingOrder) {
            sortType->setIcon(QIcon::fromTheme("view-sort-ascending-symbolic"));
        } else {
            sortType->setIcon(QIcon::fromTheme("view-sort-descending-symbolic"));
        }
        m_window->setCurrentSortOrder(order);
    });
    connect(m_sort_type_menu, &QMenu::aboutToShow, m_sort_type_menu, [=](){
        m_sort_type_menu->setSortType(m_window->getCurrentSortColumn());
        m_sort_type_menu->setSortOrder(m_window->getCurrentSortOrder());
    });

    addSpacing(2);

    a = addAction(QIcon::fromTheme("open-menu-symbolic"), tr("Option"));
    auto popMenu = qobject_cast<QToolButton *>(widgetForAction(a));
    popMenu->setAutoRaise(false);
    popMenu->setFixedSize(QSize(57, 40));
    popMenu->setIconSize(QSize(16, 16));
    popMenu->setPopupMode(QToolButton::InstantPopup);

    m_operation_menu = new OperationMenu(m_window, this);
    popMenu->setMenu(m_operation_menu);
}

void HeaderBar::findDefaultTerminal()
{
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
    }
    g_strfreev (argv);
}

void HeaderBar::searchButtonClicked()
{
    m_search_mode = ! m_search_mode;
    //qDebug() << "searchButtonClicked" <<m_search_mode;
    m_search_button->setCheckable(m_search_mode);
    m_search_button->setChecked(m_search_mode);
    m_search_button->setDown(m_search_mode);
    m_location_bar->switchEditMode(m_search_mode);
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

void HeaderBar::setLocation(const QString &uri)
{
    m_location_bar->updateLocation(uri);
}

void HeaderBar::startEdit(bool bSearch)
{
    //qDebug() << "bSearch" <<bSearch <<m_search_mode;
    if (bSearch && m_search_mode)
        return;

    if (bSearch)
    {
        searchButtonClicked();
    }
    else
    {
        m_search_mode = false;
        m_location_bar->startEdit();
        m_location_bar->switchEditMode(false);
    }
}

void HeaderBar::finishEdit()
{
    m_location_bar->finishEdit();
}

void HeaderBar::updateIcons()
{
    qDebug()<<m_window->getCurrentUri();
    qDebug()<<m_window->getCurrentSortColumn();
    qDebug()<<m_window->getCurrentSortOrder();
    m_view_type_menu->setCurrentView(m_window->getCurrentPage()->getView()->viewId());
    m_sort_type_menu->switchSortTypeRequest(m_window->getCurrentSortColumn());
    m_sort_type_menu->switchSortOrderRequest(m_window->getCurrentSortOrder());

    //go back & go forward
    m_go_back->setEnabled(m_window->getCurrentPage()->canGoBack());
    m_go_forward->setEnabled(m_window->getCurrentPage()->canGoForward());

    //maximize & restore
    updateMaximizeState();
}

void HeaderBar::updateMaximizeState()
{
    //maximize & restore
    //do it in container
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
            button.features |= QStyleOptionToolButton::HasMenu;
            button.features |= QStyleOptionToolButton::MenuButtonPopup;
            button.subControls |= QStyle::SC_ToolButtonMenu;
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

    setFixedHeight(50);
    setMovable(false);

    m_layout = new QHBoxLayout;
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_internal_widget = new QWidget(this);
    m_internal_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

bool HeaderBarContainer::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj)
    if (e->type() == QEvent::MouseMove) {
        //auto w = qobject_cast<QWidget *>(obj);
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        //this->setCursor(c);
        //w->setCursor(c);
        this->topLevelWidget()->setCursor(c);
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

    addWindowButtons();

    m_internal_widget->setLayout(m_layout);
    addWidget(m_internal_widget);
}

void HeaderBarContainer::addWindowButtons()
{
    //m_window_buttons = new QWidget(this);
    auto layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 4, 0);
    layout->setSpacing(4);

    //minimize, maximize and close
    auto minimize = new QToolButton(m_internal_widget);
    minimize->setIcon(QIcon::fromTheme("window-minimize-symbolic"));
    minimize->setToolTip(tr("Minimize"));
    minimize->setAutoRaise(false);
    minimize->setFixedSize(QSize(40, 40));
    minimize->setIconSize(QSize(16, 16));
    connect(minimize, &QToolButton::clicked, this, [=](){
        m_header_bar->m_window->showMinimized();
    });

    //window-maximize-symbolic
    //window-restore-symbolic
    auto maximizeAndRestore = new QToolButton(m_internal_widget);
    maximizeAndRestore->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
    maximizeAndRestore->setAutoRaise(false);
    maximizeAndRestore->setFixedSize(QSize(40, 40));
    maximizeAndRestore->setIconSize(QSize(16, 16));
    connect(maximizeAndRestore, &QToolButton::clicked, this, [=](){
        m_header_bar->m_window->maximizeOrRestore();

        bool maximized = m_header_bar->m_window->isMaximized();
        if (maximized) {
            maximizeAndRestore->setIcon(QIcon::fromTheme("window-restore-symbolic"));
        } else {
            maximizeAndRestore->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        }
    });

    auto close = new QToolButton(m_internal_widget);
    close->setIcon(QIcon::fromTheme("window-close-symbolic"));
    close->setToolTip(tr("Close"));
    close->setAutoRaise(false);
    close->setFixedSize(QSize(40, 40));
    close->setIconSize(QSize(16, 16));
    connect(close, &QToolButton::clicked, this, [=](){
        m_header_bar->m_window->close();
    });

    layout->addWidget(minimize);
    layout->addWidget(maximizeAndRestore);
    layout->addWidget(close);

    m_layout->addLayout(layout);

    minimize->setMouseTracking(true);
    minimize->installEventFilter(this);
    maximizeAndRestore->setMouseTracking(true);
    maximizeAndRestore->installEventFilter(this);
    close->setMouseTracking(true);
    close->installEventFilter(this);
}
