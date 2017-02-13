/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-directory.h: Peony directory model.

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

#ifndef PEONY_DIRECTORY_H
#define PEONY_DIRECTORY_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libpeony-private/peony-file-attributes.h>

/* PeonyDirectory is a class that manages the model for a directory,
   real or virtual, for Peony, mainly the file-manager component. The directory is
   responsible for managing both real data and cached metadata. On top of
   the file system independence provided by gio, the directory
   object also provides:

       1) A synchronization framework, which notifies via signals as the
          set of known files changes.
       2) An abstract interface for getting attributes and performing
          operations on files.
*/

#define PEONY_TYPE_DIRECTORY peony_directory_get_type()
#define PEONY_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_DIRECTORY, PeonyDirectory))
#define PEONY_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_DIRECTORY, PeonyDirectoryClass))
#define PEONY_IS_DIRECTORY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_DIRECTORY))
#define PEONY_IS_DIRECTORY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_DIRECTORY))
#define PEONY_DIRECTORY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_DIRECTORY, PeonyDirectoryClass))

/* PeonyFile is defined both here and in peony-file.h. */
#ifndef PEONY_FILE_DEFINED
#define PEONY_FILE_DEFINED
typedef struct PeonyFile PeonyFile;
#endif

typedef struct PeonyDirectoryDetails PeonyDirectoryDetails;

typedef struct
{
    GObject object;
    PeonyDirectoryDetails *details;
} PeonyDirectory;

typedef void (*PeonyDirectoryCallback) (PeonyDirectory *directory,
                                       GList             *files,
                                       gpointer           callback_data);

typedef struct
{
    GObjectClass parent_class;

    /*** Notification signals for clients to connect to. ***/

    /* The files_added signal is emitted as the directory model
     * discovers new files.
     */
    void     (* files_added)         (PeonyDirectory          *directory,
                                      GList                      *added_files);

    /* The files_changed signal is emitted as changes occur to
     * existing files that are noticed by the synchronization framework,
     * including when an old file has been deleted. When an old file
     * has been deleted, this is the last chance to forget about these
     * file objects, which are about to be unref'd. Use a call to
     * peony_file_is_gone () to test for this case.
     */
    void     (* files_changed)       (PeonyDirectory         *directory,
                                      GList                     *changed_files);

    /* The done_loading signal is emitted when a directory load
     * request completes. This is needed because, at least in the
     * case where the directory is empty, the caller will receive
     * no kind of notification at all when a directory load
     * initiated by `peony_directory_file_monitor_add' completes.
     */
    void     (* done_loading)        (PeonyDirectory         *directory);

    void     (* load_error)          (PeonyDirectory         *directory,
                                      GError                    *error);

    /*** Virtual functions for subclasses to override. ***/
    gboolean (* contains_file)       (PeonyDirectory         *directory,
                                      PeonyFile              *file);
    void     (* call_when_ready)     (PeonyDirectory         *directory,
                                      PeonyFileAttributes     file_attributes,
                                      gboolean                   wait_for_file_list,
                                      PeonyDirectoryCallback  callback,
                                      gpointer                   callback_data);
    void     (* cancel_callback)     (PeonyDirectory         *directory,
                                      PeonyDirectoryCallback  callback,
                                      gpointer                   callback_data);
    void     (* file_monitor_add)    (PeonyDirectory          *directory,
                                      gconstpointer              client,
                                      gboolean                   monitor_hidden_files,
                                      PeonyFileAttributes     monitor_attributes,
                                      PeonyDirectoryCallback  initial_files_callback,
                                      gpointer                   callback_data);
    void     (* file_monitor_remove) (PeonyDirectory         *directory,
                                      gconstpointer              client);
    void     (* force_reload)        (PeonyDirectory         *directory);
    gboolean (* are_all_files_seen)  (PeonyDirectory         *directory);
    gboolean (* is_not_empty)        (PeonyDirectory         *directory);
    char *	 (* get_name_for_self_as_new_file) (PeonyDirectory *directory);

    /* get_file_list is a function pointer that subclasses may override to
     * customize collecting the list of files in a directory.
     * For example, the PeonyDesktopDirectory overrides this so that it can
     * merge together the list of files in the $HOME/Desktop directory with
     * the list of standard icons (Computer, Home, Trash) on the desktop.
     */
    GList *	 (* get_file_list)	 (PeonyDirectory *directory);

    /* Should return FALSE if the directory is read-only and doesn't
     * allow setting of metadata.
     * An example of this is the search directory.
     */
    gboolean (* is_editable)         (PeonyDirectory *directory);
} PeonyDirectoryClass;

