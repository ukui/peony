/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
   peony-mime-application-chooser.c: Manages applications for mime types

   Copyright (C) 2004 Novell, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but APPLICATIONOUT ANY WARRANTY; applicationout even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along application the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Dave Camp <dave@novell.com>
*/

#ifndef PEONY_MIME_APPLICATION_CHOOSER_H
#define PEONY_MIME_APPLICATION_CHOOSER_H

#include <gtk/gtk.h>

#define PEONY_TYPE_MIME_APPLICATION_CHOOSER         (peony_mime_application_chooser_get_type ())
#define PEONY_MIME_APPLICATION_CHOOSER(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_MIME_APPLICATION_CHOOSER, PeonyMimeApplicationChooser))
#define PEONY_MIME_APPLICATION_CHOOSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_MIME_APPLICATION_CHOOSER, PeonyMimeApplicationChooserClass))
#define PEONY_IS_MIME_APPLICATION_CHOOSER(obj)      (G_TYPE_INSTANCE_CHECK_TYPE ((obj), PEONY_TYPE_MIME_APPLICATION_CHOOSER)

typedef struct _PeonyMimeApplicationChooser        PeonyMimeApplicationChooser;
typedef struct _PeonyMimeApplicationChooserClass   PeonyMimeApplicationChooserClass;
typedef struct _PeonyMimeApplicationChooserDetails PeonyMimeApplicationChooserDetails;

struct _PeonyMimeApplicationChooser
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBox parent;
#else
    GtkVBox parent;
#endif
    PeonyMimeApplicationChooserDetails *details;
};

struct _PeonyMimeApplicationChooserClass
{
#if GTK_CHECK_VERSION (3, 0, 0)
    GtkBoxClass parent_class;
#else
    GtkVBoxClass parent_class;
#endif
};

GType      peony_mime_application_chooser_get_type (void);
GtkWidget* peony_mime_application_chooser_new      (const char *uri,
        const char *mime_type);
GtkWidget* peony_mime_application_chooser_new_for_multiple_files (GList *uris,
        const char *mime_type);

#endif /* PEONY_MIME_APPLICATION_CHOOSER_H */
