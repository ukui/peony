/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
 *
 * Peony is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 *  This is the header file for the index panel widget, which displays oversidebar information
 *  in a vertical panel and hosts the meta-sidebars.
 */

#ifndef PEONY_EMBLEM_SIDEBAR_H
#define PEONY_EMBLEM_SIDEBAR_H

#include <gtk/gtk.h>

#define PEONY_TYPE_EMBLEM_SIDEBAR peony_emblem_sidebar_get_type()
#define PEONY_EMBLEM_SIDEBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_EMBLEM_SIDEBAR, PeonyEmblemSidebar))
#define PEONY_EMBLEM_SIDEBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_EMBLEM_SIDEBAR, PeonyEmblemSidebarClass))
#define PEONY_IS_EMBLEM_SIDEBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_EMBLEM_SIDEBAR))
#define PEONY_IS_EMBLEM_SIDEBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_EMBLEM_SIDEBAR))
#define PEONY_EMBLEM_SIDEBAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_EMBLEM_SIDEBAR, PeonyEmblemSidebarClass))

#define PEONY_EMBLEM_SIDEBAR_ID "emblems"

typedef struct PeonyEmblemSidebarDetails PeonyEmblemSidebarDetails;

typedef struct
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBox parent_slot;
#else
    GtkVBox parent_slot;
#endif
    PeonyEmblemSidebarDetails *details;
} PeonyEmblemSidebar;

typedef struct
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBoxClass parent_slot;
#else
    GtkVBoxClass parent_slot;
#endif

} PeonyEmblemSidebarClass;

GType	peony_emblem_sidebar_get_type     (void);
void    peony_emblem_sidebar_register     (void);

#endif /* PEONY_EMBLEM_SIDEBAR_H */
