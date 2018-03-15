/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-navigation-window-slot.c: Peony navigation window slot

   Copyright (C) 2008 Free Software Foundation, Inc.
   Copyright (C) 2018, Tianjin KYLIN Information Technology Co., Ltd.

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

   Author: Christian Neumair <cneumair@gnome.org>
   Modified by: liupeng <liupeng@kylinos.cn>
*/

#include "peony-window-slot.h"
#include "peony-navigation-window-slot.h"
#include "peony-window-private.h"
#include "peony-search-bar.h"
#include "peony-navigation-window-pane.h"
#include <libpeony-private/peony-window-slot-info.h>
#include <libpeony-private/peony-file.h>
#include <eel/eel-gtk-macros.h>

static void peony_navigation_window_slot_init       (PeonyNavigationWindowSlot *slot);
static void peony_navigation_window_slot_class_init (PeonyNavigationWindowSlotClass *class);

G_DEFINE_TYPE (PeonyNavigationWindowSlot, peony_navigation_window_slot, PEONY_TYPE_WINDOW_SLOT)
#define parent_class peony_navigation_window_slot_parent_class

gboolean
peony_navigation_window_slot_should_close_with_mount (PeonyNavigationWindowSlot *slot,
        GMount *mount)
{
    PeonyBookmark *bookmark;
    GFile *mount_location, *bookmark_location;
    GList *l;
    gboolean close_with_mount;

    if (slot->parent.pane->window->details->initiated_unmount)
    {
        return FALSE;
    }

    mount_location = g_mount_get_root (mount);

    close_with_mount = TRUE;

    for (l = slot->back_list; l != NULL; l = l->next)
    {
        bookmark = PEONY_BOOKMARK (l->data);

        bookmark_location = peony_bookmark_get_location (bookmark);
        close_with_mount &= g_file_has_prefix (bookmark_location, mount_location) ||
                            g_file_equal (bookmark_location, mount_location);
        g_object_unref (bookmark_location);

        if (!close_with_mount)
        {
            break;
        }
    }

    close_with_mount &= g_file_has_prefix (PEONY_WINDOW_SLOT (slot)->location, mount_location) ||
                        g_file_equal (PEONY_WINDOW_SLOT (slot)->location, mount_location);

    /* we could also consider the forward list here, but since the “go home” request
     * in peony-window-manager-views.c:mount_removed_callback() would discard those
     * anyway, we don't consider them.
     */

    g_object_unref (mount_location);

    return close_with_mount;
}

void
peony_navigation_window_slot_clear_forward_list (PeonyNavigationWindowSlot *slot)
{
    g_assert (PEONY_IS_NAVIGATION_WINDOW_SLOT (slot));

    g_list_free_full (slot->forward_list, g_object_unref);
    slot->forward_list = NULL;
}

void
peony_navigation_window_slot_clear_back_list (PeonyNavigationWindowSlot *slot)
{
    g_assert (PEONY_IS_NAVIGATION_WINDOW_SLOT (slot));

    g_list_free_full (slot->back_list, g_object_unref);
    slot->back_list = NULL;
}

static void
query_editor_changed_callback (PeonySearchBar *bar,
                               PeonyQuery *query,
                               gboolean reload,
                               PeonyWindowSlot *slot)
{
    PeonyDirectory *directory;
	GFile *location = NULL;
	
	if(TRUE == query_is_go_to_location(query))
	{
        if (NULL != slot && NULL != slot->query_editor && NULL != slot->query_editor->local_uri)
        {
			location = g_file_new_for_uri (slot->query_editor->local_uri);
			if(NULL != location)
			{
				peony_window_slot_go_to (slot, location, FALSE);
        		g_object_unref (location);
			}
        }
		g_object_unref (query);
		return;
	}
	
    g_assert (PEONY_IS_FILE (slot->viewed_file));

    directory = peony_directory_get_for_file (slot->viewed_file);
    g_assert (PEONY_IS_SEARCH_DIRECTORY (directory));

    peony_search_directory_set_query (PEONY_SEARCH_DIRECTORY (directory),
                                     query);
    if (reload)
    {
        peony_window_slot_reload (slot);
    }

    peony_directory_unref (directory);
}


