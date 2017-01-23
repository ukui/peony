/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Red Hat, Inc.
 *
 * Peony is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/* peony-application.h
 */

#ifndef PEONY_APPLICATION_H
#define PEONY_APPLICATION_H

#include <gdk/gdk.h>
#include <gio/gio.h>
#include <unique/unique.h>
#include <libegg/eggsmclient.h>

#define PEONY_DESKTOP_ICON_VIEW_IID "OAFIID:Peony_File_Manager_Desktop_Icon_View"

#define PEONY_TYPE_APPLICATION \
	peony_application_get_type()
#define PEONY_APPLICATION(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), PEONY_TYPE_APPLICATION, PeonyApplication))
#define PEONY_APPLICATION_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass), PEONY_TYPE_APPLICATION, PeonyApplicationClass))
#define PEONY_IS_APPLICATION(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), PEONY_TYPE_APPLICATION))
#define PEONY_IS_APPLICATION_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass), PEONY_TYPE_APPLICATION))
#define PEONY_APPLICATION_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS((obj), PEONY_TYPE_APPLICATION, PeonyApplicationClass))

#ifndef PEONY_WINDOW_DEFINED
#define PEONY_WINDOW_DEFINED
typedef struct PeonyWindow PeonyWindow;
#endif

#ifndef PEONY_SPATIAL_WINDOW_DEFINED
#define PEONY_SPATIAL_WINDOW_DEFINED
typedef struct _PeonySpatialWindow PeonySpatialWindow;
#endif

typedef struct PeonyShell PeonyShell;

typedef struct
{
    GObject parent;
    UniqueApp* unique_app;
    EggSMClient* smclient;
    GVolumeMonitor* volume_monitor;
    unsigned int automount_idle_id;
    gboolean screensaver_active;
    guint ss_watch_id;
    GDBusProxy *ss_proxy;
    GList *volume_queue;
} PeonyApplication;

typedef struct
{
    GObjectClass parent_class;
} PeonyApplicationClass;

GType                peony_application_get_type          (void);
PeonyApplication *peony_application_new               (void);
void                 peony_application_startup           (PeonyApplication *application,
        gboolean             kill_shell,
        gboolean             no_default_window,
        gboolean             no_desktop,
        gboolean             browser_window,
        const char          *default_geometry,
        char               **urls);
GList *              peony_application_get_window_list           (void);
GList *              peony_application_get_spatial_window_list    (void);
unsigned int         peony_application_get_n_windows            (void);

PeonyWindow *     peony_application_get_spatial_window     (PeonyApplication *application,
        PeonyWindow      *requesting_window,
        const char      *startup_id,
        GFile           *location,
        GdkScreen       *screen,
        gboolean        *existing);

PeonyWindow *     peony_application_create_navigation_window     (PeonyApplication *application,
        const char          *startup_id,
        GdkScreen           *screen);

void peony_application_close_all_navigation_windows (void);
void peony_application_close_parent_windows     (PeonySpatialWindow *window);
void peony_application_close_all_spatial_windows  (void);
void peony_application_open_desktop      (PeonyApplication *application);
void peony_application_close_desktop     (void);
gboolean peony_application_save_accel_map    (gpointer data);
void peony_application_open_location (PeonyApplication *application,
        GFile *location,
        GFile *selection,
        const char *startup_id);

#endif /* PEONY_APPLICATION_H */
