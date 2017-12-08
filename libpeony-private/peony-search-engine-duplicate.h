/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Red Hat, Inc
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

#ifndef PEONY_SEARCH_ENGINE_DUPLICATE_H
#define PEONY_SEARCH_ENGINE_DUPLICATE_H

#include <libpeony-private/peony-search-engine.h>

#define PEONY_TYPE_SEARCH_ENGINE_DUPLICATE		(peony_search_engine_duplicate_get_type ())
#define PEONY_SEARCH_ENGINE_DUPLICATE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SEARCH_ENGINE_DUPLICATE, PeonySearchEngineDuplicate))
#define PEONY_SEARCH_ENGINE_DUPLICATE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_SEARCH_ENGINE_DUPLICATE, PeonySearchEngineDuplicateClass))
#define PEONY_IS_SEARCH_ENGINE_DUPLICATE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SEARCH_ENGINE_DUPLICATE))
#define PEONY_IS_SEARCH_ENGINE_DUPLICATE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_SEARCH_ENGINE_DUPLICATE))
#define PEONY_SEARCH_ENGINE_DUPLICATE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_SEARCH_ENGINE_DUPLICATE, PeonySearchEngineDuplicateClass))

typedef struct PeonySearchEngineDuplicateDetails PeonySearchEngineDuplicateDetails;

typedef struct PeonySearchEngineDuplicate
{
    PeonySearchEngine parent;
    PeonySearchEngineDuplicateDetails *details;
} PeonySearchEngineDuplicate;

typedef struct
{
    PeonySearchEngineClass parent_class;
} PeonySearchEngineDuplicateClass;

GType          peony_search_engine_duplicate_get_type  (void);

PeonySearchEngine* peony_search_engine_duplicate_new       (void);

#endif /* PEONY_SEARCH_ENGINE_SIMPLE_H */
