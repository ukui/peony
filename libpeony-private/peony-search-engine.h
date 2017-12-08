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

#ifndef PEONY_SEARCH_ENGINE_H
#define PEONY_SEARCH_ENGINE_H

#include <glib-object.h>
#include <libpeony-private/peony-query.h>

#define PEONY_TYPE_SEARCH_ENGINE		(peony_search_engine_get_type ())
#define PEONY_SEARCH_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SEARCH_ENGINE, PeonySearchEngine))
#define PEONY_SEARCH_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SEARCH_ENGINE, PeonySearchEngineClass))
#define PEONY_IS_SEARCH_ENGINE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SEARCH_ENGINE))
#define PEONY_IS_SEARCH_ENGINE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SEARCH_ENGINE))
#define PEONY_SEARCH_ENGINE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SEARCH_ENGINE, PeonySearchEngineClass))

typedef struct PeonySearchEngineDetails PeonySearchEngineDetails;

typedef struct PeonySearchEngine
{
    GObject parent;
    PeonySearchEngineDetails *details;
} PeonySearchEngine;

typedef struct
{
    GObjectClass parent_class;

    /* VTable */
    void (*set_query) (PeonySearchEngine *engine, PeonyQuery *query);
    void (*start) (PeonySearchEngine *engine);
    void (*stop) (PeonySearchEngine *engine);
    gboolean (*is_indexed) (PeonySearchEngine *engine);

    /* Signals */
    void (*hits_added) (PeonySearchEngine *engine, GList *hits);
    void (*hits_subtracted) (PeonySearchEngine *engine, GList *hits);
    void (*finished) (PeonySearchEngine *engine);
    void (*error) (PeonySearchEngine *engine, const char *error_message);
} PeonySearchEngineClass;

GType          peony_search_engine_get_type  (void);
gboolean       peony_search_engine_enabled (void);

PeonySearchEngine* peony_search_engine_new       (void);
PeonySearchEngine *peony_search_duplicate_engine_new (void);
void           peony_search_engine_set_query (PeonySearchEngine *engine, PeonyQuery *query);
void	       peony_search_engine_start (PeonySearchEngine *engine);
void	       peony_search_engine_stop (PeonySearchEngine *engine);
gboolean       peony_search_engine_is_indexed (PeonySearchEngine *engine);

void	       peony_search_engine_hits_added (PeonySearchEngine *engine, GList *hits);
void	       peony_search_engine_hits_subtracted (PeonySearchEngine *engine, GList *hits);
void	       peony_search_engine_finished (PeonySearchEngine *engine);
void	       peony_search_engine_error (PeonySearchEngine *engine, const char *error_message);

#endif /* PEONY_SEARCH_ENGINE_H */
