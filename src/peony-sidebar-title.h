/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Peony is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 */

/*
 * This is the header file for the sidebar title, which is part of the sidebar.
 */

#ifndef PEONY_SIDEBAR_TITLE_H
#define PEONY_SIDEBAR_TITLE_H

#include <gtk/gtk.h>
#include <eel/eel-background.h>
#include <libpeony-private/peony-file.h>

#define PEONY_TYPE_SIDEBAR_TITLE peony_sidebar_title_get_type()
#define PEONY_SIDEBAR_TITLE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SIDEBAR_TITLE, PeonySidebarTitle))
#define PEONY_SIDEBAR_TITLE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SIDEBAR_TITLE, PeonySidebarTitleClass))
#define PEONY_IS_SIDEBAR_TITLE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SIDEBAR_TITLE))
#define PEONY_IS_SIDEBAR_TITLE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SIDEBAR_TITLE))
#define PEONY_SIDEBAR_TITLE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SIDEBAR_TITLE, PeonySidebarTitleClass))

typedef struct PeonySidebarTitleDetails PeonySidebarTitleDetails;

typedef struct
{
    GtkBox box;
    PeonySidebarTitleDetails *details;
} PeonySidebarTitle;

typedef struct
{
    GtkBoxClass parent_class;
} PeonySidebarTitleClass;

GType      peony_sidebar_title_get_type          (void);
GtkWidget *peony_sidebar_title_new               (void);
void       peony_sidebar_title_set_file          (PeonySidebarTitle *sidebar_title,
        PeonyFile         *file,
        const char           *initial_text);
void       peony_sidebar_title_set_text          (PeonySidebarTitle *sidebar_title,
        const char           *new_title);
char *     peony_sidebar_title_get_text          (PeonySidebarTitle *sidebar_title);
gboolean   peony_sidebar_title_hit_test_icon     (PeonySidebarTitle *sidebar_title,
        int                   x,
        int                   y);
void       peony_sidebar_title_select_text_color (PeonySidebarTitle *sidebar_title,
        					 EelBackground        *background);

#endif /* PEONY_SIDEBAR_TITLE_H */