/* Basic GObject requirements. */
GType              peony_directory_get_type                 (void);

/* Get a directory given a uri.
 * Creates the appropriate subclass given the uri mappings.
 * Returns a referenced object, not a floating one. Unref when finished.
 * If two windows are viewing the same uri, the directory object is shared.
 */
PeonyDirectory *peony_directory_get                      (GFile                     *location);
PeonyDirectory *peony_directory_get_by_uri               (const char                *uri);
PeonyDirectory *peony_directory_get_for_file             (PeonyFile              *file);

/* Covers for g_object_ref and g_object_unref that provide two conveniences:
 * 1) Using these is type safe.
 * 2) You are allowed to call these with NULL,
 */
PeonyDirectory *peony_directory_ref                      (PeonyDirectory         *directory);
void               peony_directory_unref                    (PeonyDirectory         *directory);

/* Access to a URI. */
char *             peony_directory_get_uri                  (PeonyDirectory         *directory);
GFile *            peony_directory_get_location             (PeonyDirectory         *directory);

/* Is this file still alive and in this directory? */
gboolean           peony_directory_contains_file            (PeonyDirectory         *directory,
        PeonyFile              *file);

/* Get (and ref) a PeonyFile object for this directory. */
PeonyFile *     peony_directory_get_corresponding_file   (PeonyDirectory         *directory);

/* Waiting for data that's read asynchronously.
 * The file attribute and metadata keys are for files in the directory.
 */
void               peony_directory_call_when_ready          (PeonyDirectory         *directory,
        PeonyFileAttributes     file_attributes,
        gboolean                   wait_for_all_files,
        PeonyDirectoryCallback  callback,
        gpointer                   callback_data);
void               peony_directory_cancel_callback          (PeonyDirectory         *directory,
        PeonyDirectoryCallback  callback,
        gpointer                   callback_data);


/* Monitor the files in a directory. */
void               peony_directory_file_monitor_add         (PeonyDirectory         *directory,
        gconstpointer              client,
        gboolean                   monitor_hidden_files,
        PeonyFileAttributes     attributes,
        PeonyDirectoryCallback  initial_files_callback,
        gpointer                   callback_data);
void               peony_directory_file_monitor_remove      (PeonyDirectory         *directory,
        gconstpointer              client);
void               peony_directory_force_reload             (PeonyDirectory         *directory);

/* Get a list of all files currently known in the directory. */
GList *            peony_directory_get_file_list            (PeonyDirectory         *directory);

GList *            peony_directory_match_pattern            (PeonyDirectory         *directory,
        const char *glob);


/* Return true if the directory has information about all the files.
 * This will be false until the directory has been read at least once.
 */
gboolean           peony_directory_are_all_files_seen       (PeonyDirectory         *directory);

/* Return true if the directory is local. */
gboolean           peony_directory_is_local                 (PeonyDirectory         *directory);

gboolean           peony_directory_is_in_trash              (PeonyDirectory         *directory);

/* Return false if directory contains anything besides a Peony metafile.
 * Only valid if directory is monitored. Used by the Trash monitor.
 */
gboolean           peony_directory_is_not_empty             (PeonyDirectory         *directory);

/* Convenience functions for dealing with a list of PeonyDirectory objects that each have a ref.
 * These are just convenient names for functions that work on lists of GtkObject *.
 */
GList *            peony_directory_list_ref                 (GList                     *directory_list);
void               peony_directory_list_unref               (GList                     *directory_list);
void               peony_directory_list_free                (GList                     *directory_list);
GList *            peony_directory_list_copy                (GList                     *directory_list);
GList *            peony_directory_list_sort_by_uri         (GList                     *directory_list);

/* Fast way to check if a directory is the desktop directory */
gboolean           peony_directory_is_desktop_directory     (PeonyDirectory         *directory);

gboolean           peony_directory_is_editable              (PeonyDirectory         *directory);


#endif /* PEONY_DIRECTORY_H */
