/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Peony is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 */

/* This is the header file for the property browser window, which
 * gives the user access to an extensible palette of properties which
 * can be dropped on various elements of the user interface to
 * customize them
 */

#ifndef PEONY_PROPERTY_BROWSER_H
#define PEONY_PROPERTY_BROWSER_H

#include <gdk/gdk.h>
#include <gtk/gtk.h>

typedef struct PeonyPropertyBrowser PeonyPropertyBrowser;
typedef struct PeonyPropertyBrowserClass  PeonyPropertyBrowserClass;

#define PEONY_TYPE_PROPERTY_BROWSER peony_property_browser_get_type()
#define PEONY_PROPERTY_BROWSER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_PROPERTY_BROWSER, PeonyPropertyBrowser))
#define PEONY_PROPERTY_BROWSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_PROPERTY_BROWSER, PeonyPropertyBrowserClass))
#define PEONY_IS_PROPERTY_BROWSER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_PROPERTY_BROWSER))
#define PEONY_IS_PROPERTY_BROWSER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_PROPERTY_BROWSER))
#define PEONY_PROPERTY_BROWSER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_PROPERTY_BROWSER, PeonyPropertyBrowserClass))

typedef struct PeonyPropertyBrowserDetails PeonyPropertyBrowserDetails;

struct PeonyPropertyBrowser
{
    GtkWindow window;
    PeonyPropertyBrowserDetails *details;
};

struct PeonyPropertyBrowserClass
{
    GtkWindowClass parent_class;
};

GType                    peony_property_browser_get_type (void);
PeonyPropertyBrowser *peony_property_browser_new      (GdkScreen               *screen);
void                     peony_property_browser_show     (GdkScreen               *screen);
void                     peony_property_browser_set_path (PeonyPropertyBrowser *panel,
        const char              *new_path);

#endif /* PEONY_PROPERTY_BROWSER_H */
