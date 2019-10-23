#ifndef MENUPLUGIN_TEST1
#define MENUPLUGIN_TEST1

#include <QObject>
#include <QtPlugin>
#include "menu-plugin-iface.h"

namespace Peony {

class MenuPluginTest1 : public QObject, public MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid FILE "menuplugintest1.json")
    //Q_PLUGIN_METADATA(IID MenuInterface_iid)
    Q_INTERFACES(Peony::MenuPluginInterface)

public:
    PluginInterface::PluginType pluginType() override {return PluginInterface::MenuPlugin;}
    const QString name() override {return "testMenuPlugin1";}
    const QString description() override {return "This is a menu type test plugin";}
    const QIcon icon() override {return QIcon::fromTheme("search");}
    void setEnable(bool enable) override {m_enable = enable;}
    bool isEnable() override {return m_enable;}

    QString testPlugin() override;
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;

private:
    bool m_enable;
};

}

#endif
