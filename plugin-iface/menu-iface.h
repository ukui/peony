#ifndef MENUIFACE_H
#define MENUIFACE_H

#include <QPluginLoader>
#include <QtPlugin>
#include <QString>

#define MenuInterface_iid "org.ukui.plugin-iface.MenuInterface"

/**
 * @brief The MenuInterface class
 * @details MenuInterface is a Qt Plugin Interface, the methods of this class must all be VIRTUAL and has a initial return value.
 * Otherwise, the plugin will not be loaded correctly.
 * For example, if we define a virtual QString method, we must do as this:
 * <pre>
 *  virtual QString xxx() = 0;
 * </pre>
 * DO NOT FORGET to initial virtual method in class definition.
 */

class MenuInterface
{
public:
    virtual ~MenuInterface() {}
    virtual QString testPlugin() = 0;
};

Q_DECLARE_INTERFACE (MenuInterface, MenuInterface_iid)

#endif

