/*
 *  peony-info-provider.h - Interface for Peony extensions that
 *                             provide info about files.
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
 * provide information about files.  Extensions are called when Peony
 * needs information about a file.  They are passed a PeonyFileInfo
 * object which should be filled with relevant information */

#ifndef PEONY_INFO_PROVIDER_H
#define PEONY_INFO_PROVIDER_H

#include <glib-object.h>
#include "peony-extension-types.h"
#include "peony-file-info.h"

G_BEGIN_DECLS

#define PEONY_TYPE_INFO_PROVIDER           (peony_info_provider_get_type ())
#define PEONY_INFO_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_INFO_PROVIDER, PeonyInfoProvider))
#define PEONY_IS_INFO_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_INFO_PROVIDER))
#define PEONY_INFO_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), PEONY_TYPE_INFO_PROVIDER, PeonyInfoProviderIface))

typedef struct _PeonyInfoProvider       PeonyInfoProvider;
typedef struct _PeonyInfoProviderIface  PeonyInfoProviderIface;

typedef void (*PeonyInfoProviderUpdateComplete) (PeonyInfoProvider    *provider,
                                                PeonyOperationHandle *handle,
                                                PeonyOperationResult  result,
                                                gpointer             user_data);

/**
 * PeonyInfoProviderIface:
 * @g_iface: The parent interface.
 * @update_file_info: Returns a #PeonyOperationResult.
 *   See peony_info_provider_update_file_info() for details.
 * @cancel_update: Cancels a previous call to peony_info_provider_update_file_info().
 *   See peony_info_provider_cancel_update() for details.
 *
 * Interface for extensions to provide additional information about files.
 */

struct _PeonyInfoProviderIface {
    GTypeInterface g_iface;

    PeonyOperationResult (*update_file_info) (PeonyInfoProvider     *provider,
                                             PeonyFileInfo         *file,
                                             GClosure             *update_complete,
                                             PeonyOperationHandle **handle);
    void                (*cancel_update)    (PeonyInfoProvider     *provider,
                                             PeonyOperationHandle  *handle);
};

/* Interface Functions */
GType               peony_info_provider_get_type               (void);
PeonyOperationResult peony_info_provider_update_file_info       (PeonyInfoProvider     *provider,
                                                               PeonyFileInfo         *file,
                                                               GClosure             *update_complete,
                                                               PeonyOperationHandle **handle);
void                peony_info_provider_cancel_update          (PeonyInfoProvider     *provider,
                                                               PeonyOperationHandle  *handle);



/* Helper functions for implementations */
void                peony_info_provider_update_complete_invoke (GClosure             *update_complete,
                                                               PeonyInfoProvider     *provider,
                                                               PeonyOperationHandle  *handle,
                                                               PeonyOperationResult   result);

G_END_DECLS

#endif
