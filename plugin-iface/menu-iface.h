#ifndef MENUIFACE_H
#define MENUIFACE_H

#include <QPluginLoader>
#include <QtPlugin>
#include <QString>
#include <QAction>

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

namespace Peony {

class MenuInterface
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
        Common,
        File,//selected files in a view
        Volume,//maybe an item computer view, or an item in sidebar
        DirectoryBackground,//a view background menu
        DesktopBackground,//a desktop view background menu
        Other
    };

    virtual ~MenuInterface() {}

    virtual QString testPlugin() = 0;
    /*!
     * \brief menuActions
     * \param type the menu type
     * \param uris
     * \return the list of actions which plugin provided.
     * \details
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
    virtual QList<QAction *> menuActions(Type type, const QStringList &uris) = 0;
};

}

Q_DECLARE_INTERFACE (Peony::MenuInterface, MenuInterface_iid)

#endif
