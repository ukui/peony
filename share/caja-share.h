/* caja-share -- Caja File Sharing Extension
 *
 * Sebastien Estienne <sebastien.estienne@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * (C) Copyright 2005 Ethium, Inc.
 */

#ifndef CAJA_SHARE_H
#define CAJA_SHARE_H

#include <glib-object.h>

G_BEGIN_DECLS

/* Declarations for the Share extension object.  This object will be
 * instantiated by caja.  It implements the GInterfaces
 * exported by libcaja. */


typedef struct _CajaShare      CajaShare;
typedef struct _CajaShareClass CajaShareClass;

struct _CajaShare {
	GObject parent_slot;
};

struct _CajaShareClass {
	GObjectClass parent_slot;

	/* No extra class members */
};


typedef struct _CajaShareData      CajaShareData;

struct _CajaShareData {
  gchar		*fullpath;
  gchar		*section;
  CajaFileInfo *fileinfo;
};

G_END_DECLS

typedef enum {
  CAJA_SHARE_NOT_SHARED,
  CAJA_SHARE_SHARED_RO,
  CAJA_SHARE_SHARED_RW
} CajaShareStatus;

#endif
 
