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
#include <libcaja-extension/caja-extension-types.h>
#include <libcaja-extension/caja-column-provider.h>
#include <glib/gi18n-lib.h>
#include "caja-wallpaper-extension.h"

void
caja_module_initialize (GTypeModule*module)
{
    caja_cwe_register_type (module);
}

void
caja_module_shutdown (void)
{
}

void 
caja_module_list_types (const GType **types, int *num_types)
{
    static GType type_list[1];
    
    type_list[0] = CAJA_TYPE_CWE;
    *types = type_list;
    *num_types = 1;
}
