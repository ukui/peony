/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-directory-private.h: Peony directory model.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.

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

   Author: Darin Adler <darin@bentspoon.com>
*/

#include <gio/gio.h>
#include <eel/eel-vfs-extensions.h>
#include <libpeony-private/peony-directory.h>
#include <libpeony-private/peony-file-queue.h>
#include <libpeony-private/peony-file.h>
#include <libpeony-private/peony-monitor.h>
#include <libpeony-extension/peony-info-provider.h>
#include <libxml/tree.h>

typedef struct LinkInfoReadState LinkInfoReadState;
typedef struct TopLeftTextReadState TopLeftTextReadState;
typedef struct FileMonitors FileMonitors;
typedef struct DirectoryLoadState DirectoryLoadState;
typedef struct DirectoryCountState DirectoryCountState;
typedef struct DeepCountState DeepCountState;
typedef struct GetInfoState GetInfoState;
typedef struct NewFilesState NewFilesState;
typedef struct MimeListState MimeListState;
typedef struct ThumbnailState ThumbnailState;
typedef struct MountState MountState;
typedef struct FilesystemInfoState FilesystemInfoState;

typedef enum
{
    REQUEST_LINK_INFO,
    REQUEST_DEEP_COUNT,
    REQUEST_DIRECTORY_COUNT,
    REQUEST_FILE_INFO,
    REQUEST_FILE_LIST, /* always FALSE if file != NULL */
    REQUEST_MIME_LIST,
    REQUEST_TOP_LEFT_TEXT,
    REQUEST_LARGE_TOP_LEFT_TEXT,
    REQUEST_EXTENSION_INFO,
    REQUEST_THUMBNAIL,
    REQUEST_MOUNT,
    REQUEST_FILESYSTEM_INFO,
    REQUEST_TYPE_LAST
} RequestType;

/* A request for information about one or more files. */
typedef guint32 Request;
typedef gint32 RequestCounter[REQUEST_TYPE_LAST];

#define REQUEST_WANTS_TYPE(request, type) ((request) & (1<<(type)))
#define REQUEST_SET_TYPE(request, type) (request) |= (1<<(type))

struct PeonyDirectoryDetails
{
    /* The location. */
    GFile *location;

    /* The file objects. */
    PeonyFile *as_file;
    GList *file_list;
    GHashTable *file_hash;

    /* Queues of files needing some I/O done. */
    PeonyFileQueue *high_priority_queue;
    PeonyFileQueue *low_priority_queue;
    PeonyFileQueue *extension_queue;

    /* These lists are going to be pretty short.  If we think they
     * are going to get big, we can use hash tables instead.
     */
    GList *call_when_ready_list;
    RequestCounter call_when_ready_counters;
    GList *monitor_list;
    RequestCounter monitor_counters;
    guint call_ready_idle_id;

    PeonyMonitor *monitor;
    gulong 		 mime_db_monitor;

    gboolean in_async_service_loop;
    gboolean state_changed;

    gboolean file_list_monitored;
    gboolean directory_loaded;
    gboolean directory_loaded_sent_notification;
    DirectoryLoadState *directory_load_in_progress;

    GList *pending_file_info; /* list of UkuiVFSFileInfo's that are pending */
    int confirmed_file_count;
    guint dequeue_pending_idle_id;

    GList *new_files_in_progress; /* list of NewFilesState * */

    DirectoryCountState *count_in_progress;

    PeonyFile *deep_count_file;
    DeepCountState *deep_count_in_progress;

    MimeListState *mime_list_in_progress;

    PeonyFile *get_info_file;
    GetInfoState *get_info_in_progress;

    PeonyFile *extension_info_file;
    PeonyInfoProvider *extension_info_provider;
    PeonyOperationHandle *extension_info_in_progress;
    guint extension_info_idle;

    ThumbnailState *thumbnail_state;

    MountState *mount_state;

    FilesystemInfoState *filesystem_info_state;

    TopLeftTextReadState *top_left_read_state;

    LinkInfoReadState *link_info_read_state;

    GList *file_operations_in_progress; /* list of FileOperation * */

    GHashTable *hidden_file_hash;

    guint64 free_space; /* (guint)-1 for unknown */
    time_t free_space_read; /* The time free_space was updated, or 0 for never */
};

PeonyDirectory *peony_directory_get_existing                    (GFile                     *location);

/* async. interface */
void               peony_directory_async_state_changed             (PeonyDirectory         *directory);
void               peony_directory_call_when_ready_internal        (PeonyDirectory         *directory,
        PeonyFile              *file,
        PeonyFileAttributes     file_attributes,
        gboolean                   wait_for_file_list,
        PeonyDirectoryCallback  directory_callback,
        PeonyFileCallback       file_callback,
        gpointer                   callback_data);
