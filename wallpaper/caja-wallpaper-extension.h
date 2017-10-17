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
 
#ifndef CAJA_WALLPAPER_EXTENSION_H
#define CAJA_WALLPAPER_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define CAJA_TYPE_CWE  (caja_cwe_get_type ())
#define CAJA_CWE(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), CAJA_TYPE_CWE, CajaCwe))
#define CAJA_IS_CWE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), CAJA_TYPE_CWE))

typedef struct _CajaCwe      CajaCwe;
typedef struct _CajaCweClass CajaCweClass;

struct _CajaCwe {
	GObject __parent;
};

struct _CajaCweClass {
	GObjectClass __parent;
};

GType caja_cwe_get_type      (void);
void  caja_cwe_register_type (GTypeModule *module);

G_END_DECLS

#endif /* CAJA_WALLPAPER_EXTENSION_H */
