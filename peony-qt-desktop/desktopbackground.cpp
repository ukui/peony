#include "desktopbackground.h"

#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QVariantAnimation>
#include <QTimeLine>
#include <QGSettings/QGSettings>

#include <QDebug>

DesktopBackground::DesktopBackground(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);

    m_animation = new QVariantAnimation(this);
    m_animation->setDuration(1000);
    m_animation->setStartValue(qreal(0));
    m_animation->setEndValue(qreal(1));

    m_timeLine = new QTimeLine(200, this);
    connect(m_timeLine, &QTimeLine::finished, this, &DesktopBackground::updateScreens);

    connect(m_animation, &QVariantAnimation::valueChanged, this, QOverload<>::of(&QWidget::update));
    connect(m_animation, &QVariantAnimation::finished, this, [=](){
        m_backPixmap = m_frontPixmap;
        if (!m_pendingPixmap.isNull()) {
            m_frontPixmap = m_pendingPixmap;
            m_pendingPixmap = QPixmap();
            m_animation->start();
        }
        update();
    });

    connectScreensChangement();

    updateScreens();
}

void DesktopBackground::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    if (!m_paintBackground)
        return;

    QPainter p(this);
    if (m_usePureColor) {
        p.fillRect(rect(), m_color);
        return;
    }

    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    if (m_animation->state() == QVariantAnimation::Running) {
        qreal opacity = m_animation->currentValue().toReal();
        for (auto screen : qApp->screens()) {
            p.drawPixmap(screen->geometry(), m_backPixmap, m_backPixmap.rect());
            p.save();
            p.setOpacity(opacity);
            p.drawPixmap(screen->geometry(), m_frontPixmap, m_frontPixmap.rect());
            p.restore();
        }
    } else {
        for (auto screen : qApp->screens()) {
            p.drawPixmap(screen->geometry(), m_backPixmap, m_backPixmap.rect());
        }
    }
}

void DesktopBackground::updateScreens()
{
    QRegion screensRegion;
    for (auto screen : qApp->screens()) {
        screensRegion += screen->geometry();
    }
    move(0, 0);
    screensRegion += rect();
    auto size = screensRegion.boundingRect().size();
    //auto maxLength = qMax(size.width(), size.height());
    //setFixedSize(maxLength, maxLength);
    setFixedSize(size);

    update();
}

void DesktopBackground::initBackground()
{
    m_paintBackground = true;
    if (QGSettings::isSchemaInstalled("org.mate.background")) {
        m_backgroundSettings = new QGSettings("org.mate.background", QByteArray(), this);

        switchBackground();

        connect(m_backgroundSettings, &QGSettings::changed, this, [=](const QString &key){
            if (key == "pictureFilename") {
                switchBackground();
            }

            if (key == "primaryColor") {
                auto colorName = m_backgroundSettings->get("primaryColor").toString();
                m_color = QColor(colorName);
                if (m_usePureColor) {
                    update();
                }
            }
        });
    } else {
        m_frontPixmap = QPixmap("/usr/share/background/calla.png");
    }
}

void DesktopBackground::switchBackground()
{
    auto path = m_backgroundSettings->get("pictureFilename").toString();
    if (path.isEmpty()) {
        m_usePureColor = true;
        auto colorName = m_backgroundSettings->get("primaryColor").toString();
        m_color = QColor(colorName);
    } else {
        m_usePureColor = false;
        auto colorName = m_backgroundSettings->get("primaryColor").toString();
        m_color = QColor(colorName);
    }
    if (m_animation->state() == QVariantAnimation::Running) {
        m_pendingPixmap = QPixmap(path);
    } else {
        m_frontPixmap = QPixmap(path);
        m_animation->start();
    }
}

void DesktopBackground::connectScreensChangement()
{
    for (auto screen : QApplication::screens()) {
        QObject::connect(screen, &QScreen::geometryChanged, this, [=](){
            qDebug()<<screen->manufacturer()<<"changed";
            m_timeLine->setCurrentTime(0);
            if (m_timeLine->state() == QTimeLine::NotRunning) {
                m_timeLine->start();
            }
        });

        QObject::connect(screen, &QScreen::destroyed, this, [=]{
            m_timeLine->setCurrentTime(0);
            if (m_timeLine->state() == QTimeLine::NotRunning) {
                m_timeLine->start();
            }
        });
    }

    QObject::connect(qApp, &QApplication::screenAdded, this, [=](QScreen *screen){
        qDebug()<<screen->manufacturer()<<"changed";
        m_timeLine->setCurrentTime(0);
        if (m_timeLine->state() == QTimeLine::NotRunning) {
            m_timeLine->start();
        }

        QObject::connect(screen, &QScreen::geometryChanged, this, [=](){
            qDebug()<<screen->manufacturer()<<"changed";
            m_timeLine->setCurrentTime(0);
            if (m_timeLine->state() == QTimeLine::NotRunning) {
                m_timeLine->start();
            }
        });

        QObject::connect(screen, &QScreen::destroyed, this, [=]{
            m_timeLine->setCurrentTime(0);
            if (m_timeLine->state() == QTimeLine::NotRunning) {
                m_timeLine->start();
            }
        });
    });
}
