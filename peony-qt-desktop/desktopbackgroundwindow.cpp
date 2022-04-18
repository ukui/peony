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
        // fix #115384, context menu key issue
//        auto index = PeonyDesktopApplication::getIconView()->indexAt(relativePos);
//        if (!index.isValid()) {
//            PeonyDesktopApplication::getIconView()->clearSelection();
//        } else {
//            if (!PeonyDesktopApplication::getIconView()->selectionModel()->selection().indexes().contains(index)) {
//                PeonyDesktopApplication::getIconView()->clearSelection();
//                PeonyDesktopApplication::getIconView()->selectionModel()->select(index, QItemSelectionModel::Select);
//            }
//        }

        QTimer::singleShot(1, [=]() {
            DesktopMenu menu(PeonyDesktopApplication::getIconView());
            if (PeonyDesktopApplication::getIconView()->getSelections().isEmpty()) {
                auto action = menu.addAction(QObject::tr("set background"));
                connect(action, &QAction::triggered, [=]() {
                    //go to control center set background
                    PeonyDesktopApplication::gotoSetBackground();
                });
                auto action1 = menu.addAction(QObject::tr("set resolution"));
                connect(action1, &QAction::triggered, [=]() {
                    //go to control center set resolution ratio
                    PeonyDesktopApplication::gotoSetResolution();
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

    qInfo()<<"bg window geometry changed slot"<<screen()->name()<<geometry;

    // raise primary window to make sure icon view is visible.
    if (centralWidget()) {
        if (screen() == qApp->primaryScreen()) {
            qInfo()<<"has center widget, raise window";
        } else {
            qCritical()<<"raise a window which not in primary screen, but has central widget";
        }
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

    qreal shortEdge = isShortX ? width : height;
    qreal longEdge = isShortX ? height : width;

    while (shortEdge > 1) {
        qint32 temp = qFloor(shortEdge * screenScale);
        if (temp <= longEdge) {
            longEdge = temp;
            break;
        }

        qint32 spacing = qRound(shortEdge / 20);
        if (spacing <= 0) {
            spacing = 1;
        }
        shortEdge -= spacing;
    }

    QSize sourceSize = pixmap.size();
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
