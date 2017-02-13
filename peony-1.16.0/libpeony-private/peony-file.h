/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-file.h: Peony file model.

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

#ifndef PEONY_FILE_H
#define PEONY_FILE_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libpeony-private/peony-file-attributes.h>
#include <libpeony-private/peony-icon-info.h>

/* PeonyFile is an object used to represent a single element of a
 * PeonyDirectory. It's lightweight and relies on PeonyDirectory
 * to do most of the work.
 */

/* PeonyFile is defined both here and in peony-directory.h. */
#ifndef PEONY_FILE_DEFINED
#define PEONY_FILE_DEFINED
typedef struct PeonyFile PeonyFile;
#endif

#define PEONY_TYPE_FILE peony_file_get_type()
#define PEONY_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_FILE, PeonyFile))
#define PEONY_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_FILE, PeonyFileClass))
#define PEONY_IS_FILE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_FILE))
#define PEONY_IS_FILE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_FILE))
#define PEONY_FILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_FILE, PeonyFileClass))

typedef enum
{
    PEONY_FILE_SORT_NONE,
    PEONY_FILE_SORT_BY_DISPLAY_NAME,
    PEONY_FILE_SORT_BY_DIRECTORY,
    PEONY_FILE_SORT_BY_SIZE,
    PEONY_FILE_SORT_BY_TYPE,
    PEONY_FILE_SORT_BY_MTIME,
    PEONY_FILE_SORT_BY_ATIME,
    PEONY_FILE_SORT_BY_EMBLEMS,
    PEONY_FILE_SORT_BY_TRASHED_TIME
} PeonyFileSortType;

typedef enum
{
    PEONY_REQUEST_NOT_STARTED,
    PEONY_REQUEST_IN_PROGRESS,
    PEONY_REQUEST_DONE
} PeonyRequestStatus;

typedef enum
{
    PEONY_FILE_ICON_FLAGS_NONE = 0,
    PEONY_FILE_ICON_FLAGS_USE_THUMBNAILS = (1<<0),
    PEONY_FILE_ICON_FLAGS_IGNORE_VISITING = (1<<1),
    PEONY_FILE_ICON_FLAGS_EMBEDDING_TEXT = (1<<2),
    PEONY_FILE_ICON_FLAGS_FOR_DRAG_ACCEPT = (1<<3),
    PEONY_FILE_ICON_FLAGS_FOR_OPEN_FOLDER = (1<<4),
    /* whether the thumbnail size must match the display icon size */
    PEONY_FILE_ICON_FLAGS_FORCE_THUMBNAIL_SIZE = (1<<5),
    /* uses the icon of the mount if present */
    PEONY_FILE_ICON_FLAGS_USE_MOUNT_ICON = (1<<6),
    /* render the mount icon as an emblem over the regular one */
    PEONY_FILE_ICON_FLAGS_USE_MOUNT_ICON_AS_EMBLEM = (1<<7)
} PeonyFileIconFlags;

/* Emblems sometimes displayed for PeonyFiles. Do not localize. */
#define PEONY_FILE_EMBLEM_NAME_SYMBOLIC_LINK "symbolic-link"
#define PEONY_FILE_EMBLEM_NAME_CANT_READ "noread"
#define PEONY_FILE_EMBLEM_NAME_CANT_WRITE "nowrite"
#define PEONY_FILE_EMBLEM_NAME_TRASH "trash"
#define PEONY_FILE_EMBLEM_NAME_NOTE "note"
#define PEONY_FILE_EMBLEM_NAME_DESKTOP "desktop"
#define PEONY_FILE_EMBLEM_NAME_SHARED "shared"

typedef void (*PeonyFileCallback)          (PeonyFile  *file,
        gpointer       callback_data);
typedef void (*PeonyFileListCallback)      (GList         *file_list,
        gpointer       callback_data);
typedef void (*PeonyFileOperationCallback) (PeonyFile  *file,
        GFile         *result_location,
        GError        *error,
        gpointer       callback_data);
typedef int (*PeonyWidthMeasureCallback)   (const char    *string,
        void	     *context);
typedef char * (*PeonyTruncateCallback)    (const char    *string,
        int	      width,
        void	     *context);


