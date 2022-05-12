/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

#include "desktop-background-window.h"
#include "desktop-background-manager.h"
#include "peony-desktop-application.h"
#include "desktop-icon-view.h"
#include "desktop-menu.h"
#include "desktop-manager.h"

#include <QScreen>
#include <QPainter>
#include <QVariantAnimation>
#include <QTimeLine>
#include <KWindowSystem>
#include <QGraphicsBlurEffect>

static int desktop_window_id = 0;
static QTimeLine *gTimeLine = nullptr;

using namespace Peony;

DesktopBackgroundWindow::DesktopBackgroundWindow(QScreen *screen, QWidget *parent) : QMainWindow(parent)
{
    if (!gTimeLine) {
        gTimeLine = new QTimeLine(100);
    }
    this->setProperty("useStyleWindowManager", false);
    connect(gTimeLine, &QTimeLine::finished, this, &DesktopBackgroundWindow::updateWindowGeometry);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setAttribute(Qt::WA_TranslucentBackground);

    setContextMenuPolicy(Qt::CustomContextMenu);

    m_screen = screen;
    m_id = desktop_window_id;
    desktop_window_id++;
    move(screen->geometry().topLeft());
    setFixedSize(screen->geometry().size());
    setContentsMargins(0, 0, 0, 0);
    connect(screen, &QScreen::geometryChanged, this, QOverload<const QRect&>::of(&DesktopBackgroundWindow::setWindowGeometry));

    auto manager = DesktopBackgroundManager::globalInstance();
    connect(manager, &DesktopBackgroundManager::screensUpdated, this, QOverload<>::of(&DesktopBackgroundWindow::update));

    connect(this, &QWidget::customContextMenuRequested, this, [=](const QPoint &pos) {
        //TODO 需要处理 #80416 20210916
        if (!m_currentDesktop) {
            return;
        }
        if (m_currentDesktop->getDesktopType() != DesktopType::Desktop) {
            return;
        }

        DesktopIconView *view = qobject_cast<DesktopIconView *>(m_currentDesktop->getRealDesktop());
        qInfo() << pos;
        auto index = view->indexAt(pos);
        if (!index.isValid()) {
            view->clearSelection();
        } else {
            if (!view->selectionModel()->selection().indexes().contains(index)) {
                view->clearSelection();
                view->selectionModel()->select(index, QItemSelectionModel::Select);
            }
        }

        QTimer::singleShot(1, this, [=]() {
            DesktopMenu menu(view);
            if (view->getSelections().isEmpty()) {
                auto action = menu.addAction(QObject::tr("set background"));
                connect(action, &QAction::triggered, [=]() {
                    //go to control center set background
                    PeonyDesktopApplication::gotoSetBackground();
                });
            }

            for (auto screen : qApp->screens()) {
                if (screen->geometry().contains(pos));
                //menu.windowHandle()->setScreen(screen);
            }
            menu.exec(QCursor::pos());
            auto urisToEdit = menu.urisToEdit();
            if (urisToEdit.count() == 1) {
                QTimer::singleShot(100, this, [=]() {
                    view->editUri(urisToEdit.first());
                });
            }
        });
    });
}

