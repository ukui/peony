/*
 *  Parchives
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
 *  Author: Paolo Bacchilega <paobac@cvs.mate.org>
 *  Modified by : liupeng <liupeng@kylinos.cn>
 */

#include <config.h>
#include <libpeony-extension/peony-extension-types.h>
#include <libpeony-extension/peony-column-provider.h>
#include <glib/gi18n-lib.h>
#include "peony-parchives.h"


void
peony_module_initialize (GTypeModule*module)
{
	peony_fr_register_type (module);

	bindtextdomain (GETTEXT_PACKAGE, MATELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
}


void
peony_module_shutdown (void)
{
}


void 
peony_module_list_types (const GType **types,
			    int          *num_types)
{
	static GType type_list[1];
	
	type_list[0] = PEONY_TYPE_FR;
	*types = type_list;
	*num_types = 1;
}
