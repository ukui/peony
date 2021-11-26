#include "desktopbackgroundwindow.h"
#include "desktop-background-manager.h"
#include "peony-desktop-application.h"
#include "desktop-icon-view.h"
#include "desktop-menu.h"
#include <QScreen>
#include <QPainter>
#include <QVariantAnimation>
#include <QTimeLine>
#include <KWindowSystem>

static int desktop_window_id = 0;
static QTimeLine *gTimeLine = nullptr;

DesktopBackgroundWindow::DesktopBackgroundWindow(QScreen *screen, QWidget *parent) : QMainWindow(parent)
{
    if (!gTimeLine) {
        gTimeLine = new QTimeLine(100);
    }
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

    if (QGSettings::isSchemaInstalled("org.ukui.panel.settings")) {
        m_panelSetting = new QGSettings("org.ukui.panel.settings", QByteArray(), this);
    }

    connect(this, &QWidget::customContextMenuRequested, this, [=](const QPoint &pos){
        QPoint relativePos = getRelativePos(pos);
        qInfo()<<pos;
        auto index = PeonyDesktopApplication::getIconView()->indexAt(relativePos);
        if (!index.isValid()) {
            PeonyDesktopApplication::getIconView()->clearSelection();
        } else {
            if (!PeonyDesktopApplication::getIconView()->selectionModel()->selection().indexes().contains(index)) {
                PeonyDesktopApplication::getIconView()->clearSelection();
                PeonyDesktopApplication::getIconView()->selectionModel()->select(index, QItemSelectionModel::Select);
            }
        }

        QTimer::singleShot(1, [=]() {
            DesktopMenu menu(PeonyDesktopApplication::getIconView());
            if (PeonyDesktopApplication::getIconView()->getSelections().isEmpty()) {
                auto action = menu.addAction(QObject::tr("set background"));
                connect(action, &QAction::triggered, [=]() {
                    //go to control center set background
                    PeonyDesktopApplication::gotoSetBackground();
                });
            }

            for (auto screen : qApp->screens()) {
                if (screen->geometry().contains(relativePos));
                //menu.windowHandle()->setScreen(screen);
            }
            menu.exec(QCursor::pos());
            auto urisToEdit = menu.urisToEdit();
            if (urisToEdit.count() == 1) {
                QTimer::singleShot(
                            100, this, [=]() {
                    PeonyDesktopApplication::getIconView()->editUri(urisToEdit.first());
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
        if (animation->state() == QVariantAnimation::Running) {
            p.drawPixmap(rect().adjusted(0, 0, -1, -1), manager->getBackPixmap(), manager->getBackPixmap().rect());
            auto opacity = animation->currentValue().toReal();
            p.setOpacity(opacity);
            p.drawPixmap(rect().adjusted(0, 0, -1, -1), manager->getFrontPixmap(), manager->getFrontPixmap().rect());
        } else {
            p.drawPixmap(rect().adjusted(0, 0, -1, -1), manager->getBackPixmap(), manager->getBackPixmap().rect());
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

//获取iconview中图标的相对位置
QPoint DesktopBackgroundWindow::getRelativePos(const QPoint &pos)
{
    QPoint relativePos = pos;
    if (m_screen == QApplication::primaryScreen()) {
        if (m_panelSetting) {
            int position = m_panelSetting->get("panelposition").toInt();
            int offset = m_panelSetting->get("panelsize").toInt();

            switch (position) {
                case 1: {
                    relativePos -= QPoint(0, offset);
                    break;
                }
                case 2: {
                    relativePos -= QPoint(offset, 0);
                    break;
                }
                case 3: {
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

    return relativePos;
}
