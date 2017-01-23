/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2003 Ximian, Inc.
 *
 * Peony is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef PEONY_LOCATION_DIALOG_H
#define PEONY_LOCATION_DIALOG_H

#include <gtk/gtk.h>
#include "peony-window.h"

#define PEONY_TYPE_LOCATION_DIALOG         (peony_location_dialog_get_type ())
#define PEONY_LOCATION_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_LOCATION_DIALOG, PeonyLocationDialog))
#define PEONY_LOCATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_LOCATION_DIALOG, PeonyLocationDialogClass))
#define PEONY_IS_LOCATION_DIALOG(obj)      (G_TYPE_INSTANCE_CHECK_TYPE ((obj), PEONY_TYPE_LOCATION_DIALOG)

typedef struct _PeonyLocationDialog        PeonyLocationDialog;
typedef struct _PeonyLocationDialogClass   PeonyLocationDialogClass;
typedef struct _PeonyLocationDialogDetails PeonyLocationDialogDetails;

struct _PeonyLocationDialog
{
    GtkDialog parent;
    PeonyLocationDialogDetails *details;
};

struct _PeonyLocationDialogClass
{
    GtkDialogClass parent_class;
};

GType      peony_location_dialog_get_type     (void);
GtkWidget* peony_location_dialog_new          (PeonyWindow         *window);
void       peony_location_dialog_set_location (PeonyLocationDialog *dialog,
        const char             *location);

#endif /* PEONY_LOCATION_DIALOG_H */
