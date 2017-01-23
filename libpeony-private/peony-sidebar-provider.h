/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-sidebar-provider.h: register and create PeonySidebars

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

#ifndef PEONY_SIDEBAR_PROVIDER_H
#define PEONY_SIDEBAR_PROVIDER_H

#include <libpeony-private/peony-sidebar.h>
#include <libpeony-private/peony-window-info.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEONY_TYPE_SIDEBAR_PROVIDER           (peony_sidebar_provider_get_type ())
#define PEONY_SIDEBAR_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_SIDEBAR_PROVIDER, PeonySidebarProvider))
#define PEONY_IS_SIDEBAR_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_SIDEBAR_PROVIDER))
#define PEONY_SIDEBAR_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), PEONY_TYPE_SIDEBAR_PROVIDER, PeonySidebarProviderIface))

    typedef struct _PeonySidebarProvider       PeonySidebarProvider;
    typedef struct _PeonySidebarProviderIface  PeonySidebarProviderIface;

    struct _PeonySidebarProviderIface
    {
        GTypeInterface g_iface;

        PeonySidebar * (*create) (PeonySidebarProvider *provider,
                                 PeonyWindowInfo *window);
    };

    /* Interface Functions */
    GType                   peony_sidebar_provider_get_type  (void);
    PeonySidebar *       peony_sidebar_provider_create (PeonySidebarProvider *provider,
            PeonyWindowInfo  *window);
    GList *                 peony_list_sidebar_providers (void);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_SIDEBAR_PROVIDER_H */
