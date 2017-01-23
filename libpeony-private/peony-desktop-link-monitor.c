/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-desktop-link-monitor.c: singleton thatn manages the links

   Copyright (C) 2003 Red Hat, Inc.

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

#include <config.h>
#include "peony-desktop-link-monitor.h"
#include "peony-desktop-link.h"
#include "peony-desktop-icon-file.h"
#include "peony-directory.h"
#include "peony-desktop-directory.h"
#include "peony-global-preferences.h"

#include <eel/eel-debug.h>
#include <eel/eel-gtk-macros.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-vfs-extensions.h>
#include <eel/eel-stock-dialogs.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
#include <libpeony-private/peony-trash-monitor.h>
#include <string.h>

struct PeonyDesktopLinkMonitorDetails
{
    GVolumeMonitor *volume_monitor;
    PeonyDirectory *desktop_dir;

    PeonyDesktopLink *home_link;
    PeonyDesktopLink *computer_link;
    PeonyDesktopLink *trash_link;
    PeonyDesktopLink *network_link;

    gulong mount_id;
    gulong unmount_id;
    gulong changed_id;

    GList *mount_links;
};


static void peony_desktop_link_monitor_init       (gpointer              object,
        gpointer              klass);
static void peony_desktop_link_monitor_class_init (gpointer              klass);

EEL_CLASS_BOILERPLATE (PeonyDesktopLinkMonitor,
                       peony_desktop_link_monitor,
                       G_TYPE_OBJECT)

static PeonyDesktopLinkMonitor *the_link_monitor = NULL;

static void
destroy_desktop_link_monitor (void)
{
    if (the_link_monitor != NULL)
    {
        g_object_unref (the_link_monitor);
    }
}

PeonyDesktopLinkMonitor *
peony_desktop_link_monitor_get (void)
{
    if (the_link_monitor == NULL)
    {
        g_object_new (PEONY_TYPE_DESKTOP_LINK_MONITOR, NULL);
        eel_debug_call_at_shutdown (destroy_desktop_link_monitor);
    }
    return the_link_monitor;
}

static gboolean
volume_file_name_used (PeonyDesktopLinkMonitor *monitor,
                       const char *name)
{
    GList *l;
    char *other_name;
    gboolean same;

    for (l = monitor->details->mount_links; l != NULL; l = l->next)
    {
        other_name = peony_desktop_link_get_file_name (l->data);
        same = strcmp (name, other_name) == 0;
        g_free (other_name);

        if (same)
        {
            return TRUE;
        }
    }

    return FALSE;
}

char *
peony_desktop_link_monitor_make_filename_unique (PeonyDesktopLinkMonitor *monitor,
        const char *filename)
{
    char *unique_name;
    int i;

    i = 2;
    unique_name = g_strdup (filename);
    while (volume_file_name_used (monitor, unique_name))
    {
        g_free (unique_name);
        unique_name = g_strdup_printf ("%s.%d", filename, i++);
    }
    return unique_name;
}

static gboolean
has_mount (PeonyDesktopLinkMonitor *monitor,
           GMount                     *mount)
{
    gboolean ret;
    GMount *other_mount;
    GList *l;

    ret = FALSE;

    for (l = monitor->details->mount_links; l != NULL; l = l->next)
    {
        other_mount = peony_desktop_link_get_mount (l->data);
        if (mount == other_mount)
        {
            g_object_unref (other_mount);
            ret = TRUE;
            break;
        }
        g_object_unref (other_mount);
    }

    return ret;
}

static void
create_mount_link (PeonyDesktopLinkMonitor *monitor,
                   GMount *mount)
{
    PeonyDesktopLink *link;

    if (has_mount (monitor, mount))
        return;

    if ((!g_mount_is_shadowed (mount)) &&
            g_settings_get_boolean (peony_desktop_preferences, PEONY_PREFERENCES_DESKTOP_VOLUMES_VISIBLE))
    {
        link = peony_desktop_link_new_from_mount (mount);
        monitor->details->mount_links = g_list_prepend (monitor->details->mount_links, link);
    }
}

static void
remove_mount_link (PeonyDesktopLinkMonitor *monitor,
                   GMount *mount)
{
    GList *l;
    PeonyDesktopLink *link;
    GMount *other_mount;

    link = NULL;
    for (l = monitor->details->mount_links; l != NULL; l = l->next)
    {
        other_mount = peony_desktop_link_get_mount (l->data);
        if (mount == other_mount)
        {
            g_object_unref (other_mount);
            link = l->data;
            break;
        }
        g_object_unref (other_mount);
    }

    if (link)
    {
        monitor->details->mount_links = g_list_remove (monitor->details->mount_links, link);
        g_object_unref (link);
    }
}



static void
mount_added_callback (GVolumeMonitor *volume_monitor,
                      GMount *mount,
                      PeonyDesktopLinkMonitor *monitor)
{
    create_mount_link (monitor, mount);
}


static void
mount_removed_callback (GVolumeMonitor *volume_monitor,
                        GMount *mount,
                        PeonyDesktopLinkMonitor *monitor)
{
    remove_mount_link (monitor, mount);
}

