/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Copyright (C) 2004 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 */

#ifndef PEONY_IMAGE_PROPERTIES_PAGE_H
#define PEONY_IMAGE_PROPERTIES_PAGE_H

#include <gtk/gtk.h>

#define PEONY_TYPE_IMAGE_PROPERTIES_PAGE peony_image_properties_page_get_type()
#define PEONY_IMAGE_PROPERTIES_PAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_IMAGE_PROPERTIES_PAGE, PeonyImagePropertiesPage))
#define PEONY_IMAGE_PROPERTIES_PAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_IMAGE_PROPERTIES_PAGE, PeonyImagePropertiesPageClass))
#define PEONY_IS_IMAGE_PROPERTIES_PAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_IMAGE_PROPERTIES_PAGE))
#define PEONY_IS_IMAGE_PROPERTIES_PAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_IMAGE_PROPERTIES_PAGE))
#define PEONY_IMAGE_PROPERTIES_PAGE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_IMAGE_PROPERTIES_PAGE, PeonyImagePropertiesPageClass))

typedef struct PeonyImagePropertiesPageDetails PeonyImagePropertiesPageDetails;

typedef struct
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBox parent;
#else
    GtkVBox parent;
#endif
    PeonyImagePropertiesPageDetails *details;
} PeonyImagePropertiesPage;

typedef struct
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBoxClass parent;
#else
    GtkVBoxClass parent;
#endif
} PeonyImagePropertiesPageClass;

GType peony_image_properties_page_get_type (void);
void  peony_image_properties_page_register (void);

#endif /* PEONY_IMAGE_PROPERTIES_PAGE_H */
