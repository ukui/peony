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

#include "navigation-tab-bar.h"
#include "x11-window-manager.h"

#include "file-utils.h"
#include "search-vfs-uri-parser.h"

#include <QToolButton>

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>

#include <QMimeData>
#include <QDrag>
#include <QGraphicsOpacityEffect>

#include <KWindowSystem>

#include <QApplication>
#include <QWindow>

#include "FMWindowIface.h"
#include "main-window.h"

static TabBarStyle *global_instance = nullptr;

NavigationTabBar::NavigationTabBar(QWidget *parent) : QTabBar(parent)
{
    setAcceptDrops(true);
    m_drag_timer.setInterval(750);
    m_drag_timer.setSingleShot(true);

    setStyle(TabBarStyle::getStyle());

    setContentsMargins(0, 0, 0, 0);
    //setFixedHeight(36);

    setProperty("useStyleWindowManager", false);
    setMovable(true);
    setExpanding(false);
    setTabsClosable(true);
    X11WindowManager::getInstance()->registerWidget(this);

    connect(this, &QTabBar::currentChanged, this, [=](int index){
        //qDebug()<<"current changed"<<index;
    });

    connect(this, &QTabBar::tabMoved, this, [=](int from, int to){
        //qDebug()<<"move"<<from<<"to"<<to;
    });

    connect(this, &QTabBar::tabBarClicked, this, [=](int index){
        //qDebug()<<"tab bar clicked"<<index;
    });

    connect(this, &QTabBar::tabBarDoubleClicked, this, [=](int index){
        //qDebug()<<"tab bar double clicked"<<index;
    });

    QToolButton *addPageButton = new QToolButton(this);
    addPageButton->setProperty("useIconHighlightEffect", true);
    addPageButton->setFixedSize(QSize(this->height() + 2, this->height() + 2));
    addPageButton->setIcon(QIcon::fromTheme("list-add-symbolic"));
    connect(addPageButton, &QToolButton::clicked, this, [=](){
        auto uri = tabData(currentIndex()).toString();
        Q_EMIT addPageRequest(uri, true);
    });

    m_float_button = addPageButton;

//    connect(this, &QTabBar::tabCloseRequested, this, [=](int index){
//        removeTab(index);
//    });

    setDrawBase(false);
}

void NavigationTabBar::addPages(const QStringList &uri)
{

}

void NavigationTabBar::updateLocation(int index, const QString &uri)
{
    auto iconName = Peony::FileUtils::getFileIconName(uri);
    auto displayName = Peony::FileUtils::getFileDisplayName(uri);
    //qDebug() << "updateLocation text:" <<displayName <<uri;
    if (uri.startsWith("search:///"))
    {
        QString nameRegexp = Peony::SearchVFSUriParser::getSearchUriNameRegexp(uri);
        QString targetDirectory = Peony::SearchVFSUriParser::getSearchUriTargetDirectory(uri);
        displayName = tr("Search \"%1\" in \"%2\"").arg(nameRegexp).arg(targetDirectory);
    }
    setTabText(index, displayName);
    setTabIcon(index, QIcon::fromTheme(iconName));
    setTabData(index, uri);
    relayoutFloatButton(false);

    Q_EMIT this->locationUpdated(uri);
}

void NavigationTabBar::addPage(const QString &uri, bool jumpToNewTab)
{
    if (!uri.isNull()) {
        auto iconName = Peony::FileUtils::getFileIconName(uri);
        auto displayName = Peony::FileUtils::getFileDisplayName(uri);
        addTab(QIcon::fromTheme(iconName), displayName);
        setTabData(count() - 1, uri);
        if (jumpToNewTab)
            setCurrentIndex(count() - 1);
        Q_EMIT this->pageAdded(uri);
    } else {
        if (currentIndex() == -1) {
            addPage("file:///", true);
        } else {
            QString uri = tabData(currentIndex()).toString();
            addPage(uri, jumpToNewTab);
        }
    }
}

void NavigationTabBar::tabRemoved(int index)
{
    //qDebug()<<"tab removed"<<index;
    QTabBar::tabRemoved(index);
    if (count() == 0) {
        Q_EMIT closeWindowRequest();
    }
    relayoutFloatButton(false);
}

void NavigationTabBar::tabInserted(int index)
{
    //qDebug()<<"tab inserted"<<index;
    QTabBar::tabInserted(index);
    relayoutFloatButton(true);
}

