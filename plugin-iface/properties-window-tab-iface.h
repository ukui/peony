#ifndef PROPERTIESWINDOWTABIFACE_H
#define PROPERTIESWINDOWTABIFACE_H

#include <QWidget>

#define PropertiesWindowTabIface_iid "org.ukui.peony-qt.plugin-iface.PropertiesWindowTabInterface"
namespace Peony {

class PropertiesWindowTabIface : public QWidget
{
    Q_OBJECT

public:
    PropertiesWindowTabIface(QWidget *parent = nullptr) : QWidget(parent){}
    virtual ~PropertiesWindowTabIface() {}
    PropertiesWindowTabIface();

    virtual void saveAllChange() = 0;
};

}

Q_DECLARE_INTERFACE(Peony::PropertiesWindowTabIface, PropertiesWindowTabIface_iid)

#endif // PROPERTIESWINDOWTABIFACE_H
