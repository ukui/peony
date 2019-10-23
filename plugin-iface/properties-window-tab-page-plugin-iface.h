#ifndef PROPERTIESWINDOWTABPAGEPLUGINIFACE_H
#define PROPERTIESWINDOWTABPAGEPLUGINIFACE_H

#include <QPluginLoader>
#include <QtPlugin>
#include <QStringList>
#include "plugin-iface.h"

class QWidget;

#define PropertiesWindowTabPagePluginIface_iid "org.ukui.peony-qt.plugin-iface.PropertiesWindowTabPagePluginInterface"

namespace Peony {

class PropertiesWindowTabPagePluginIface : public PluginInterface
{
public:
    virtual ~PropertiesWindowTabPagePluginIface() {}

    virtual int tabOrder() = 0;
    virtual bool supportUris(const QStringList &uris) = 0;
    virtual QWidget *createTabPage(const QStringList &uris) = 0;

    virtual void closeFactory() = 0;
};

}

Q_DECLARE_INTERFACE(Peony::PropertiesWindowTabPagePluginIface, PropertiesWindowTabPagePluginIface_iid)

#endif // PROPERTIESWINDOWTABPAGEPLUGINIFACE_H
