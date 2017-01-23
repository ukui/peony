/*
 *  peony-info-provider.c - Interface for Peony extensions that
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

#include <config.h>
#include "peony-info-provider.h"

#include <glib-object.h>

/**
 * SECTION:peony-info-provider
 * @title: PeonyInfoProvider
 * @short_description: Interface to provide additional information about files
 * @include: libpeony-extension/peony-column-provider.h
 *
 * #PeonyInfoProvider allows extension to provide additional information about
 * files. When peony_info_provider_update_file_info() is called by the application,
 * extensions will know that it's time to add extra information to the provided
 * #PeonyFileInfo.
 */

static void
peony_info_provider_base_init (gpointer g_class)
{
}

GType
peony_info_provider_get_type (void)
{
    static GType type = 0;

    if (!type) {
        const GTypeInfo info = {
            sizeof (PeonyInfoProviderIface),
            peony_info_provider_base_init,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE,
                                       "PeonyInfoProvider",
                                       &info, 0);
        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

PeonyOperationResult
peony_info_provider_update_file_info (PeonyInfoProvider     *provider,
                                     PeonyFileInfo         *file,
                                     GClosure             *update_complete,
                                     PeonyOperationHandle **handle)
{
    g_return_val_if_fail (PEONY_IS_INFO_PROVIDER (provider),
                          PEONY_OPERATION_FAILED);
    g_return_val_if_fail (PEONY_INFO_PROVIDER_GET_IFACE (provider)->update_file_info != NULL,
                          PEONY_OPERATION_FAILED);
    g_return_val_if_fail (update_complete != NULL,
                          PEONY_OPERATION_FAILED);
    g_return_val_if_fail (handle != NULL, PEONY_OPERATION_FAILED);

    return PEONY_INFO_PROVIDER_GET_IFACE (provider)->update_file_info
           (provider, file, update_complete, handle);
}

void
peony_info_provider_cancel_update (PeonyInfoProvider    *provider,
                                  PeonyOperationHandle *handle)
{
    g_return_if_fail (PEONY_IS_INFO_PROVIDER (provider));
    g_return_if_fail (PEONY_INFO_PROVIDER_GET_IFACE (provider)->cancel_update != NULL);
    g_return_if_fail (handle != NULL);

    PEONY_INFO_PROVIDER_GET_IFACE (provider)->cancel_update (provider,
            handle);
}

void
peony_info_provider_update_complete_invoke (GClosure            *update_complete,
                                           PeonyInfoProvider    *provider,
                                           PeonyOperationHandle *handle,
                                           PeonyOperationResult  result)
{
    GValue args[3] = { { 0, } };
    GValue return_val = { 0, };

    g_return_if_fail (update_complete != NULL);
    g_return_if_fail (PEONY_IS_INFO_PROVIDER (provider));

    g_value_init (&args[0], PEONY_TYPE_INFO_PROVIDER);
    g_value_init (&args[1], G_TYPE_POINTER);
    g_value_init (&args[2], PEONY_TYPE_OPERATION_RESULT);

    g_value_set_object (&args[0], provider);
    g_value_set_pointer (&args[1], handle);
    g_value_set_enum (&args[2], result);

    g_closure_invoke (update_complete, &return_val, 3, args, NULL);

    g_value_unset (&args[0]);
    g_value_unset (&args[1]);
    g_value_unset (&args[2]);
}

