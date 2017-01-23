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
#include <config.h>
#include "peony-saved-search-file.h"
#include "peony-file-private.h"

G_DEFINE_TYPE(PeonySavedSearchFile, peony_saved_search_file, PEONY_TYPE_VFS_FILE)


static void
peony_saved_search_file_init (PeonySavedSearchFile *search_file)
{
}

static void
peony_saved_search_file_class_init (PeonySavedSearchFileClass * klass)
{
    PeonyFileClass *file_class;

    file_class = PEONY_FILE_CLASS (klass);

    file_class->default_file_type = G_FILE_TYPE_DIRECTORY;
}

