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

#include <QHBoxLayout>
#include <advanced-location-bar.h>

static HeaderBarStyle *global_instance = nullptr;

HeaderBar::HeaderBar(MainWindow *parent) : QToolBar(parent)
{
    setStyle(HeaderBarStyle::getStyle());

    m_window = parent;
    //disable default menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet(".HeaderBar{"
                  "background-color: transparent;"
                  "border: 0px solid transparent;"
                  "margin: 4px 5px 4px 5px;"
                  "};");

    setMovable(false);

    auto a = addAction(QIcon::fromTheme("folder-new-symbolic"), tr("Create Folder"), [=](){
        //FIXME:
    });
    auto createFolder = qobject_cast<QToolButton *>(widgetForAction(a));
    createFolder->setAutoRaise(false);
    createFolder->setFixedSize(QSize(40, 40));
    createFolder->setIconSize(QSize(16, 16));

    a = addAction(QIcon::fromTheme("terminal-app-symbolic"), tr("Open Terminal"), [=](){
        //FIXME:
    });
    auto openTerminal = qobject_cast<QToolButton *>(widgetForAction(a));
    openTerminal->setAutoRaise(false);
    openTerminal->setFixedSize(QSize(40, 40));
    openTerminal->setIconSize(QSize(16, 16));

    addSeparator();

    auto goBack = new HeadBarPushButton(this);
    goBack->setToolTip(tr("Go Back"));
    goBack->setFixedSize(QSize(36, 28));
    goBack->setIcon(QIcon::fromTheme("go-previous-symbolic"));
    addWidget(goBack);

    auto goForward = new HeadBarPushButton(this);
    goForward->setToolTip(tr("Go Forward"));
    goForward->setFixedSize(QSize(36, 28));
    goForward->setIcon(QIcon::fromTheme("go-next-symbolic"));
    addWidget(goForward);

    addSeparator();

    auto locationBar = new Peony::AdvancedLocationBar(this);
    addWidget(locationBar);

    addSeparator();
    a = addAction(QIcon::fromTheme("edit-find-symbolic"), tr("Search"), [=](){
        //FIXME:
    });
    auto search = qobject_cast<QToolButton *>(widgetForAction(a));
    search->setAutoRaise(false);
    search->setFixedSize(QSize(40, 40));
    setIconSize(QSize(16, 16));

    addSeparator();

    a = addAction(QIcon::fromTheme("view-grid-symbolic"), tr("View Type"), [=](){
        //FIXME:
    });
    auto viewType = qobject_cast<QToolButton *>(widgetForAction(a));
    viewType->setAutoRaise(false);
    viewType->setFixedSize(QSize(57, 40));
    viewType->setIconSize(QSize(16, 16));
    viewType->setPopupMode(QToolButton::MenuButtonPopup);

    a = addAction(QIcon::fromTheme("view-sort-descending-symbolic"), tr("Sort Type"), [=](){
        //FIXME:
    });
    auto sortType = qobject_cast<QToolButton *>(widgetForAction(a));
    sortType->setAutoRaise(false);
    sortType->setFixedSize(QSize(57, 40));
    sortType->setIconSize(QSize(16, 16));
    sortType->setPopupMode(QToolButton::MenuButtonPopup);

    a = addAction(QIcon::fromTheme("open-menu-symbolic"), tr("Option"), [=](){
        //FIXME:
    });
    auto popMenu = qobject_cast<QToolButton *>(widgetForAction(a));
    popMenu->setAutoRaise(false);
    popMenu->setFixedSize(QSize(57, 40));
    popMenu->setIconSize(QSize(16, 16));
    popMenu->setPopupMode(QToolButton::MenuButtonPopup);

    //minimize, maximize and close
    a = addAction(QIcon::fromTheme("window-minimize-symbolic"), tr("Minimize"), [=](){
        //FIXME:
    });
    auto minimize = qobject_cast<QToolButton *>(widgetForAction(a));
    minimize->setAutoRaise(false);
    minimize->setFixedSize(QSize(40, 40));
    minimize->setIconSize(QSize(16, 16));

    //window-maximize-symbolic
    //window-restore-symbolic
    a = addAction(QIcon::fromTheme("window-maximize-symbolic"), nullptr, [=](){
        //FIXME:
    });
    auto maximizeAndRestore = qobject_cast<QToolButton *>(widgetForAction(a));
    maximizeAndRestore->setAutoRaise(false);
    maximizeAndRestore->setFixedSize(QSize(40, 40));
    maximizeAndRestore->setIconSize(QSize(16, 16));

    a = addAction(QIcon::fromTheme("window-close-symbolic"), tr("Close"), [=](){

    });
    auto close = qobject_cast<QToolButton *>(widgetForAction(a));
    close->setAutoRaise(false);
    close->setFixedSize(QSize(40, 40));
    close->setIconSize(QSize(16, 16));
    connect(close, &QToolButton::clicked, this, [=](){
        m_window->close();
    });
}

void HeaderBar::setLocation(const QString &uri)
{
    //FIXME:
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
    switch (metric) {
    case PM_ToolBarIconSize:
        return 16;
    case PM_ToolBarSeparatorExtent:
        return 6;
    case PM_ToolBarItemSpacing: {
        return 2;
    }
    case PM_ToolBarItemMargin:
    case PM_ToolBarFrameWidth:
        return 0;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

void HeaderBarStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == PE_IndicatorToolBarSeparator) {
        return;
    }
    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}
