#ifndef PROPERTIESWINDOWTABPAGEEXAMPLEFACTORY_H
#define PROPERTIESWINDOWTABPAGEEXAMPLEFACTORY_H

#include "properties-window-tab-page-plugin-example_global.h"

#include "properties-window-tab-page-plugin-iface.h"
#include <QObject>

class PROPERTIESWINDOWTABPAGEPLUGINEXAMPLESHARED_EXPORT PropertiesWindowTabPageExampleFactory :
        public QObject, public Peony::PropertiesWindowTabPagePluginIface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PropertiesWindowTabPagePluginIface_iid)
    Q_INTERFACES(Peony::PropertiesWindowTabPagePluginIface)
public:
    explicit PropertiesWindowTabPageExampleFactory(QObject *parent = nullptr);
    ~PropertiesWindowTabPageExampleFactory() override;

    PluginInterface::PluginType pluginType() override {return PluginInterface::PropertiesWindowPlugin;}
    const QString name() override {return "Properties Page Plugin Example";}
    const QString description() override {return "This plugin is a factory plugin providing the add-on properties page for peony-qt's properties window";}
    const QIcon icon() override {return QIcon::fromTheme("view-paged-symbolic");}
    void setEnable(bool enable) override {m_enable = enable;}
    bool isEnable() override {return m_enable;}

    int tabOrder() override {return 0;}
    bool supportUris(const QStringList &uris) override {return true;}
    QWidget *createTabPage(const QStringList &uris) override;

    void closeFactory() override {deleteLater();}

private:
    bool m_enable = true;
};

#endif // PROPERTIESWINDOWTABPAGEEXAMPLEFACTORY_H
