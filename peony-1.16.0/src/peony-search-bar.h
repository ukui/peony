/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Novell, Inc.
 *
 * Peony is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Anders Carlsson <andersca@imendio.com>
 *
 */

#ifndef PEONY_SEARCH_BAR_H
#define PEONY_SEARCH_BAR_H

#include <gtk/gtk.h>
#include <libpeony-private/peony-query.h>

#define PEONY_TYPE_SEARCH_BAR peony_search_bar_get_type()
#define PEONY_SEARCH_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SEARCH_BAR, PeonySearchBar))
#define PEONY_SEARCH_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SEARCH_BAR, PeonySearchBarClass))
#define PEONY_IS_SEARCH_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SEARCH_BAR))
#define PEONY_IS_SEARCH_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SEARCH_BAR))
#define PEONY_SEARCH_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SEARCH_BAR, PeonySearchBarClass))

typedef struct PeonySearchBarDetails PeonySearchBarDetails;

typedef struct PeonySearchBar
{
    GtkEventBox parent;
    PeonySearchBarDetails *details;
} PeonySearchBar;

typedef struct
{
    GtkEventBoxClass parent_class;

    void (* activate) (PeonySearchBar *bar);
    void (* cancel)   (PeonySearchBar *bar);
    void (* focus_in) (PeonySearchBar *bar);
} PeonySearchBarClass;

GType      peony_search_bar_get_type     	(void);
GtkWidget* peony_search_bar_new          	(void);

GtkWidget *    peony_search_bar_borrow_entry  (PeonySearchBar *bar);
void           peony_search_bar_return_entry  (PeonySearchBar *bar);
void           peony_search_bar_grab_focus    (PeonySearchBar *bar);
PeonyQuery *peony_search_bar_get_query     (PeonySearchBar *bar);
void           peony_search_bar_clear         (PeonySearchBar *bar);

#endif /* PEONY_SEARCH_BAR_H */
