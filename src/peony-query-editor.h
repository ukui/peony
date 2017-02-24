/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Red Hat, Inc.
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
 * Author: Alexander Larsson <alexl@redhat.com>
 *
 */

#ifndef PEONY_QUERY_EDITOR_H
#define PEONY_QUERY_EDITOR_H

#include <gtk/gtk.h>
#include <libpeony-private/peony-query.h>
#include <libpeony-private/peony-window-info.h>
#include <peony-search-bar.h>

#define PEONY_TYPE_QUERY_EDITOR peony_query_editor_get_type()
#define PEONY_QUERY_EDITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_QUERY_EDITOR, PeonyQueryEditor))
#define PEONY_QUERY_EDITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_QUERY_EDITOR, PeonyQueryEditorClass))
#define PEONY_IS_QUERY_EDITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_QUERY_EDITOR))
#define PEONY_IS_QUERY_EDITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_QUERY_EDITOR))
#define PEONY_QUERY_EDITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_QUERY_EDITOR, PeonyQueryEditorClass))

typedef struct PeonyQueryEditorDetails PeonyQueryEditorDetails;

typedef struct PeonyQueryEditor
{
    GtkBox parent;
    PeonyQueryEditorDetails *details;
} PeonyQueryEditor;

typedef struct
{
    GtkBoxClass parent_class;

    void (* changed) (PeonyQueryEditor  *editor,
                      PeonyQuery        *query,
                      gboolean              reload);
    void (* cancel)   (PeonyQueryEditor *editor);
} PeonyQueryEditorClass;

GType      peony_query_editor_get_type     	   (void);
GtkWidget* peony_query_editor_new          	   (gboolean start_hidden,
        gboolean is_indexed);
GtkWidget* peony_query_editor_new_with_bar      (gboolean start_hidden,
        gboolean is_indexed,
        gboolean start_attached,
        PeonySearchBar *bar,
        PeonyWindowSlot *slot);
void       peony_query_editor_set_default_query (PeonyQueryEditor *editor);

void	   peony_query_editor_grab_focus (PeonyQueryEditor *editor);
void       peony_query_editor_clear_query (PeonyQueryEditor *editor);

PeonyQuery *peony_query_editor_get_query   (PeonyQueryEditor *editor);
void           peony_query_editor_set_query   (PeonyQueryEditor *editor,
        PeonyQuery       *query);
void           peony_query_editor_set_visible (PeonyQueryEditor *editor,
        gboolean             visible);

#endif /* PEONY_QUERY_EDITOR_H */
