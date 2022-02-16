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
 *
 */

#include "desktopbackground.h"
#include "peony-desktop-application.h"

#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QVariantAnimation>
#include <QTimeLine>
#include <QGSettings/QGSettings>
#include <QDBusInterface>
#include <QDBusReply>
#include <QFile>
#include <global-settings.h>

#include <QDebug>

#define BACKGROUND_SETTINGS "org.mate.background"

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

    initGSettings();

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
    auto screensSize = screensRegion.boundingRect().size();
    QSize size = this->size();
    int maxWidth = qMax(size.width(), screensSize.width());
    int maxHeight = qMax(size.height(), screensSize.height());
    int maxLength = qMax(maxWidth, maxHeight);
    setFixedSize(maxLength, maxLength);

    update();

    auto app = static_cast<PeonyDesktopApplication *>(qApp);
    Q_EMIT app->requestSetUKUIOutputEnable(true);
}

void DesktopBackground::initGSettings()
{
    if (QGSettings::isSchemaInstalled("org.ukui.style"))
    {
        //font monitor
        QGSettings *fontSetting = new QGSettings(FONT_SETTINGS, QByteArray(), this);
        connect(fontSetting, &QGSettings::changed, this, [=](const QString &key){
            qDebug() << "fontSetting changed:" << key;
            if (key == "systemFont" || key == "systemFontSize")
            {
                QFont font = this->font();
                for(auto widget : qApp->allWidgets())
                    widget->setFont(font);
            }
        });
    }
}

void DesktopBackground::initBackground()
{
    if (QGSettings::isSchemaInstalled(BACKGROUND_SETTINGS)) {
        m_backgroundSettings = new QGSettings(BACKGROUND_SETTINGS, QByteArray(), this);
    }
    m_paintBackground = true;
    setBackground();
    if (m_backgroundSettings) {
        connect(m_backgroundSettings, &QGSettings::changed, this, [=](const QString &key){
            if (key == "pictureFilename" || key == "primaryColor") {
                switchBackground();
            }
        });
    }
}

void DesktopBackground::setBackground()
{
    QString defaultBg;
    auto accountBack = getAccountBackground();
    if (accountBack != "" && QFile::exists(accountBack))
        defaultBg = accountBack;

    //if default bg and account bg not exist, use color bg
    if (! QFile::exists(defaultBg))
    {
       qWarning() << "default bg and account bg not exist";
       switchBackground();
       return;
    }

    m_frontPixmap = QPixmap(defaultBg);
    m_current_bg_path = defaultBg;
    if (defaultBg != accountBack)
        setAccountBackground();

    m_animation->finished();
}

void DesktopBackground::switchBackground()
{
    if (!m_backgroundSettings)
        return;

    auto path = m_backgroundSettings->get("pictureFilename").toString();
    if (! QFile::exists(path))
        path = getAccountBackground();
    if (path.isEmpty()) {
        m_usePureColor = true;
        auto colorName = m_backgroundSettings->get("primaryColor").toString();
        m_color = QColor(colorName);
        m_animation->stop();
        m_backPixmap = QPixmap();
        m_frontPixmap = QPixmap();
        m_current_bg_path = "";
        update();
    } else {
        m_usePureColor = false;
        auto colorName = m_backgroundSettings->get("primaryColor").toString();
        m_color = QColor(colorName);
        if (m_animation->state() == QVariantAnimation::Running) {
            m_pendingPixmap = QPixmap(path);
            m_current_bg_path = path;
        } else {
            m_frontPixmap = QPixmap(path);
            if (m_backPixmap.isNull()) {
                m_backPixmap = m_frontPixmap;
            }
            m_animation->start();
            m_current_bg_path = path;
        }
    }

    //if background picture changed, update it
    if (m_current_bg_path != getAccountBackground())
        setAccountBackground();
}

QString DesktopBackground::getAccountBackground()
{
    uid_t uid = getuid();
    QDBusInterface iface("org.freedesktop.Accounts", "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",QDBusConnection::systemBus());

    QDBusReply<QDBusObjectPath> userPath = iface.call("FindUserById", (qint64)uid);
    if(!userPath.isValid())
        qWarning() << "Get UserPath error:" << userPath.error();
    else {
        QDBusInterface userIface("org.freedesktop.Accounts", userPath.value().path(),
                                 "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
        QDBusReply<QDBusVariant> backgroundReply = userIface.call("Get", "org.freedesktop.Accounts.User", "BackgroundFile");
        if(backgroundReply.isValid())
            return  backgroundReply.value().variant().toString();
    }
    return "";
}

void DesktopBackground::setAccountBackground()
{
    QDBusInterface * interface = new QDBusInterface("org.freedesktop.Accounts",
                                     "/org/freedesktop/Accounts",
                                     "org.freedesktop.Accounts",
                                     QDBusConnection::systemBus());

    if (!interface->isValid()){
        qCritical() << "Create /org/freedesktop/Accounts Client Interface Failed " << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusReply<QDBusObjectPath> reply =  interface->call("FindUserByName", g_get_user_name());
    QString userPath;
    if (reply.isValid()){
        userPath = reply.value().path();
    }
    else {
        qCritical() << "Call 'GetComputerInfo' Failed!" << reply.error().message();
        return;
    }

    QDBusInterface * useriFace = new QDBusInterface("org.freedesktop.Accounts",
                                                    userPath,
                                                    "org.freedesktop.Accounts.User",
                                                    QDBusConnection::systemBus());

    if (!useriFace->isValid()){
        qCritical() << QString("Create %1 Client Interface Failed").arg(userPath) << QDBusConnection::systemBus().lastError();
        return;
    }

    QDBusMessage msg = useriFace->call("SetBackgroundFile", m_current_bg_path);
    qDebug() << "setAccountBackground path:" <<m_current_bg_path;
    if (!msg.errorMessage().isEmpty())
        qDebug() << "update user background file error: " << msg.errorMessage();
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
