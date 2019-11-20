#ifndef DESKTOPMENUPLUGINMANAGER_H
#define DESKTOPMENUPLUGINMANAGER_H

#include <QObject>
#include <QMap>
#include "menu-plugin-iface.h"

namespace Peony {

class DesktopMenuPluginManager : public QObject
{
    Q_OBJECT
public:
    static DesktopMenuPluginManager *getInstance();
    bool isLoaded() {return m_is_loaded;}
    const QStringList getPluginIds();
    QList<MenuPluginInterface*> getPlugins();
    MenuPluginInterface *getPlugin(const QString &pluginId);

protected:
    void loadAsync();

private:
    explicit DesktopMenuPluginManager(QObject *parent = nullptr);
    ~DesktopMenuPluginManager();

    QMap<QString, MenuPluginInterface*> m_map;
    bool m_is_loaded = false;
};

}

#endif // DESKTOPMENUPLUGINMANAGER_H