void DesktopBackgroundWindow::paintEvent(QPaintEvent *event)
{
    auto manager = DesktopBackgroundManager::globalInstance();
    if (!manager->getPaintBackground())
        return;

    QPainter p(this);
    if (manager->getUsePureColor()) {
        p.fillRect(this->rect(), manager->getColor());
    } else {
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        auto animation = manager->getAnimation();
        QPixmap frontPixmap = manager->getFrontPixmap();

        if (animation->state() == QVariantAnimation::Running) {
            auto opacity = animation->currentValue().toReal();
            QPixmap backPixmap = manager->getBackPixmap();

            if (manager->getBackgroundOption() == "centered") {
                //居中
                p.drawPixmap((m_screen->size().width() - backPixmap.rect().width()) / 2,
                             (m_screen->size().height() - backPixmap.rect().height()) / 2,
                             backPixmap);
                p.setOpacity(opacity);
                p.drawPixmap((m_screen->size().width() - frontPixmap.rect().width()) / 2,
                             (m_screen->size().height() - frontPixmap.rect().height()) / 2,
                             frontPixmap);
            } else if (manager->getBackgroundOption() == "stretched") {
                //拉伸
                p.drawPixmap(this->rect(), backPixmap, backPixmap.rect());
                p.setOpacity(opacity);
                p.drawPixmap(this->rect(), frontPixmap, frontPixmap.rect());
            } else if (manager->getBackgroundOption() == "scaled") {
                //填充
                p.drawPixmap(this->rect(), backPixmap, getSourceRect(backPixmap));
                p.setOpacity(opacity);
                p.drawPixmap(this->rect(), frontPixmap, getSourceRect(frontPixmap));
            } else if (manager->getBackgroundOption() == "wallpaper") {
                //平铺
                int drawedWidth = 0;
                int drawedHeight = 0;
                while (1) {
                    drawedWidth = 0;
                    while (1) {
                        p.drawPixmap(drawedWidth, drawedHeight, backPixmap);
                        drawedWidth += backPixmap.width();
                        if (drawedWidth >= m_screen->size().width()) {
                            break;
                        }
                    }
                    drawedHeight += backPixmap.height();
                    if (drawedHeight >= m_screen->size().height()) {
                        break;
                    }
                }
                p.setOpacity(opacity);
                drawedWidth = 0;
                drawedHeight = 0;
                while (1) {
                    drawedWidth = 0;
                    while (1) {
                        p.drawPixmap(drawedWidth, drawedHeight, frontPixmap);
                        drawedWidth += frontPixmap.width();
                        if (drawedWidth >= m_screen->size().width()) {
                            break;
                        }
                    }
                    drawedHeight += frontPixmap.height();
                    if (drawedHeight >= m_screen->size().height()) {
                        break;
                    }
                }
            } else {
                p.drawPixmap(rect().adjusted(0, 0, -1, -1), backPixmap, backPixmap.rect());
                p.setOpacity(opacity);
                p.drawPixmap(rect().adjusted(0, 0, -1, -1), frontPixmap, frontPixmap.rect());
            }

        } else {
            if (manager->getBackgroundOption() == "centered") {
                p.drawPixmap((m_screen->size().width() - frontPixmap.rect().width()) / 2,
                             (m_screen->size().height() - frontPixmap.rect().height()) / 2,
                             frontPixmap);
            } else if (manager->getBackgroundOption() == "stretched") {
                p.drawPixmap(this->rect(), frontPixmap, frontPixmap.rect());

            } else if (manager->getBackgroundOption() == "scaled") {
                p.drawPixmap(this->rect(), frontPixmap, getSourceRect(frontPixmap));

            } else if (manager->getBackgroundOption() == "wallpaper") {
                int drawedWidth = 0;
                int drawedHeight = 0;
                while (1) {
                    drawedWidth = 0;
                    while (1) {
                        p.drawPixmap(drawedWidth, drawedHeight, frontPixmap);
                        drawedWidth += frontPixmap.width();
                        if (drawedWidth >= m_screen->size().width()) {
                            break;
                        }
                    }
                    drawedHeight += frontPixmap.height();
                    if (drawedHeight >= m_screen->size().height()) {
                        break;
                    }
                }
            } else {
                p.drawPixmap(rect().adjusted(0, 0, -1, -1), frontPixmap, frontPixmap.rect());
            }
        }
        p.restore();
    }
}

QScreen *DesktopBackgroundWindow::screen() const
{
    return m_screen;
}

void DesktopBackgroundWindow::setWindowGeometry(const QRect &geometry)
{
    qInfo()<<"bg window geometry changed"<<screen()->name()<<geometry<<screen()->geometry();
    if (gTimeLine->state() != QTimeLine::Running) {
        gTimeLine->start();
    } else {
        gTimeLine->setCurrentTime(0);
    }
}

void DesktopBackgroundWindow::updateWindowGeometry()
{
    auto geometry = m_screen->geometry();
    move(geometry.topLeft());
    setFixedSize(geometry.size());
    //bug#111897 修改QmlDesktop在旋转之后没有调整界面大小
    if (m_currentDesktop) {
        m_currentDesktop->initDesktop(QRect(0, 0, geometry.width(), geometry.height()));
    }
    // raise primary window to make sure icon view is visible.
    if (centralWidget()) {
        KWindowSystem::raiseWindow(this->winId());
    }
}

