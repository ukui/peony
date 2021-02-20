#include "primary-manager.h"
#include <QDebug>
#include <QTimer>

#define DBUS_NAME       "org.ukui.SettingsDaemon"
#define DBUS_PATH       "/org/ukui/SettingsDaemon/xrandr"
#define DBUS_INTERFACE  "org.ukui.SettingsDaemon.xrandr"

PrimaryManager::PrimaryManager()
{
    //QDBusConnection conn = QDBusConnection::sessionBus();
    mDbusXrandInter = new QDBusInterface(DBUS_NAME,
                                         DBUS_PATH,
                                         DBUS_INTERFACE,
                                         QDBusConnection::sessionBus());
    connect(mDbusXrandInter, SIGNAL(screenPrimaryChanged(int,int,int,int)),
            this, SLOT(priScreenChanged(int,int,int,int)));

}

void PrimaryManager::start()
{
    int priX, priY, priWid, priHei;
    QString priName;
    priX = getScreenGeometry("x");
    priY = getScreenGeometry("y");
    priWid = getScreenGeometry("width");
    priHei = getScreenGeometry("height");
    priName = getScreenName ("priScreenName");

    qDebug("Start: Primary screen geometry is x=%d, y=%d, windth=%d, height=%d,",
           priX, priY, priWid, priHei);
    qDebug()<<"Primary screen name = " << priName;
}

int PrimaryManager::getScreenGeometry(QString methodName)
{
    int res = 0;
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_NAME,
                               DBUS_PATH,
                               DBUS_INTERFACE,
                               methodName);
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        if(response.arguments().isEmpty() == false) {
            int value = response.arguments().takeFirst().toInt();
            res = value;
            qDebug() << value;
        }
    } else {
        qDebug()<<methodName<<"called failed";
    }
    return res;
}

QString PrimaryManager::getScreenName(QString methodName)
{
    QString res = 0;
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_NAME,
                               DBUS_PATH,
                               DBUS_INTERFACE,
                               methodName);
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        if(response.arguments().isEmpty() == false) {
            QString value = response.arguments().takeFirst().toString();
            res = value;
            qDebug() << value;
        }
    } else {
        qDebug()<<methodName<<"called failed";
    }
    return res;
}

/* get primary screen changed */
void PrimaryManager::priScreenChanged(int x, int y, int width, int height)
{
    Q_EMIT this->priScreenChangedSignal(x, y, width, height);
    // re-check
    QTimer::singleShot(500, this, [=](){
        Q_EMIT this->priScreenChangedSignal(x, y, width, height);
    });
    qDebug("primary screen  changed, geometry is  x=%d, y=%d, windth=%d, height=%d", x, y, width, height);
}
