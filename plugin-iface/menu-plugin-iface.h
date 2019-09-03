#ifndef MENUPLUGINIFACE_H
#define MENUPLUGINIFACE_H

#include <QPluginLoader>
#include <QtPlugin>
#include <QString>
#include <QAction>
#include "plugin-iface.h"

#define MenuPluginInterface_iid "org.ukui.peony-qt.plugin-iface.MenuPluginInterface"

namespace Peony {

/**
 * @brief The MenuPluginInterface class
 * @details MenuPluginInterface is a Qt Plugin Interface, the methods of this class must all be
 * pure VIRTUAL and has ZERO as return value.
 * Otherwise, the plugin will not be loaded correctly.
 * For example, if we define a virtual QString method, we must do as this:
 * <pre>
 *  virtual QString xxx() = 0;
 * </pre>
 * DO NOT FORGET to initial virtual method in class definition.
 */
class MenuPluginInterface : public PluginInterface
{
public:
    /*!
     * \brief The Type enum
     * \details
     * Type is a enum filter which could help you return different actions
     * by different menu iface type.
     * For example, if you write a desktop background wrapper plugin,
     * you should only return your actions when the passing type is
     * DesktopBackground.
     */
    enum Type {
        Invalid,
        File,//selected files in a view
        Volume = File << 1,//maybe an item computer view, or an item in sidebar
        DirectoryBackground = File << 2,//a view background menu
        DesktopBackground = File << 3,//a desktop view background menu
        Other = File << 4
    };
    Q_DECLARE_FLAGS(Types, Type)

    //virtual ~MenuPluginInterface() {}

    virtual QString testPlugin() = 0;

    /*!
     * \brief menuActions
     * \param types
     * \param uri
     * \param selectionUris
     * \return
     * In peony-qt, user can install the menu extensions. This is because
     * peony-qt's view/menu frameworks is desgined to be extensible.
     * When a peony view context menu was request it will call the plugin's
     * menuActions method get the extension actions.
     * \note
     * A menu can be more than one type, a desktop background menu is also
     * could be a directory background menu. This is decied by peony-qt's
     * view/menu framework itself.
     * You can only implement one type of a menu, but you should never directly
     * return the actions without a type check, this might cause actions in menu
     * duplicated.
     */
    virtual QList<QAction *> menuActions(Types types, const QString &uri, const QStringList &selectionUris) = 0;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Peony::MenuPluginInterface::Types)
Q_DECLARE_INTERFACE (Peony::MenuPluginInterface, MenuPluginInterface_iid)

#endif
