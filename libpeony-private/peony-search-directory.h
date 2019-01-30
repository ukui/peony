/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-search-directory.h: Subclass of PeonyDirectory to implement
   a virtual directory consisting of the search directory and the search
   icons

   Copyright (C) 2005 Novell, Inc
   Copyright (C) 2018, Tianjin KYLIN Information Technology Co., Ltd.

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
   Modified by: liupeng <liupeng@kylinos.cn>
*/

#ifndef PEONY_SEARCH_DIRECTORY_H
#define PEONY_SEARCH_DIRECTORY_H

#include <libpeony-private/peony-directory.h>
#include <libpeony-private/peony-query.h>

#define PEONY_TYPE_SEARCH_DIRECTORY peony_search_directory_get_type()
#define PEONY_SEARCH_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SEARCH_DIRECTORY, PeonySearchDirectory))
#define PEONY_SEARCH_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SEARCH_DIRECTORY, PeonySearchDirectoryClass))
#define PEONY_IS_SEARCH_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SEARCH_DIRECTORY))
#define PEONY_IS_SEARCH_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SEARCH_DIRECTORY))
#define PEONY_SEARCH_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SEARCH_DIRECTORY, PeonySearchDirectoryClass))

typedef struct PeonySearchDirectoryDetails PeonySearchDirectoryDetails;

typedef struct
{
    PeonyDirectory parent_slot;
    PeonySearchDirectoryDetails *details;
} PeonySearchDirectory;

typedef struct
{
    PeonyDirectoryClass parent_slot;
} PeonySearchDirectoryClass;

GType   peony_search_directory_get_type             (void);

char   *peony_search_directory_generate_new_uri     (void);

PeonySearchDirectory *peony_search_directory_new_from_saved_search (const char *uri);

gboolean       peony_search_directory_is_saved_search (PeonySearchDirectory *search);
gboolean       peony_search_directory_is_modified     (PeonySearchDirectory *search);
gboolean       peony_search_directory_is_indexed      (PeonySearchDirectory *search);
void           peony_search_directory_save_search     (PeonySearchDirectory *search);
void           peony_search_directory_save_to_file    (PeonySearchDirectory *search,
        const char              *save_file_uri);

PeonyQuery *peony_search_directory_get_query       (PeonySearchDirectory *search);
void           peony_search_directory_set_query       (PeonySearchDirectory *search,
        PeonyQuery           *query);
void set_search_duplicate (PeonySearchDirectory *search,gboolean bDuplicate);
gboolean get_search_duplicate (PeonySearchDirectory *search);

void set_search_is_image_search (PeonySearchDirectory *search,gboolean is_image_search);
gboolean get_search_is_image_search (PeonySearchDirectory *search);

#endif /* PEONY_SEARCH_DIRECTORY_H */
