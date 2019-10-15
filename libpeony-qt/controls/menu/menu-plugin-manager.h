#ifndef MENUPLUGINMANAGER_H
#define MENUPLUGINMANAGER_H

#include <QObject>
#include "peony-core_global.h"
#include "menu-plugin-iface.h"

namespace Peony {

class MenuPluginManager : public QObject
{
    Q_OBJECT
public:
    bool registerPlugin(MenuPluginInterface *plugin);

    static MenuPluginManager *getInstance();
    const QStringList getPluginIds();
    MenuPluginInterface *getPlugin(const QString &pluginId);

    void close();

private:
    QHash<QString, MenuPluginInterface*> m_hash;

    explicit MenuPluginManager(QObject *parent = nullptr);
    ~MenuPluginManager();
};

}

#endif // MENUPLUGINMANAGER_H
