/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Peony
 *
 * Copyright (C) 2003 Red Hat, Inc.
 * Copyright (C) 2010 Cosimo Cecchi <cosimoc@gnome.org>
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

#ifndef PEONY_CONNECT_SERVER_DIALOG_H
#define PEONY_CONNECT_SERVER_DIALOG_H

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "peony-window.h"

#define PEONY_TYPE_CONNECT_SERVER_DIALOG\
	(peony_connect_server_dialog_get_type ())
#define PEONY_CONNECT_SERVER_DIALOG(obj)\
        (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_CONNECT_SERVER_DIALOG,\
				     PeonyConnectServerDialog))
#define PEONY_CONNECT_SERVER_DIALOG_CLASS(klass)\
	(G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_CONNECT_SERVER_DIALOG,\
				  PeonyConnectServerDialogClass))
#define PEONY_IS_CONNECT_SERVER_DIALOG(obj)\
	(G_TYPE_INSTANCE_CHECK_TYPE ((obj), PEONY_TYPE_CONNECT_SERVER_DIALOG)

typedef struct _PeonyConnectServerDialog PeonyConnectServerDialog;
typedef struct _PeonyConnectServerDialogClass PeonyConnectServerDialogClass;
typedef struct _PeonyConnectServerDialogDetails PeonyConnectServerDialogDetails;

struct _PeonyConnectServerDialog
{
    GtkDialog parent;
    PeonyConnectServerDialogDetails *details;
};

struct _PeonyConnectServerDialogClass
{
    GtkDialogClass parent_class;
};

GType peony_connect_server_dialog_get_type (void);

GtkWidget* peony_connect_server_dialog_new (PeonyWindow *window);

void peony_connect_server_dialog_display_location_async (PeonyConnectServerDialog *self,
							    PeonyApplication *application,
							    GFile *location,
							    GAsyncReadyCallback callback,
							    gpointer user_data);
gboolean peony_connect_server_dialog_display_location_finish (PeonyConnectServerDialog *self,
								 GAsyncResult *result,
								 GError **error);

void peony_connect_server_dialog_fill_details_async (PeonyConnectServerDialog *self,
							GMountOperation *operation,
							const gchar *default_user,
							const gchar *default_domain,
							GAskPasswordFlags flags,
							GAsyncReadyCallback callback,
							gpointer user_data);
gboolean peony_connect_server_dialog_fill_details_finish (PeonyConnectServerDialog *self,
							     GAsyncResult *result);

#endif /* PEONY_CONNECT_SERVER_DIALOG_H */
