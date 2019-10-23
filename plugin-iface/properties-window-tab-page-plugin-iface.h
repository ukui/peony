#ifndef PROPERTIESWINDOWTABPAGEPLUGINIFACE_H
#define PROPERTIESWINDOWTABPAGEPLUGINIFACE_H

#include <QPluginLoader>
#include <QtPlugin>
#include <QString>
#include "plugin-iface.h"

class QWidget;

#define PropertiesWindowTabPagePluginIface_iid "org.ukui.peony-qt.plugin-iface.PropertiesWindowTabPagePluginInterface"

namespace Peony {

class PropertiesWindowTabPagePluginIface : public PluginInterface
{
public:
    virtual ~PropertiesWindowTabPagePluginIface() {}

    virtual int tabOrder() = 0;
    virtual bool supportUri(const QString &uri) = 0;
    virtual QWidget *createTabPage(const QString &uri) = 0;

    virtual void closeFactory() = 0;
};

}

Q_DECLARE_INTERFACE(Peony::PropertiesWindowTabPagePluginIface, PropertiesWindowTabPagePluginIface_iid)

#endif // PROPERTIESWINDOWTABPAGEPLUGINIFACE_H
