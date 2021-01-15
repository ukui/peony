#ifndef PROPERTIESWINDOWTABPAGEPLUGINSAVEIFACE_H
#define PROPERTIESWINDOWTABPAGEPLUGINSAVEIFACE_H

#include <QWidget>

#define PropertiesWindowTabPagePluginSaveIface_iid "org.ukui.peony-qt.plugin-iface.PropertiesWindowTabPagePluginSaveInterface"
namespace Peony {

class PropertiesWindowTabPagePluginSaveIface : public QWidget
{
    Q_OBJECT

public:
    PropertiesWindowTabPagePluginSaveIface(QWidget *parent = nullptr) : QWidget(parent){}
    virtual ~PropertiesWindowTabPagePluginSaveIface() {}
    PropertiesWindowTabPagePluginSaveIface();

     virtual void saveAllChange() = 0;
};

}

Q_DECLARE_INTERFACE(Peony::PropertiesWindowTabPagePluginSaveIface, PropertiesWindowTabPagePluginSaveIface_iid)


#endif // PROPERTIESWINDOWTABPAGEPLUGINSAVEIFACE_H
