#include "fm-dbus-service.h"

#include <QDBusConnection>
#include <QDBusConnectionInterface>

#include <QDebug>

using namespace Peony;

FMDBusService::FMDBusService(QObject *parent) : QObject(parent)
{
    qDebug()<<QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/freedesktop/FileManager1"), this,
            QDBusConnection::ExportScriptableContents | QDBusConnection::ExportAdaptors);
    qDebug()<<QDBusConnection::sessionBus().interface()->registerService(QStringLiteral("org.freedesktop.FileManager1"),
                                                                         QDBusConnectionInterface::QueueService);
}

void FMDBusService::ShowFolders(const QStringList& uriList, const QString& startUpId)
{
    qDebug()<<"show folders:"<<uriList;
    Q_EMIT showFolderRequest(uriList, startUpId);
}

void FMDBusService::ShowItems(const QStringList& uriList, const QString& startUpId)
{
    qDebug()<<"show items:"<<uriList;
    Q_EMIT showItemsRequest(uriList, startUpId);
}

void FMDBusService::ShowItemProperties(const QStringList& uriList, const QString& startUpId)
{
    qDebug()<<"show item properties:"<<uriList;
    Q_EMIT showItemPropertiesRequest(uriList, startUpId);
}
