/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-ui-utilities.c - helper functions for GtkUIManager stuff

   Copyright (C) 2004 Red Hat, Inc.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Alexander Larsson <alexl@redhat.com>
*/

#include <config.h>
#include "peony-ui-utilities.h"
#include "peony-file-utilities.h"
#include "peony-icon-info.h"
#include <gio/gio.h>

#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <eel/eel-debug.h>
#include <eel/eel-graphic-effects.h>

void
peony_ui_unmerge_ui (GtkUIManager *ui_manager,
                    guint *merge_id,
                    GtkActionGroup **action_group)
{
    if (*merge_id != 0)
    {
        gtk_ui_manager_remove_ui (ui_manager,
                                  *merge_id);
        *merge_id = 0;
    }
    if (*action_group != NULL)
    {
        gtk_ui_manager_remove_action_group (ui_manager,
                                            *action_group);
        *action_group = NULL;
    }
}

void
peony_ui_prepare_merge_ui (GtkUIManager *ui_manager,
                          const char *name,
                          guint *merge_id,
                          GtkActionGroup **action_group)
{
    *merge_id = gtk_ui_manager_new_merge_id (ui_manager);
    *action_group = gtk_action_group_new (name);
    gtk_action_group_set_translation_domain (*action_group, GETTEXT_PACKAGE);
    gtk_ui_manager_insert_action_group (ui_manager, *action_group, 0);
    g_object_unref (*action_group); /* owned by ui manager */
}


char *
peony_get_ui_directory (void)
{
    return g_strdup (DATADIR "/peony/ui");
}

char *
peony_ui_file (const char *partial_path)
{
    char *path;

    path = g_build_filename (DATADIR "/peony/ui", partial_path, NULL);
    if (g_file_test (path, G_FILE_TEST_EXISTS))
    {
        return path;
    }
    g_free (path);
    return NULL;
}

const char *
peony_ui_string_get (const char *filename)
{
    static GHashTable *ui_cache = NULL;
    char *ui;
    char *path;

    if (ui_cache == NULL)
    {
        ui_cache = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
        eel_debug_call_at_shutdown_with_data ((GFreeFunc)g_hash_table_destroy, ui_cache);
    }

    ui = g_hash_table_lookup (ui_cache, filename);
    if (ui == NULL)
    {
        path = peony_ui_file (filename);
        if (path == NULL || !g_file_get_contents (path, &ui, NULL, NULL))
        {
            g_warning ("Unable to load ui file %s\n", filename);
        }
        g_free (path);
        g_hash_table_insert (ui_cache,
                             g_strdup (filename),
                             ui);
    }

    return ui;
}

static void
extension_action_callback (GtkAction *action,
                           gpointer callback_data)
{
    peony_menu_item_activate (PEONY_MENU_ITEM (callback_data));
}

static void
extension_action_sensitive_callback (PeonyMenuItem *item,
                                     GParamSpec *arg1,
                                     gpointer user_data)
{
    gboolean value;

    g_object_get (G_OBJECT (item),
                  "sensitive", &value,
                  NULL);

    gtk_action_set_sensitive (GTK_ACTION (user_data), value);
}

static GdkPixbuf *
get_action_icon (const char *icon_name, int size)
{
    PeonyIconInfo *info;
    GdkPixbuf *pixbuf;

    if (g_path_is_absolute (icon_name))
    {
        info = peony_icon_info_lookup_from_path (icon_name, size);
    }
    else
    {
        info = peony_icon_info_lookup_from_name (icon_name, size);
    }
    pixbuf = peony_icon_info_get_pixbuf_nodefault_at_size (info, size);
    g_object_unref (info);

    return pixbuf;
}

