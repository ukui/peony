#include "desktopbackgroundwindow.h"
#include "desktop-background-manager.h"
#include <QScreen>
#include <QPainter>
#include <QVariantAnimation>

static int desktop_window_id = 0;

DesktopBackgroundWindow::DesktopBackgroundWindow(QScreen *screen, QWidget *parent) : QMainWindow(parent)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setAttribute(Qt::WA_TranslucentBackground);

    m_id = desktop_window_id;
    desktop_window_id++;
    setGeometry(screen->geometry());
    setContentsMargins(0, 0, 0, 0);
    connect(screen, &QScreen::geometryChanged, this, QOverload<const QRect&>::of(&DesktopBackgroundWindow::setGeometry));

    auto manager = DesktopBackgroundManager::globalInstance();
    connect(manager, &DesktopBackgroundManager::screensUpdated, this, QOverload<>::of(&DesktopBackgroundWindow::update));
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

int DesktopBackgroundWindow::id() const
{
    return m_id;
}
