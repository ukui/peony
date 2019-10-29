#ifndef MENUPLUGINEXAMPLE_H
#define MENUPLUGINEXAMPLE_H

#include "peony-qt-menu-plugin-example_global.h"
#include <QObject>
#include <QtPlugin>

#include "menu-plugin-iface.h"

namespace Peony {

class PEONYQTMENUPLUGINEXAMPLESHARED_EXPORT MenuPluginExample : public QObject, public MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid)
    Q_INTERFACES(Peony::MenuPluginInterface)
public:
    explicit MenuPluginExample(QObject *parent = nullptr);

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

#endif // MENUPLUGINEXAMPLE_H
