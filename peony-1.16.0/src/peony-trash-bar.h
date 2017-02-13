/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2006 Paolo Borelli <pborelli@katamail.com>
 *
 * This program is free software; you can redistribute it and/or modify
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Authors: Paolo Borelli <pborelli@katamail.com>
 *
 */

#ifndef __PEONY_TRASH_BAR_H
#define __PEONY_TRASH_BAR_H

#include "peony-window.h"

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEONY_TYPE_TRASH_BAR         (peony_trash_bar_get_type ())
#define PEONY_TRASH_BAR(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_TRASH_BAR, PeonyTrashBar))
#define PEONY_TRASH_BAR_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_TRASH_BAR, PeonyTrashBarClass))
#define PEONY_IS_TRASH_BAR(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_TRASH_BAR))
#define PEONY_IS_TRASH_BAR_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_TRASH_BAR))
#define PEONY_TRASH_BAR_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_TRASH_BAR, PeonyTrashBarClass))

    typedef struct PeonyTrashBarPrivate PeonyTrashBarPrivate;

    typedef struct
    {
#if GTK_CHECK_VERSION (3, 0, 0)
        GtkBox	box;
#else
        GtkHBox	box;
#endif

        PeonyTrashBarPrivate *priv;
    } PeonyTrashBar;

    typedef struct
    {
#if GTK_CHECK_VERSION (3, 0, 0)
        GtkBoxClass	    parent_class;
#else
        GtkHBoxClass	    parent_class;
#endif
    } PeonyTrashBarClass;

    GType		 peony_trash_bar_get_type	(void) G_GNUC_CONST;

    GtkWidget       *peony_trash_bar_new         (PeonyWindow *window);


#ifdef __cplusplus
}
#endif

#endif /* __GS_TRASH_BAR_H */