#define PEONY_FILE_ATTRIBUTES_FOR_ICON (PEONY_FILE_ATTRIBUTE_INFO | PEONY_FILE_ATTRIBUTE_LINK_INFO | PEONY_FILE_ATTRIBUTE_THUMBNAIL)

typedef void PeonyFileListHandle;

/* GObject requirements. */
GType                   peony_file_get_type                          (void);

/* Getting at a single file. */
PeonyFile *          peony_file_get                               (GFile                          *location);
PeonyFile *          peony_file_get_by_uri                        (const char                     *uri);

/* Get a file only if the peony version already exists */
PeonyFile *          peony_file_get_existing                      (GFile                          *location);
PeonyFile *          peony_file_get_existing_by_uri               (const char                     *uri);

/* Covers for g_object_ref and g_object_unref that provide two conveniences:
 * 1) Using these is type safe.
 * 2) You are allowed to call these with NULL,
 */
PeonyFile *          peony_file_ref                               (PeonyFile                   *file);
void                    peony_file_unref                             (PeonyFile                   *file);

/* Monitor the file. */
void                    peony_file_monitor_add                       (PeonyFile                   *file,
        gconstpointer                   client,
        PeonyFileAttributes          attributes);
void                    peony_file_monitor_remove                    (PeonyFile                   *file,
        gconstpointer                   client);

/* Waiting for data that's read asynchronously.
 * This interface currently works only for metadata, but could be expanded
 * to other attributes as well.
 */
void                    peony_file_call_when_ready                   (PeonyFile                   *file,
        PeonyFileAttributes          attributes,
        PeonyFileCallback            callback,
        gpointer                        callback_data);
void                    peony_file_cancel_call_when_ready            (PeonyFile                   *file,
        PeonyFileCallback            callback,
        gpointer                        callback_data);
gboolean                peony_file_check_if_ready                    (PeonyFile                   *file,
        PeonyFileAttributes          attributes);
void                    peony_file_invalidate_attributes             (PeonyFile                   *file,
        PeonyFileAttributes          attributes);
void                    peony_file_invalidate_all_attributes         (PeonyFile                   *file);

/* Basic attributes for file objects. */
gboolean                peony_file_contains_text                     (PeonyFile                   *file);
gboolean                peony_file_is_binary                         (PeonyFile                   *file);
char *                  peony_file_get_display_name                  (PeonyFile                   *file);
char *                  peony_file_get_edit_name                     (PeonyFile                   *file);
char *                  peony_file_get_name                          (PeonyFile                   *file);
GFile *                 peony_file_get_location                      (PeonyFile                   *file);
char *			 peony_file_get_description			 (PeonyFile			 *file);
char *                  peony_file_get_uri                           (PeonyFile                   *file);
char *                  peony_file_get_uri_scheme                    (PeonyFile                   *file);
PeonyFile *          peony_file_get_parent                        (PeonyFile                   *file);
GFile *                 peony_file_get_parent_location               (PeonyFile                   *file);
char *                  peony_file_get_parent_uri                    (PeonyFile                   *file);
char *                  peony_file_get_parent_uri_for_display        (PeonyFile                   *file);
gboolean                peony_file_can_get_size                      (PeonyFile                   *file);
goffset                 peony_file_get_size                          (PeonyFile                   *file);
time_t                  peony_file_get_mtime                         (PeonyFile                   *file);
GFileType               peony_file_get_file_type                     (PeonyFile                   *file);
char *                  peony_file_get_mime_type                     (PeonyFile                   *file);
gboolean                peony_file_is_mime_type                      (PeonyFile                   *file,
        const char                     *mime_type);
gboolean                peony_file_is_launchable                     (PeonyFile                   *file);
gboolean                peony_file_is_symbolic_link                  (PeonyFile                   *file);
gboolean                peony_file_is_mountpoint                     (PeonyFile                   *file);
GMount *                peony_file_get_mount                         (PeonyFile                   *file);
char *                  peony_file_get_volume_free_space             (PeonyFile                   *file);
char *                  peony_file_get_volume_name                   (PeonyFile                   *file);
char *                  peony_file_get_symbolic_link_target_path     (PeonyFile                   *file);
char *                  peony_file_get_symbolic_link_target_uri      (PeonyFile                   *file);
gboolean                peony_file_is_broken_symbolic_link           (PeonyFile                   *file);
gboolean                peony_file_is_peony_link                  (PeonyFile                   *file);
gboolean                peony_file_is_executable                     (PeonyFile                   *file);
gboolean                peony_file_is_directory                      (PeonyFile                   *file);
gboolean                peony_file_is_user_special_directory         (PeonyFile                   *file,
        GUserDirectory                 special_directory);