int DesktopBackgroundWindow::id() const
{
    return m_id;
}

void DesktopBackgroundWindow::setWindowDesktop(DesktopWidgetBase *desktop)
{
    if (!desktop) {
        qWarning() << "[DesktopBackgroundWindow::setWindowDesktop] Try to set up a desktop that does not exist !";
        return;
    }
    //TODO 尝试实现不同窗口设置不同背景 20210810
    this->takeCurrentDesktop();

    m_currentDesktop = desktop;
    m_currentDesktop->setParent(this);
    //不适用主题框架的窗口拖动，避免被主题框架屏蔽移动事件
    m_currentDesktop->setProperty("useStyleWindowManager", false);
    this->setProperty("useStyleWindowManager", false);

    this->setCentralWidget(m_currentDesktop);

    m_currentDesktop->setPause(false);
    m_currentDesktop->initDesktop({0, 0, geometry().width(), geometry().height()});
    m_currentDesktop->setActivated(true);
    if (m_currentDesktop->hasCustomAnimation()) {
        m_currentDesktop->startAnimation(true);
    }

    connect(m_currentDesktop, &DesktopWidgetBase::desktopMoveRequest, this, &DesktopBackgroundWindow::desktopMoveProcess);
    connect(m_currentDesktop, &DesktopWidgetBase::desktopReboundRequest, this, &DesktopBackgroundWindow::desktopReboundProcess);
}


void DesktopBackgroundWindow::desktopMoveProcess(AnimationType animationType, quint32 moveLength, quint32 duration)
{
    if (moveLength <= 0 || m_currentDesktop->isPause()) {
        return;
    }

    const QRect &currentRect = m_currentDesktop->geometry();
    QRect nextRect;

    switch (animationType) {
        case AnimationType::LeftToRight:
            nextRect = QRect(currentRect.x() + moveLength, 0, currentRect.width(), currentRect.height());
            break;
        case AnimationType::RightToLeft:
            nextRect = QRect(currentRect.x() - moveLength, 0, currentRect.width(), currentRect.height());
            break;
        case AnimationType::CenterToEdge: {
            nextRect = QRect(currentRect.x() - moveLength,
                             currentRect.y() - moveLength,
                             currentRect.width() + (moveLength * 2),
                             currentRect.height() + (moveLength * 2));
            break;
        }
        case AnimationType::EdgeToCenter: {
            nextRect = QRect(currentRect.x() + moveLength,
                             currentRect.y() + moveLength,
                             currentRect.width() - (moveLength * 2),
                             currentRect.height() - (moveLength * 2));
            break;
        }
        case AnimationType::OpacityFull:
        case AnimationType::OpacityLess:
        default:
            nextRect = currentRect;
            break;
    }

    if (duration >= 50) {
        QPropertyAnimation *animation = new QPropertyAnimation(m_currentDesktop, "geometry");
        animation->setStartValue(currentRect);
        animation->setEndValue(nextRect);
        animation->setDuration(duration);
        animation->setEasingCurve(QEasingCurve::InOutQuad);

        connect(animation, &QPropertyAnimation::finished, this, [=] {
            animation->deleteLater();
        });
    } else {
        m_currentDesktop->setGeometry(nextRect);
    }
}

void DesktopBackgroundWindow::desktopReboundProcess()
{
    QPropertyAnimation *animation = new QPropertyAnimation(m_currentDesktop, "geometry");

    animation->setStartValue(m_currentDesktop->geometry());
    animation->setEndValue(geometry());
    animation->setDuration(300);
    animation->setEasingCurve(QEasingCurve::InOutCubic);

    animation->start();
}

