/*
 *  caja-image-converter.c
 * 
 *  Copyright (C) 2004-2005 Jürg Billeter
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author: Jürg Billeter <j@bitron.ch>
 * 
 */

#ifdef HAVE_CONFIG_H
 #include <config.h> /* for GETTEXT_PACKAGE */
#endif

#include "caja-image-converter.h"
#include "caja-image-resizer.h"
#include "caja-image-rotator.h"

#include <libcaja-extension/caja-menu-provider.h>

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include <string.h> /* for strcmp */

static void caja_image_converter_instance_init (CajaImageConverter      *img);
static void caja_image_converter_class_init    (CajaImageConverterClass *class);
GList *     caja_image_converter_get_file_items (CajaMenuProvider *provider,
						     GtkWidget            *window,
						     GList                *files);

static GType image_converter_type = 0;

static gboolean
image_converter_file_is_image (CajaFileInfo *file_info)
{
	gchar            *uri_scheme;
	gchar            *mime_type;
	gboolean	maybe_image;
	
	maybe_image = TRUE;
	uri_scheme = caja_file_info_get_uri_scheme (file_info);
	if (strcmp (uri_scheme, "file") != 0)
		maybe_image = FALSE;
	g_free (uri_scheme);

	mime_type = caja_file_info_get_mime_type (file_info);
	if (strncmp (mime_type, "image/", 6) != 0)
		maybe_image = FALSE;
	g_free (mime_type);
	
	return maybe_image;
}

static GList *
image_converter_filter_images (GList *files)
{
	GList *images;
	GList *file;
	
	images = NULL;
	
	for (file = files; file != NULL; file = file->next) {
		if (image_converter_file_is_image (file->data))
			images = g_list_prepend (images, file->data);
	}
	
	return images;
}

static void
image_resize_callback (CajaMenuItem *item,
			GList *files)
{
	CajaImageResizer *resizer = caja_image_resizer_new (image_converter_filter_images (files));
	caja_image_resizer_show_dialog (resizer);
}

static void
image_rotate_callback (CajaMenuItem *item,
			GList *files)
{
	CajaImageRotator *rotator = caja_image_rotator_new (image_converter_filter_images (files));
	caja_image_rotator_show_dialog (rotator);
}

static GList *
caja_image_converter_get_background_items (CajaMenuProvider *provider,
					     GtkWidget		  *window,
					     CajaFileInfo	  *file_info)
{
	return NULL;
}

GList *
caja_image_converter_get_file_items (CajaMenuProvider *provider,
				       GtkWidget            *window,
				       GList                *files)
{
	CajaMenuItem *item;
	GList *file;
	GList *items = NULL;
	
	for (file = files; file != NULL; file = file->next) {
		if (image_converter_file_is_image (file->data)) {
			item = caja_menu_item_new ("CajaImageConverter::resize",
				        _("_Resize Images..."),
				        _("Resize each selected image"),
				       "stock_position-size");
			g_signal_connect (item, "activate",
					  G_CALLBACK (image_resize_callback),
					  caja_file_info_list_copy (files));
					
			items = g_list_prepend (items, item);

			item = caja_menu_item_new ("CajaImageConverter::rotate",
				        _("Ro_tate Images..."),
				        _("Rotate each selected image"),
				       "stock_rotate");
			g_signal_connect (item, "activate",
					  G_CALLBACK (image_rotate_callback),
					  caja_file_info_list_copy (files));

			items = g_list_prepend (items, item);
			
			items = g_list_reverse (items);

			return items;
		}
	}
	
	return NULL;
}

static void
caja_image_converter_menu_provider_iface_init (CajaMenuProviderIface *iface)
{
	iface->get_background_items = caja_image_converter_get_background_items;
	iface->get_file_items = caja_image_converter_get_file_items;
}

static void 
caja_image_converter_instance_init (CajaImageConverter *img)
{
}

static void
caja_image_converter_class_init (CajaImageConverterClass *class)
{
}

GType
caja_image_converter_get_type (void) 
{
	return image_converter_type;
}

void
caja_image_converter_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
		sizeof (CajaImageConverterClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) caja_image_converter_class_init,
		NULL, 
		NULL,
		sizeof (CajaImageConverter),
		0,
		(GInstanceInitFunc) caja_image_converter_instance_init,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) caja_image_converter_menu_provider_iface_init,
		NULL,
		NULL
	};

	image_converter_type = g_type_module_register_type (module,
						     G_TYPE_OBJECT,
						     "CajaImageConverter",
						     &info, 0);

	g_type_module_add_interface (module,
				     image_converter_type,
				     CAJA_TYPE_MENU_PROVIDER,
				     &menu_provider_iface_info);
}
