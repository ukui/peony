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

#include "x11-window-manager.h"
#include "navigation-tab-bar.h"

#include <QWidget>
#include <QMouseEvent>

#include <QTabBar>
#include <QStyle>
#include <QStyleOptionTabBarBase>

#include <QApplication>

#include <QDebug>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xatom.h>

#define START_DRAG_TIME 120

static X11WindowManager *global_instance = nullptr;

X11WindowManager *X11WindowManager::getInstance()
{
    if (!global_instance) {
        global_instance = new X11WindowManager;
    }
    return global_instance;
}

X11WindowManager::X11WindowManager(QObject *parent) : QObject(parent)
{

}

bool X11WindowManager::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug()<<event->type();

    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchCancel:
    case QEvent::TouchEnd: {
        qDebug()<<event->type();
        break;
    }

    case QEvent::MouseButtonPress: {

        //Make tabbar blank area dragable and do not effect tablabel.
        if (qobject_cast<QTabBar *>(watched)) {
            QMouseEvent *e = static_cast<QMouseEvent *>(event);
            QTabBar *tabBar = qobject_cast<QTabBar *>(watched);
            if (tabBar->tabAt(e->pos()) != -1)
                return false;
        }
        QMouseEvent *e = static_cast<QMouseEvent *>(event);

        if (QObject::eventFilter(watched, event))
            return true;
        if (e->button() == Qt::LeftButton) {
            m_prepare_drag_time = e->timestamp();
            //修改页签可以拖拽，记录鼠标位置
            m_press_pos = QCursor::pos();
            m_is_draging = true;
            m_current_widget = static_cast<QWidget *>(watched);
            m_toplevel_offset = m_current_widget->topLevelWidget()->mapFromGlobal(m_press_pos);
        }

        //qDebug()<<event->type();

        break;
    }
    case QEvent::MouseMove: {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);

        qDebug()<<e->type()<<e->pos();

        bool isTouchMove = e->source() == Qt::MouseEventSynthesizedByQt;

        if (m_is_draging) {
            if (QX11Info::isPlatformX11()) {
                Display *display = QX11Info::display();
                Atom netMoveResize = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
                XEvent xEvent;
                const auto pos = QCursor::pos();

                memset(&xEvent, 0, sizeof(XEvent));
                xEvent.xclient.type = ClientMessage;
                xEvent.xclient.message_type = netMoveResize;
                xEvent.xclient.display = display;
                xEvent.xclient.window = m_current_widget->topLevelWidget()->winId();
                xEvent.xclient.format = 32;
                xEvent.xclient.data.l[0] = pos.x() * qApp->devicePixelRatio();
                xEvent.xclient.data.l[1] = pos.y() * qApp->devicePixelRatio();
                xEvent.xclient.data.l[2] = 8;
                xEvent.xclient.data.l[3] = Button1;
                xEvent.xclient.data.l[4] = 0;

                XUngrabPointer(display, CurrentTime);
                XSendEvent(display, QX11Info::appRootWindow(QX11Info::appScreen()),
                           False, SubstructureNotifyMask | SubstructureRedirectMask,
                           &xEvent);
                XFlush(display);

                m_is_draging = false;

                //NOTE: use x11 move will ungrab the window focus
                //hide and show will restore the focus and it seems
                //there is no bad effect for peony main window.
                if (isTouchMove) {
                    if (!m_current_widget->mouseGrabber()) {
                        m_current_widget->grabMouse();
                        m_current_widget->releaseMouse();
                    }
                }

                if (qobject_cast<NavigationTabBar *>(m_current_widget)) {
                    m_current_widget->hide();
                    m_current_widget->show();
                }

                //balance mouse release event
                QMouseEvent me(QMouseEvent::MouseButtonRelease, e->pos(), e->windowPos(), e->screenPos(), e->button(), e->buttons(), e->modifiers(), Qt::MouseEventSynthesizedByApplication);
                qApp->sendEvent(watched, &me);

                return true;
            } else {
                //auto me = static_cast<QMouseEvent *>(event);
                auto widget = qobject_cast<QWidget *>(watched);
                auto topLevel = widget->topLevelWidget();
                auto globalPos = QCursor::pos();
                //auto offset = globalPos - m_press_pos;
                topLevel->move(globalPos - m_toplevel_offset);
            }
        }
        break;
    }
    case QEvent::MouseButtonRelease: {
        auto me = static_cast<QMouseEvent *>(event);

        if (me->source() == Qt::MouseEventSynthesizedByApplication)
            break;

        m_prepare_drag_time = 0;
        m_press_pos = QPoint();
        m_is_draging = false;
        m_current_widget = nullptr;
        break;
    }
    default:
        return false;
    }
    return false;
}

void X11WindowManager::registerWidget(QWidget *widget)
{
    widget->removeEventFilter(this);
    widget->installEventFilter(this);
}

