#ifndef DIRECTORYVIEWFACTORYMANAGER_H
#define DIRECTORYVIEWFACTORYMANAGER_H

#include <QObject>
#include <QHash>
#include <QSettings>

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

    const QString getDefaultViewId();

public Q_SLOTS:
    void setDefaultViewId(const QString &viewId);
    void saveDefaultViewOption();

private:
    QHash<QString, DirectoryViewPluginIface*> *m_hash = nullptr;
    explicit DirectoryViewFactoryManager(QObject *parent = nullptr);
    ~DirectoryViewFactoryManager();

    QSettings *m_settings;
    QString m_default_view_id_cache;
};

}

#endif // DIRECTORYVIEWFACTORYMANAGER_H
