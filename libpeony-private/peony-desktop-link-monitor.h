/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-desktop-link-monitor.h: singleton that manages the desktop links

   Copyright (C) 2003 Red Hat, Inc.

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

   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef PEONY_DESKTOP_LINK_MONITOR_H
#define PEONY_DESKTOP_LINK_MONITOR_H

#include <gtk/gtk.h>
#include <libpeony-private/peony-desktop-link.h>

#define PEONY_TYPE_DESKTOP_LINK_MONITOR peony_desktop_link_monitor_get_type()
#define PEONY_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_DESKTOP_LINK_MONITOR, PeonyDesktopLinkMonitor))
#define PEONY_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_DESKTOP_LINK_MONITOR, PeonyDesktopLinkMonitorClass))
#define PEONY_IS_DESKTOP_LINK_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_DESKTOP_LINK_MONITOR))
#define PEONY_IS_DESKTOP_LINK_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_DESKTOP_LINK_MONITOR))
#define PEONY_DESKTOP_LINK_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_DESKTOP_LINK_MONITOR, PeonyDesktopLinkMonitorClass))

typedef struct PeonyDesktopLinkMonitorDetails PeonyDesktopLinkMonitorDetails;

typedef struct
{
    GObject parent_slot;
    PeonyDesktopLinkMonitorDetails *details;
} PeonyDesktopLinkMonitor;

typedef struct
{
    GObjectClass parent_slot;
} PeonyDesktopLinkMonitorClass;

GType   peony_desktop_link_monitor_get_type (void);

PeonyDesktopLinkMonitor *   peony_desktop_link_monitor_get (void);

/* Used by peony-desktop-link.c */
char * peony_desktop_link_monitor_make_filename_unique (PeonyDesktopLinkMonitor *monitor,
        const char *filename);

#endif /* PEONY_DESKTOP_LINK_MONITOR_H */
