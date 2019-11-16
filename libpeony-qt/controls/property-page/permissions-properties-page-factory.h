#ifndef PERMISSIONSPROPERTIESPAGEFACTORY_H
#define PERMISSIONSPROPERTIESPAGEFACTORY_H

#include <QObject>

#include "peony-core_global.h"
#include "properties-window-tab-page-plugin-iface.h"

namespace Peony {

class PEONYCORESHARED_EXPORT PermissionsPropertiesPageFactory : public QObject, public PropertiesWindowTabPagePluginIface
{
    Q_OBJECT
public:
    static PermissionsPropertiesPageFactory *getInstance();

    //plugin iface
    const QString name() override {return QObject::tr("Permissions Page");}
    PluginType pluginType() override {return PluginType::PropertiesWindowPlugin;}
    const QString description() override {return QObject::tr("Show and modify file's permission, owner and group.");}
    const QIcon icon() override {return QIcon::fromTheme("view-paged-symbolic", QIcon::fromTheme("folder"));}
    void setEnable(bool enable) override {Q_UNUSED(enable)}
    bool isEnable() override {return true;}

    //properties plugin iface
    int tabOrder() override {return 800;}
    bool supportUris(const QStringList &uris) override;
    QWidget *createTabPage(const QStringList &uris) override;

    void closeFactory() override;

private:
    explicit PermissionsPropertiesPageFactory(QObject *parent = nullptr);
};

}

#endif // PERMISSIONSPROPERTIESPAGEFACTORY_H
