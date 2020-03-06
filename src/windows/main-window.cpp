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

#include "navigation-side-bar.h"

#include "peony-main-window-style.h"

#include <QPainter>

#include <QDebug>

#include <X11/Xlib.h>

MainWindow::MainWindow(const QString &uri, QWidget *parent) : QMainWindow(parent)
{
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
    setAttribute(Qt::WA_OpaquePaintEvent);
    setWindowFlag(Qt::FramelessWindowHint);
    setContentsMargins(4, 4, 4, 4);

    //bind resize handler
    auto handler = new QWidgetResizeHandler(this);
    handler->setMovingEnabled(false);

    //disable style window manager
    setProperty("useStyleWindowManager", false);

    //init UI
    initUI();
}

void MainWindow::syncControlsLocation(const QString &uri)
{
    //FIXME:
}

void MainWindow::goToUri(const QString &uri, bool addHistory, bool force)
{
    //FIXME:
    //go to uri
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    validBorder();
    update();
    QMainWindow::resizeEvent(e);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    validBorder();
    QColor color = this->palette().base().color();
    color.setAlphaF(0.5);
    m_effect->setWindowBackground(color);
    QPainter p(this);
    m_effect->drawWindowShadowManually(&p, this->rect());
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

void MainWindow::initUI()
{
    //HeaderBar
    auto headerBar = new HeaderBar(this);
    m_header_bar = headerBar;
    addToolBar(headerBar);

    //SideBar
    QDockWidget *sidebarContainer = new QDockWidget(this);
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

    sidebarContainer->setWidget(sidebar);
    addDockWidget(Qt::LeftDockWidgetArea, sidebarContainer);

    auto views = new TabWidget;
    views->addPage("file:///");
    views->addPage("file:///home");

    connect(views, &TabWidget::closeWindowRequest, this, &QWidget::close);

    X11WindowManager *tabBarHandler = X11WindowManager::getInstance();
    tabBarHandler->registerWidget(views->tabBar());

    setCentralWidget(views);
}
