#ifndef MATETERMINALMENUPLUGIN_H
#define MATETERMINALMENUPLUGIN_H

#include "peony-qt-menu-plugin-mate-terminal_global.h"

#include <QObject>
#include <QtPlugin>

#include "menu-plugin-iface.h"

namespace Peony {

class PEONYQTMENUPLUGINMATETERMINALSHARED_EXPORT MateTerminalMenuPlugin : public QObject, public MenuPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MenuPluginInterface_iid)
    Q_INTERFACES(Peony::MenuPluginInterface)
public:
    explicit MateTerminalMenuPlugin(QObject *parent = nullptr);

    PluginInterface::PluginType pluginType() override {return PluginInterface::MenuPlugin;}
    const QString name() override {return tr("Peony-Qt Mate Terminal Menu Extension");}
    const QString description() override {return tr("Open Terminal with menu.");}
    const QIcon icon() override {return QIcon::fromTheme("utilities-terminal-symbolic");}
    void setEnable(bool enable) override {m_enable = enable;}
    bool isEnable() override {return m_enable;}

    QString testPlugin() override {return "test";}
    QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) override;

private:
    bool m_enable;
};

}

#endif // MATETERMINALMENUPLUGIN_H
