#ifndef DIRECTORYVIEWPLUGINIFACE2_H
#define DIRECTORYVIEWPLUGINIFACE2_H

#include "plugin-iface.h"

#include <QWidget>

#define DirectoryViewPluginIface2_iid "org.ukui.peony-qt.plugin-iface.DirectoryViewPluginInterface2"

namespace Peony {

class DirectoryViewWidget;

class DirectoryViewPluginIface2 : public PluginInterface
{
public:
    virtual ~DirectoryViewPluginIface2() {}

    virtual QString viewIdentity() = 0;
    virtual QIcon viewIcon() = 0;
    virtual bool supportUri(const QString &uri) = 0;

    virtual int zoom_level_hint() = 0;

    virtual int priority(const QString &directoryUri) = 0;

    //virtual void fillDirectoryView(DirectoryViewWidget *view) = 0;

    virtual DirectoryViewWidget *create() = 0;
};

}

Q_DECLARE_INTERFACE (Peony::DirectoryViewPluginIface2, DirectoryViewPluginIface2_iid)

#endif // DIRECTORYVIEWPLUGINIFACE2_H
