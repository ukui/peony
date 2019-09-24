#ifndef TOOLBARACTIONPLUGINIFACE_H
#define TOOLBARACTIONPLUGINIFACE_H

#include "plugin-iface.h"
#include <QAction>

#define ToolBarActionPluginIface_iid "org.ukui.peony-qt.plugin-iface.ToolBarActionPluginInterface"

namespace Peony {

class ToolBarExtensionActionIface : public QAction
{
    Q_OBJECT

    ~ToolBarExtensionActionIface() {}

public Q_SLOTS:
    virtual void excuteAction(const QString &directoryUri,
                              const QStringList &selectedUris) = 0;
};

class ToolBarActionPluginIface : public PluginInterface
{

public:
    ~ToolBarActionPluginIface() {}

    virtual ToolBarExtensionActionIface *create() = 0;
};

}

Q_DECLARE_INTERFACE(Peony::ToolBarActionPluginIface, ToolBarActionPluginIface_iid)

#endif // TOOLBARACTIONPLUGINIFACE_H
