/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-file-drag.c - Drag & drop handling code that operated on
   PeonyFile objects.

   Copyright (C) 2000 Eazel, Inc.

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

   Authors: Pavel Cisler <pavel@eazel.com>,
*/

#include <config.h>
#include "peony-file-dnd.h"
#include "peony-desktop-icon-file.h"

#include "peony-dnd.h"
#include "peony-directory.h"
#include "peony-file-utilities.h"
#include <string.h>

static gboolean
peony_drag_can_accept_files (PeonyFile *drop_target_item)
{
    PeonyDirectory *directory;

    if (peony_file_is_directory (drop_target_item))
    {
        gboolean res;

        /* target is a directory, accept if editable */
        directory = peony_directory_get_for_file (drop_target_item);
        res = peony_directory_is_editable (directory);
        peony_directory_unref (directory);
        return res;
    }

    if (PEONY_IS_DESKTOP_ICON_FILE (drop_target_item))
    {
        return TRUE;
    }

    /* All Peony links are assumed to be links to directories.
     * Therefore, they all can accept drags, like all other
     * directories to. As with other directories, there can be
     * errors when the actual copy is attempted due to
     * permissions.
     */
    if (peony_file_is_peony_link (drop_target_item))
    {
        return TRUE;
    }

    if (peony_is_engrampa_installed () &&
            peony_file_is_archive (drop_target_item))
    {
        return TRUE;
    }

    return FALSE;
}

gboolean
peony_drag_can_accept_item (PeonyFile *drop_target_item,
                           const char *item_uri)
{
    if (peony_file_matches_uri (drop_target_item, item_uri))
    {
        /* can't accept itself */
        return FALSE;
    }

    return peony_drag_can_accept_files (drop_target_item);
}

gboolean
peony_drag_can_accept_items (PeonyFile *drop_target_item,
                            const GList *items)
{
    int max;

    if (drop_target_item == NULL)
        return FALSE;

    g_assert (PEONY_IS_FILE (drop_target_item));

    /* Iterate through selection checking if item will get accepted by the
     * drop target. If more than 100 items selected, return an over-optimisic
     * result
     */
    for (max = 100; items != NULL && max >= 0; items = items->next, max--)
    {
        if (!peony_drag_can_accept_item (drop_target_item,
                                        ((PeonyDragSelectionItem *)items->data)->uri))
        {
            return FALSE;
        }
    }

    return TRUE;
}

gboolean
peony_drag_can_accept_info (PeonyFile *drop_target_item,
                           PeonyIconDndTargetType drag_type,
                           const GList *items)
{
    switch (drag_type)
    {
    case PEONY_ICON_DND_UKUI_ICON_LIST:
        return peony_drag_can_accept_items (drop_target_item, items);

    case PEONY_ICON_DND_URI_LIST:
    case PEONY_ICON_DND_NETSCAPE_URL:
    case PEONY_ICON_DND_TEXT:
        return peony_drag_can_accept_files (drop_target_item);

    case PEONY_ICON_DND_XDNDDIRECTSAVE:
    case PEONY_ICON_DND_RAW:
        return peony_drag_can_accept_files (drop_target_item); /* Check if we can accept files at this location */

    case PEONY_ICON_DND_KEYWORD:
        return TRUE;

    case PEONY_ICON_DND_ROOTWINDOW_DROP:
        return FALSE;

        /* TODO return TRUE for folders as soon as drop handling is implemented */
    case PEONY_ICON_DND_COLOR:
    case PEONY_ICON_DND_BGIMAGE:
    case PEONY_ICON_DND_RESET_BACKGROUND:
        return FALSE;

    default:
        g_assert_not_reached ();
        return FALSE;
    }
}

void
peony_drag_file_receive_dropped_keyword (PeonyFile *file,
                                        const char *keyword)
{
    GList *keywords, *word;

    g_return_if_fail (PEONY_IS_FILE (file));
    g_return_if_fail (keyword != NULL);

    /* special case the erase emblem */
    if (strcmp (keyword, PEONY_FILE_DND_ERASE_KEYWORD) == 0)
    {
        keywords = NULL;
    }
    else
    {
        keywords = peony_file_get_keywords (file);
        word = g_list_find_custom (keywords, keyword, (GCompareFunc) strcmp);
        if (word == NULL)
        {
            keywords = g_list_prepend (keywords, g_strdup (keyword));
        }
        else
        {
            keywords = g_list_remove_link (keywords, word);
            g_free (word->data);
            g_list_free_1 (word);
        }
    }

    peony_file_set_keywords (file, keywords);
    g_list_free_full (keywords, g_free);
}