DesktopWidgetBase *DesktopBackgroundWindow::takeCurrentDesktop()
{
    if (m_currentDesktop) {
        //取消掉上一个桌面的信号链接
        disconnect(m_currentDesktop, &DesktopWidgetBase::desktopMoveRequest, this, &DesktopBackgroundWindow::desktopMoveProcess);
        disconnect(m_currentDesktop, &DesktopWidgetBase::desktopReboundRequest, this, &DesktopBackgroundWindow::desktopReboundProcess);
        m_currentDesktop->setActivated(false);

        DesktopWidgetBase *desktop = m_currentDesktop;

        m_currentDesktop = nullptr;
        this->takeCentralWidget();
        if (desktop->getDesktopType() == DesktopType::Animation) {
            desktop->deleteLater();
            return nullptr;
        }
        return desktop;
    }

    return nullptr;
}

/**
 * @brief 通过屏幕比例，从图片中间获取一块与屏幕比例相同的区域
 * @param pixmap 源图片
 * @return
 *
 * 0.如果图片的宽高比例与屏幕比例已经相同，那么直接使用原图片的rect即可。
 *
 * 如果比例不相同：
 * 1.首先判断图片的宽度与高度之间的关系，取出长度较短的一条边：shortEdge。
 *   这个短边很重要，在进行对比时，短边需要作为基准
 * 2.然后根据短边是图片的宽还是高，计算屏幕的宽高比例
 *   如果短边是图片的宽,那么screenScale=(height/width),否则 screenScale=(width/height)
 *   确保短边乘以屏幕比例始终可以得到 "当前短边" 需要多长的长边 才能满足与屏幕比例相同这个条件。
 *   并且这个长边在图片的长度范围内。
 *   这一步与具体需要的rect无关，仅计算出需要的长边和短边。
 *
 * 举例：
 *   原理: b=c/a,a*b=c;
 *   屏幕比例：screenScale
 *   屏幕宽度：sWidth, 屏幕高度：sHeight。
 *   图片宽度：pWidth, 图片高度：pHeight
 *   需要的短边：shortEdge, 需要的长边：longEdge
 *
 *   1.屏幕横屏，图片横屏
 *     1. pWidth > pHeight;
 *     2. shortEdge = height;
 *     2. screenScale = sWidth / sHeight;
 *     3. longEdge = shortEdge * screenScale;
 *
 *   2.屏幕横屏，图片竖屏 3.屏幕竖屏，图片横屏 4.屏幕竖屏，图片竖屏
 *     ...可自行推导
 */
QRect DesktopBackgroundWindow::getSourceRect(const QPixmap &pixmap)
{
    qreal screenScale = qreal(this->rect().width()) / qreal(this->rect().height());
    qreal width = pixmap.width();
    qreal height = pixmap.height();

    if ((width / height) == screenScale) {
        return pixmap.rect();
    }

    bool isShortX = (width <= height);
    if (isShortX) {
        screenScale = qreal(this->rect().height()) / qreal(this->rect().width());
    }

    //使用长边与短边目的是屏蔽单独的宽与高概念，减少一部分判断逻辑
    qreal shortEdge = isShortX ? width : height;
    qreal longEdge = isShortX ? height : width;

    while (shortEdge > 1) {
        qint32 temp = qFloor(shortEdge * screenScale);
        if (temp <= longEdge) {
            longEdge = temp;
            break;
        }

        //每次递减5%进行逼近
        qint32 spacing = qRound(shortEdge / 20);
        if (spacing <= 0) {
            spacing = 1;
        }
        shortEdge -= spacing;
    }

    QSize sourceSize = pixmap.size();
    //获取需要的rect
    if (shortEdge > 1 && longEdge > 1) {
        sourceSize.setWidth(isShortX ? shortEdge : longEdge);
        sourceSize.setHeight(isShortX ? longEdge : shortEdge);
    }

    qint32 offsetX = 0;
    qint32 offsetY = 0;
    if (pixmap.width() > sourceSize.width()) {
        offsetX = (pixmap.width() - sourceSize.width()) / 2;
    }

    if (pixmap.height() > sourceSize.height()) {
        offsetY = (pixmap.height() - sourceSize.height()) / 2;
    }

    QPoint offsetPoint = pixmap.rect().topLeft();
    offsetPoint += QPoint(offsetX, offsetY);

    return QRect(offsetPoint, sourceSize);
}
