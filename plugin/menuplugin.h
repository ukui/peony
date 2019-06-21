#ifndef MENUPLUGIN_TEST1
#define MENUPLUGIN_TEST1

#include <QObject>
#include <QtPlugin>
#include "menu-iface.h"

class MenuPluginTest1 : public QObject, public MenuInterface
{
    Q_OBJECT
    //Q_PLUGIN_METADATA(IID MenuInterface_iid FILE "menuplugintest1.json")
    Q_PLUGIN_METADATA(IID MenuInterface_iid)
    Q_INTERFACES(MenuInterface)

public:
    QString testPlugin() override;
};

#endif
