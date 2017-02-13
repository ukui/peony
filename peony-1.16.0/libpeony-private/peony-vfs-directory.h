/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-vfs-directory.h: Subclass of PeonyDirectory to implement the
   the case of a VFS directory.

   Copyright (C) 1999, 2000 Eazel, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Author: Darin Adler <darin@bentspoon.com>
*/

#ifndef PEONY_VFS_DIRECTORY_H
#define PEONY_VFS_DIRECTORY_H

#include <libpeony-private/peony-directory.h>

#define PEONY_TYPE_VFS_DIRECTORY peony_vfs_directory_get_type()
#define PEONY_VFS_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_VFS_DIRECTORY, PeonyVFSDirectory))
#define PEONY_VFS_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_VFS_DIRECTORY, PeonyVFSDirectoryClass))
#define PEONY_IS_VFS_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_VFS_DIRECTORY))
#define PEONY_IS_VFS_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_VFS_DIRECTORY))
#define PEONY_VFS_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_VFS_DIRECTORY, PeonyVFSDirectoryClass))

typedef struct PeonyVFSDirectoryDetails PeonyVFSDirectoryDetails;

typedef struct
{
    PeonyDirectory parent_slot;
} PeonyVFSDirectory;

typedef struct
{
    PeonyDirectoryClass parent_slot;
} PeonyVFSDirectoryClass;

GType   peony_vfs_directory_get_type (void);

#endif /* PEONY_VFS_DIRECTORY_H */