gboolean           peony_directory_check_if_ready_internal         (PeonyDirectory         *directory,
        PeonyFile              *file,
        PeonyFileAttributes     file_attributes);
void               peony_directory_cancel_callback_internal        (PeonyDirectory         *directory,
        PeonyFile              *file,
        PeonyDirectoryCallback  directory_callback,
        PeonyFileCallback       file_callback,
        gpointer                   callback_data);
void               peony_directory_monitor_add_internal            (PeonyDirectory         *directory,
        PeonyFile              *file,
        gconstpointer              client,
        gboolean                   monitor_hidden_files,
        PeonyFileAttributes     attributes,
        PeonyDirectoryCallback  callback,
        gpointer                   callback_data);
void               peony_directory_monitor_remove_internal         (PeonyDirectory         *directory,
        PeonyFile              *file,
        gconstpointer              client);
void               peony_directory_get_info_for_new_files          (PeonyDirectory         *directory,
        GList                     *vfs_uris);
PeonyFile *     peony_directory_get_existing_corresponding_file (PeonyDirectory         *directory);
void               peony_directory_invalidate_count_and_mime_list  (PeonyDirectory         *directory);
gboolean           peony_directory_is_file_list_monitored          (PeonyDirectory         *directory);
gboolean           peony_directory_is_anyone_monitoring_file_list  (PeonyDirectory         *directory);
gboolean           peony_directory_has_active_request_for_file     (PeonyDirectory         *directory,
        PeonyFile              *file);
void               peony_directory_remove_file_monitor_link        (PeonyDirectory         *directory,
        GList                     *link);
void               peony_directory_schedule_dequeue_pending        (PeonyDirectory         *directory);
void               peony_directory_stop_monitoring_file_list       (PeonyDirectory         *directory);
void               peony_directory_cancel                          (PeonyDirectory         *directory);
void               peony_async_destroying_file                     (PeonyFile              *file);
void               peony_directory_force_reload_internal           (PeonyDirectory         *directory,
        PeonyFileAttributes     file_attributes);
void               peony_directory_cancel_loading_file_attributes  (PeonyDirectory         *directory,
        PeonyFile              *file,
        PeonyFileAttributes     file_attributes);

/* Calls shared between directory, file, and async. code. */
void               peony_directory_emit_files_added                (PeonyDirectory         *directory,
        GList                     *added_files);
void               peony_directory_emit_files_changed              (PeonyDirectory         *directory,
        GList                     *changed_files);
void               peony_directory_emit_change_signals             (PeonyDirectory         *directory,
        GList                     *changed_files);
void               emit_change_signals_for_all_files		      (PeonyDirectory	 *directory);
void               emit_change_signals_for_all_files_in_all_directories (void);
void               peony_directory_emit_done_loading               (PeonyDirectory         *directory);
void               peony_directory_emit_load_error                 (PeonyDirectory         *directory,
        GError                    *error);
PeonyDirectory *peony_directory_get_internal                    (GFile                     *location,
        gboolean                   create);
char *             peony_directory_get_name_for_self_as_new_file   (PeonyDirectory         *directory);
Request            peony_directory_set_up_request                  (PeonyFileAttributes     file_attributes);

/* Interface to the file list. */
PeonyFile *     peony_directory_find_file_by_name               (PeonyDirectory         *directory,
        const char                *filename);

void               peony_directory_add_file                        (PeonyDirectory         *directory,
        PeonyFile              *file);
void               peony_directory_remove_file                     (PeonyDirectory         *directory,
        PeonyFile              *file);
FileMonitors *     peony_directory_remove_file_monitors            (PeonyDirectory         *directory,
        PeonyFile              *file);
void               peony_directory_add_file_monitors               (PeonyDirectory         *directory,
        PeonyFile              *file,
        FileMonitors              *monitors);
void               peony_directory_add_file                        (PeonyDirectory         *directory,
        PeonyFile              *file);
GList *            peony_directory_begin_file_name_change          (PeonyDirectory         *directory,
        PeonyFile              *file);
void               peony_directory_end_file_name_change            (PeonyDirectory         *directory,
        PeonyFile              *file,
        GList                     *node);
void               peony_directory_moved                           (const char                *from_uri,
        const char                *to_uri);
/* Interface to the work queue. */

void               peony_directory_add_file_to_work_queue          (PeonyDirectory *directory,
        PeonyFile *file);
void               peony_directory_remove_file_from_work_queue     (PeonyDirectory *directory,
        PeonyFile *file);

/* KDE compatibility hacks */

void               peony_set_kde_trash_name                        (const char *trash_dir);

/* debugging functions */
int                peony_directory_number_outstanding              (void);
