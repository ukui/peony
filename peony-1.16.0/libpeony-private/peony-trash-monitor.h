/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
   peony-trash-monitor.h: Peony trash state watcher.

   Copyright (C) 2000 Eazel, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Author: Pavel Cisler <pavel@eazel.com>
*/

#ifndef PEONY_TRASH_MONITOR_H
#define PEONY_TRASH_MONITOR_H

#include <gtk/gtk.h>
#include <gio/gio.h>

typedef struct PeonyTrashMonitor PeonyTrashMonitor;
typedef struct PeonyTrashMonitorClass PeonyTrashMonitorClass;
typedef struct PeonyTrashMonitorDetails PeonyTrashMonitorDetails;

#define PEONY_TYPE_TRASH_MONITOR peony_trash_monitor_get_type()
#define PEONY_TRASH_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_TRASH_MONITOR, PeonyTrashMonitor))
#define PEONY_TRASH_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_TRASH_MONITOR, PeonyTrashMonitorClass))
#define PEONY_IS_TRASH_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_TRASH_MONITOR))
#define PEONY_IS_TRASH_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_TRASH_MONITOR))
#define PEONY_TRASH_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_TRASH_MONITOR, PeonyTrashMonitorClass))

struct PeonyTrashMonitor
{
    GObject object;
    PeonyTrashMonitorDetails *details;
};

struct PeonyTrashMonitorClass
{
    GObjectClass parent_class;

    void (* trash_state_changed)		(PeonyTrashMonitor 	*trash_monitor,
                                         gboolean 		 new_state);
};

GType			peony_trash_monitor_get_type				(void);

PeonyTrashMonitor   *peony_trash_monitor_get 				(void);
gboolean		peony_trash_monitor_is_empty 			(void);
GIcon                  *peony_trash_monitor_get_icon                         (void);

#endif
