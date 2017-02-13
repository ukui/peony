/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-saved-search-file.h: Subclass of PeonyVFSFile to implement the
   the case of a Saved Search file.

   Copyright (C) 2005 Red Hat, Inc

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

   Author: Alexander Larsson
*/

#ifndef PEONY_SAVED_SEARCH_FILE_H
#define PEONY_SAVED_SEARCH_FILE_H

#include <libpeony-private/peony-vfs-file.h>

#define PEONY_TYPE_SAVED_SEARCH_FILE peony_saved_search_file_get_type()
#define PEONY_SAVED_SEARCH_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SAVED_SEARCH_FILE, PeonySavedSearchFile))
#define PEONY_SAVED_SEARCH_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SAVED_SEARCH_FILE, PeonySavedSearchFileClass))
#define PEONY_IS_SAVED_SEARCH_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SAVED_SEARCH_FILE))
#define PEONY_IS_SAVED_SEARCH_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SAVED_SEARCH_FILE))
#define PEONY_SAVED_SEARCH_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SAVED_SEARCH_FILE, PeonySavedSearchFileClass))


typedef struct PeonySavedSearchFileDetails PeonySavedSearchFileDetails;

typedef struct
{
    PeonyFile parent_slot;
} PeonySavedSearchFile;

typedef struct
{
    PeonyFileClass parent_slot;
} PeonySavedSearchFileClass;

GType   peony_saved_search_file_get_type (void);

#endif /* PEONY_SAVED_SEARCH_FILE_H */
