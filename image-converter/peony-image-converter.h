/*
 *  peony-image-converter.h
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

#ifndef PEONY_IMAGE_CONVERTER_H
#define PEONY_IMAGE_CONVERTER_H

#include <glib-object.h>

G_BEGIN_DECLS

/* Declarations for the open terminal extension object.  This object will be
 * instantiated by peony.  It implements the GInterfaces 
 * exported by libpeony. */


#define PEONY_TYPE_IMAGE_CONVERTER	  (peony_image_converter_get_type ())
#define PEONY_IMAGE_CONVERTER(o)		  (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_IMAGE_CONVERTER, PeonyImageConverter))
#define PEONY_IS_IMAGE_CONVERTER(o)	  (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_IMAGE_CONVERTER))
typedef struct _PeonyImageConverter	  PeonyImageConverter;
typedef struct _PeonyImageConverterClass	  PeonyImageConverterClass;

struct _PeonyImageConverter {
	GObject parent_slot;
};

struct _PeonyImageConverterClass {
	GObjectClass parent_slot;
};

GType peony_image_converter_get_type      (void);
void  peony_image_converter_register_type (GTypeModule *module);

G_END_DECLS

#endif
