#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H

#include <QObject>
#include <QProcess>
#include "peony-core_global.h"

namespace Peony{

class PEONYCORESHARED_EXPORT SyncThread : public QObject
{
    Q_OBJECT
public:
    explicit SyncThread(QObject *parent = nullptr);

private:
    void notifyUser(QString notifyContent);

Q_SIGNALS:
    void syncFinished();

public Q_SLOTS:
    void parentStartedSlot();

private:
    QString mHint;

};

}

#endif // SYNCTHREAD_H