static void
peony_navigation_window_slot_update_query_editor (PeonyWindowSlot *slot)
{
    PeonyDirectory *directory;
    PeonySearchDirectory *search_directory;
    PeonyQuery *query;
    GtkWidget *query_editor;

    g_assert (slot->pane->window != NULL);

    query_editor = NULL;

    directory = peony_directory_get (slot->location);
    if (PEONY_IS_SEARCH_DIRECTORY (directory))
    {
        search_directory = PEONY_SEARCH_DIRECTORY (directory);

        if (peony_search_directory_is_saved_search (search_directory))
        {
            query_editor = peony_query_editor_new (TRUE,
                                                  peony_search_directory_is_indexed (search_directory));
        }
        else
        {
            query_editor = peony_query_editor_new_with_bar (FALSE,
                           peony_search_directory_is_indexed (search_directory),
                           slot->pane->window->details->active_pane->active_slot == slot,
                           PEONY_SEARCH_BAR (PEONY_NAVIGATION_WINDOW_PANE (slot->pane)->search_bar),
                           slot);
        }
    }

    slot->query_editor = PEONY_QUERY_EDITOR (query_editor);

    if (query_editor != NULL)
    {
        g_signal_connect_object (query_editor, "changed",
                                 G_CALLBACK (query_editor_changed_callback), slot, 0);

        query = peony_search_directory_get_query (search_directory);
        if (query != NULL)
        {
        	set_query_duplicate(query,get_search_duplicate(search_directory));
            peony_query_editor_set_query (PEONY_QUERY_EDITOR (query_editor),
                                         query);
            g_object_unref (query);
        }
        else
        {
            peony_query_editor_set_default_query (PEONY_QUERY_EDITOR (query_editor));
        }

        peony_window_slot_add_extra_location_widget (slot, query_editor);
        gtk_widget_show (query_editor);
        peony_query_editor_grab_focus (PEONY_QUERY_EDITOR (query_editor));
    }

    peony_directory_unref (directory);
}

static void
peony_navigation_window_slot_active (PeonyWindowSlot *slot)
{
    PeonyNavigationWindow *window;
    PeonyNavigationWindowPane *pane;
    int page_num;

    pane = PEONY_NAVIGATION_WINDOW_PANE (slot->pane);
    window = PEONY_NAVIGATION_WINDOW (slot->pane->window);

    page_num = gtk_notebook_page_num (GTK_NOTEBOOK (pane->notebook),
                                      slot->content_box);
    g_assert (page_num >= 0);

    gtk_notebook_set_current_page (GTK_NOTEBOOK (pane->notebook), page_num);

    EEL_CALL_PARENT (PEONY_WINDOW_SLOT_CLASS, active, (slot));

    if (slot->viewed_file != NULL)
    {
        peony_navigation_window_load_extension_toolbar_items (window);
    }
}

static void
peony_navigation_window_slot_dispose (GObject *object)
{
    PeonyNavigationWindowSlot *slot;

    slot = PEONY_NAVIGATION_WINDOW_SLOT (object);

    peony_navigation_window_slot_clear_forward_list (slot);
    peony_navigation_window_slot_clear_back_list (slot);

    G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
peony_navigation_window_slot_init (PeonyNavigationWindowSlot *slot)
{
}

static void
peony_navigation_window_slot_class_init (PeonyNavigationWindowSlotClass *class)
{
    PEONY_WINDOW_SLOT_CLASS (class)->active = peony_navigation_window_slot_active;
    PEONY_WINDOW_SLOT_CLASS (class)->update_query_editor = peony_navigation_window_slot_update_query_editor;

    G_OBJECT_CLASS (class)->dispose = peony_navigation_window_slot_dispose;
}

