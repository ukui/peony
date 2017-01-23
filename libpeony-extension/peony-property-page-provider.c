/*
 *  peony-property-page-provider.c - Interface for Peony extensions
 *                                      that provide property pages for
 *                                      files.
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
#include "peony-property-page-provider.h"

#include <glib-object.h>

/**
 * SECTION:peony-property-page-provider
 * @title: PeonyPropertyPageProvider
 * @short_description: Interface to provide additional property pages
 * @include: libpeony-extension/peony-property-page-provider.h
 *
 * #PeonyPropertyPageProvider allows extension to provide additional pages
 * for the file properties dialog.
 */

static void
peony_property_page_provider_base_init (gpointer g_class)
{
}

GType
peony_property_page_provider_get_type (void)
{
    static GType type = 0;

    if (!type) {
        const GTypeInfo info = {
            sizeof (PeonyPropertyPageProviderIface),
            peony_property_page_provider_base_init,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE,
                                       "PeonyPropertyPageProvider",
                                       &info, 0);
        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

/**
 * peony_property_page_provider_get_pages:
 * @provider: a #PeonyPropertyPageProvider
 * @files: (element-type PeonyFileInfo): a #GList of #PeonyFileInfo
 *
 * This function is called by Peony when it wants property page
 * items from the extension.
 *
 * This function is called in the main thread before a property page
 * is shown, so it should return quickly.
 *
 * Returns: (element-type PeonyPropertyPage) (transfer full): A #GList of allocated #PeonyPropertyPage items.
 */
GList *
peony_property_page_provider_get_pages (PeonyPropertyPageProvider *provider,
                                       GList *files)
{
    g_return_val_if_fail (PEONY_IS_PROPERTY_PAGE_PROVIDER (provider), NULL);
    g_return_val_if_fail (PEONY_PROPERTY_PAGE_PROVIDER_GET_IFACE (provider)->get_pages != NULL, NULL);

    return PEONY_PROPERTY_PAGE_PROVIDER_GET_IFACE (provider)->get_pages
           (provider, files);
}


