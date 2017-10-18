/*
 *  Peony SendTo extension 
 * 
 *  Copyright (C) 2005 Roberto Majadas 
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
 * 
 */

#ifndef PEONY_NSTE_H
#define PEONY_NSTE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define PEONY_TYPE_NSTE  (peony_nste_get_type ())
#define PEONY_NSTE(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_NSTE, PeonyNste))
#define PEONY_IS_NSTE(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_NSTE))

typedef struct _PeonyNste      PeonyNste;
typedef struct _PeonyNsteClass PeonyNsteClass;

struct _PeonyNste {
	GObject __parent;
};

struct _PeonyNsteClass {
	GObjectClass __parent;
};

GType peony_nste_get_type      (void);
void  peony_nste_register_type (GTypeModule *module);

G_END_DECLS

#endif /* PEONY_NSTE_H */