gboolean		peony_file_is_archive			(PeonyFile			*file);
gboolean                peony_file_is_in_trash                       (PeonyFile                   *file);
gboolean                peony_file_is_in_desktop                     (PeonyFile                   *file);
gboolean		peony_file_is_home				(PeonyFile                   *file);
gboolean                peony_file_is_desktop_directory              (PeonyFile                   *file);
GError *                peony_file_get_file_info_error               (PeonyFile                   *file);
gboolean                peony_file_get_directory_item_count          (PeonyFile                   *file,
        guint                          *count,
        gboolean                       *count_unreadable);
void                    peony_file_recompute_deep_counts             (PeonyFile                   *file);
PeonyRequestStatus   peony_file_get_deep_counts                   (PeonyFile                   *file,
        guint                          *directory_count,
        guint                          *file_count,
        guint                          *unreadable_directory_count,
        goffset               *total_size,
        gboolean                        force);
gboolean                peony_file_should_show_thumbnail             (PeonyFile                   *file);
gboolean                peony_file_should_show_directory_item_count  (PeonyFile                   *file);
gboolean                peony_file_should_show_type                  (PeonyFile                   *file);
GList *                 peony_file_get_keywords                      (PeonyFile                   *file);
void                    peony_file_set_keywords                      (PeonyFile                   *file,
        GList                          *keywords);
GList *                 peony_file_get_emblem_icons                  (PeonyFile                   *file,
        char                          **exclude);
GList *                 peony_file_get_emblem_pixbufs                (PeonyFile                   *file,
        int                             size,
        gboolean                        force_size,
        char                          **exclude);
char *                  peony_file_get_top_left_text                 (PeonyFile                   *file);
char *                  peony_file_peek_top_left_text                (PeonyFile                   *file,
        gboolean                        need_large_text,
        gboolean                       *got_top_left_text);

