/*
 *  peony-column-provider.h - Interface for Peony extensions that
 *                               provide column descriptions.
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author:  Dave Camp <dave@ximian.com>
 *
 */

/* This interface is implemented by Peony extensions that want to
 * add columns to the list view and details to the icon view.
 * Extensions are asked for a list of columns to display.  Each
 * returned column refers to a string attribute which can be filled in
 * by PeonyInfoProvider */

#ifndef PEONY_COLUMN_PROVIDER_H
#define PEONY_COLUMN_PROVIDER_H

#include <glib-object.h>
#include "peony-extension-types.h"
#include "peony-column.h"

G_BEGIN_DECLS

#define PEONY_TYPE_COLUMN_PROVIDER           (peony_column_provider_get_type ())
#define PEONY_COLUMN_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_COLUMN_PROVIDER, PeonyColumnProvider))
#define PEONY_IS_COLUMN_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_COLUMN_PROVIDER))
#define PEONY_COLUMN_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), PEONY_TYPE_COLUMN_PROVIDER, PeonyColumnProviderIface))

typedef struct _PeonyColumnProvider       PeonyColumnProvider;
typedef struct _PeonyColumnProviderIface  PeonyColumnProviderIface;

/**
 * PeonyColumnProviderIface:
 * @g_iface: The parent interface.
 * @get_columns: Returns a #GList of #PeonyColumn.
 *   See peony_column_provider_get_columns() for details.
 *
 * Interface for extensions to provide additional list view columns.
 */

struct _PeonyColumnProviderIface {
    GTypeInterface g_iface;

    GList *(*get_columns) (PeonyColumnProvider *provider);
};

/* Interface Functions */
GType  peony_column_provider_get_type    (void);
GList *peony_column_provider_get_columns (PeonyColumnProvider *provider);

G_END_DECLS

#endif
