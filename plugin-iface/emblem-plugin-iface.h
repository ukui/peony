#ifndef EMBLEMPLUGINIFACE_H
#define EMBLEMPLUGINIFACE_H

#include "plugin-iface.h"
#include "peony-core_global.h"

#include "emblem-provider.h"

#define EmblemPluginInterface_iid "org.ukui.peony-qt.plugin-iface.EmblemPluginInterface"

namespace Peony {

class PEONYCORESHARED_EXPORT EmblemPluginInterface : public PluginInterface
{
public:
    virtual ~EmblemPluginInterface() {}

    virtual EmblemProvider *create() = 0;
};

}
Q_DECLARE_INTERFACE (Peony::EmblemPluginInterface, EmblemPluginInterface_iid)

#endif // EMBLEMPLUGINIFACE_H
