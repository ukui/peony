/*
 *  peony-file-info.c - Information about a file
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

#include <config.h>
#include "peony-file-info.h"
#include "peony-extension-private.h"

PeonyFileInfo *(*peony_file_info_getter) (GFile *location, gboolean create);

/**
 * SECTION:peony-file-info
 * @title: PeonyFileInfo
 * @short_description: File interface for peony extensions
 * @include: libpeony-extension/peony-file-info.h
 *
 * #PeonyFileInfo provides methods to get and modify information
 * about file objects in the file manager.
 */

/**
 * peony_file_info_list_copy:
 * @files: (element-type PeonyFileInfo): the files to copy
 *
 * Returns: (element-type PeonyFileInfo) (transfer full): a copy of @files.
 *  Use #peony_file_info_list_free to free the list and unref its contents.
 */
GList *
peony_file_info_list_copy (GList *files)
{
    GList *ret;
    GList *l;

    ret = g_list_copy (files);
    for (l = ret; l != NULL; l = l->next)
    {
        g_object_ref (G_OBJECT (l->data));
    }

    return ret;
}

/**
 * peony_file_info_list_free:
 * @files: (element-type PeonyFileInfo): a list created with
 *   #peony_file_info_list_copy
 *
 */
void
peony_file_info_list_free (GList *files)
{
    GList *l;

    for (l = files; l != NULL; l = l->next)
    {
        g_object_unref (G_OBJECT (l->data));
    }

    g_list_free (files);
}

static void
peony_file_info_base_init (gpointer g_class)
{
}

GType
peony_file_info_get_type (void)
{
    static GType type = 0;

    if (!type) {
        const GTypeInfo info = {
            sizeof (PeonyFileInfoIface),
            peony_file_info_base_init,
            NULL,
            NULL,
            NULL,
            NULL,
            0,
            0,
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE,
                                       "PeonyFileInfo",
                                       &info, 0);
        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

gboolean
peony_file_info_is_gone (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), FALSE);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->is_gone != NULL, FALSE);

    return PEONY_FILE_INFO_GET_IFACE (file)->is_gone (file);
}

GFileType
peony_file_info_get_file_type (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), G_FILE_TYPE_UNKNOWN);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_file_type != NULL, G_FILE_TYPE_UNKNOWN);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_file_type (file);
}

char *
peony_file_info_get_name (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_name != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_name (file);
}

/**
 * peony_file_info_get_location:
 * @file: a #PeonyFileInfo
 *
 * Returns: (transfer full): a #GFile for the location of @file
 */
GFile *
peony_file_info_get_location (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_location != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_location (file);
}

char *
peony_file_info_get_uri (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_uri != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_uri (file);
}

char *
peony_file_info_get_activation_uri (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_activation_uri != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_activation_uri (file);
}

/**
 * peony_file_info_get_parent_location:
 * @file: a #PeonyFileInfo
 *
 * Returns: (allow-none) (transfer full): a #GFile for the parent location of @file,
 *   or %NULL if @file has no parent
 */
GFile *
peony_file_info_get_parent_location (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_parent_location != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_parent_location (file);
}

char *
peony_file_info_get_parent_uri (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_parent_uri != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_parent_uri (file);
}

/**
 * peony_file_info_get_parent_info:
 * @file: a #PeonyFileInfo
 *
 * Returns: (allow-none) (transfer full): a #PeonyFileInfo for the parent of @file,
 *   or %NULL if @file has no parent
 */
PeonyFileInfo *
peony_file_info_get_parent_info (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_parent_info != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_parent_info (file);
}

/**
 * peony_file_info_get_mount:
 * @file: a #PeonyFileInfo
 *
 * Returns: (allow-none) (transfer full): a #GMount for the mount of @file,
 *   or %NULL if @file has no mount
 */
GMount *
peony_file_info_get_mount (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_mount != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_mount (file);
}

char *
peony_file_info_get_uri_scheme (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_uri_scheme != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_uri_scheme (file);
}

