/*
 *  peony-menu.h - Menus exported by PeonyMenuProvider objects.
 *
 *  Copyright (C) 2005 Raffaele Sandrini
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author:  Dave Camp <dave@ximian.com>
 *           Raffaele Sandrini <rasa@gmx.ch>
 *
 */

#ifndef PEONY_MENU_H
#define PEONY_MENU_H

#include <glib-object.h>
#include "peony-extension-types.h"

G_BEGIN_DECLS

/* PeonyMenu defines */
#define PEONY_TYPE_MENU         (peony_menu_get_type ())
#define PEONY_MENU(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_MENU, PeonyMenu))
#define PEONY_MENU_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_MENU, PeonyMenuClass))
#define PEONY_IS_MENU(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_MENU))
#define PEONY_IS_MENU_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_MENU))
#define PEONY_MENU_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_MENU, PeonyMenuClass))
/* PeonyMenuItem defines */
#define PEONY_TYPE_MENU_ITEM            (peony_menu_item_get_type())
#define PEONY_MENU_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_MENU_ITEM, PeonyMenuItem))
#define PEONY_MENU_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_MENU_ITEM, PeonyMenuItemClass))
#define PEONY_MENU_IS_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_MENU_ITEM))
#define PEONY_MENU_IS_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), PEONY_TYPE_MENU_ITEM))
#define PEONY_MENU_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PEONY_TYPE_MENU_ITEM, PeonyMenuItemClass))

/* PeonyMenu types */
typedef struct _PeonyMenu		PeonyMenu;
typedef struct _PeonyMenuPrivate	PeonyMenuPrivate;
typedef struct _PeonyMenuClass	PeonyMenuClass;
/* PeonyMenuItem types */
typedef struct _PeonyMenuItem        PeonyMenuItem;
typedef struct _PeonyMenuItemDetails PeonyMenuItemDetails;
typedef struct _PeonyMenuItemClass   PeonyMenuItemClass;

/* PeonyMenu structs */
struct _PeonyMenu {
    GObject parent;
    PeonyMenuPrivate *priv;
};

struct _PeonyMenuClass {
    GObjectClass parent_class;
};

/* PeonyMenuItem structs */
struct _PeonyMenuItem {
    GObject parent;

    PeonyMenuItemDetails *details;
};

struct _PeonyMenuItemClass {
    GObjectClass parent;

    void (*activate) (PeonyMenuItem *item);
};

/* PeonyMenu methods */
GType     peony_menu_get_type       (void);
PeonyMenu *peony_menu_new            (void);

void      peony_menu_append_item    (PeonyMenu     *menu,
                                    PeonyMenuItem *item);
GList    *peony_menu_get_items      (PeonyMenu *menu);
void      peony_menu_item_list_free (GList *item_list);

/* PeonyMenuItem methods */
GType         peony_menu_item_get_type    (void);
PeonyMenuItem *peony_menu_item_new         (const char   *name,
                                          const char   *label,
                                          const char   *tip,
                                          const char   *icon);

void          peony_menu_item_activate    (PeonyMenuItem *item);
void          peony_menu_item_set_submenu (PeonyMenuItem *item,
                                          PeonyMenu     *menu);

/* PeonyMenuItem has the following properties:
 *   name (string)        - the identifier for the menu item
 *   label (string)       - the user-visible label of the menu item
 *   tip (string)         - the tooltip of the menu item
 *   icon (string)        - the name of the icon to display in the menu item
 *   sensitive (boolean)  - whether the menu item is sensitive or not
 *   priority (boolean)   - used for toolbar items, whether to show priority
 *                          text.
 *   menu (PeonyMenu)      - The menu belonging to this item. May be null.
 */

G_END_DECLS

#endif /* PEONY_MENU_H */
