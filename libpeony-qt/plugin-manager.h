#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include "peony-core_global.h"

#include "plugin-iface.h"

namespace Peony {

class PluginManager : public QObject
{
    Q_OBJECT
public:
    static void init();
    static PluginManager *getInstance();
    void close();

Q_SIGNALS:
    void pluginStateChanged(const QString &pluginName, bool enable);

public Q_SLOTS:
    void setPluginEnableByName(const QString &pluginName, bool enable);

private:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager();

    QHash<QString, PluginInterface*> m_hash;
};

}

#endif // PLUGINMANAGER_H
