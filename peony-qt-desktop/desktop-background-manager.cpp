#include "peony-desktop-application.h"

#include "desktop-background-manager.h"
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

static DesktopBackgroundManager *global_instance = nullptr;

#define BACKGROUND_SETTINGS "org.mate.background"

DesktopBackgroundManager::DesktopBackgroundManager(QObject *parent) : QObject(parent)
{
    m_animation = new QVariantAnimation(this);
    m_animation->setDuration(1000);
    m_animation->setStartValue(qreal(0));
    m_animation->setEndValue(qreal(1));

    m_timeLine = new QTimeLine(200, this);
    connect(m_timeLine, &QTimeLine::finished, this, &DesktopBackgroundManager::updateScreens);

    connect(m_animation, &QVariantAnimation::valueChanged, this, &DesktopBackgroundManager::updateScreens);
    connect(m_animation, &QVariantAnimation::finished, this, [=](){
        m_backPixmap = m_frontPixmap;
        if (!m_pendingPixmap.isNull()) {
            m_frontPixmap = m_pendingPixmap;
            m_pendingPixmap = QPixmap();
            m_animation->start();
        }
        updateScreens();
    });

    initGSettings();
}

void DesktopBackgroundManager::initGSettings()
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

void DesktopBackgroundManager::updateScreens()
{
    Q_EMIT screensUpdated();
}

void DesktopBackgroundManager::initBackground()
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

void DesktopBackgroundManager::setBackground()
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

QString DesktopBackgroundManager::getAccountBackground()
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

void DesktopBackgroundManager::setAccountBackground()
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

void DesktopBackgroundManager::switchBackground()
{
    if (!m_backgroundSettings)
        return;

    //intel项目背景绘制模式 平铺，拉伸等
    m_backgroundOption = m_backgroundSettings->get("pictureOptions").toString();
    if (m_backgroundOption.isEmpty()) {
        //判断系统类型
        bool intel = false;
        if (intel) {
            m_backgroundOption = "stretched";
        } else {
            m_backgroundOption = "";
        }
    }

    auto path = m_backgroundSettings->get("pictureFilename").toString();
    if (! QFile::exists(path) && !path.isEmpty())
        path = getAccountBackground();
    if (path.isEmpty()) {
        m_usePureColor = true;
        auto colorName = m_backgroundSettings->get("primaryColor").toString();
        m_color = QColor(colorName);
        m_animation->stop();
        m_backPixmap = QPixmap();
        m_frontPixmap = QPixmap();
        m_current_bg_path = "";
        updateScreens();
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
        updateScreens();
    }

    //if background picture changed, update it
    if (m_current_bg_path != getAccountBackground())
        setAccountBackground();
}

bool DesktopBackgroundManager::getPaintBackground() const
{
    return m_paintBackground;
}

QColor DesktopBackgroundManager::getColor() const
{
    return m_color;
}

bool DesktopBackgroundManager::getUsePureColor() const
{
    return m_usePureColor;
}

QVariantAnimation *DesktopBackgroundManager::getAnimation() const
{
    return m_animation;
}

QPixmap DesktopBackgroundManager::getFrontPixmap() const
{
    return m_frontPixmap;
}

DesktopBackgroundManager *DesktopBackgroundManager::globalInstance()
{
    if (!global_instance) {
        global_instance = new DesktopBackgroundManager;
    }
    return global_instance;
}

QPixmap DesktopBackgroundManager::getBackPixmap() const
{
    return m_backPixmap;
}

const QString &DesktopBackgroundManager::getBackgroundOption()
{
    return m_backgroundOption;
}
