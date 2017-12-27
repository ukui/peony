/*
 *  Peony-sendto 
 * 
 *  Copyright (C) 2004 Free Software Foundation, Inc.
 *  Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
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
 *  Author: Roberto Majadas <roberto.majadas@openshine.com> 
 *  Modified by : liupeng <liupeng@kylinos.cn>
 */

#include <config.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <libpeony-extension/peony-extension-types.h>
#include <libpeony-extension/peony-file-info.h>
#include <libpeony-extension/peony-menu-provider.h>
#include "peony-nste.h"


static GObjectClass *parent_class;

static void
sendto_callback (PeonyMenuItem *item,
	      gpointer          user_data)
{
	GList            *files, *scan;
	PeonyFileInfo *file;
	gchar            *uri;
	GString          *cmd;

	files = g_object_get_data (G_OBJECT (item), "files");
	file = files->data;

	cmd = g_string_new ("peony-sendto");

	for (scan = files; scan; scan = scan->next) {
		PeonyFileInfo *file = scan->data;

		uri = peony_file_info_get_uri (file);
		g_string_append_printf (cmd, " \"%s\"", uri);
		g_free (uri);
	}

	g_spawn_command_line_async (cmd->str, NULL);

	g_string_free (cmd, TRUE);
}

static GList *
peony_nste_get_file_items (PeonyMenuProvider *provider,
			      GtkWidget            *window,
			      GList                *files)
{
	GList    *items = NULL;
	gboolean  one_item;
	PeonyMenuItem *item;

	if (files == NULL)
		return NULL;

	one_item = (files != NULL) && (files->next == NULL);
	if (one_item && 
	    !peony_file_info_is_directory ((PeonyFileInfo *)files->data)) {
		item = peony_menu_item_new ("PeonyNste::sendto",
					       _("Send to..."),
					       _("Send file by mail, instant message..."),
					       "document-send");
	} else {
		item = peony_menu_item_new ("PeonyNste::sendto",
					       _("Send to..."),
					       _("Send files by mail, instant message..."),
					       "document-send");
	}
  g_signal_connect (item, 
      "activate",
      G_CALLBACK (sendto_callback),
      provider);
  g_object_set_data_full (G_OBJECT (item), 
      "files",
      peony_file_info_list_copy (files),
      (GDestroyNotify) peony_file_info_list_free);

  items = g_list_append (items, item);

	return items;
}


static void 
peony_nste_menu_provider_iface_init (PeonyMenuProviderIface *iface)
{
	iface->get_file_items = peony_nste_get_file_items;
}


static void 
peony_nste_instance_init (PeonyNste *nste)
{
}


static void
peony_nste_class_init (PeonyNsteClass *class)
{
	parent_class = g_type_class_peek_parent (class);
}


static GType nste_type = 0;


GType
peony_nste_get_type (void) 
{
	return nste_type;
}


void
peony_nste_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
		sizeof (PeonyNsteClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) peony_nste_class_init,
		NULL, 
		NULL,
		sizeof (PeonyNste),
		0,
		(GInstanceInitFunc) peony_nste_instance_init,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) peony_nste_menu_provider_iface_init,
		NULL,
		NULL
	};

	nste_type = g_type_module_register_type (module,
					         G_TYPE_OBJECT,
					         "PeonyNste",
					         &info, 0);

	g_type_module_add_interface (module,
				     nste_type,
				     PEONY_TYPE_MENU_PROVIDER,
				     &menu_provider_iface_info);
}
