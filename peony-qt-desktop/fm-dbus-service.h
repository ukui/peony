#ifndef FMDBUSSERVICE_H
#define FMDBUSSERVICE_H

#include <QObject>

namespace Peony {

class FMDBusService : public QObject
{
    Q_OBJECT
    //Do not modify this row.
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.FileManager1")

public:
    explicit FMDBusService(QObject *parent = nullptr);
    Q_SCRIPTABLE void ShowFolders(const QStringList& uriList, const QString& startUpId);
    Q_SCRIPTABLE void ShowItems(const QStringList& uriList, const QString& startUpId);
    Q_SCRIPTABLE void ShowItemProperties(const QStringList& uriList, const QString& startUpId);

Q_SIGNALS:
    void showFolderRequest(const QStringList& uriList, const QString& startUpId);
    void showItemsRequest(const QStringList& uriList, const QString& startUpId);
    void showItemPropertiesRequest(const QStringList& uriList, const QString& startUpId);
};

}

#endif // FMDBUSSERVICE_H
