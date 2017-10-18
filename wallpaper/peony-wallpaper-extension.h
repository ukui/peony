/*
 *  Peony Wallpaper extension
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
 
#ifndef PEONY_WALLPAPER_EXTENSION_H
#define PEONY_WALLPAPER_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define PEONY_TYPE_CWE  (peony_cwe_get_type ())
#define PEONY_CWE(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_CWE, PeonyCwe))
#define PEONY_IS_CWE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_CWE))

typedef struct _PeonyCwe      PeonyCwe;
typedef struct _PeonyCweClass PeonyCweClass;

struct _PeonyCwe {
	GObject __parent;
};

struct _PeonyCweClass {
	GObjectClass __parent;
};

GType peony_cwe_get_type      (void);
void  peony_cwe_register_type (GTypeModule *module);

G_END_DECLS

#endif /* PEONY_WALLPAPER_EXTENSION_H */
