/*
 *  peony-image-rotator.h
 * 
 *  Copyright (C) 2004-2006 Jürg Billeter
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
 *  Author: Jürg Billeter <j@bitron.ch>
 *  Modified by : liupeng <liupeng@kylinos.cn>
 */

#ifndef PEONY_IMAGE_ROTATOR_H
#define PEONY_IMAGE_ROTATOR_H

#include <glib-object.h>

G_BEGIN_DECLS

#define PEONY_TYPE_IMAGE_ROTATOR         (peony_image_rotator_get_type ())
#define PEONY_IMAGE_ROTATOR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_IMAGE_ROTATOR, PeonyImageRotator))
#define PEONY_IMAGE_ROTATOR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_IMAGE_ROTATOR, PeonyImageRotatorClass))
#define PEONY_IS_IMAGE_ROTATOR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_IMAGE_ROTATOR))
#define PEONY_IS_IMAGE_ROTATOR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_IMAGE_ROTATOR))
#define PEONY_IMAGE_ROTATOR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_IMAGE_ROTATOR, PeonyImageRotatorClass))

typedef struct _PeonyImageRotator PeonyImageRotator;
typedef struct _PeonyImageRotatorClass PeonyImageRotatorClass;

struct _PeonyImageRotator {
	GObject parent;
};

struct _PeonyImageRotatorClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType peony_image_rotator_get_type (void);
PeonyImageRotator *peony_image_rotator_new (GList *files);
void peony_image_rotator_show_dialog (PeonyImageRotator *dialog);

G_END_DECLS

#endif /* PEONY_IMAGE_ROTATOR_H */
