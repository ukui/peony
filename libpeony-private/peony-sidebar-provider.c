/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-sidebar-provider.c: register and create PeonySidebars

   Copyright (C) 2004 Red Hat Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Author: Alexander Larsson <alexl@redhat.com>
*/

#include <string.h>
#include "peony-sidebar-provider.h"

static void
peony_sidebar_provider_base_init (gpointer g_class)
{
}

GType
peony_sidebar_provider_get_type (void)
{
    static GType type = 0;

    if (!type)
    {
        const GTypeInfo info =
        {
            sizeof (PeonySidebarProviderIface),
            peony_sidebar_provider_base_init,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE,
                                       "PeonySidebarProvider",
                                       &info, 0);
        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

PeonySidebar *
peony_sidebar_provider_create (PeonySidebarProvider *provider,
                              PeonyWindowInfo  *window)
{
    return (* PEONY_SIDEBAR_PROVIDER_GET_IFACE (provider)->create) (provider, window);
}


GList *
peony_list_sidebar_providers (void)
{
    return NULL;
}
