/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Peony
 *
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
 *
 * Author: Cosimo Cecchi <cosimoc@gnome.org>
 */

#ifndef __PEONY_CONNECT_SERVER_OPERATION_H__
#define __PEONY_CONNECT_SERVER_OPERATION_H__

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "peony-connect-server-dialog.h"

#define PEONY_TYPE_CONNECT_SERVER_OPERATION\
	(peony_connect_server_operation_get_type ())
#define PEONY_CONNECT_SERVER_OPERATION(obj)\
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
			       PEONY_TYPE_CONNECT_SERVER_OPERATION,\
			       PeonyConnectServerOperation))
#define PEONY_CONNECT_SERVER_OPERATION_CLASS(klass)\
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_CONNECT_SERVER_OPERATION,\
			    PeonyConnectServerOperationClass))
#define PEONY_IS_CONNECT_SERVER_OPERATION(obj)\
  (G_TYPE_INSTANCE_CHECK_TYPE ((obj), PEONY_TYPE_CONNECT_SERVER_OPERATION)

typedef struct _PeonyConnectServerOperationDetails
  PeonyConnectServerOperationDetails;

typedef struct {
	GtkMountOperation parent;
	PeonyConnectServerOperationDetails *details;
} PeonyConnectServerOperation;

typedef struct {
	GtkMountOperationClass parent_class;
} PeonyConnectServerOperationClass;

GType peony_connect_server_operation_get_type (void);

GMountOperation *
peony_connect_server_operation_new (PeonyConnectServerDialog *dialog);


#endif /* __PEONY_CONNECT_SERVER_OPERATION_H__ */
