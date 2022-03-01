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
#include <QStyleOptionTab>
#include <QStyle>
#include <QPainter>
#include <QPainterPath>

#include "FMWindowIface.h"
#include "main-window.h"
#include "file-info.h"
#include "file-info-job.h"

static TabBarStyle *global_instance = nullptr;

NavigationTabBar::NavigationTabBar(QWidget *parent) : QTabBar(parent)
{
    setAcceptDrops(true);
    m_drag_timer.setInterval(750);
    m_drag_timer.setSingleShot(true);

    setStyle(TabBarStyle::getStyle());


    setContentsMargins(0, 0, 0, 0);
    setFixedHeight(48);

    setProperty("useStyleWindowManager", false);
    setMovable(true);
    setExpanding(false);
    setTabsClosable(true);
    X11WindowManager::getInstance()->registerWidget(this);

    connect(this, &QTabBar::currentChanged, this, [=](int index) {
        //qDebug()<<"current changed"<<index;
    });

    connect(this, &QTabBar::tabMoved, this, [=](int from, int to) {
        //qDebug()<<"move"<<from<<"to"<<to;
    });

    connect(this, &QTabBar::tabBarClicked, this, [=](int index) {
        //qDebug()<<"tab bar clicked"<<index;
    });

    connect(this, &QTabBar::tabBarDoubleClicked, this, [=](int index) {
        //qDebug()<<"tab bar double clicked"<<index;
    });

    setDrawBase(false);
}

void NavigationTabBar::addPages(const QStringList &uri)
{

}

