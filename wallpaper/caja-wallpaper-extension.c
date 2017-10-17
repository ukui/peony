/*
 *  Caja Wallpaper extension
 *
 *  Copyright (C) 2005 Adam Israel
 *  Copyright (C) 2014 Stefano Karapetsas
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  Authors: Adam Israel <adam@battleaxe.net>
 *           Stefano Karapetsas <stefano@karapetsas.com>
 */
 
#include <config.h>
#include <string.h>
#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-file-info.h>
#include <libcaja-extension/caja-menu-provider.h>
#include "caja-wallpaper-extension.h"

#define WP_SCHEMA "org.mate.background"
#define WP_FILE_KEY "picture-filename"

static GObjectClass *parent_class;

static void
set_wallpaper_callback (CajaMenuItem *item,
              gpointer          user_data)
{
    GList *files;
    GSettings *settings;
    GError *err;
    CajaFileInfo *file;
    gchar *uri;
    gchar *filename;

    files = g_object_get_data (G_OBJECT (item), "files");
    file = files->data;

    uri = caja_file_info_get_uri (file);
    filename = g_filename_from_uri(uri, NULL, NULL);

    settings = g_settings_new (WP_SCHEMA);

    g_settings_set_string (settings, WP_FILE_KEY, filename);

    g_object_unref (settings);
    g_free (filename);
    g_free (uri);
    
}

static gboolean
is_image (CajaFileInfo *file)
{
    gchar *mimeType;
    gboolean isImage;
    
    mimeType = caja_file_info_get_mime_type (file);
    
    isImage = g_str_has_prefix (caja_file_info_get_mime_type (file), "image/");
    
    g_free (mimeType);
    
    return isImage; 
}


static GList *
caja_cwe_get_file_items (CajaMenuProvider *provider,
                  GtkWidget            *window,
                  GList                *files)
{
    GList    *items = NULL;
    GList    *scan;
    gboolean  one_item;
    CajaMenuItem *item;

    for (scan = files; scan; scan = scan->next) {
        CajaFileInfo *file = scan->data;
        gchar            *scheme;
        gboolean          local;

        scheme = caja_file_info_get_uri_scheme (file);
        local = strncmp (scheme, "file", 4) == 0;
        g_free (scheme);

        if (!local)
            return NULL;
    }

    one_item = (files != NULL) && (files->next == NULL);
    if (one_item && is_image ((CajaFileInfo *)files->data) &&
        !caja_file_info_is_directory ((CajaFileInfo *)files->data)) {
        item = caja_menu_item_new ("CajaCwe::sendto",
                           _("Set as wallpaper"),
                           _("Set image as the current wallpaper"),
                           NULL);
        g_signal_connect (item, 
                  "activate",
                  G_CALLBACK (set_wallpaper_callback),
                provider);
        g_object_set_data_full (G_OBJECT (item), 
                    "files",
                    caja_file_info_list_copy (files),
                    (GDestroyNotify) caja_file_info_list_free);
        items = g_list_append (items, item);
    }
    return items;
}


static void 
caja_cwe_menu_provider_iface_init (CajaMenuProviderIface *iface)
{
    iface->get_file_items = caja_cwe_get_file_items;
}


static void 
caja_cwe_instance_init (CajaCwe *cwe)
{
}


static void
caja_cwe_class_init (CajaCweClass *class)
{
    parent_class = g_type_class_peek_parent (class);
}


static GType cwe_type = 0;


GType
caja_cwe_get_type (void) 
{
    return cwe_type;
}


void
caja_cwe_register_type (GTypeModule *module)
{
    static const GTypeInfo info = {
        sizeof (CajaCweClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) caja_cwe_class_init,
        NULL, 
        NULL,
        sizeof (CajaCwe),
        0,
        (GInstanceInitFunc) caja_cwe_instance_init,
    };

    static const GInterfaceInfo menu_provider_iface_info = {
        (GInterfaceInitFunc) caja_cwe_menu_provider_iface_init,
        NULL,
        NULL
    };

    cwe_type = g_type_module_register_type (module,
                             G_TYPE_OBJECT,
                             "CajaCwe",
                             &info, 0);

    g_type_module_add_interface (module,
                     cwe_type,
                     CAJA_TYPE_MENU_PROVIDER,
                     &menu_provider_iface_info);
}
