/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-mime-actions.h - uri-specific versions of mime action functions

   Copyright (C) 2000 Eazel, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Maciej Stachowiak <mjs@eazel.com>
*/

#ifndef PEONY_MIME_ACTIONS_H
#define PEONY_MIME_ACTIONS_H

#include <gio/gio.h>

#include <libpeony-private/peony-file.h>
#include <libpeony-private/peony-window-info.h>
#include <libpeony-private/peony-window-slot-info.h>

PeonyFileAttributes peony_mime_actions_get_required_file_attributes (void);

GAppInfo *             peony_mime_get_default_application_for_file     (PeonyFile            *file);
GList *                peony_mime_get_applications_for_file            (PeonyFile            *file);

GAppInfo *             peony_mime_get_default_application_for_files    (GList                   *files);
GList *                peony_mime_get_applications_for_files           (GList                   *file);

gboolean               peony_mime_has_any_applications_for_file        (PeonyFile            *file);

gboolean               peony_mime_file_opens_in_view                   (PeonyFile            *file);
gboolean               peony_mime_file_opens_in_external_app           (PeonyFile            *file);
void                   peony_mime_activate_files                       (GtkWindow               *parent_window,
        PeonyWindowSlotInfo  *slot_info,
        GList                   *files,
        const char              *launch_directory,
        PeonyWindowOpenMode   mode,
        PeonyWindowOpenFlags  flags,
        gboolean                 user_confirmation);
void                   peony_mime_activate_file                        (GtkWindow               *parent_window,
        PeonyWindowSlotInfo  *slot_info,
        PeonyFile            *file,
        const char              *launch_directory,
        PeonyWindowOpenMode   mode,
        PeonyWindowOpenFlags  flags);


#endif /* PEONY_MIME_ACTIONS_H */
