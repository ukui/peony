/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-desktop-file.h: Subclass of PeonyFile to implement the
   the case of a desktop icon file

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

#ifndef PEONY_DESKTOP_ICON_FILE_H
#define PEONY_DESKTOP_ICON_FILE_H

#include <libpeony-private/peony-file.h>
#include <libpeony-private/peony-desktop-link.h>

#define PEONY_TYPE_DESKTOP_ICON_FILE peony_desktop_icon_file_get_type()
#define PEONY_DESKTOP_ICON_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_DESKTOP_ICON_FILE, PeonyDesktopIconFile))
#define PEONY_DESKTOP_ICON_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_DESKTOP_ICON_FILE, PeonyDesktopIconFileClass))
#define PEONY_IS_DESKTOP_ICON_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_DESKTOP_ICON_FILE))
#define PEONY_IS_DESKTOP_ICON_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_DESKTOP_ICON_FILE))
#define PEONY_DESKTOP_ICON_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_DESKTOP_ICON_FILE, PeonyDesktopIconFileClass))

typedef struct PeonyDesktopIconFileDetails PeonyDesktopIconFileDetails;

typedef struct
{
    PeonyFile parent_slot;
    PeonyDesktopIconFileDetails *details;
} PeonyDesktopIconFile;

typedef struct
{
    PeonyFileClass parent_slot;
} PeonyDesktopIconFileClass;

GType   peony_desktop_icon_file_get_type (void);

PeonyDesktopIconFile *peony_desktop_icon_file_new      (PeonyDesktopLink     *link);
void                     peony_desktop_icon_file_update   (PeonyDesktopIconFile *icon_file);
void                     peony_desktop_icon_file_remove   (PeonyDesktopIconFile *icon_file);
PeonyDesktopLink     *peony_desktop_icon_file_get_link (PeonyDesktopIconFile *icon_file);

#endif /* PEONY_DESKTOP_ICON_FILE_H */