char *
peony_file_info_get_mime_type (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_mime_type != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_mime_type (file);
}

gboolean
peony_file_info_is_mime_type (PeonyFileInfo *file,
                             const char *mime_type)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), FALSE);
    g_return_val_if_fail (mime_type != NULL, FALSE);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->is_mime_type != NULL, FALSE);

    return PEONY_FILE_INFO_GET_IFACE (file)->is_mime_type (file,
           mime_type);
}

gboolean
peony_file_info_is_directory (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), FALSE);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->is_directory != NULL, FALSE);

    return PEONY_FILE_INFO_GET_IFACE (file)->is_directory (file);
}

gboolean
peony_file_info_can_write (PeonyFileInfo *file)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), FALSE);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->can_write != NULL, FALSE);

    return PEONY_FILE_INFO_GET_IFACE (file)->can_write (file);
}

void
peony_file_info_add_emblem (PeonyFileInfo *file,
                           const char *emblem_name)
{
    g_return_if_fail (PEONY_IS_FILE_INFO (file));
    g_return_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->add_emblem != NULL);

    PEONY_FILE_INFO_GET_IFACE (file)->add_emblem (file, emblem_name);
}

char *
peony_file_info_get_string_attribute (PeonyFileInfo *file,
                                     const char *attribute_name)
{
    g_return_val_if_fail (PEONY_IS_FILE_INFO (file), NULL);
    g_return_val_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->get_string_attribute != NULL, NULL);
    g_return_val_if_fail (attribute_name != NULL, NULL);

    return PEONY_FILE_INFO_GET_IFACE (file)->get_string_attribute
           (file, attribute_name);
}

void
peony_file_info_add_string_attribute (PeonyFileInfo *file,
                                     const char *attribute_name,
                                     const char *value)
{
    g_return_if_fail (PEONY_IS_FILE_INFO (file));
    g_return_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->add_string_attribute != NULL);
    g_return_if_fail (attribute_name != NULL);
    g_return_if_fail (value != NULL);

    PEONY_FILE_INFO_GET_IFACE (file)->add_string_attribute
        (file, attribute_name, value);
}

void
peony_file_info_invalidate_extension_info (PeonyFileInfo *file)
{
    g_return_if_fail (PEONY_IS_FILE_INFO (file));
    g_return_if_fail (PEONY_FILE_INFO_GET_IFACE (file)->invalidate_extension_info != NULL);

    PEONY_FILE_INFO_GET_IFACE (file)->invalidate_extension_info (file);
}

/**
 * peony_file_info_lookup:
 * @location: the location to lookup the file info for
 *
 * Returns: (transfer full): a #PeonyFileInfo
 */
PeonyFileInfo *
peony_file_info_lookup (GFile *location)
{
    return peony_file_info_getter (location, FALSE);
}

/**
 * peony_file_info_create:
 * @location: the location to create the file info for
 *
 * Returns: (transfer full): a #PeonyFileInfo
 */
PeonyFileInfo *
peony_file_info_create (GFile *location)
{
    return peony_file_info_getter (location, TRUE);
}

/**
 * peony_file_info_lookup_for_uri:
 * @uri: the URI to lookup the file info for
 *
 * Returns: (transfer full): a #PeonyFileInfo
 */
PeonyFileInfo *
peony_file_info_lookup_for_uri (const char *uri)
{
    GFile *location;
    PeonyFile *file;

    location = g_file_new_for_uri (uri);
    file = peony_file_info_lookup (location);
    g_object_unref (location);

    return file;
}

/**
 * peony_file_info_create_for_uri:
 * @uri: the URI to lookup the file info for
 *
 * Returns: (transfer full): a #PeonyFileInfo
 */
PeonyFileInfo *
peony_file_info_create_for_uri (const char *uri)
{
    GFile *location;
    PeonyFile *file;

    location = g_file_new_for_uri (uri);
    file = peony_file_info_create (location);
    g_object_unref (location);

    return file;
}
