#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H

#include <QObject>
#include <QProcess>
#include "peony-core_global.h"
#include <libnotify/notify.h>

namespace Peony{

class PEONYCORESHARED_EXPORT SyncThread : public QObject
{
    Q_OBJECT
public:
    explicit SyncThread(QString uri, QObject *parent = nullptr);

private:
    void notifyUser(QString notifyContent);

Q_SIGNALS:
    void syncFinished();

public Q_SLOTS:
    void parentStartedSlot();

private:
    QString                         mHint;
    QString                         mUri;

};

}

#endif // SYNCTHREAD_H
