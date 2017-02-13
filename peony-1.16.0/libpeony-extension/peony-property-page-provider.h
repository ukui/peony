/*
 *  peony-property-page-provider.h - Interface for Peony extensions
 *                                      that provide property pages.
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
 * add property page to property dialogs.  Extensions are called when
 * Peony needs property pages for a selection.  They are passed a
 * list of PeonyFileInfo objects for which information should
 * be displayed  */

#ifndef PEONY_PROPERTY_PAGE_PROVIDER_H
#define PEONY_PROPERTY_PAGE_PROVIDER_H

#include <glib-object.h>
#include "peony-extension-types.h"
#include "peony-file-info.h"
#include "peony-property-page.h"

G_BEGIN_DECLS

#define PEONY_TYPE_PROPERTY_PAGE_PROVIDER           (peony_property_page_provider_get_type ())
#define PEONY_PROPERTY_PAGE_PROVIDER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_PROPERTY_PAGE_PROVIDER, PeonyPropertyPageProvider))
#define PEONY_IS_PROPERTY_PAGE_PROVIDER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_PROPERTY_PAGE_PROVIDER))
#define PEONY_PROPERTY_PAGE_PROVIDER_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), PEONY_TYPE_PROPERTY_PAGE_PROVIDER, PeonyPropertyPageProviderIface))

typedef struct _PeonyPropertyPageProvider       PeonyPropertyPageProvider;
typedef struct _PeonyPropertyPageProviderIface  PeonyPropertyPageProviderIface;

/**
 * PeonyPropertyPageProviderIface:
 * @g_iface: The parent interface.
 * @get_pages: Returns a #GList of #PeonyPropertyPage.
 *   See peony_property_page_provider_get_pages() for details.
 *
 * Interface for extensions to provide additional property pages.
 */

struct _PeonyPropertyPageProviderIface {
    GTypeInterface g_iface;

    GList *(*get_pages) (PeonyPropertyPageProvider *provider,
                         GList                    *files);
};

/* Interface Functions */
GType  peony_property_page_provider_get_type  (void);
GList *peony_property_page_provider_get_pages (PeonyPropertyPageProvider *provider,
                                              GList                    *files);

G_END_DECLS

#endif
