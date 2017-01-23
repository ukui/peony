/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-search-directory-file.h: Subclass of PeonyFile to implement the
   the case of the search directory

   Copyright (C) 2003 Red Hat, Inc.

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

   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef PEONY_SEARCH_DIRECTORY_FILE_H
#define PEONY_SEARCH_DIRECTORY_FILE_H

#include <libpeony-private/peony-file.h>

#define PEONY_TYPE_SEARCH_DIRECTORY_FILE peony_search_directory_file_get_type()
#define PEONY_SEARCH_DIRECTORY_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SEARCH_DIRECTORY_FILE, PeonySearchDirectoryFile))
#define PEONY_SEARCH_DIRECTORY_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SEARCH_DIRECTORY_FILE, PeonySearchDirectoryFileClass))
#define PEONY_IS_SEARCH_DIRECTORY_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SEARCH_DIRECTORY_FILE))
#define PEONY_IS_SEARCH_DIRECTORY_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SEARCH_DIRECTORY_FILE))
#define PEONY_SEARCH_DIRECTORY_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SEARCH_DIRECTORY_FILE, PeonySearchDirectoryFileClass))

typedef struct PeonySearchDirectoryFileDetails PeonySearchDirectoryFileDetails;

typedef struct
{
    PeonyFile parent_slot;
    PeonySearchDirectoryFileDetails *details;
} PeonySearchDirectoryFile;

typedef struct
{
    PeonyFileClass parent_slot;
} PeonySearchDirectoryFileClass;

GType   peony_search_directory_file_get_type (void);
void    peony_search_directory_file_update_display_name (PeonySearchDirectoryFile *search_file);

#endif /* PEONY_SEARCH_DIRECTORY_FILE_H */
