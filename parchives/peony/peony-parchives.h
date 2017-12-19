/*
 *  Parchives
 * 
 *  Copyright (C) 2004 Free Software Foundation, Inc.
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
 * 
 */

#ifndef PEONY_RNGRAMPA_H
#define PEONY_RNGRAMPA_H

#include <glib-object.h>

G_BEGIN_DECLS

#define PEONY_TYPE_FR  (peony_fr_get_type ())
#define PEONY_FR(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_FR, PeonyFr))
#define PEONY_IS_FR(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_FR))

typedef struct _PeonyFr      PeonyFr;
typedef struct _PeonyFrClass PeonyFrClass;

struct _PeonyFr {
	GObject __parent;
};

struct _PeonyFrClass {
	GObjectClass __parent;
};

GType peony_fr_get_type      (void);
void  peony_fr_register_type (GTypeModule *module);

G_END_DECLS

#endif /* PEONY_RNGRAMPA_H */
