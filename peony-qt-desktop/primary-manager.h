#ifndef PRIMARYMANAGER_H
#define PRIMARYMANAGER_H

#include <QObject>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>

class PrimaryManager : public QObject
{
    Q_OBJECT
public:
    PrimaryManager();
    void start();
    int getScreenGeometry (QString methodName);
    QString getScreenName (QString methodName);

Q_SIGNALS:
    void priScreenChangedSignal(int x, int y, int width, int height);

public Q_SLOTS:
    void priScreenChanged(int x, int y, int width, int height);

private:
    QDBusInterface  *mDbusXrandInter;

};

#endif // PRIMARYMANAGER_H
