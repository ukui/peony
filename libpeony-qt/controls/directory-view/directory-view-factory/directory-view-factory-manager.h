#ifndef DIRECTORYVIEWFACTORYMANAGER_H
#define DIRECTORYVIEWFACTORYMANAGER_H

#include <QObject>
#include <QHash>
#include <QSettings>

namespace Peony {

class DirectoryViewIface;
class DirectoryViewPluginIface;
class DirectoryViewPluginIface2;

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

class DirectoryViewFactoryManager2 : public QObject
{
    Q_OBJECT
public:
    static DirectoryViewFactoryManager2 *getInstance();

    void registerFactory(const QString &name, DirectoryViewPluginIface2 *factory);
    QStringList getFactoryNames();
    DirectoryViewPluginIface2 *getFactory(const QString &name);

    const QString getDefaultViewId();

public Q_SLOTS:
    void setDefaultViewId(const QString &viewId);
    void saveDefaultViewOption();

private:
    QHash<QString, DirectoryViewPluginIface2*> *m_hash = nullptr;
    explicit DirectoryViewFactoryManager2(QObject *parent = nullptr);
    ~DirectoryViewFactoryManager2();

    QSettings *m_settings;
    QString m_default_view_id_cache;
};

}

#endif // DIRECTORYVIEWFACTORYMANAGER_H
