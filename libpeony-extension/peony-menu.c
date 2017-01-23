/*
 *  peony-menu.h - Menus exported by PeonyMenuProvider objects.
 *
 *  Copyright (C) 2005 Raffaele Sandrini
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
 *  Author:  Raffaele Sandrini <rasa@gmx.ch>
 *
 */

#include <config.h>
#include "peony-menu.h"
#include "peony-extension-i18n.h"

#include <glib.h>

/**
 * SECTION:peony-menu
 * @title: PeonyMenu
 * @short_description: Menu descriptor object
 * @include: libpeony-extension/peony-menu.h
 *
 * #PeonyMenu is an object that describes a submenu in a file manager
 * menu. Extensions can provide #PeonyMenu objects by attaching them to
 * #PeonyMenuItem objects, using peony_menu_item_set_submenu().
 */

#define PEONY_MENU_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), PEONY_TYPE_MENU, PeonyMenuPrivate))
G_DEFINE_TYPE (PeonyMenu, peony_menu, G_TYPE_OBJECT);

struct _PeonyMenuPrivate {
    GList *item_list;
};

void
peony_menu_append_item (PeonyMenu *menu, PeonyMenuItem *item)
{
    g_return_if_fail (menu != NULL);
    g_return_if_fail (item != NULL);

    menu->priv->item_list = g_list_append (menu->priv->item_list, g_object_ref (item));
}

/**
 * peony_menu_get_items:
 * @menu: a #PeonyMenu
 *
 * Returns: (element-type PeonyMenuItem) (transfer full): the provided #PeonyMenuItem list
 */
GList *
peony_menu_get_items (PeonyMenu *menu)
{
    GList *item_list;

    g_return_val_if_fail (menu != NULL, NULL);

    item_list = g_list_copy (menu->priv->item_list);
    g_list_foreach (item_list, (GFunc)g_object_ref, NULL);

    return item_list;
}

/**
 * peony_menu_item_list_free:
 * @item_list: (element-type PeonyMenuItem): a list of #PeonyMenuItem
 *
 */
void
peony_menu_item_list_free (GList *item_list)
{
    g_return_if_fail (item_list != NULL);

    g_list_foreach (item_list, (GFunc)g_object_unref, NULL);
    g_list_free (item_list);
}

/* Type initialization */

static void
peony_menu_finalize (GObject *object)
{
    PeonyMenu *menu = PEONY_MENU (object);

    if (menu->priv->item_list) {
        g_list_free (menu->priv->item_list);
    }

    G_OBJECT_CLASS (peony_menu_parent_class)->finalize (object);
}

static void
peony_menu_init (PeonyMenu *menu)
{
    menu->priv = PEONY_MENU_GET_PRIVATE (menu);

    menu->priv->item_list = NULL;
}

static void
peony_menu_class_init (PeonyMenuClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (PeonyMenuPrivate));

    object_class->finalize = peony_menu_finalize;
}

/* public constructors */

PeonyMenu *
peony_menu_new (void)
{
    PeonyMenu *obj;

    obj = PEONY_MENU (g_object_new (PEONY_TYPE_MENU, NULL));

    return obj;
}
