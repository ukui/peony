/*
 *  peony-property-page.h - Property pages exported by
 *                             PeonyPropertyProvider objects.
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

#ifndef PEONY_PROPERTY_PAGE_H
#define PEONY_PROPERTY_PAGE_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "peony-extension-types.h"

G_BEGIN_DECLS

#define PEONY_TYPE_PROPERTY_PAGE            (peony_property_page_get_type())
#define PEONY_PROPERTY_PAGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_PROPERTY_PAGE, PeonyPropertyPage))
#define PEONY_PROPERTY_PAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_PROPERTY_PAGE, PeonyPropertyPageClass))
#define PEONY_IS_PROPERTY_PAGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_PROPERTY_PAGE))
#define PEONY_IS_PROPERTY_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((obj), PEONY_TYPE_PROPERTY_PAGE))
#define PEONY_PROPERTY_PAGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), PEONY_TYPE_PROPERTY_PAGE, PeonyPropertyPageClass))

typedef struct _PeonyPropertyPage        PeonyPropertyPage;
typedef struct _PeonyPropertyPageDetails PeonyPropertyPageDetails;
typedef struct _PeonyPropertyPageClass   PeonyPropertyPageClass;

struct _PeonyPropertyPage {
    GObject parent;

    PeonyPropertyPageDetails *details;
};

struct _PeonyPropertyPageClass {
    GObjectClass parent;
};

GType             peony_property_page_get_type  (void);
PeonyPropertyPage *peony_property_page_new       (const char *name,
                                                GtkWidget  *label,
                                                GtkWidget  *page);

/* PeonyPropertyPage has the following properties:
 *   name (string)        - the identifier for the property page
 *   label (widget)       - the user-visible label of the property page
 *   page (widget)        - the property page to display
 */

G_END_DECLS

#endif
