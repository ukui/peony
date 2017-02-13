/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
   peony-open-with-dialog.c: an open-with dialog

   Copyright (C) 2004 Novell, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Dave Camp <dave@novell.com>
*/

#ifndef PEONY_OPEN_WITH_DIALOG_H
#define PEONY_OPEN_WITH_DIALOG_H

#include <gtk/gtk.h>
#include <gio/gio.h>

#define PEONY_TYPE_OPEN_WITH_DIALOG         (peony_open_with_dialog_get_type ())
#define PEONY_OPEN_WITH_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_OPEN_WITH_DIALOG, PeonyOpenWithDialog))
#define PEONY_OPEN_WITH_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_OPEN_WITH_DIALOG, PeonyOpenWithDialogClass))
#define PEONY_IS_OPEN_WITH_DIALOG(obj)      (G_TYPE_INSTANCE_CHECK_TYPE ((obj), PEONY_TYPE_OPEN_WITH_DIALOG)

typedef struct _PeonyOpenWithDialog        PeonyOpenWithDialog;
typedef struct _PeonyOpenWithDialogClass   PeonyOpenWithDialogClass;
typedef struct _PeonyOpenWithDialogDetails PeonyOpenWithDialogDetails;

struct _PeonyOpenWithDialog
{
    GtkDialog parent;
    PeonyOpenWithDialogDetails *details;
};

struct _PeonyOpenWithDialogClass
{
    GtkDialogClass parent_class;

    void (*application_selected) (PeonyOpenWithDialog *dialog,
                                  GAppInfo *application);
};

GType      peony_open_with_dialog_get_type (void);
GtkWidget* peony_open_with_dialog_new      (const char *uri,
        const char *mime_type,
        const char *extension);
GtkWidget* peony_add_application_dialog_new (const char *uri,
        const char *mime_type);
GtkWidget* peony_add_application_dialog_new_for_multiple_files (const char *extension,
        const char *mime_type);



#endif /* PEONY_OPEN_WITH_DIALOG_H */
