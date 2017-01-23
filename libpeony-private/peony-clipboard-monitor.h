/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-clipboard-monitor.h: lets you notice clipboard changes.

   Copyright (C) 2004 Red Hat, Inc.

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

#ifndef PEONY_CLIPBOARD_MONITOR_H
#define PEONY_CLIPBOARD_MONITOR_H

#include <gtk/gtk.h>

#define PEONY_TYPE_CLIPBOARD_MONITOR peony_clipboard_monitor_get_type()
#define PEONY_CLIPBOARD_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_CLIPBOARD_MONITOR, PeonyClipboardMonitor))
#define PEONY_CLIPBOARD_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_CLIPBOARD_MONITOR, PeonyClipboardMonitorClass))
#define PEONY_IS_CLIPBOARD_MONITOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_CLIPBOARD_MONITOR))
#define PEONY_IS_CLIPBOARD_MONITOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_CLIPBOARD_MONITOR))
#define PEONY_CLIPBOARD_MONITOR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_CLIPBOARD_MONITOR, PeonyClipboardMonitorClass))

typedef struct PeonyClipboardMonitorDetails PeonyClipboardMonitorDetails;
typedef struct PeonyClipboardInfo PeonyClipboardInfo;

typedef struct
{
    GObject parent_slot;

    PeonyClipboardMonitorDetails *details;
} PeonyClipboardMonitor;

typedef struct
{
    GObjectClass parent_slot;

    void (* clipboard_changed) (PeonyClipboardMonitor *monitor);
    void (* clipboard_info) (PeonyClipboardMonitor *monitor,
                             PeonyClipboardInfo *info);
} PeonyClipboardMonitorClass;

struct PeonyClipboardInfo
{
    GList *files;
    gboolean cut;
};

GType   peony_clipboard_monitor_get_type (void);

PeonyClipboardMonitor *   peony_clipboard_monitor_get (void);
void peony_clipboard_monitor_set_clipboard_info (PeonyClipboardMonitor *monitor,
        PeonyClipboardInfo *info);
PeonyClipboardInfo * peony_clipboard_monitor_get_clipboard_info (PeonyClipboardMonitor *monitor);
void peony_clipboard_monitor_emit_changed (void);

void peony_clear_clipboard_callback (GtkClipboard *clipboard,
                                    gpointer      user_data);
void peony_get_clipboard_callback   (GtkClipboard     *clipboard,
                                    GtkSelectionData *selection_data,
                                    guint             info,
                                    gpointer          user_data);



#endif /* PEONY_CLIPBOARD_MONITOR_H */