static void
mount_changed_callback (GVolumeMonitor *volume_monitor,
                        GMount *mount,
                        PeonyDesktopLinkMonitor *monitor)
{
    /* TODO: update the mount with other details */

    /* remove a mount if it goes into the shadows */
    if (g_mount_is_shadowed (mount) && has_mount (monitor, mount))
    {
        remove_mount_link (monitor, mount);
    }
}

static void
update_link_visibility (PeonyDesktopLinkMonitor *monitor,
                        PeonyDesktopLink       **link_ref,
                        PeonyDesktopLinkType     link_type,
                        const char                 *preference_key)
{
    if (g_settings_get_boolean (peony_desktop_preferences, preference_key))
    {
        if (*link_ref == NULL)
        {
            *link_ref = peony_desktop_link_new (link_type);
        }
    }
    else
    {
        if (*link_ref != NULL)
        {
            g_object_unref (*link_ref);
            *link_ref = NULL;
        }
    }
}

static void
desktop_home_visible_changed (gpointer callback_data)
{
    PeonyDesktopLinkMonitor *monitor;

    monitor = PEONY_DESKTOP_LINK_MONITOR (callback_data);

    update_link_visibility (PEONY_DESKTOP_LINK_MONITOR (monitor),
                            &monitor->details->home_link,
                            PEONY_DESKTOP_LINK_HOME,
                            PEONY_PREFERENCES_DESKTOP_HOME_VISIBLE);
}

static void
desktop_computer_visible_changed (gpointer callback_data)
{
    PeonyDesktopLinkMonitor *monitor;

    monitor = PEONY_DESKTOP_LINK_MONITOR (callback_data);

    update_link_visibility (PEONY_DESKTOP_LINK_MONITOR (callback_data),
                            &monitor->details->computer_link,
                            PEONY_DESKTOP_LINK_COMPUTER,
                            PEONY_PREFERENCES_DESKTOP_COMPUTER_VISIBLE);
}

static void
desktop_trash_visible_changed (gpointer callback_data)
{
    PeonyDesktopLinkMonitor *monitor;

    monitor = PEONY_DESKTOP_LINK_MONITOR (callback_data);

    update_link_visibility (PEONY_DESKTOP_LINK_MONITOR (callback_data),
                            &monitor->details->trash_link,
                            PEONY_DESKTOP_LINK_TRASH,
                            PEONY_PREFERENCES_DESKTOP_TRASH_VISIBLE);
}

static void
desktop_network_visible_changed (gpointer callback_data)
{
    PeonyDesktopLinkMonitor *monitor;

    monitor = PEONY_DESKTOP_LINK_MONITOR (callback_data);

    update_link_visibility (PEONY_DESKTOP_LINK_MONITOR (callback_data),
                            &monitor->details->network_link,
                            PEONY_DESKTOP_LINK_NETWORK,
                            PEONY_PREFERENCES_DESKTOP_NETWORK_VISIBLE);
}

static void
desktop_volumes_visible_changed (gpointer callback_data)
{
    PeonyDesktopLinkMonitor *monitor;
    GList *l, *mounts;

    monitor = PEONY_DESKTOP_LINK_MONITOR (callback_data);

    if (g_settings_get_boolean (peony_desktop_preferences, PEONY_PREFERENCES_DESKTOP_VOLUMES_VISIBLE))
    {
        if (monitor->details->mount_links == NULL)
        {
            mounts = g_volume_monitor_get_mounts (monitor->details->volume_monitor);
            for (l = mounts; l != NULL; l = l->next)
            {
                create_mount_link (monitor, l->data);
                g_object_unref (l->data);
            }
            g_list_free (mounts);
        }
    }
    else
    {
        g_list_foreach (monitor->details->mount_links, (GFunc)g_object_unref, NULL);
        g_list_free (monitor->details->mount_links);
        monitor->details->mount_links = NULL;
    }
}

static void
create_link_and_add_preference (PeonyDesktopLink   **link_ref,
                                PeonyDesktopLinkType link_type,
                                const char             *preference_key,
                                GCallback               callback,
                                gpointer                callback_data)
{
    char *detailed_signal;

    if (g_settings_get_boolean (peony_desktop_preferences, preference_key))
    {
        *link_ref = peony_desktop_link_new (link_type);
    }

    detailed_signal = g_strconcat ("changed::", preference_key, NULL);
    g_signal_connect_swapped (peony_desktop_preferences,
                              detailed_signal,
                              callback, callback_data);
}

