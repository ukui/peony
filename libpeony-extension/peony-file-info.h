/*
 *  peony-file-info.h - Information about a file
 *
 *  Copyright (C) 2003 Novell, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

/* PeonyFileInfo is an interface to the PeonyFile object.  It
 * provides access to the asynchronous data in the PeonyFile.
 * Extensions are passed objects of this type for operations. */

#ifndef PEONY_FILE_INFO_H
#define PEONY_FILE_INFO_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define PEONY_TYPE_FILE_INFO           (peony_file_info_get_type ())
#define PEONY_FILE_INFO(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_FILE_INFO, PeonyFileInfo))
#define PEONY_IS_FILE_INFO(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_FILE_INFO))
#define PEONY_FILE_INFO_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), PEONY_TYPE_FILE_INFO, PeonyFileInfoIface))

#ifndef PEONY_FILE_DEFINED
#define PEONY_FILE_DEFINED
/* Using PeonyFile for the vtable to make implementing this in
 * PeonyFile easier */
typedef struct PeonyFile            PeonyFile;
#endif

typedef PeonyFile                   PeonyFileInfo;
typedef struct _PeonyFileInfoIface  PeonyFileInfoIface;

/**
 * PeonyFileInfoIface:
 * @g_iface: The parent interface.
 * @is_gone: Returns whether the file info is gone.
 *   See peony_file_info_is_gone() for details.
 * @get_name: Returns the file name as a string.
 *   See peony_file_info_get_name() for details.
 * @get_uri: Returns the file URI as a string.
 *   See peony_file_info_get_uri() for details.
 * @get_parent_uri: Returns the file parent URI as a string.
 *   See peony_file_info_get_parent_uri() for details.
 * @get_uri_scheme: Returns the file URI scheme as a string.
 *   See peony_file_info_get_uri_scheme() for details.
 * @get_mime_type: Returns the file mime type as a string.
 *   See peony_file_info_get_mime_type() for details.
 * @is_mime_type: Returns whether the file is the given mime type.
 *   See peony_file_info_is_mime_type() for details.
 * @is_directory: Returns whether the file is a directory.
 *   See peony_file_info_is_directory() for details.
 * @add_emblem: Adds an emblem to this file.
 *   See peony_file_info_add_emblem() for details.
 * @get_string_attribute: Returns the specified file attribute as a string.
 *   See peony_file_info_get_string_attribute() for details.
 * @add_string_attribute: Sets the specified string file attribute value.
 *   See peony_file_info_add_string_attribute() for details.
 * @invalidate_extension_info: Invalidates information of the file provided by extensions.
 *   See peony_file_info_invalidate_extension_info() for details.
 * @get_activation_uri: Returns the file activation URI as a string.
 *   See peony_file_info_get_activation_uri() for details.
 * @get_file_type: Returns the file type.
 *   See peony_file_info_get_file_type() for details.
 * @get_location: Returns the file location as a #GFile.
 *   See peony_file_info_get_location() for details.
 * @get_parent_location: Returns the file parent location as a #GFile.
 *   See peony_file_info_get_parent_location() for details.
 * @get_parent_info: Returns the file parent #PeonyFileInfo.
 *   See peony_file_info_get_parent_info() for details.
 * @get_mount: Returns the file mount as a #GMount.
 *   See peony_file_info_get_mount() for details.
 * @can_write: Returns whether the file is writable.
 *   See peony_file_info_can_write() for details.
 *
 * Interface for extensions to get and modify information
 * about file objects.
 */

struct _PeonyFileInfoIface {
    GTypeInterface g_iface;

    gboolean      (*is_gone)              (PeonyFileInfo *file);

    char         *(*get_name)             (PeonyFileInfo *file);
    char         *(*get_uri)              (PeonyFileInfo *file);
    char         *(*get_parent_uri)       (PeonyFileInfo *file);
    char         *(*get_uri_scheme)       (PeonyFileInfo *file);

    char         *(*get_mime_type)        (PeonyFileInfo *file);
    gboolean      (*is_mime_type)         (PeonyFileInfo *file,
                                           const char   *mime_Type);
    gboolean      (*is_directory)         (PeonyFileInfo *file);

    void          (*add_emblem)           (PeonyFileInfo *file,
                                           const char   *emblem_name);
    char         *(*get_string_attribute) (PeonyFileInfo *file,
                                           const char   *attribute_name);
    void          (*add_string_attribute) (PeonyFileInfo *file,
                                           const char   *attribute_name,
                                           const char   *value);
    void          (*invalidate_extension_info) (PeonyFileInfo *file);

    char         *(*get_activation_uri)   (PeonyFileInfo *file);

    GFileType     (*get_file_type)        (PeonyFileInfo *file);
    GFile        *(*get_location)         (PeonyFileInfo *file);
    GFile        *(*get_parent_location)  (PeonyFileInfo *file);
    PeonyFileInfo *(*get_parent_info)      (PeonyFileInfo *file);
    GMount       *(*get_mount)            (PeonyFileInfo *file);
    gboolean      (*can_write)            (PeonyFileInfo *file);
};

GList       *peony_file_info_list_copy             (GList        *files);
void         peony_file_info_list_free             (GList        *files);
GType        peony_file_info_get_type              (void);

/* Return true if the file has been deleted */
gboolean     peony_file_info_is_gone               (PeonyFileInfo *file);

/* Name and Location */
GFileType    peony_file_info_get_file_type         (PeonyFileInfo *file);
GFile        *peony_file_info_get_location         (PeonyFileInfo *file);
char         *peony_file_info_get_name             (PeonyFileInfo *file);
char         *peony_file_info_get_uri              (PeonyFileInfo *file);
char         *peony_file_info_get_activation_uri   (PeonyFileInfo *file);
GFile        *peony_file_info_get_parent_location  (PeonyFileInfo *file);
char         *peony_file_info_get_parent_uri       (PeonyFileInfo *file);
GMount       *peony_file_info_get_mount            (PeonyFileInfo *file);
char         *peony_file_info_get_uri_scheme       (PeonyFileInfo *file);
/* It's not safe to call this recursively multiple times, as it works
 * only for files already cached by Peony.
 */
PeonyFileInfo *peony_file_info_get_parent_info      (PeonyFileInfo *file);

/* File Type */
char         *peony_file_info_get_mime_type        (PeonyFileInfo *file);
gboolean      peony_file_info_is_mime_type         (PeonyFileInfo *file,
                                                   const char   *mime_type);
gboolean      peony_file_info_is_directory         (PeonyFileInfo *file);
gboolean      peony_file_info_can_write            (PeonyFileInfo *file);


/* Modifying the PeonyFileInfo */
void          peony_file_info_add_emblem           (PeonyFileInfo *file,
                                                   const char   *emblem_name);
char         *peony_file_info_get_string_attribute (PeonyFileInfo *file,
                                                   const char   *attribute_name);
void          peony_file_info_add_string_attribute (PeonyFileInfo *file,
                                                   const char   *attribute_name,
                                                   const char   *value);

/* Invalidating file info */
void          peony_file_info_invalidate_extension_info (PeonyFileInfo *file);

PeonyFileInfo *peony_file_info_lookup                (GFile       *location);
PeonyFileInfo *peony_file_info_create                (GFile       *location);
PeonyFileInfo *peony_file_info_lookup_for_uri        (const char  *uri);
PeonyFileInfo *peony_file_info_create_for_uri        (const char  *uri);

G_END_DECLS

#endif