void                    peony_file_set_attributes                    (PeonyFile                   *file,
        GFileInfo                      *attributes,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
GFilesystemPreviewType  peony_file_get_filesystem_use_preview        (PeonyFile *file);

char *                  peony_file_get_filesystem_id                 (PeonyFile                   *file);

PeonyFile *          peony_file_get_trash_original_file           (PeonyFile                   *file);

/* Permissions. */
gboolean                peony_file_can_get_permissions               (PeonyFile                   *file);
gboolean                peony_file_can_set_permissions               (PeonyFile                   *file);
guint                   peony_file_get_permissions                   (PeonyFile                   *file);
gboolean                peony_file_can_get_owner                     (PeonyFile                   *file);
gboolean                peony_file_can_set_owner                     (PeonyFile                   *file);
gboolean                peony_file_can_get_group                     (PeonyFile                   *file);
gboolean                peony_file_can_set_group                     (PeonyFile                   *file);
char *                  peony_file_get_owner_name                    (PeonyFile                   *file);
char *                  peony_file_get_group_name                    (PeonyFile                   *file);
GList *                 peony_get_user_names                         (void);
GList *                 peony_get_all_group_names                    (void);
GList *                 peony_file_get_settable_group_names          (PeonyFile                   *file);
gboolean                peony_file_can_get_selinux_context           (PeonyFile                   *file);
char *                  peony_file_get_selinux_context               (PeonyFile                   *file);

/* "Capabilities". */
gboolean                peony_file_can_read                          (PeonyFile                   *file);
gboolean                peony_file_can_write                         (PeonyFile                   *file);
gboolean                peony_file_can_execute                       (PeonyFile                   *file);
gboolean                peony_file_can_rename                        (PeonyFile                   *file);
gboolean                peony_file_can_delete                        (PeonyFile                   *file);
gboolean                peony_file_can_trash                         (PeonyFile                   *file);

gboolean                peony_file_can_mount                         (PeonyFile                   *file);
gboolean                peony_file_can_unmount                       (PeonyFile                   *file);
gboolean                peony_file_can_eject                         (PeonyFile                   *file);
gboolean                peony_file_can_start                         (PeonyFile                   *file);
gboolean                peony_file_can_start_degraded                (PeonyFile                   *file);
gboolean                peony_file_can_stop                          (PeonyFile                   *file);
GDriveStartStopType     peony_file_get_start_stop_type               (PeonyFile                   *file);
gboolean                peony_file_can_poll_for_media                (PeonyFile                   *file);
gboolean                peony_file_is_media_check_automatic          (PeonyFile                   *file);

void                    peony_file_mount                             (PeonyFile                   *file,
        GMountOperation                *mount_op,
        GCancellable                   *cancellable,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_unmount                           (PeonyFile                   *file,
        GMountOperation                *mount_op,
        GCancellable                   *cancellable,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_eject                             (PeonyFile                   *file,
        GMountOperation                *mount_op,
        GCancellable                   *cancellable,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);

void                    peony_file_start                             (PeonyFile                   *file,
        GMountOperation                *start_op,
        GCancellable                   *cancellable,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_stop                              (PeonyFile                   *file,
        GMountOperation                *mount_op,
        GCancellable                   *cancellable,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_poll_for_media                    (PeonyFile                   *file);

/* Basic operations for file objects. */
void                    peony_file_set_owner                         (PeonyFile                   *file,
        const char                     *user_name_or_id,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_set_group                         (PeonyFile                   *file,
        const char                     *group_name_or_id,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_set_permissions                   (PeonyFile                   *file,
        guint32                         permissions,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_rename                            (PeonyFile                   *file,
        const char                     *new_name,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);
void                    peony_file_cancel                            (PeonyFile                   *file,
        PeonyFileOperationCallback   callback,
        gpointer                        callback_data);

/* Return true if this file has already been deleted.
 * This object will be unref'd after sending the files_removed signal,
 * but it could hang around longer if someone ref'd it.
 */
gboolean                peony_file_is_gone                           (PeonyFile                   *file);

/* Return true if this file is not confirmed to have ever really
 * existed. This is true when the PeonyFile object has been created, but no I/O
 * has yet confirmed the existence of a file by that name.
 */
gboolean                peony_file_is_not_yet_confirmed              (PeonyFile                   *file);

/* Simple getting and setting top-level metadata. */
char *                  peony_file_get_metadata                      (PeonyFile                   *file,
        const char                     *key,
        const char                     *default_metadata);
GList *                 peony_file_get_metadata_list                 (PeonyFile                   *file,
        const char                     *key);
void                    peony_file_set_metadata                      (PeonyFile                   *file,
        const char                     *key,
        const char                     *default_metadata,
        const char                     *metadata);
void                    peony_file_set_metadata_list                 (PeonyFile                   *file,
        const char                     *key,
        GList                          *list);

/* Covers for common data types. */
gboolean                peony_file_get_boolean_metadata              (PeonyFile                   *file,
        const char                     *key,
        gboolean                        default_metadata);
void                    peony_file_set_boolean_metadata              (PeonyFile                   *file,
        const char                     *key,
        gboolean                        default_metadata,
        gboolean                        metadata);
int                     peony_file_get_integer_metadata              (PeonyFile                   *file,
        const char                     *key,
        int                             default_metadata);
void                    peony_file_set_integer_metadata              (PeonyFile                   *file,
        const char                     *key,
        int                             default_metadata,
        int                             metadata);

#define UNDEFINED_TIME ((time_t) (-1))

time_t                  peony_file_get_time_metadata                 (PeonyFile                  *file,
        const char                    *key);
void                    peony_file_set_time_metadata                 (PeonyFile                  *file,
        const char                    *key,
        time_t                         time);


/* Attributes for file objects as user-displayable strings. */
char *                  peony_file_get_string_attribute              (PeonyFile                   *file,
        const char                     *attribute_name);
char *                  peony_file_get_string_attribute_q            (PeonyFile                   *file,
        GQuark                          attribute_q);
char *                  peony_file_get_string_attribute_with_default (PeonyFile                   *file,
        const char                     *attribute_name);
char *                  peony_file_get_string_attribute_with_default_q (PeonyFile                  *file,
        GQuark                          attribute_q);
char *			peony_file_fit_modified_date_as_string	(PeonyFile 			*file,
        int				 width,
        PeonyWidthMeasureCallback    measure_callback,
        PeonyTruncateCallback	 truncate_callback,
        void				*measure_truncate_context);

/* Matching with another URI. */
gboolean                peony_file_matches_uri                       (PeonyFile                   *file,
        const char                     *uri);

/* Is the file local? */
gboolean                peony_file_is_local                          (PeonyFile                   *file);

/* Comparing two file objects for sorting */
PeonyFileSortType    peony_file_get_default_sort_type             (PeonyFile                   *file,
        gboolean                       *reversed);
const gchar *           peony_file_get_default_sort_attribute        (PeonyFile                   *file,
        gboolean                       *reversed);

int                     peony_file_compare_for_sort                  (PeonyFile                   *file_1,
        PeonyFile                   *file_2,
        PeonyFileSortType            sort_type,
        gboolean			 directories_first,
        gboolean		  	 reversed);
int                     peony_file_compare_for_sort_by_attribute     (PeonyFile                   *file_1,
        PeonyFile                   *file_2,
        const char                     *attribute,
        gboolean                        directories_first,
        gboolean                        reversed);
int                     peony_file_compare_for_sort_by_attribute_q   (PeonyFile                   *file_1,
        PeonyFile                   *file_2,
        GQuark                          attribute,
        gboolean                        directories_first,
        gboolean                        reversed);
gboolean                peony_file_is_date_sort_attribute_q          (GQuark                          attribute);

int                     peony_file_compare_display_name              (PeonyFile                   *file_1,
        const char                     *pattern);
int                     peony_file_compare_location                  (PeonyFile                    *file_1,
        PeonyFile                    *file_2);

/* filtering functions for use by various directory views */
gboolean                peony_file_is_hidden_file                    (PeonyFile                   *file);
gboolean                peony_file_should_show                       (PeonyFile                   *file,
        gboolean                        show_hidden,
        gboolean                        show_foreign);
GList                  *peony_file_list_filter_hidden                (GList                          *files,
        gboolean                        show_hidden);


/* Get the URI that's used when activating the file.
 * Getting this can require reading the contents of the file.
 */
gboolean                peony_file_is_launcher                       (PeonyFile                   *file);
gboolean                peony_file_is_foreign_link                   (PeonyFile                   *file);
gboolean                peony_file_is_trusted_link                   (PeonyFile                   *file);
gboolean                peony_file_has_activation_uri                (PeonyFile                   *file);
char *                  peony_file_get_activation_uri                (PeonyFile                   *file);
GFile *                 peony_file_get_activation_location           (PeonyFile                   *file);

char *                  peony_file_get_drop_target_uri               (PeonyFile                   *file);

/* Get custom icon (if specified by metadata or link contents) */
char *                  peony_file_get_custom_icon                   (PeonyFile                   *file);


GIcon *                 peony_file_get_gicon                         (PeonyFile                   *file,
        PeonyFileIconFlags           flags);
PeonyIconInfo *      peony_file_get_icon                          (PeonyFile                   *file,
        int                             size,
        PeonyFileIconFlags           flags);
GdkPixbuf *             peony_file_get_icon_pixbuf                   (PeonyFile                   *file,
        int                             size,
        gboolean                        force_size,
        PeonyFileIconFlags           flags);

gboolean                peony_file_has_open_window                   (PeonyFile                   *file);
void                    peony_file_set_has_open_window               (PeonyFile                   *file,
        gboolean                        has_open_window);

/* Thumbnailing handling */
gboolean                peony_file_is_thumbnailing                   (PeonyFile                   *file);

/* Convenience functions for dealing with a list of PeonyFile objects that each have a ref.
 * These are just convenient names for functions that work on lists of GtkObject *.
 */
GList *                 peony_file_list_ref                          (GList                          *file_list);
void                    peony_file_list_unref                        (GList                          *file_list);
void                    peony_file_list_free                         (GList                          *file_list);
GList *                 peony_file_list_copy                         (GList                          *file_list);
GList *			peony_file_list_sort_by_display_name		(GList				*file_list);
void                    peony_file_list_call_when_ready              (GList                          *file_list,
        PeonyFileAttributes          attributes,
        PeonyFileListHandle        **handle,
        PeonyFileListCallback        callback,
        gpointer                        callback_data);
void                    peony_file_list_cancel_call_when_ready       (PeonyFileListHandle         *handle);

/* Debugging */
void                    peony_file_dump                              (PeonyFile                   *file);

typedef struct PeonyFileDetails PeonyFileDetails;

struct PeonyFile
{
    GObject parent_slot;
    PeonyFileDetails *details;
};

/* This is actually a "protected" type, but it must be here so we can
 * compile the get_date function pointer declaration below.
 */
typedef enum
{
    PEONY_DATE_TYPE_MODIFIED,
    PEONY_DATE_TYPE_CHANGED,
    PEONY_DATE_TYPE_ACCESSED,
    PEONY_DATE_TYPE_PERMISSIONS_CHANGED,
    PEONY_DATE_TYPE_TRASHED
} PeonyDateType;

typedef struct
{
    GObjectClass parent_slot;

    /* Subclasses can set this to something other than G_FILE_TYPE_UNKNOWN and
       it will be used as the default file type. This is useful when creating
       a "virtual" PeonyFile subclass that you can't actually get real
       information about. For exaple PeonyDesktopDirectoryFile. */
    GFileType default_file_type;

    /* Called when the file notices any change. */
    void                  (* changed)                (PeonyFile *file);

    /* Called periodically while directory deep count is being computed. */
    void                  (* updated_deep_count_in_progress) (PeonyFile *file);

    /* Virtual functions (mainly used for trash directory). */
    void                  (* monitor_add)            (PeonyFile           *file,
            gconstpointer           client,
            PeonyFileAttributes  attributes);
    void                  (* monitor_remove)         (PeonyFile           *file,
            gconstpointer           client);
    void                  (* call_when_ready)        (PeonyFile           *file,
            PeonyFileAttributes  attributes,
            PeonyFileCallback    callback,
            gpointer                callback_data);
    void                  (* cancel_call_when_ready) (PeonyFile           *file,
            PeonyFileCallback    callback,
            gpointer                callback_data);
    gboolean              (* check_if_ready)         (PeonyFile           *file,
            PeonyFileAttributes  attributes);
    gboolean              (* get_item_count)         (PeonyFile           *file,
            guint                  *count,
            gboolean               *count_unreadable);
    PeonyRequestStatus (* get_deep_counts)        (PeonyFile           *file,
            guint                  *directory_count,
            guint                  *file_count,
            guint                  *unreadable_directory_count,
            goffset       *total_size);
    gboolean              (* get_date)               (PeonyFile           *file,
            PeonyDateType        type,
            time_t                 *date);
    char *                (* get_where_string)       (PeonyFile           *file);

    void                  (* set_metadata)           (PeonyFile           *file,
            const char             *key,
            const char             *value);
    void                  (* set_metadata_as_list)   (PeonyFile           *file,
            const char             *key,
            char                  **value);

    void                  (* mount)                  (PeonyFile                   *file,
            GMountOperation                *mount_op,
            GCancellable                   *cancellable,
            PeonyFileOperationCallback   callback,
            gpointer                        callback_data);
    void                 (* unmount)                 (PeonyFile                   *file,
            GMountOperation                *mount_op,
            GCancellable                   *cancellable,
            PeonyFileOperationCallback   callback,
            gpointer                        callback_data);
    void                 (* eject)                   (PeonyFile                   *file,
            GMountOperation                *mount_op,
            GCancellable                   *cancellable,
            PeonyFileOperationCallback   callback,
            gpointer                        callback_data);

    void                  (* start)                  (PeonyFile                   *file,
            GMountOperation                *start_op,
            GCancellable                   *cancellable,
            PeonyFileOperationCallback   callback,
            gpointer                        callback_data);
    void                 (* stop)                    (PeonyFile                   *file,
            GMountOperation                *mount_op,
            GCancellable                   *cancellable,
            PeonyFileOperationCallback   callback,
            gpointer                        callback_data);

    void                 (* poll_for_media)          (PeonyFile                   *file);
} PeonyFileClass;

#endif /* PEONY_FILE_H */
