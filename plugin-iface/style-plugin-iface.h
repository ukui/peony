#ifndef STYLEPLUGINIFACE_H
#define STYLEPLUGINIFACE_H

#include <QPluginLoader>
#include <QtPlugin>
#include <QString>

#include "plugin-iface.h"

#define StylePluginIface_iid "org.ukui.peony-qt.plugin-iface.StylePluginInterface"

class QProxyStyle;

namespace Peony {

class StylePluginIface : public PluginInterface
{
public:
    ~StylePluginIface() {}

    virtual int defaultPriority() = 0;

    virtual QProxyStyle *getStyle() = 0;
};

}

Q_DECLARE_INTERFACE(Peony::StylePluginIface, StylePluginIface_iid)

#endif // STYLEPLUGINIFACE_H
