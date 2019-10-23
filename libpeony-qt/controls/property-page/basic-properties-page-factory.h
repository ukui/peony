#ifndef BASICPROPERTIESPAGEFACTORY_H
#define BASICPROPERTIESPAGEFACTORY_H

#include <QObject>
#include "peony-core_global.h"
#include "properties-window-tab-page-plugin-iface.h"

namespace Peony {

class BasicPropertiesPageFactory : public QObject, public PropertiesWindowTabPagePluginIface
{
    Q_OBJECT
public:
    static BasicPropertiesPageFactory *getInstance();

    //plugin iface
    const QString name() override {return QObject::tr("Basic Preview Page");}
    PluginType pluginType() override {return PluginType::PropertiesWindowPlugin;}
    const QString description() override {return QObject::tr("Show the basic file properties, and allow to modify the access and name.");}
    const QIcon icon() override {return QIcon::fromTheme("view-paged-symbolic", QIcon::fromTheme("folder"));}
    void setEnable(bool enable) override {Q_UNUSED(enable)}
    bool isEnable() override {return true;}

    //properties plugin iface
    int tabOrder() override {return 1000;}
    bool supportUri(const QString &uri) override;
    QWidget *createTabPage(const QString &uri) override;

    void closeFactory() override;

private:
    explicit BasicPropertiesPageFactory(QObject *parent = nullptr);
    ~BasicPropertiesPageFactory() override;
};

}

#endif // BASICPROPERTIESPAGEFACTORY_H
