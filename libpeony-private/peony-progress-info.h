/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-progress-info.h: file operation progress info.

   Copyright (C) 2007 Red Hat, Inc.

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

#ifndef PEONY_PROGRESS_INFO_H
#define PEONY_PROGRESS_INFO_H

#include <glib-object.h>
#include <gio/gio.h>

#define PEONY_TYPE_PROGRESS_INFO         (peony_progress_info_get_type ())
#define PEONY_PROGRESS_INFO(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), PEONY_TYPE_PROGRESS_INFO, PeonyProgressInfo))
#define PEONY_PROGRESS_INFO_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), PEONY_TYPE_PROGRESS_INFO, PeonyProgressInfoClass))
#define PEONY_IS_PROGRESS_INFO(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), PEONY_TYPE_PROGRESS_INFO))
#define PEONY_IS_PROGRESS_INFO_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), PEONY_TYPE_PROGRESS_INFO))
#define PEONY_PROGRESS_INFO_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), PEONY_TYPE_PROGRESS_INFO, PeonyProgressInfoClass))

typedef struct _PeonyProgressInfo      PeonyProgressInfo;
typedef struct _PeonyProgressInfoClass PeonyProgressInfoClass;

GType peony_progress_info_get_type (void) G_GNUC_CONST;

/* Signals:
   "changed" - status or details changed
   "progress-changed" - the percentage progress changed (or we pulsed if in activity_mode
   "started" - emited on job start
   "finished" - emitted when job is done

   All signals are emitted from idles in main loop.
   All methods are threadsafe.
 */

PeonyProgressInfo *peony_progress_info_new (gboolean should_start, gboolean can_pause);
void peony_progress_info_get_ready (PeonyProgressInfo *info);
void peony_progress_info_disable_pause (PeonyProgressInfo *info);

GList *       peony_get_all_progress_info (void);

char *        peony_progress_info_get_status      (PeonyProgressInfo *info);
char *        peony_progress_info_get_details     (PeonyProgressInfo *info);
double        peony_progress_info_get_progress    (PeonyProgressInfo *info);
GCancellable *peony_progress_info_get_cancellable (PeonyProgressInfo *info);
void          peony_progress_info_cancel          (PeonyProgressInfo *info);
gboolean      peony_progress_info_get_is_started  (PeonyProgressInfo *info);
gboolean      peony_progress_info_get_is_finished (PeonyProgressInfo *info);
gboolean      peony_progress_info_get_is_paused   (PeonyProgressInfo *info);

void          peony_progress_info_start           (PeonyProgressInfo *info);
void          peony_progress_info_finish          (PeonyProgressInfo *info);
void          peony_progress_info_pause           (PeonyProgressInfo *info);
void          peony_progress_info_resume          (PeonyProgressInfo *info);
void          peony_progress_info_set_status      (PeonyProgressInfo *info,
        const char           *status);
void          peony_progress_info_take_status     (PeonyProgressInfo *info,
        char                 *status);
void          peony_progress_info_set_details     (PeonyProgressInfo *info,
        const char           *details);
void          peony_progress_info_take_details    (PeonyProgressInfo *info,
        char                 *details);
void          peony_progress_info_set_progress    (PeonyProgressInfo *info,
        double                current,
        double                total);
void          peony_progress_info_pulse_progress  (PeonyProgressInfo *info);


#endif /* PEONY_PROGRESS_INFO_H */
