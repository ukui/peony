/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-ui-utilities.h - helper functions for GtkUIManager stuff

   Copyright (C) 2004 Red Hat, Inc.

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

   Authors: Alexander Larsson <alexl@redhat.com>
*/
#ifndef PEONY_UI_UTILITIES_H
#define PEONY_UI_UTILITIES_H

#include <gtk/gtk.h>
#include <libpeony-extension/peony-menu-item.h>

char *      peony_get_ui_directory              (void);
char *      peony_ui_file                       (const char        *partial_path);
void        peony_ui_unmerge_ui                 (GtkUIManager      *ui_manager,
        guint             *merge_id,
        GtkActionGroup   **action_group);
void        peony_ui_prepare_merge_ui           (GtkUIManager      *ui_manager,
        const char        *name,
        guint             *merge_id,
        GtkActionGroup   **action_group);
GtkAction * peony_action_from_menu_item         (PeonyMenuItem  *item);
GtkAction * peony_toolbar_action_from_menu_item (PeonyMenuItem  *item);
const char *peony_ui_string_get                 (const char        *filename);
void   peony_ui_frame_image                     (GdkPixbuf        **pixbuf);

#endif /* PEONY_UI_UTILITIES_H */
