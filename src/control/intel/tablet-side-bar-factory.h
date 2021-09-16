#ifndef TABLETSIDEBARFACTORY_H
#define TABLETSIDEBARFACTORY_H

#include "side-bar-plugin-iface.h"

namespace Peony {

namespace Intel {

class TabletSideBarFactory : public QObject, public SideBarPluginInterface
{
    Q_OBJECT
public:
    explicit TabletSideBarFactory(QObject *parent = nullptr);

    const QStringList keys();
    SideBar *create(QWidget *parent);

    PluginType pluginType();

    const QString name();
    const QString description();
    const QIcon icon();
    void setEnable(bool enable);
    bool isEnable();
};

}

}

#endif // TABLETSIDEBARFACTORY_H
