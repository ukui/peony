/*
 *  Peony xattr tags extension
 *
 *  Copyright (C) 2016 Felipe Barriga Richards
 *  Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
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
 *  Authors: Felipe Barriga Richards <spam@felipebarriga.cl>
 *  Modified by : liupeng <liupeng@kylinos.cn>
 */
 
#ifndef PEONY_XATTR_TAGS_EXTENSION_H
#define PEONY_XATTR_TAGS_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define PEONY_TYPE_XATTR_TAGS  (peony_xattr_tags_get_type ())
#define PEONY_XATTR_TAGS(o)    (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_XATTR_TAGS, peonyXattrTags))
#define PEONY_IS_XATTR_TAGS(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_XATTR_TAGS))

typedef struct _PeonyXattrTags      PeonyXattrTags;
typedef struct _PeonyXattrTagsClass PeonyXattrTagsClass;

struct _PeonyXattrTags {
	GObject __parent;
};

struct _PeonyXattrTagsClass {
	GObjectClass __parent;
};

GType peony_xattr_tags_get_type(void);
void  peony_xattr_tags_register_type(GTypeModule *module);

//////////////////////////////////////////////////////
// hack: libpeony-private/peony-file.c
char *peony_file_get_activation_uri(PeonyFile *file);
//////////////////////////////////////////////////////

G_END_DECLS

#endif /* PEONY_XATTR_TAGS_EXTENSION_H */
