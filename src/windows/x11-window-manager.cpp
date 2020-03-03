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

#include <QWidget>
#include <QMouseEvent>

#include <QTabBar>
#include <QStyle>
#include <QStyleOptionTabBarBase>

#include <QDebug>

#include <QX11Info>
#include <X11/Xlib.h>

X11WindowManager::X11WindowManager(QObject *parent) : QObject(parent)
{

}

bool X11WindowManager::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type()) {
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
            m_is_draging = true;
            m_current_widget = static_cast<QWidget *>(watched);
        }

        qDebug()<<event->type();

        break;
    }
    case QEvent::MouseMove: {
        if (m_is_draging) {
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
        break;
    }
    case QEvent::MouseButtonRelease:{
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
