#include "sync-thread.h"
#include <QThread>
#include <QDebug>
#include <QDBusInterface>
#include <libnotify/notify.h>

using namespace Peony;

SyncThread::SyncThread(QString uri, QObject *parent) : QObject(parent), mUri(uri)
{
    mHint = QObject::tr("It need to synchronize before operating the device,place wait!");
}

void SyncThread::parentStartedSlot()
{
    if (mUri == "file:///"
            || mUri == "computer:///root.link") {
        return;
    }

    QProcess syncProcess;

    notifyUser(mHint);

    syncProcess.start("sync");
    syncProcess.waitForFinished(-1);

    Q_EMIT this->syncFinished();
}

void SyncThread::notifyUser(QString notifyContent)
{
    QDBusInterface iface ("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications", QDBusConnection::sessionBus ());

    QList <QVariant> args;
    QStringList actions;
    QMap <QString, QVariant> hints;

    args << QObject::tr("File Manager").toUtf8().constData()
         << ((unsigned int) 0)
         << "system-file-manager"
         << tr("notify")
         << notifyContent
         << actions
         << hints
         << (int) -1;

    iface.callWithArgumentList (QDBus::AutoDetect, "Notify", args);
}

