/*
 *  open-terminal.c
 * 
 *  Copyright (C) 2004, 2005 Free Software Foundation, Inc.
 *  Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
 *
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
 *  Author: Christian Neumair <chris@gnome-de.org>
 *  Modified by : liupeng <liupeng@kylinos.cn>
 */

#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif

#include "peony-open-terminal.h"

#include <libintl.h>

static GType type_list[1];

void
peony_module_initialize (GTypeModule *module)
{
	g_print ("Initializing peony-open-terminal extension\n");

	peony_open_terminal_register_type (module);
	type_list[0] = PEONY_TYPE_OPEN_TERMINAL;

	bindtextdomain (GETTEXT_PACKAGE, MATELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
}

void
peony_module_shutdown (void)
{
	g_print ("Shutting down peony-open-terminal extension\n");
}

void 
peony_module_list_types (const GType **types,
			    int          *num_types)
{
	*types = type_list;
	*num_types = G_N_ELEMENTS (type_list);
}
