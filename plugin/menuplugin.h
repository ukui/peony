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
    QString testPlugin() override;
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;
};

}

#endif