void NavigationTabBar::relayoutFloatButton(bool insterted)
{
    int fixedY = 0;
    if (count() == 0) {
        m_float_button->move(0, fixedY);
        return;
    }
    //qDebug()<<"relayout";
    auto lastTabRect = tabRect(count() - 1);
    fixedY = lastTabRect.center().y() - m_float_button->height()/2;
    m_float_button->move(lastTabRect.right(), fixedY);
    setFixedHeight(lastTabRect.height());
}

void NavigationTabBar::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept();
    return;
}

void NavigationTabBar::dragMoveEvent(QDragMoveEvent *e)
{
    e->accept();
    return;
}

void NavigationTabBar::dragLeaveEvent(QDragLeaveEvent *e)
{
    QTabBar::dragLeaveEvent(e);
}

void NavigationTabBar::dropEvent(QDropEvent *e)
{
    if (e->source() != this) {
        if (e->mimeData()->hasUrls()) {
            for (auto url : e->mimeData()->urls()) {
                if (Peony::FileUtils::isFileDirectory(url.url())) {
                    addPageRequest(url.url(), true);
                }
            }
        } else if (e->mimeData()->hasFormat("peony/tab-index")) {
            auto uri = e->mimeData()->data("peony/tab-index");
            if (Peony::FileUtils::isFileDirectory(uri)) {
                addPageRequest(uri, true);
            }
        }

        //finish the drag, remove old tab page from old tab.
        if (auto oldTab = qobject_cast<NavigationTabBar *>(e->source())) {
            oldTab->removeTab(oldTab->currentIndex());
        }
    }
}

void NavigationTabBar::mousePressEvent(QMouseEvent *e)
{
    QTabBar::mousePressEvent(e);
    if (!m_drag_timer.isActive()) {
        m_start_drag = true;
        m_drag_timer.start();
    } else {
        m_start_drag = false;
    }

    QTimer::singleShot(1, this, [=](){
        if (!m_start_drag)
            return;

        QTimer::singleShot(750, this, [=](){
            if (tabAt(e->pos()) == -1)
                return;

            if (!m_start_drag)
                return;
            //start a drag
            //note that we should remove this tab from the window
            //at other tab's drop event.

            auto pixmap = this->topLevelWidget()->grab().scaledToWidth(this->topLevelWidget()->width()/2, Qt::SmoothTransformation);

            auto thisWindow = this->topLevelWidget();
            //KWindowSystem::lowerWindow(this->topLevelWidget()->winId());
            for (auto win : qApp->allWidgets()) {
                if (auto mainWin = qobject_cast<MainWindow *>(win)) {
                    if (thisWindow != mainWin)
                        KWindowSystem::raiseWindow(win->winId());
                }
            }


            QDrag *d = new QDrag(this);
            QMimeData *data = new QMimeData();
            auto uri = tabData(currentIndex()).toString();
            //data->setText(uri);
            data->setData("peony/tab-index", uri.toUtf8());
            d->setMimeData(data);

            d->setPixmap(pixmap);
            d->setHotSpot(pixmap.rect().center());
            d->exec();

            if (auto tab = qobject_cast<NavigationTabBar *>(d->target())) {
                //do nothing for target tab bar helped us handling yet.
            } else {
                auto window = dynamic_cast<Peony::FMWindowIface *>(this->topLevelWidget());
                auto newWindow = dynamic_cast<QWidget *>(window->create(this->tabData(currentIndex()).toString()));
                newWindow->show();
                KWindowSystem::raiseWindow(newWindow->winId());
                newWindow->move(QCursor::pos() - newWindow->rect().center());
                removeTab(currentIndex());
            }
        });
    });
}

void NavigationTabBar::mouseMoveEvent(QMouseEvent *e)
{
    m_start_drag = false;
    QTabBar::mouseMoveEvent(e);
}

void NavigationTabBar::mouseReleaseEvent(QMouseEvent *e)
{
    QTabBar::mouseReleaseEvent(e);
    m_start_drag = false;
}

TabBarStyle *TabBarStyle::getStyle()
{
    if (!global_instance) {
        global_instance = new TabBarStyle;
    }
    return global_instance;
}

int TabBarStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_TabBarTabShiftVertical:
    case PM_TabBarBaseHeight:
        return 0;
    case PM_TabBarBaseOverlap:
        return 0;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}
