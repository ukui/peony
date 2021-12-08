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
                p.drawPixmap(this->rect().topLeft(), backPixmap.scaled(this->rect().size(), Qt::KeepAspectRatioByExpanding));
                p.setOpacity(opacity);
                p.drawPixmap(this->rect().topLeft(), frontPixmap.scaled(this->rect().size(), Qt::KeepAspectRatioByExpanding));
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
                p.drawPixmap(this->rect().topLeft(), frontPixmap.scaled(this->rect().size(), Qt::KeepAspectRatioByExpanding));

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
    m_currentDesktop->initDesktop(geometry());
    m_currentDesktop->setActivated(true);

    connect(m_currentDesktop, &DesktopWidgetBase::desktopMoveRequest, this, &DesktopBackgroundWindow::desktopMoveProcess);
    connect(m_currentDesktop, &DesktopWidgetBase::desktopReboundRequest, this, &DesktopBackgroundWindow::desktopReboundProcess);
}


void DesktopBackgroundWindow::desktopMoveProcess(AnimationType animationType, quint32 moveLength, quint32 duration)
{
    if (moveLength <= 0 | m_currentDesktop->isPause()) {
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

        return desktop;
    }

    return nullptr;
}
