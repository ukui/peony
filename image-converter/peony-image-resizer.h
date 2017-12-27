/*
 *  peony-image-resize-dialog.h
 * 
 *  Copyright (C) 2004-2005 Jürg Billeter
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

#ifndef PEONY_IMAGE_RESIZER_H
#define PEONY_IMAGE_RESIZER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define PEONY_TYPE_IMAGE_RESIZER         (peony_image_resizer_get_type ())
#define PEONY_IMAGE_RESIZER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_IMAGE_RESIZER, PeonyImageResizer))
#define PEONY_IMAGE_RESIZER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_IMAGE_RESIZER, PeonyImageResizerClass))
#define PEONY_IS_IMAGE_RESIZER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_IMAGE_RESIZER))
#define PEONY_IS_IMAGE_RESIZER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_IMAGE_RESIZER))
#define PEONY_IMAGE_RESIZER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_IMAGE_RESIZER, PeonyImageResizerClass))

typedef struct _PeonyImageResizer PeonyImageResizer;
typedef struct _PeonyImageResizerClass PeonyImageResizerClass;

struct _PeonyImageResizer {
	GObject parent;
};

struct _PeonyImageResizerClass {
	GObjectClass parent_class;
	/* Add Signal Functions Here */
};

GType peony_image_resizer_get_type (void);
PeonyImageResizer *peony_image_resizer_new (GList *files);
void peony_image_resizer_show_dialog (PeonyImageResizer *dialog);

G_END_DECLS

#endif /* PEONY_IMAGE_RESIZER_H */