GtkAction *
peony_action_from_menu_item (PeonyMenuItem *item)
{
    char *name, *label, *tip, *icon_name;
    gboolean sensitive, priority;
    GtkAction *action;
    GdkPixbuf *pixbuf;

    g_object_get (G_OBJECT (item),
                  "name", &name, "label", &label,
                  "tip", &tip, "icon", &icon_name,
                  "sensitive", &sensitive,
                  "priority", &priority,
                  NULL);

    action = gtk_action_new (name,
                             label,
                             tip,
                             icon_name);

    if (icon_name != NULL)
    {
        pixbuf = get_action_icon (icon_name,
                                  peony_get_icon_size_for_stock_size (GTK_ICON_SIZE_MENU));
        if (pixbuf != NULL)
        {
            g_object_set_data_full (G_OBJECT (action), "menu-icon",
                                    pixbuf,
                                    g_object_unref);
        }
    }

    gtk_action_set_sensitive (action, sensitive);
    g_object_set (action, "is-important", priority, NULL);

    g_signal_connect_data (action, "activate",
                           G_CALLBACK (extension_action_callback),
                           g_object_ref (item),
                           (GClosureNotify)g_object_unref, 0);

    g_free (name);
    g_free (label);
    g_free (tip);
    g_free (icon_name);

    return action;
}

GtkAction *
peony_toolbar_action_from_menu_item (PeonyMenuItem *item)
{
    char *name, *label, *tip, *icon_name;
    gboolean sensitive, priority;
    GtkAction *action;
    GdkPixbuf *pixbuf;

    g_object_get (G_OBJECT (item),
                  "name", &name, "label", &label,
                  "tip", &tip, "icon", &icon_name,
                  "sensitive", &sensitive,
                  "priority", &priority,
                  NULL);

    action = gtk_action_new (name,
                             label,
                             tip,
                             icon_name);

    if (icon_name != NULL)
    {
        pixbuf = get_action_icon (icon_name,
                                  peony_get_icon_size_for_stock_size (GTK_ICON_SIZE_LARGE_TOOLBAR));
        if (pixbuf != NULL)
        {
            g_object_set_data_full (G_OBJECT (action), "toolbar-icon",
                                    pixbuf,
                                    g_object_unref);
        }
    }

    gtk_action_set_sensitive (action, sensitive);
    g_object_set (action, "is-important", priority, NULL);

    g_signal_connect_data (action, "activate",
                           G_CALLBACK (extension_action_callback),
                           g_object_ref (item),
                           (GClosureNotify)g_object_unref, 0);

    g_signal_connect_object (item, "notify::sensitive",
                             G_CALLBACK (extension_action_sensitive_callback),
                             action,
                             0);

    g_free (name);
    g_free (label);
    g_free (tip);
    g_free (icon_name);

    return action;
}

static GdkPixbuf *
peony_get_thumbnail_frame (void)
{
    char *image_path;
    static GdkPixbuf *thumbnail_frame = NULL;

    if (thumbnail_frame == NULL)
    {
        image_path = peony_pixmap_file ("thumbnail_frame.png");
        if (image_path != NULL)
        {
            thumbnail_frame = gdk_pixbuf_new_from_file (image_path, NULL);
        }
        g_free (image_path);
    }

    return thumbnail_frame;
}

#define PEONY_THUMBNAIL_FRAME_LEFT 3
#define PEONY_THUMBNAIL_FRAME_TOP 3
#define PEONY_THUMBNAIL_FRAME_RIGHT 3
#define PEONY_THUMBNAIL_FRAME_BOTTOM 3

void
peony_ui_frame_image (GdkPixbuf **pixbuf)
{
    GdkPixbuf *pixbuf_with_frame, *frame;
    int left_offset, top_offset, right_offset, bottom_offset;

    frame = peony_get_thumbnail_frame ();
    if (frame == NULL) {
        return;
    }

    left_offset = PEONY_THUMBNAIL_FRAME_LEFT;
    top_offset = PEONY_THUMBNAIL_FRAME_TOP;
    right_offset = PEONY_THUMBNAIL_FRAME_RIGHT;
    bottom_offset = PEONY_THUMBNAIL_FRAME_BOTTOM;

    pixbuf_with_frame = eel_embed_image_in_frame
        (*pixbuf, frame,
         left_offset, top_offset, right_offset, bottom_offset);
    g_object_unref (*pixbuf);

    *pixbuf = pixbuf_with_frame;
}
