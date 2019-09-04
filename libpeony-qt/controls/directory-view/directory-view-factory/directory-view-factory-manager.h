#ifndef DIRECTORYVIEWFACTORYMANAGER_H
#define DIRECTORYVIEWFACTORYMANAGER_H

#include <QObject>
#include <QHash>

namespace Peony {

class DirectoryViewIface;
class DirectoryViewPluginIface;

class DirectoryViewFactoryManager : public QObject
{
    Q_OBJECT
public:
    static DirectoryViewFactoryManager *getInstance();

    void registerFactory(const QString &name, DirectoryViewPluginIface *factory);
    QStringList getFactoryNames();
    DirectoryViewPluginIface *getFactory(const QString &name);

private:
    QHash<QString, DirectoryViewPluginIface*> *m_hash = nullptr;
    explicit DirectoryViewFactoryManager(QObject *parent = nullptr);
    ~DirectoryViewFactoryManager();
};

}

#endif // DIRECTORYVIEWFACTORYMANAGER_H