static void
peony_desktop_link_monitor_init (gpointer object, gpointer klass)
{
    PeonyDesktopLinkMonitor *monitor;
    GList *l, *mounts;
    GMount *mount;

    monitor = PEONY_DESKTOP_LINK_MONITOR (object);

    the_link_monitor = monitor;

    monitor->details = g_new0 (PeonyDesktopLinkMonitorDetails, 1);

    monitor->details->volume_monitor = g_volume_monitor_get ();

    /* We keep around a ref to the desktop dir */
    monitor->details->desktop_dir = peony_directory_get_by_uri (EEL_DESKTOP_URI);

    /* Default links */

    create_link_and_add_preference (&monitor->details->home_link,
                                    PEONY_DESKTOP_LINK_HOME,
                                    PEONY_PREFERENCES_DESKTOP_HOME_VISIBLE,
                                    G_CALLBACK (desktop_home_visible_changed),
                                    monitor);

    create_link_and_add_preference (&monitor->details->computer_link,
                                    PEONY_DESKTOP_LINK_COMPUTER,
                                    PEONY_PREFERENCES_DESKTOP_COMPUTER_VISIBLE,
                                    G_CALLBACK (desktop_computer_visible_changed),
                                    monitor);

    create_link_and_add_preference (&monitor->details->trash_link,
                                    PEONY_DESKTOP_LINK_TRASH,
                                    PEONY_PREFERENCES_DESKTOP_TRASH_VISIBLE,
                                    G_CALLBACK (desktop_trash_visible_changed),
                                    monitor);

    create_link_and_add_preference (&monitor->details->network_link,
                                    PEONY_DESKTOP_LINK_NETWORK,
                                    PEONY_PREFERENCES_DESKTOP_NETWORK_VISIBLE,
                                    G_CALLBACK (desktop_network_visible_changed),
                                    monitor);

    /* Mount links */

    mounts = g_volume_monitor_get_mounts (monitor->details->volume_monitor);
    for (l = mounts; l != NULL; l = l->next)
    {
        mount = l->data;
        create_mount_link (monitor, mount);
        g_object_unref (mount);
    }
    g_list_free (mounts);

    g_signal_connect_swapped (peony_desktop_preferences,
                              "changed::" PEONY_PREFERENCES_DESKTOP_VOLUMES_VISIBLE,
                              G_CALLBACK (desktop_volumes_visible_changed),
                              monitor);

    monitor->details->mount_id =
        g_signal_connect_object (monitor->details->volume_monitor, "mount_added",
                                 G_CALLBACK (mount_added_callback), monitor, 0);
    monitor->details->unmount_id =
        g_signal_connect_object (monitor->details->volume_monitor, "mount_removed",
                                 G_CALLBACK (mount_removed_callback), monitor, 0);
    monitor->details->changed_id =
        g_signal_connect_object (monitor->details->volume_monitor, "mount_changed",
                                 G_CALLBACK (mount_changed_callback), monitor, 0);

}

static void
remove_link_and_preference (PeonyDesktopLink       **link_ref,
                            const char             *preference_key,
                            GCallback               callback,
                            gpointer                callback_data)
{
    if (*link_ref != NULL)
    {
        g_object_unref (*link_ref);
        *link_ref = NULL;
    }

    g_signal_handlers_disconnect_by_func (peony_desktop_preferences,
                                          callback, callback_data);
}

static void
desktop_link_monitor_finalize (GObject *object)
{
    PeonyDesktopLinkMonitor *monitor;

    monitor = PEONY_DESKTOP_LINK_MONITOR (object);

    g_object_unref (monitor->details->volume_monitor);

    /* Default links */

    remove_link_and_preference (&monitor->details->home_link,
                                PEONY_PREFERENCES_DESKTOP_HOME_VISIBLE,
                                G_CALLBACK (desktop_home_visible_changed),
                                monitor);

    remove_link_and_preference (&monitor->details->computer_link,
                                PEONY_PREFERENCES_DESKTOP_COMPUTER_VISIBLE,
                                G_CALLBACK (desktop_computer_visible_changed),
                                monitor);

    remove_link_and_preference (&monitor->details->trash_link,
                                PEONY_PREFERENCES_DESKTOP_TRASH_VISIBLE,
                                G_CALLBACK (desktop_trash_visible_changed),
                                monitor);

    remove_link_and_preference (&monitor->details->network_link,
                                PEONY_PREFERENCES_DESKTOP_NETWORK_VISIBLE,
                                G_CALLBACK (desktop_network_visible_changed),
                                monitor);

    /* Mounts */

    g_list_foreach (monitor->details->mount_links, (GFunc)g_object_unref, NULL);
    g_list_free (monitor->details->mount_links);
    monitor->details->mount_links = NULL;

    peony_directory_unref (monitor->details->desktop_dir);
    monitor->details->desktop_dir = NULL;

    g_signal_handlers_disconnect_by_func (peony_desktop_preferences,
                                          desktop_volumes_visible_changed,
                                          monitor);

    if (monitor->details->mount_id != 0)
    {
        g_source_remove (monitor->details->mount_id);
    }
    if (monitor->details->unmount_id != 0)
    {
        g_source_remove (monitor->details->unmount_id);
    }
    if (monitor->details->changed_id != 0)
    {
        g_source_remove (monitor->details->changed_id);
    }

    g_free (monitor->details);

    EEL_CALL_PARENT (G_OBJECT_CLASS, finalize, (object));
}

static void
peony_desktop_link_monitor_class_init (gpointer klass)
{
    GObjectClass *object_class;

    object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = desktop_link_monitor_finalize;

}