void NavigationTabBar::updateLocation(int index, const QString &uri)
{
    //bug#94981 修改拖拽tab页时出现断错误，改成异步查询更新tab数据
    auto info = Peony::FileInfo::fromUri(uri);
    auto infoJob = new Peony::FileInfoJob(info);
    infoJob->setAutoDelete();
    setTabData(index, uri);

    connect(infoJob, &Peony::FileInfoJob::queryAsyncFinished, this, [=](){
        if (uri != tabData(index).toString())
            return;
        auto iconName = Peony::FileUtils::getFileIconName(uri);
        auto displayName = Peony::FileUtils::getFileDisplayName(uri);
        if (uri.startsWith("search:///"))
        {
            QString nameRegexp = Peony::SearchVFSUriParser::getSearchUriNameRegexp(uri);
            QString targetDirectory = Peony::SearchVFSUriParser::getSearchUriTargetDirectory(uri);
            displayName = tr("Search \"%1\" in \"%2\"").arg(nameRegexp).arg(targetDirectory);
        }

        //elide text if it is too long
        if (displayName.length() > ELIDE_TEXT_LENGTH)
        {
            int  charWidth = fontMetrics().averageCharWidth();
            displayName = fontMetrics().elidedText(displayName, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
        }

        setTabText(index, displayName);
        setTabData(index, uri);

        Q_EMIT this->locationUpdated(uri);
    });

    infoJob->queryAsync();
}

void NavigationTabBar::addPage(const QString &uri, bool jumpToNewTab)
{
    if (!uri.isNull()) {
        //FIXME: replace BLOCKING api in ui thread.
//        auto iconName = Peony::FileUtils::getFileIconName(uri);
        auto displayName = Peony::FileUtils::getFileDisplayName(uri);
        //去除tabpage图标
//        addTab(QIcon::fromTheme(iconName), displayName);
        addTab(displayName);
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

    Q_EMIT pageRemoved();
    if (count() == 0) {
        Q_EMIT closeWindowRequest();
    }
}

void NavigationTabBar::tabInserted(int index)
{
    //qDebug()<<"tab inserted"<<index;
    QTabBar::tabInserted(index);
}

void NavigationTabBar::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept();
    return;
}

void NavigationTabBar::dragMoveEvent(QDragMoveEvent *e)
{
    //bug#94981 点击tab页签可以抓取界面，获取创建新的窗口
    if (e->source() == this) {
        m_should_trigger_drop = false;
        m_drag->cancel();
        grabMouse();
    }
    e->accept();
    return;
}

void NavigationTabBar::dragLeaveEvent(QDragLeaveEvent *e)
{
    QTabBar::dragLeaveEvent(e);
}

void NavigationTabBar::dropEvent(QDropEvent *e)
{
    //点击tab页签，抓取页面得到新的窗口
    m_start_drag = false;
    if (e->source() != this) {
        if (e->mimeData()->hasUrls()) {
            for (auto url : e->mimeData()->urls()) {
                //FIXME: replace BLOCKING api in ui thread.
                if (Peony::FileUtils::isFileDirectory(url.url())) {
                    addPageRequest(url.url(), true);
                }
            }
        } else if (e->mimeData()->hasFormat("peony/tab-index")) {
            auto uri = e->mimeData()->data("peony/tab-index");
            //FIXME: replace BLOCKING api in ui thread.
            if (Peony::FileUtils::isFileDirectory(uri)) {
                addPageRequest(uri, true);
            }
        }

        //finish the drag, remove old tab page from old tab.
        if (auto oldTab = qobject_cast<NavigationTabBar *>(e->source())) {
            oldTab->removeTab(oldTab->currentIndex());
        }
    }
    releaseMouse();
}

void NavigationTabBar::mousePressEvent(QMouseEvent *e)
{
    QTabBar::mousePressEvent(e);
    m_press_pos = e->pos();
    if (tabAt(e->pos()) >= 0)
        m_start_drag = true;
    else
        m_start_drag = false;
}

void NavigationTabBar::mouseMoveEvent(QMouseEvent *e)
{
    QTabBar::mouseMoveEvent(e);
    //bug#94981 点击tab页签可以抓取界面，获取创建新的窗口
    if (e->source() != Qt::MouseEventNotSynthesized) {
        return;
    }

    auto offset = e->pos() - m_press_pos;
    auto offsetX = qAbs(offset.x());
    auto offsetY = qAbs(offset.y());

    if (e->pos().y() >= 0 && e->pos().y() <= this->height()) {
        return;
    }

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
    m_drag = d;
    QMimeData *data = new QMimeData();
    auto uri = tabData(currentIndex()).toString();
    //data->setText(uri);
    data->setData("peony/tab-index", uri.toUtf8());
    d->setMimeData(data);

    d->setPixmap(pixmap);
    d->setHotSpot(pixmap.rect().center());
    m_should_trigger_drop = true;
    d->exec();
    qApp->restoreOverrideCursor();
    m_drag = nullptr;

    if (m_should_trigger_drop) {
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
        delete d;
        releaseMouse();
        m_start_drag = false;
    } else {
        d->deleteLater();
    }
}

void NavigationTabBar::mouseReleaseEvent(QMouseEvent *e)
{
    QTabBar::mouseReleaseEvent(e);
    m_start_drag = false;
    releaseMouse();
}

void NavigationTabBar::resizeEvent(QResizeEvent *e)
{
    QTabBar::resizeEvent(e);
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
    case PM_TabBarScrollButtonWidth:
        return 48;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}
QRect TabBarStyle::subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    switch (element) {
    case SE_TabBarScrollLeftButton:{
        QRect tabRect = option->rect;
        tabRect.setRight(tabRect.left() + 48);
        return tabRect;
    }
    case SE_TabBarScrollRightButton:{
        QRect tabRect = option->rect;
        tabRect.setLeft(tabRect.right() - 48);
        return tabRect;
    }
    default:
        return QProxyStyle::subElementRect(element, option, widget);
    }
}
void TabBarStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case PE_PanelButtonTool:{
        QPainterPath path;
        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        path.addRoundedRect(widget->rect(), 16, 16);
        painter->setClipPath(path);
        return  QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
    default:
        return QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}

void TabBarStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if (widget &&  widget->objectName() == "toolButton") {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addRoundedRect(widget->rect(), 16, 16);
        painter->setClipPath(path);
        QProxyStyle::drawComplexControl(control, option, painter, widget);
        painter->restore();
     } else {
        QProxyStyle::drawComplexControl(control, option, painter, widget);
    }
}
