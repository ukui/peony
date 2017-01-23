/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-column-utilities.h - Utilities related to column specifications

   Copyright (C) 2004 Novell, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the column COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Dave Camp <dave@ximian.com>
*/

#include <config.h>
#include "peony-column-utilities.h"

#include <string.h>
#include <eel/eel-glib-extensions.h>
#include <glib/gi18n.h>
#include <libpeony-extension/peony-column-provider.h>
#include <libpeony-private/peony-extensions.h>
#include <libpeony-private/peony-module.h>

static GList *
get_builtin_columns (void)
{
    GList *columns;

    columns = g_list_append (NULL,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "name",
                                           "attribute", "name",
                                           "label", _("Name"),
                                           "description", _("The name and icon of the file."),
                                           NULL));
    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "size",
                                           "attribute", "size",
                                           "label", _("Size"),
                                           "description", _("The size of the file."),
                                           "xalign", 1.0,
                                           NULL));
    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "type",
                                           "attribute", "type",
                                           "label", _("Type"),
                                           "description", _("The type of the file."),
                                           NULL));
    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "date_modified",
                                           "attribute", "date_modified",
                                           "label", _("Date Modified"),
                                           "description", _("The date the file was modified."),
                                           NULL));

    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "date_accessed",
                                           "attribute", "date_accessed",
                                           "label", _("Date Accessed"),
                                           "description", _("The date the file was accessed."),
                                           NULL));

    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "owner",
                                           "attribute", "owner",
                                           "label", _("Owner"),
                                           "description", _("The owner of the file."),
                                           NULL));

    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "group",
                                           "attribute", "group",
                                           "label", _("Group"),
                                           "description", _("The group of the file."),
                                           NULL));

    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "permissions",
                                           "attribute", "permissions",
                                           "label", _("Permissions"),
                                           "description", _("The permissions of the file."),
                                           NULL));

    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "octal_permissions",
                                           "attribute", "octal_permissions",
                                           "label", _("Octal Permissions"),
                                           "description", _("The permissions of the file, in octal notation."),
                                           NULL));

    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "mime_type",
                                           "attribute", "mime_type",
                                           "label", _("MIME Type"),
                                           "description", _("The mime type of the file."),
                                           NULL));
#ifdef HAVE_SELINUX
    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "selinux_context",
                                           "attribute", "selinux_context",
                                           "label", _("SELinux Context"),
                                           "description", _("The SELinux security context of the file."),
                                           NULL));
#endif
    columns = g_list_append (columns,
                             g_object_new (PEONY_TYPE_COLUMN,
                                           "name", "where",
                                           "attribute", "where",
                                           "label", _("Location"),
                                           "description", _("The location of the file."),
                                           NULL));

    return columns;
}

static GList *
get_extension_columns (void)
{
    GList *columns;
    GList *providers;
    GList *l;

    providers = peony_extensions_get_for_type (PEONY_TYPE_COLUMN_PROVIDER);

    columns = NULL;

    for (l = providers; l != NULL; l = l->next)
    {
        PeonyColumnProvider *provider;
        GList *provider_columns;

        provider = PEONY_COLUMN_PROVIDER (l->data);
        provider_columns = peony_column_provider_get_columns (provider);
        columns = g_list_concat (columns, provider_columns);
    }

    peony_module_extension_list_free (providers);

    return columns;
}

static GList *
get_trash_columns (void)
{
    static GList *columns = NULL;

    if (columns == NULL)
    {
        columns = g_list_append (columns,
                                 g_object_new (PEONY_TYPE_COLUMN,
                                               "name", "trashed_on",
                                               "attribute", "trashed_on",
                                               "label", _("Trashed On"),
                                               "description", _("Date when file was moved to the Trash"),
                                               NULL));
        columns = g_list_append (columns,
                                 g_object_new (PEONY_TYPE_COLUMN,
                                               "name", "trash_orig_path",
                                               "attribute", "trash_orig_path",
                                               "label", _("Original Location"),
                                               "description", _("Original location of file before moved to the Trash"),
                                               NULL));
    }

    return peony_column_list_copy (columns);
}

GList *
peony_get_common_columns (void)
{
    static GList *columns = NULL;

    if (!columns)
    {
        columns = g_list_concat (get_builtin_columns (),
                                 get_extension_columns ());
    }

    return peony_column_list_copy (columns);
}

GList *
peony_get_all_columns (void)
{
    GList *columns = NULL;

    columns = g_list_concat (peony_get_common_columns (),
                             get_trash_columns ());

    return columns;
}

GList *
peony_get_columns_for_file (PeonyFile *file)
{
    GList *columns;

    columns = peony_get_common_columns ();

    if (file != NULL && peony_file_is_in_trash (file))
    {
        columns = g_list_concat (columns,
                                 get_trash_columns ());
    }

    return columns;
}

GList *
peony_column_list_copy (GList *columns)
{
    GList *ret;
    GList *l;

    ret = g_list_copy (columns);

    for (l = ret; l != NULL; l = l->next)
    {
        g_object_ref (l->data);
    }

    return ret;
}

void
peony_column_list_free (GList *columns)
{
    GList *l;

    for (l = columns; l != NULL; l = l->next)
    {
        g_object_unref (l->data);
    }

    g_list_free (columns);
}

static int
strv_index (char **strv, const char *str)
{
    int i;

    for (i = 0; strv[i] != NULL; ++i)
    {
        if (strcmp (strv[i], str) == 0)
            return i;
    }

    return -1;
}

static int
column_compare (PeonyColumn *a, PeonyColumn *b, char **column_order)
{
    int index_a;
    int index_b;
    char *name;

    g_object_get (G_OBJECT (a), "name", &name, NULL);
    index_a = strv_index (column_order, name);
    g_free (name);

    g_object_get (G_OBJECT (b), "name", &name, NULL);
    index_b = strv_index (column_order, name);
    g_free (name);

    if (index_a == index_b)
    {
        int ret;
        char *label_a;
        char *label_b;

        g_object_get (G_OBJECT (a), "label", &label_a, NULL);
        g_object_get (G_OBJECT (b), "label", &label_b, NULL);
        ret = strcmp (label_a, label_b);
        g_free (label_a);
        g_free (label_b);

        return ret;
    }
    else if (index_a == -1)
    {
        return 1;
    }
    else if (index_b == -1)
    {
        return -1;
    }
    else
    {
        return index_a - index_b;
    }
}

GList *
peony_sort_columns (GList  *columns,
                   char  **column_order)
{
    if (!column_order)
    {
        return NULL;
    }

    return g_list_sort_with_data (columns,
                                  (GCompareDataFunc)column_compare,
                                  column_order);
}

