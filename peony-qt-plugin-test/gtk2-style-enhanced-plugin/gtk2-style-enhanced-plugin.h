#ifndef GTK2STYLEENHANCEDPLUGIN_H
#define GTK2STYLEENHANCEDPLUGIN_H

#include "gtk2-style-enhanced-plugin_global.h"

#include <QObject>
#include <QtPlugin>

#include "style-plugin-iface.h"

namespace Peony {

class GTK2STYLEENHANCEDPLUGINSHARED_EXPORT Gtk2StyleEnhancedPlugin : public QObject, public StylePluginIface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID StylePluginIface_iid)
    Q_INTERFACES(Peony::StylePluginIface)
public:
    explicit Gtk2StyleEnhancedPlugin(QObject *parent = nullptr);

    PluginInterface::PluginType pluginType() {return PluginInterface::StylePlugin;}
    const QString name() {return tr("Gtk theme enhanced extension");}
    const QString description() {return tr("Improve the gtk-themed controls' style and painting");}
    const QIcon icon() {return QIcon::fromTheme("utilities-terminal-symbolic");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}

    int defaultPriority() {return 0;}

    QProxyStyle *getStyle();

private:
    bool m_enable;
};

}

#endif // GTK2STYLEENHANCEDPLUGIN_H
