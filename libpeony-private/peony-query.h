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

#ifndef PEONY_QUERY_H
#define PEONY_QUERY_H

#include <glib-object.h>

#define PEONY_TYPE_QUERY		(peony_query_get_type ())
#define PEONY_QUERY(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_QUERY, PeonyQuery))
#define PEONY_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_QUERY, PeonyQueryClass))
#define PEONY_IS_QUERY(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_QUERY))
#define PEONY_IS_QUERY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_QUERY))
#define PEONY_QUERY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_QUERY, PeonyQueryClass))

typedef struct PeonyQueryDetails PeonyQueryDetails;

typedef struct PeonyQuery
{
    GObject parent;
    PeonyQueryDetails *details;
} PeonyQuery;

typedef struct
{
    GObjectClass parent_class;
} PeonyQueryClass;

GType          peony_query_get_type (void);
gboolean       peony_query_enabled  (void);

PeonyQuery* peony_query_new      (void);

char *         peony_query_get_text           (PeonyQuery *query);
void           peony_query_set_text           (PeonyQuery *query, const char *text);

char *         peony_query_get_location       (PeonyQuery *query);
void           peony_query_set_location       (PeonyQuery *query, const char *uri);

GList *        peony_query_get_mime_types     (PeonyQuery *query);
void           peony_query_set_mime_types     (PeonyQuery *query, GList *mime_types);
void           peony_query_add_mime_type      (PeonyQuery *query, const char *mime_type);

char *         peony_query_to_readable_string (PeonyQuery *query);
PeonyQuery *peony_query_load               (char *file);
gboolean       peony_query_save               (PeonyQuery *query, char *file);

#endif /* PEONY_QUERY_H */
