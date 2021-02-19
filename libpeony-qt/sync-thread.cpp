#include "sync-thread.h"
#include <QThread>
#include <QDebug>
#include <libnotify/notify.h>

using namespace Peony;

SyncThread::SyncThread(QObject *parent) : QObject(parent)
{
    mHint = QObject::tr("It need to synchronize before operating the device,place wait!");
}

void SyncThread::parentStartedSlot()
{
    QProcess syncProcess;

    notifyUser(mHint);

    syncProcess.start("sync");
    syncProcess.waitForFinished(-1);

    Q_EMIT this->syncFinished();
}

void SyncThread::notifyUser(QString notifyContent)
{
    NotifyNotification* notify;

    notify_init("PeonyNotify");
    notify  = notify_notification_new(QObject::tr("File Manager").toUtf8().constData(),
                                      notifyContent.toUtf8().constData(),
                                      "system-file-manager");
    notify_notification_show(notify,nullptr);

    notify_uninit();
    g_object_unref(G_OBJECT(notify));
}

