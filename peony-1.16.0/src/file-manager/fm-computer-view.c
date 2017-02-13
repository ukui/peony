/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-empty-view.c - implementation of empty view of directory.

   Copyright (C) 2006 Free Software Foundation, Inc.

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

   Authors: Christian Neumair <chris@mate-de.org>
*/

#include <config.h>
#include "fm-computer-view.h"

#include <string.h>
#include <libpeony-private/peony-file-utilities.h>
#include <libpeony-private/peony-ui-utilities.h>
#include <libpeony-private/peony-view.h>
#include <libpeony-private/peony-view-factory.h>
#include <libpeony-private/peony-file.h>
#include <libpeony-private/peony-icon-container.h>
#include <libpeony-private/peony-clipboard.h>
#include <libpeony-private/peony-icon-info.h>
#include <libpeony-private/peony-icon-private.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-gtk-macros.h>
#include <eel/eel-vfs-extensions.h>



#include <eel/eel-gdk-pixbuf-extensions.h>
#include <glib/gi18n.h>
#include <libpeony-private/peony-dnd.h>
#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gtk/gtkbox.h>
#include <gdk/gdkx.h>
#include <gio/gdesktopappinfo.h>
#include <X11/XKBlib.h>
#include <gdk/gdkkeysyms.h>

#include <eel/eel-glib-extensions.h>

#include "fm-icon-view.h"
#include "fm-computer-container.h"
#include "fm-desktop-icon-view.h"

#include "fm-actions.h"
#include "fm-desktop-icon-view.h"
#include "fm-error-reporting.h"
#include <stdlib.h>
#include <eel/eel-background.h>
#include <eel/eel-gtk-extensions.h>

#include <eel/eel-stock-dialogs.h>
#include <eel/eel-string.h>
#include <eel/eel-vfs-extensions.h>
#include <errno.h>
#include <fcntl.h>
#include <libpeony-private/peony-clipboard-monitor.h>

#include <libpeony-private/peony-global-preferences.h>
#include <libpeony-private/peony-icon-container.h>
#include <libpeony-private/peony-icon-dnd.h>
#include <libpeony-private/peony-link.h>
#include <libpeony-private/peony-metadata.h>


#include <libpeony-private/peony-desktop-icon-file.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "peony-audio-mime-types.h"



#define POPUP_PATH_ICON_APPEARANCE      "/selection/Icon Appearance Items"
#define DISK "disk"
#define MOVABLEDISK "movable-disk"
#define OTHER "other"
enum
{
    PROP_0,
    PROP_COMPACT
};

typedef struct
{
    const PeonyFileSortType sort_type;
    const char *metadata_text;
    const char *action;
    const char *menu_label;
    const char *menu_hint;
} SortCriterion;


struct FMComputerViewDetails
{
    int number_of_files;

    GtkWidget *popup;
    
    GList *icons_not_positioned;

    guint react_to_icon_change_idle_id;

    const SortCriterion *sort;
    gboolean sort_reversed;

    GtkActionGroup *icon_action_group;
    guint icon_merge_id;

    int audio_preview_timeout;
    PeonyFile *audio_preview_file;
    int audio_preview_child_watch;
    GPid audio_preview_child_pid;

    gboolean filter_by_screen;
    int num_screens;

    gboolean compact;

    gulong clipboard_handler_id;
    GVolumeMonitor *volume_monitor;
};
/* Note that the first item in this list is the default sort,
 * and that the items show up in the menu in the order they
 * appear in this list.
 */
static const SortCriterion sort_criteria[] =
{
    {
        PEONY_FILE_SORT_BY_DISPLAY_NAME,
        "name",
        "Sort by Name",
        N_("by _Name"),
        N_("Keep icons sorted by name in rows")
    },
    {
        PEONY_FILE_SORT_BY_SIZE,
        "size",
        "Sort by Size",
        N_("by _Size"),
        N_("Keep icons sorted by size in rows")
    },
    {
        PEONY_FILE_SORT_BY_TYPE,
        "type",
        "Sort by Type",
        N_("by _Type"),
        N_("Keep icons sorted by type in rows")
    },
    {
        PEONY_FILE_SORT_BY_MTIME,
        "modification date",
        "Sort by Modification Date",
        N_("by Modification _Date"),
        N_("Keep icons sorted by modification date in rows")
    },
    {
        PEONY_FILE_SORT_BY_EMBLEMS,
        "emblems",
        "Sort by Emblems",
        N_("by _Emblems"),
        N_("Keep icons sorted by emblems in rows")
    },
    {
        PEONY_FILE_SORT_BY_TRASHED_TIME,
        "trashed",
        "Sort by Trash Time",
        N_("by T_rash Time"),
        N_("Keep icons sorted by trash time in rows")
    }
};


static gboolean default_sort_in_reverse_order = FALSE;
static int preview_sound_auto_value;

static void                 fm_computer_view_set_directory_sort_by        (FMComputerView           *computer_view,
        PeonyFile         *file,
        const char           *sort_by);
static void                 fm_computer_view_set_zoom_level               (FMComputerView           *view,
        PeonyZoomLevel     new_level,
        gboolean              always_emit);
static void                 fm_computer_view_update_click_mode            (FMComputerView           *computer_view);
static void                 fm_computer_view_set_directory_tighter_layout (FMComputerView           *computer_view,
        PeonyFile         *file,
        gboolean              tighter_layout);
static gboolean             fm_computer_view_supports_manual_layout       (FMComputerView           *computer_view);
static gboolean             fm_computer_view_supports_scaling         (FMComputerView           *computer_view);
static void                 fm_computer_view_reveal_selection             (FMDirectoryView      *view);
static const SortCriterion *get_sort_criterion_by_sort_type           (PeonyFileSortType  sort_type);
static void                 set_sort_criterion_by_sort_type           (FMComputerView           *computer_view,
        PeonyFileSortType  sort_type);
static gboolean             set_sort_reversed                         (FMComputerView           *computer_view,
        gboolean              new_value);
static void                 switch_to_manual_layout                   (FMComputerView           *view);
static void                 preview_audio                             (FMComputerView           *computer_view,
        PeonyFile         *file,
        gboolean              start_flag);
static void                 update_layout_menus                       (FMComputerView           *view);
static PeonyFileSortType get_default_sort_order                    (PeonyFile         *file,
        gboolean             *reversed);
static void fm_computer_view_restore_default_zoom_level (FMDirectoryView *view);
static PeonyZoomLevel fm_computer_view_get_zoom_level (FMDirectoryView *view);
static char *fm_computer_view_get_directory_sort_by (FMComputerView *computer_view,
                                    PeonyFile *file);
static gboolean fm_computer_view_get_directory_tighter_layout (FMComputerView *computer_view,
        PeonyFile *file);
static const SortCriterion *get_sort_criterion_by_metadata_text (const char *metadata_text);
static void fm_computer_view_set_directory_sort_reversed (FMComputerView *computer_view,
        PeonyFile *file,
        gboolean sort_reversed);
static void set_columns_same_width (FMComputerView *computer_view);
static PeonyZoomLevel get_default_zoom_level (FMComputerView *computer_view);
static PeonyIconContainer *get_icon_container (FMComputerView *computer_view,char*container_name);
static gboolean fm_computer_view_supports_auto_layout (FMComputerView *view);
static gboolean fm_computer_view_get_directory_sort_reversed (FMComputerView *computer_view,
        PeonyFile *file);
static gboolean fm_computer_view_get_directory_keep_aligned (FMComputerView *computer_view,
        PeonyFile *file);

static void fm_icon_view_set_directory_keep_aligned (FMComputerView *computer_view,
        PeonyFile *file,
        gboolean keep_aligned);
static gboolean fm_computer_view_supports_keep_aligned (FMComputerView *view);
static void fm_computer_view_remove_file (FMDirectoryView *view, PeonyFile *file, PeonyDirectory *directory);



static void default_sort_order_changed_callback            (gpointer callback_data);
static void default_sort_in_reverse_order_changed_callback (gpointer callback_data);
static void default_zoom_level_changed_callback            (gpointer callback_data);
static void labels_beside_icons_changed_callback           (gpointer callback_data);
static void all_columns_same_width_changed_callback        (gpointer callback_data);

static GList *fm_computer_view_get_selection                   (FMDirectoryView   *view);
static GList *fm_computer_view_get_selection_for_file_transfer (FMDirectoryView   *view);
static void   fm_computer_view_scroll_to_file                  (PeonyView      *view,
        const char        *uri);
static void   fm_computer_view_iface_init                      (PeonyViewIface *iface);

G_DEFINE_TYPE_WITH_CODE (FMComputerView, fm_computer_view, FM_TYPE_DIRECTORY_VIEW,
                         G_IMPLEMENT_INTERFACE (PEONY_TYPE_VIEW,
                                 fm_computer_view_iface_init));

/* for EEL_CALL_PARENT */
#define parent_class fm_computer_view_parent_class


static void
#if GTK_CHECK_VERSION (3, 0, 0)
fm_computer_view_destroy (GtkWidget *object)
#else
fm_computer_view_destroy (GtkObject *object)
#endif
{
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (object);

    if (computer_view->details->react_to_icon_change_idle_id != 0)
    {
        g_source_remove (computer_view->details->react_to_icon_change_idle_id);
        computer_view->details->react_to_icon_change_idle_id = 0;
    }

    if (computer_view->details->clipboard_handler_id != 0)
    {
        g_signal_handler_disconnect (peony_clipboard_monitor_get (),
                                     computer_view->details->clipboard_handler_id);
        computer_view->details->clipboard_handler_id = 0;
    }

    /* kill any sound preview process that is ongoing */
    preview_audio (computer_view, NULL, FALSE);

    if (computer_view->details->icons_not_positioned)
    {
        peony_file_list_free (computer_view->details->icons_not_positioned);
        computer_view->details->icons_not_positioned = NULL;
    }

#if GTK_CHECK_VERSION (3, 0, 0)
    GTK_WIDGET_CLASS (fm_computer_view_parent_class)->destroy (object);
#else
    GTK_OBJECT_CLASS (fm_computer_view_parent_class)->destroy (object);
#endif
}

static void
fm_computer_view_finalize (GObject *object)
{
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (object);

    g_free (computer_view->details);

    g_signal_handlers_disconnect_by_func (peony_preferences,
                                          default_sort_order_changed_callback,
                                          computer_view);
    g_signal_handlers_disconnect_by_func (peony_preferences,
                                          default_sort_in_reverse_order_changed_callback,
                                          computer_view);
    g_signal_handlers_disconnect_by_func (peony_icon_view_preferences,
                                          default_zoom_level_changed_callback,
                                          computer_view);
    g_signal_handlers_disconnect_by_func (peony_icon_view_preferences,
                                          labels_beside_icons_changed_callback,
                                          computer_view);
    g_signal_handlers_disconnect_by_func (peony_compact_view_preferences,
                                          default_zoom_level_changed_callback,
                                          computer_view);
    g_signal_handlers_disconnect_by_func (peony_compact_view_preferences,
                                          all_columns_same_width_changed_callback,
                                          computer_view);

    G_OBJECT_CLASS (fm_computer_view_parent_class)->finalize (object);
}

static gboolean
get_stored_icon_position_callback (PeonyIconContainer *container,
                                   PeonyFile *file,
                                   PeonyIconPosition *position,
                                   FMComputerView *computer_view)
{
    char *position_string, *scale_string;
    gboolean position_good;
    char c;

    g_assert (PEONY_IS_ICON_CONTAINER (container));
    g_assert (PEONY_IS_FILE (file));
    g_assert (position != NULL);
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));

    if (!fm_computer_view_supports_manual_layout (computer_view))
    {
        return FALSE;
    }

    /* Get the current position of this icon from the metadata. */
    position_string = peony_file_get_metadata
                      (file, PEONY_METADATA_KEY_ICON_POSITION, "");
    position_good = sscanf
                    (position_string, " %d , %d %c",
                     &position->x, &position->y, &c) == 2;
    g_free (position_string);

    /* If it is the desktop directory, maybe the mate-libs metadata has information about it */

    /* Disable scaling if not on the desktop */
    if (fm_computer_view_supports_scaling (computer_view))
    {
        /* Get the scale of the icon from the metadata. */
        scale_string = peony_file_get_metadata
                       (file, PEONY_METADATA_KEY_ICON_SCALE, "1");
        position->scale = g_ascii_strtod (scale_string, NULL);
        if (errno != 0)
        {
            position->scale = 1.0;
        }

        g_free (scale_string);
    }
    else
    {
        position->scale = 1.0;
    }

    return position_good;
}

static void
real_set_sort_criterion (FMComputerView *computer_view,
                         const SortCriterion *sort,
                         gboolean clear)
{
    PeonyFile *file;

    file = fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (computer_view));

    if (clear)
    {
        peony_file_set_metadata (file,
                                PEONY_METADATA_KEY_ICON_VIEW_SORT_BY, NULL, NULL);
        peony_file_set_metadata (file,
                                PEONY_METADATA_KEY_ICON_VIEW_SORT_REVERSED, NULL, NULL);
        computer_view->details->sort =
            get_sort_criterion_by_sort_type (get_default_sort_order
                                             (file, &computer_view->details->sort_reversed));
    }
    else
    {
        /* Store the new sort setting. */
        fm_computer_view_set_directory_sort_by (computer_view,
                                            file,
                                            sort->metadata_text);
    }

    /* Update the layout menus to match the new sort setting. */
    update_layout_menus (computer_view);
}

static void
set_sort_criterion (FMComputerView *computer_view, const SortCriterion *sort)
{
    if (sort == NULL ||
            computer_view->details->sort == sort)
    {
        return;
    }

    computer_view->details->sort = sort;

    real_set_sort_criterion (computer_view, sort, FALSE);
}

static void
clear_sort_criterion (FMComputerView *computer_view)
{
    real_set_sort_criterion (computer_view, NULL, TRUE);
}

static void
action_stretch_callback (GtkAction *action,
                         gpointer callback_data)
{
    g_assert (FM_IS_COMPUTER_VIEW (callback_data));

    peony_icon_container_show_stretch_handles
    (get_icon_container (FM_COMPUTER_VIEW (callback_data),DISK));
    peony_icon_container_show_stretch_handles
    (get_icon_container (FM_COMPUTER_VIEW (callback_data),MOVABLEDISK));
    peony_icon_container_show_stretch_handles
    (get_icon_container (FM_COMPUTER_VIEW (callback_data),OTHER));


}

static void
action_unstretch_callback (GtkAction *action,
                           gpointer callback_data)
{
    g_assert (FM_IS_COMPUTER_VIEW (callback_data));

    peony_icon_container_unstretch
    (get_icon_container (FM_COMPUTER_VIEW (callback_data),DISK));
    peony_icon_container_unstretch
    (get_icon_container (FM_COMPUTER_VIEW (callback_data),MOVABLEDISK));
    peony_icon_container_unstretch
    (get_icon_container (FM_COMPUTER_VIEW (callback_data),OTHER));

}

static gboolean
get_default_directory_keep_aligned (void)
{
    return TRUE;
}

gboolean
fm_computer_view_is_compact (FMComputerView *view)
{
    return FALSE;
}

static void
fm_computer_view_clean_up (FMComputerView *computer_view)
{
    EEL_CALL_METHOD (FM_COMPUTER_VIEW_CLASS, computer_view, clean_up, (computer_view));
}

static void
fm_computer_view_real_clean_up (FMComputerView *computer_view)
{
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    gboolean saved_sort_reversed;

    icon_container = get_icon_container (computer_view,DISK);

    /* Hardwire Clean Up to always be by name, in forward order */
    saved_sort_reversed = computer_view->details->sort_reversed;

    set_sort_reversed (computer_view, FALSE);
    set_sort_criterion (computer_view, &sort_criteria[0]);

    peony_icon_container_sort (icon_container);
    peony_icon_container_freeze_icon_positions (icon_container);

    set_sort_reversed (computer_view, saved_sort_reversed);

    icon_container1 = get_icon_container (computer_view,MOVABLEDISK);

    /* Hardwire Clean Up to always be by name, in forward order */
    saved_sort_reversed = computer_view->details->sort_reversed;

    set_sort_reversed (computer_view, FALSE);
    set_sort_criterion (computer_view, &sort_criteria[0]);

    peony_icon_container_sort (icon_container1);
    peony_icon_container_freeze_icon_positions (icon_container1);

    set_sort_reversed (computer_view, saved_sort_reversed);

    
    icon_container2 = get_icon_container (computer_view,OTHER);
    
    /* Hardwire Clean Up to always be by name, in forward order */
    saved_sort_reversed = computer_view->details->sort_reversed;

    set_sort_reversed (computer_view, FALSE);
    set_sort_criterion (computer_view, &sort_criteria[0]);

    peony_icon_container_sort (icon_container2);
    peony_icon_container_freeze_icon_positions (icon_container2);

    set_sort_reversed (computer_view, saved_sort_reversed);
}

static void
action_clean_up_callback (GtkAction *action, gpointer callback_data)
{
    fm_computer_view_clean_up (FM_COMPUTER_VIEW (callback_data));
}

static gboolean
fm_computer_view_get_directory_auto_layout (FMComputerView *computer_view,
                                        PeonyFile *file)
{
    if (!fm_computer_view_supports_auto_layout (computer_view))
    {
        return FALSE;
    }

    if (!fm_computer_view_supports_manual_layout (computer_view))
    {
        return TRUE;
    }

    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, computer_view,
            get_directory_auto_layout, (computer_view, file));
}

static gboolean
should_show_file_on_screen (FMDirectoryView *view, PeonyFile *file)
{
    char *screen_string;
    int screen_num;
    FMComputerView *computer_view;
    GdkScreen *screen;

    computer_view = FM_COMPUTER_VIEW (view);

    if (!fm_directory_view_should_show_file (view, file))
    {
        return FALSE;
    }

    /* Get the screen for this icon from the metadata. */
    screen_string = peony_file_get_metadata
                    (file, PEONY_METADATA_KEY_SCREEN, "0");
    screen_num = atoi (screen_string);
    g_free (screen_string);
    screen = gtk_widget_get_screen (GTK_WIDGET (view));

    if (screen_num != gdk_screen_get_number (screen) &&
            (screen_num < computer_view->details->num_screens ||
             gdk_screen_get_number (screen) > 0))
    {
        return FALSE;
    }

    return TRUE;
}



static void
fm_computer_view_add_file (FMDirectoryView *view, PeonyFile *file, PeonyDirectory *directory)
{
    static GTimer *timer = NULL;
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    GList *v[3],*e,*c,*c1,*c2,*w[3];
    GDrive *drive;
    GtkScrolledWindow *scrollwindow[3];
    GtkViewport *viewport1[3];
    FMComputerView *computer_view;
    char *display_name;
    computer_view = FM_COMPUTER_VIEW (view);
    
    g_assert (directory == fm_directory_view_get_model (view));

    GMount *mount = peony_file_get_mount(file);
    if(mount!=  NULL)
    {
        drive = g_mount_get_drive (mount);
        g_object_unref(mount);
    }
    else
    {
        drive =NULL;
    }
    
    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(computer_view));

    GtkWidget *box = gtk_bin_get_child(GTK_BIN(viewport));

    e = gtk_container_get_children (GTK_CONTAINER(box));

    GtkWidget *expander = GTK_EXPANDER(e->data);
    GtkWidget *expander1 =GTK_EXPANDER(e->next->data);
    GtkWidget *expander2 =GTK_EXPANDER(e->next->next->data);
    c =gtk_container_get_children(GTK_CONTAINER(expander));
    c1 = gtk_container_get_children(GTK_CONTAINER(expander1));
    c2 = gtk_container_get_children(GTK_CONTAINER(expander2));

    viewport1[0] = GTK_VIEWPORT(c->data);
    viewport1[1] = GTK_VIEWPORT(c1->data);
    viewport1[2] = GTK_VIEWPORT(c2->data);
    v[0] = gtk_container_get_children(GTK_CONTAINER(viewport1[0]));
    v[1] = gtk_container_get_children(GTK_CONTAINER(viewport1[1]));
    v[2] = gtk_container_get_children(GTK_CONTAINER(viewport1[2]));
    icon_container = PEONY_ICON_CONTAINER(v[0]->data);
    icon_container1 = PEONY_ICON_CONTAINER(v[1]->data);
    icon_container2 = PEONY_ICON_CONTAINER(v[2]->data);

    if (computer_view->details->filter_by_screen &&
            !should_show_file_on_screen (view, file))
    {
        return;
    }
    if(drive!=NULL)
    {
        display_name = peony_file_get_display_name(file);
        if(!g_drive_can_eject(drive)||strcmp(display_name,"数据盘")==0)
        {
            if (fm_directory_view_get_loading (view) && peony_icon_container_is_empty (icon_container))
            {
                peony_icon_container_reset_scroll_region (icon_container);
            }
            if (peony_icon_container_add (icon_container,PEONY_ICON_CONTAINER_ICON_DATA (file)))
            {
                peony_file_ref (file);
            }
        }
        else
        {
            if (fm_directory_view_get_loading (view) && peony_icon_container_is_empty (icon_container1))
            {
                peony_icon_container_reset_scroll_region (icon_container1);
            }
            if (peony_icon_container_add (icon_container1,PEONY_ICON_CONTAINER_ICON_DATA (file)))
            {
                peony_file_ref (file);
            }
        }
        g_object_unref(drive);
    }
    else if(strcmp(peony_file_get_name(file),"root.link")==0)
    {
        if (fm_directory_view_get_loading (view) && peony_icon_container_is_empty (icon_container))
        {
            peony_icon_container_reset_scroll_region (icon_container);
        }
        if (peony_icon_container_add (icon_container,PEONY_ICON_CONTAINER_ICON_DATA (file)))
        {
            peony_file_ref (file);
        }
     }
    else
    {
        if (fm_directory_view_get_loading (view) && peony_icon_container_is_empty (icon_container2))
        {
            peony_icon_container_reset_scroll_region (icon_container2);
        }
            if (peony_icon_container_add (icon_container2,PEONY_ICON_CONTAINER_ICON_DATA (file)))
            {
                peony_file_ref (file);
            }
        }
}


static void
fm_computer_view_begin_loading (FMDirectoryView *view)
{
    FMComputerView *computer_view;
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    PeonyFile *file;
    int level;
    char *sort_name;
    GList *vb,*e,*c[3],*w[3],*v[3];
    GtkWidget *scrollwindow[3];
    GtkViewport *viewport1[3];
    computer_view = FM_COMPUTER_VIEW (view);

    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(computer_view));
    
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(viewport));

    e = gtk_container_get_children (GTK_CONTAINER(box));

    GtkWidget *expander = GTK_EXPANDER(e->data);
    GtkWidget *expander1 =GTK_EXPANDER(e->next->data);
    GtkWidget *expander2 =GTK_EXPANDER(e->next->next->data);
    const char *cc = gtk_expander_get_label(GTK_EXPANDER(expander));
    c[0] =gtk_container_get_children(GTK_CONTAINER(expander));
    c[1] = gtk_container_get_children(GTK_CONTAINER(expander1));
    c[2]= gtk_container_get_children(GTK_CONTAINER(expander2));

    viewport1[0] = GTK_VIEWPORT(c[0]->data);
    viewport1[1] = GTK_VIEWPORT(c[1]->data);
    viewport1[2] = GTK_VIEWPORT(c[2]->data);
    v[0] = gtk_container_get_children(GTK_CONTAINER(viewport1[0]));
    v[1] = gtk_container_get_children(GTK_CONTAINER(viewport1[1]));
    v[2] = gtk_container_get_children(GTK_CONTAINER(viewport1[2]));
    icon_container = PEONY_ICON_CONTAINER(v[0]->data);
    icon_container1 = PEONY_ICON_CONTAINER(v[1]->data);
    icon_container2 = PEONY_ICON_CONTAINER(v[2]->data);

    g_return_if_fail (FM_IS_COMPUTER_VIEW (view));

    file = fm_directory_view_get_directory_as_file (view);
    peony_icon_container_begin_loading ( icon_container);
    peony_icon_container_begin_loading ( icon_container1);
    peony_icon_container_begin_loading ( icon_container2);
    peony_icon_container_set_allow_moves (icon_container,
                                         fm_directory_view_get_allow_moves (view));
    peony_icon_container_set_allow_moves (icon_container1,
                                       fm_directory_view_get_allow_moves (view));
    
    peony_icon_container_set_allow_moves (icon_container2,
                                       fm_directory_view_get_allow_moves (view));

    /* kill any sound preview process that is ongoing */
    preview_audio (computer_view, NULL, FALSE);

    /* FIXME bugzilla.gnome.org 45060: Should use methods instead
     * of hardcoding desktop knowledge in here.
     */
    if (FM_IS_DESKTOP_ICON_VIEW (view))
    {
       peony_connect_desktop_background_to_settings (PEONY_ICON_CONTAINER (icon_container));
    }
   else
    {
        GdkDragAction default_action;

        if (peony_window_info_get_window_type (fm_directory_view_get_peony_window (view)) == PEONY_WINDOW_NAVIGATION)
        {
            default_action = PEONY_DND_ACTION_SET_AS_GLOBAL_BACKGROUND;
        }
        else
        {
            default_action = PEONY_DND_ACTION_SET_AS_FOLDER_BACKGROUND;
        }

        peony_connect_background_to_file_metadata (GTK_WIDGET(icon_container), file, default_action);
        peony_connect_background_to_file_metadata (GTK_WIDGET(icon_container1), file, default_action);
        peony_connect_background_to_file_metadata (GTK_WIDGET(icon_container2), file, default_action);
    }


    /* Set up the zoom level from the metadata. */
    if (fm_directory_view_supports_zooming (FM_DIRECTORY_VIEW (computer_view)))
    {
        if (computer_view->details->compact)
        {
            level = peony_file_get_integer_metadata
                    (file,
                     PEONY_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL,
                     get_default_zoom_level (computer_view));
        }
        else
        {
            level = peony_file_get_integer_metadata
                    (file,
                     PEONY_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL,
                     get_default_zoom_level (computer_view));
        }

        fm_computer_view_set_zoom_level (computer_view, level, TRUE);
    }

    /* Set the sort mode.
     * It's OK not to resort the icons because the
     * container doesn't have any icons at this point.
     */
    sort_name = fm_computer_view_get_directory_sort_by (computer_view, file);
    set_sort_criterion (computer_view, get_sort_criterion_by_metadata_text (sort_name));
    g_free (sort_name);

    /* Set the sort direction from the metadata. */
    set_sort_reversed (computer_view, fm_computer_view_get_directory_sort_reversed (computer_view, file));

    peony_icon_container_set_keep_aligned
    (get_icon_container (computer_view,DISK),
     fm_computer_view_get_directory_keep_aligned (computer_view, file));
    peony_icon_container_set_keep_aligned
    (get_icon_container (computer_view,MOVABLEDISK),
     fm_computer_view_get_directory_keep_aligned (computer_view, file));
    peony_icon_container_set_keep_aligned
    (get_icon_container (computer_view,OTHER),
     fm_computer_view_get_directory_keep_aligned (computer_view, file));
        
    set_columns_same_width (computer_view);

    /* We must set auto-layout last, because it invokes the layout_changed
     * callback, which works incorrectly if the other layout criteria are
     * not already set up properly (see bug 6500, e.g.)
     */
   peony_icon_container_set_auto_layout
    (get_icon_container (computer_view,DISK),
     fm_computer_view_get_directory_auto_layout (computer_view, file));
    peony_icon_container_set_auto_layout
     (get_icon_container (computer_view,MOVABLEDISK),
      fm_computer_view_get_directory_auto_layout (computer_view, file));
    peony_icon_container_set_auto_layout
     (get_icon_container (computer_view,OTHER),
      fm_computer_view_get_directory_auto_layout (computer_view, file));

    /* e.g. keep aligned may have changed */
    update_layout_menus (computer_view);
}


static void
set_tighter_layout (FMComputerView *computer_view, gboolean new_value)
{
    fm_computer_view_set_directory_tighter_layout (computer_view,
            fm_directory_view_get_directory_as_file
            (FM_DIRECTORY_VIEW (computer_view)),
            new_value);
    peony_icon_container_set_tighter_layout (get_icon_container (computer_view,DISK),
                                            new_value);
    peony_icon_container_set_tighter_layout (get_icon_container (computer_view,MOVABLEDISK),
                                            new_value);
    peony_icon_container_set_tighter_layout (get_icon_container (computer_view,OTHER),
                                            new_value);


}

static void
action_tighter_layout_callback (GtkAction *action,
                                gpointer user_data)
{
    g_assert (FM_IS_COMPUTER_VIEW (user_data));

    set_tighter_layout (FM_COMPUTER_VIEW (user_data),
                        gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}


static gboolean
fm_computer_view_using_auto_layout (FMComputerView *computer_view)
{
    return peony_icon_container_is_auto_layout
           (get_icon_container (computer_view,DISK));
}


static void
action_sort_radio_callback (GtkAction *action,
                            GtkRadioAction *current,
                            FMComputerView *view)
{
    PeonyFileSortType sort_type;

    sort_type = gtk_radio_action_get_current_value (current);

    /* Note that id might be a toggle item.
     * Ignore non-sort ids so that they don't cause sorting.
     */
    if (sort_type == PEONY_FILE_SORT_NONE)
    {
        switch_to_manual_layout (view);
    }
    else
    {
        set_sort_criterion_by_sort_type (view, sort_type);
    }
}

static gboolean
fm_computer_view_supports_auto_layout (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, view,
            supports_auto_layout, (view));
}

static gboolean
fm_computer_view_supports_tighter_layout (FMComputerView *view)
{
    return FALSE;//!fm_icon_view_is_compact (view);
}


static void
update_layout_menus (FMComputerView *view)
{
    gboolean is_auto_layout;
    GtkAction *action;
    const char *action_name;
    PeonyFile *file;

    if (view->details->icon_action_group == NULL)
    {
        return;
    }

    is_auto_layout = fm_computer_view_using_auto_layout (view);
    file = fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (view));

    if (fm_computer_view_supports_auto_layout (view))
    {
        /* Mark sort criterion. */
        action_name = is_auto_layout ? view->details->sort->action : FM_ACTION_MANUAL_LAYOUT;
        action = gtk_action_group_get_action (view->details->icon_action_group,
                                              action_name);
        gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

        action = gtk_action_group_get_action (view->details->icon_action_group,
                                              FM_ACTION_TIGHTER_LAYOUT);
       // gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
                     ///                 fm_computer_view_using_tighter_layout (view));
        gtk_action_set_sensitive (action, fm_computer_view_supports_tighter_layout (view));
        gtk_action_set_visible (action, fm_computer_view_supports_tighter_layout (view));

        action = gtk_action_group_get_action (view->details->icon_action_group,
                                              FM_ACTION_REVERSED_ORDER);
        gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
                                      view->details->sort_reversed);
        gtk_action_set_sensitive (action, is_auto_layout);

        action = gtk_action_group_get_action (view->details->icon_action_group,
                                              FM_ACTION_SORT_TRASH_TIME);

        if (file != NULL && peony_file_is_in_trash (file))
        {
            gtk_action_set_visible (action, TRUE);
        }
        else
        {
            gtk_action_set_visible (action, FALSE);
        }
    }

    action = gtk_action_group_get_action (view->details->icon_action_group,
                                          FM_ACTION_MANUAL_LAYOUT);
    gtk_action_set_visible (action,
                            fm_computer_view_supports_manual_layout (view));

    /* Clean Up is only relevant for manual layout */
    action = gtk_action_group_get_action (view->details->icon_action_group,
                                          FM_ACTION_CLEAN_UP);
    gtk_action_set_sensitive (action, !is_auto_layout);

    if (FM_IS_DESKTOP_ICON_VIEW (view))
    {
        gtk_action_set_label (action, _("_Organize Desktop by Name"));
    }

    action = gtk_action_group_get_action (view->details->icon_action_group,
                                          FM_ACTION_KEEP_ALIGNED);
    gtk_action_set_visible (action,
                            fm_computer_view_supports_keep_aligned (view));
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
                                 peony_icon_container_is_keep_aligned (get_icon_container (view,DISK)));
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
                                     peony_icon_container_is_keep_aligned (get_icon_container (view,MOVABLEDISK)));
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
                                     peony_icon_container_is_keep_aligned (get_icon_container (view,OTHER)));

    gtk_action_set_sensitive (action, !is_auto_layout);

}


static gboolean
fm_computer_view_real_get_directory_auto_layout (FMComputerView *computer_view,
        PeonyFile *file)
{


    return peony_file_get_boolean_metadata
           (file, PEONY_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT, TRUE);
}

static void
fm_computer_view_set_directory_auto_layout (FMComputerView *computer_view,
                                        PeonyFile *file,
                                        gboolean auto_layout)
{
    if (!fm_computer_view_supports_auto_layout (computer_view) ||
            !fm_computer_view_supports_manual_layout (computer_view))
    {
        return;
    }

    EEL_CALL_METHOD (FM_COMPUTER_VIEW_CLASS, computer_view,
                     set_directory_auto_layout, (computer_view, file, auto_layout));
}

static void
fm_computer_view_real_set_directory_auto_layout (FMComputerView *computer_view,
        PeonyFile *file,
        gboolean auto_layout)
{
    if (!fm_computer_view_supports_manual_layout (computer_view))
    {
        return;
    }

    peony_file_set_boolean_metadata
    (file, PEONY_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT,
     TRUE,
     auto_layout);
}


static void
list_covers (PeonyIconData *data, gpointer callback_data)
{
    GSList **file_list;

    file_list = callback_data;

    *file_list = g_slist_prepend (*file_list, data);
}

static void
unref_cover (PeonyIconData *data, gpointer callback_data)
{
    peony_file_unref (PEONY_FILE (data));
}

static void
fm_computer_view_clear (FMDirectoryView *view)
{
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    GSList *file_list,*file_list1,*file_list2;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (view));

    icon_container = get_icon_container (FM_COMPUTER_VIEW (view),DISK);
    icon_container1 = get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK);
    icon_container2 = get_icon_container (FM_COMPUTER_VIEW (view),OTHER);
    if (!icon_container)
        return;
    if (!icon_container1)
        return;
    if (!icon_container2)
        return;


    /* Clear away the existing icons. */
    file_list = NULL;
    file_list1 = NULL;
    file_list2 = NULL;

    peony_icon_container_for_each (icon_container, list_covers, &file_list);
    peony_icon_container_clear (icon_container);
    g_slist_foreach (file_list, (GFunc)unref_cover, NULL);
    g_slist_free (file_list);

    peony_icon_container_for_each (icon_container1, list_covers, &file_list1);
    peony_icon_container_clear (icon_container1);
    g_slist_foreach (file_list1, (GFunc)unref_cover, NULL);
    g_slist_free (file_list1);  

    peony_icon_container_for_each (icon_container2, list_covers, &file_list2);
    peony_icon_container_clear (icon_container2);
    g_slist_foreach (file_list2, (GFunc)unref_cover, NULL);
    g_slist_free (file_list2);

}


static void
fm_computer_view_flush_added_files (FMDirectoryView *view)
{
    peony_icon_container_layout_now (get_icon_container (FM_COMPUTER_VIEW (view),DISK));
    peony_icon_container_layout_now (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK));
    peony_icon_container_layout_now (get_icon_container (FM_COMPUTER_VIEW (view),OTHER));

}


static void
fm_computer_view_file_changed (FMDirectoryView *view, PeonyFile *file, PeonyDirectory *directory)
{
    FMComputerView *computer_view;
    GList *e,*vb,*c,*c1,*c2,*w[3],*v[3];
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    GtkWidget *scrollwindow[3];
    GtkViewport *viewport1[3];

    g_assert (directory == fm_directory_view_get_model (view));

    g_return_if_fail (view != NULL);
    computer_view = FM_COMPUTER_VIEW (view);
    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(computer_view));
    
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(viewport));
    //GtkWidget *vbox = gtk_bin_get_child(GTK_BIN(box));

    e = gtk_container_get_children (GTK_CONTAINER(box));

    GtkWidget *expander = GTK_EXPANDER(e->data);
    GtkWidget *expander1 =GTK_EXPANDER(e->next->data);
    GtkWidget *expander2 =GTK_EXPANDER(e->next->next->data);
    const char *cc = gtk_expander_get_label(GTK_EXPANDER(expander));
    c =gtk_container_get_children(GTK_CONTAINER(expander));
    c1 = gtk_container_get_children(GTK_CONTAINER(expander1));
    c2 = gtk_container_get_children(GTK_CONTAINER(expander2));

    viewport1[0] = GTK_VIEWPORT(c->data);
    viewport1[1] = GTK_VIEWPORT(c1->data);
    viewport1[2] = GTK_VIEWPORT(c2->data);
    v[0] = gtk_container_get_children(GTK_CONTAINER(viewport1[0]));
    v[1] = gtk_container_get_children(GTK_CONTAINER(viewport1[1]));
    v[2] = gtk_container_get_children(GTK_CONTAINER(viewport1[2]));
    icon_container = PEONY_ICON_CONTAINER(v[0]->data);
    icon_container1 = PEONY_ICON_CONTAINER(v[1]->data);
    icon_container2 = PEONY_ICON_CONTAINER(v[2]->data);

   if (!computer_view->details->filter_by_screen)
    {
        peony_icon_container_request_update
        (icon_container,
         PEONY_ICON_CONTAINER_ICON_DATA (file));
        peony_icon_container_request_update
        (icon_container1,
         PEONY_ICON_CONTAINER_ICON_DATA (file));
        peony_icon_container_request_update
        (icon_container2,
         PEONY_ICON_CONTAINER_ICON_DATA (file));

        return;
    }
   if (!should_show_file_on_screen (view, file))
    {
        fm_computer_view_remove_file (view, file, directory);
    }
    else
    {
        peony_icon_container_request_update
        (icon_container,
         PEONY_ICON_CONTAINER_ICON_DATA (file));
        peony_icon_container_request_update
        (icon_container1,
         PEONY_ICON_CONTAINER_ICON_DATA (file));
        peony_icon_container_request_update
        (icon_container2,
         PEONY_ICON_CONTAINER_ICON_DATA (file));


    }
}



static PeonyIconContainer *
get_icon_container (FMComputerView *computer_view,char *container_name)
{
    GList *v[3],*vb,*e,*c[3],*w[3];
    PeonyIconContainer *icon_container[3];
    GtkWidget *expander[3],*scrollwindow[3],*viewport1[3]; 
    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(computer_view));
    GtkWidget *widget = gtk_container_get_children(GTK_CONTAINER(computer_view));
    if(!viewport)
    {
        icon_container[0] = NULL;
        return icon_container[0];   
    }
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(viewport));
    
    //GtkWidget *vbox = gtk_bin_get_child(GTK_BIN(box));
    if(!box)
    {
        icon_container[0] = NULL;
        return icon_container[0];
    }
    e = gtk_container_get_children (GTK_CONTAINER(box));
    expander[0] = GTK_EXPANDER(e->data);
    expander[1] = GTK_EXPANDER(e->next->data);
    expander[2] = GTK_EXPANDER(e->next->next->data);
    const gchar *cc = gtk_expander_get_label(GTK_EXPANDER(expander[0]));
    c[0] =gtk_container_get_children(GTK_CONTAINER(expander[0]));
    c[1] =gtk_container_get_children(GTK_CONTAINER(expander[1]));
    c[2] =gtk_container_get_children(GTK_CONTAINER(expander[2]));

    viewport1[0] = GTK_VIEWPORT(c[0]->data);
    viewport1[1] = GTK_VIEWPORT(c[1]->data);
    viewport1[2] = GTK_VIEWPORT(c[2]->data);
    v[0] = gtk_container_get_children(GTK_CONTAINER(viewport1[0]));
    v[1] = gtk_container_get_children(GTK_CONTAINER(viewport1[1]));
    v[2] = gtk_container_get_children(GTK_CONTAINER(viewport1[2]));
    icon_container[0] = PEONY_ICON_CONTAINER(v[0]->data);
    icon_container[1] = PEONY_ICON_CONTAINER(v[1]->data);
    icon_container[2] = PEONY_ICON_CONTAINER(v[2]->data);
    if(strcmp(container_name,DISK)==0)
    return icon_container[0];
    if(strcmp(container_name,MOVABLEDISK)==0)
    return icon_container[1];
    if(strcmp(container_name,OTHER)==0)
    return icon_container[2];
    //return PEONY_ICON_CONTAINER (gtk_bin_get_child (GTK_BIN (computer_view)));
}

static gboolean default_directory_tighter_layout = FALSE;

static gboolean
get_default_directory_tighter_layout (void)
{
    static gboolean auto_storaged_added = FALSE;

    if (auto_storaged_added == FALSE)
    {
        auto_storaged_added = TRUE;
        eel_g_settings_add_auto_boolean (peony_icon_view_preferences,
                                         PEONY_PREFERENCES_ICON_VIEW_DEFAULT_USE_TIGHTER_LAYOUT,
                                         &default_directory_tighter_layout);
    }

    return default_directory_tighter_layout;
}

static char *
fm_computer_view_real_get_directory_sort_by (FMComputerView *computer_view,
        PeonyFile *file)
{
    const SortCriterion *default_sort_criterion;
    default_sort_criterion = get_sort_criterion_by_sort_type (get_default_sort_order (file, NULL));
    if( default_sort_criterion == NULL )
    {
        /*
        * default_sort_criterion will be NULL if default sort order is set to:
        * 'directory' or 'atime'
        * get_sort_criterion_by_sort_type() enumerates through 'sort_criteria' table
        * but this table doesn't have 'directory' and 'atime' items
        *
        * may 'sort_criteria' table should have those two items too?
        *
        * temporarily changing it to 'sort by display name'
        */
        default_sort_criterion = &sort_criteria[0];
    }
    g_return_val_if_fail (default_sort_criterion != NULL, NULL);

    return peony_file_get_metadata
           (file, PEONY_METADATA_KEY_ICON_VIEW_SORT_BY,
            default_sort_criterion->metadata_text);
}

static void
fm_computer_view_set_directory_tighter_layout (FMComputerView *computer_view,
        PeonyFile *file,
        gboolean tighter_layout)
{
    EEL_CALL_METHOD (FM_COMPUTER_VIEW_CLASS, computer_view,
                     set_directory_tighter_layout, (computer_view, file, tighter_layout));
}

static gboolean
real_supports_auto_layout (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return TRUE;
}

static gboolean
real_supports_scaling (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return FALSE;
}

static gboolean
real_supports_manual_layout (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

   return !fm_computer_view_is_compact (view);
}

static gboolean
real_supports_keep_aligned (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return FALSE;
}

static gboolean
real_supports_labels_beside_icons (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), TRUE);

    return TRUE;
}



static gboolean
set_sort_reversed (FMComputerView *computer_view, gboolean new_value)
{
    if (computer_view->details->sort_reversed == new_value)
    {
        return FALSE;
    }
    computer_view->details->sort_reversed = new_value;

    /* Store the new sort setting. */
    fm_computer_view_set_directory_sort_reversed (computer_view, fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (computer_view)), new_value);

    /* Update the layout menus to match the new sort-order setting. */
    update_layout_menus (computer_view);

    return TRUE;
}



static const SortCriterion *
get_sort_criterion_by_metadata_text (const char *metadata_text)
{
    guint i;

    /* Figure out what the new sort setting should be. */
    for (i = 0; i < G_N_ELEMENTS (sort_criteria); i++)
    {
        if (strcmp (sort_criteria[i].metadata_text, metadata_text) == 0)
        {
            return &sort_criteria[i];
        }
    }
    return NULL;
}

static const SortCriterion *
get_sort_criterion_by_sort_type (PeonyFileSortType sort_type)
{
    guint i;

    /* Figure out what the new sort setting should be. */
    for (i = 0; i < G_N_ELEMENTS (sort_criteria); i++)
    {
        if (sort_type == sort_criteria[i].sort_type)
        {
            return &sort_criteria[i];
        }
    }

    return NULL;
}

static PeonyZoomLevel default_zoom_level = PEONY_ZOOM_LEVEL_STANDARD;
static PeonyZoomLevel default_compact_zoom_level = PEONY_ZOOM_LEVEL_STANDARD;
#define DEFAULT_ZOOM_LEVEL(computer_view) computer_view->details->compact ? default_compact_zoom_level : default_zoom_level

static PeonyZoomLevel
get_default_zoom_level (FMComputerView *computer_view)
{
    static gboolean auto_storage_added = FALSE;

    if (!auto_storage_added)
    {
        auto_storage_added = TRUE;
        eel_g_settings_add_auto_enum (peony_icon_view_preferences,
                                      PEONY_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL,
                                      (int *) &default_zoom_level);
        default_zoom_level = PEONY_ZOOM_LEVEL_STANDARD;
        eel_g_settings_add_auto_enum (peony_compact_view_preferences,
                                      PEONY_PREFERENCES_COMPACT_VIEW_DEFAULT_ZOOM_LEVEL,
                                      (int *) &default_compact_zoom_level);
    }

    //return CLAMP (DEFAULT_ZOOM_LEVEL(computer_view), PEONY_ZOOM_LEVEL_SMALLEST, PEONY_ZOOM_LEVEL_LARGEST);
    return PEONY_ZOOM_LEVEL_STANDARD;
}

static void
set_labels_beside_icons (FMComputerView *computer_view)
{
    gboolean labels_beside;


        labels_beside = TRUE;

        if (labels_beside)
        {
            peony_icon_container_set_label_position
            (get_icon_container (computer_view,DISK),
             PEONY_ICON_LABEL_POSITION_BESIDE);
            peony_icon_container_set_label_position
            (get_icon_container (computer_view,MOVABLEDISK),
             PEONY_ICON_LABEL_POSITION_BESIDE);
            peony_icon_container_set_label_position
            (get_icon_container (computer_view,OTHER),
             PEONY_ICON_LABEL_POSITION_BESIDE);
        }
    
}

static void
set_columns_same_width (FMComputerView *computer_view)
{
    gboolean all_columns_same_width;

  // if (fm_computer_view_is_compact (computer_view))
    //{
      //  all_columns_same_width = g_settings_get_boolean (peony_compact_view_preferences, PEONY_PREFERENCES_COMPACT_VIEW_ALL_COLUMNS_SAME_WIDTH);
        //peony_icon_container_set_all_columns_same_width (get_icon_container (computer_view), all_columns_same_width);
    //}
}


static gboolean
fm_computer_view_supports_scaling (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, view,
            supports_scaling, (view));
}

static gboolean
fm_computer_view_supports_manual_layout (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, view,
            supports_manual_layout, (view));
}



static gboolean
fm_computer_view_get_directory_tighter_layout (FMComputerView *computer_view,
        PeonyFile *file)
{
    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, computer_view,
            get_directory_tighter_layout, (computer_view, file));
}



static void
fm_computer_view_set_directory_sort_reversed (FMComputerView *computer_view,
        PeonyFile *file,
        gboolean sort_reversed)
{
    if (!fm_computer_view_supports_auto_layout (computer_view))
    {
        return;
    }

    EEL_CALL_METHOD (FM_COMPUTER_VIEW_CLASS, computer_view,
                     set_directory_sort_reversed,
                     (computer_view, file, sort_reversed));
}

static void
fm_computer_view_real_set_directory_sort_by (FMComputerView *computer_view,
        PeonyFile *file,
        const char *sort_by)
{
    const SortCriterion *default_sort_criterion;
    default_sort_criterion = get_sort_criterion_by_sort_type (get_default_sort_order (file, NULL));
    g_return_if_fail (default_sort_criterion != NULL);

    peony_file_set_metadata
    (file, PEONY_METADATA_KEY_ICON_VIEW_SORT_BY,
     default_sort_criterion->metadata_text,
     sort_by);
}


static char *
fm_computer_view_get_directory_sort_by (FMComputerView *computer_view,
                                    PeonyFile *file)
{
    if (!fm_computer_view_supports_auto_layout (computer_view))
    {
        return g_strdup ("name");
    }

    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, computer_view,
            get_directory_sort_by, (computer_view, file));
}

static PeonyFileSortType default_sort_order = PEONY_FILE_SORT_BY_DISPLAY_NAME;

static PeonyFileSortType
get_default_sort_order (PeonyFile *file, gboolean *reversed)
{
    static gboolean auto_storaged_added = FALSE;
    PeonyFileSortType retval;

    if (auto_storaged_added == FALSE)
    {
        auto_storaged_added = TRUE;
        eel_g_settings_add_auto_enum (peony_preferences,
                                      PEONY_PREFERENCES_DEFAULT_SORT_ORDER,
                                      (int *) &default_sort_order);
        eel_g_settings_add_auto_boolean (peony_preferences,
                                         PEONY_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER,
                                         &default_sort_in_reverse_order);
    }

    retval = peony_file_get_default_sort_type (file, reversed);

    if (retval == PEONY_FILE_SORT_NONE)
    {

        if (reversed != NULL)
        {
            *reversed = default_sort_in_reverse_order;
        }

        retval = CLAMP (default_sort_order, PEONY_FILE_SORT_BY_DISPLAY_NAME,
                        PEONY_FILE_SORT_BY_EMBLEMS);
    }

    return retval;
}

static gboolean
fm_computer_view_real_get_directory_sort_reversed (FMComputerView *computer_view,
        PeonyFile *file)
{
    gboolean reversed;

    get_default_sort_order (file, &reversed);
    return peony_file_get_boolean_metadata
           (file,
            PEONY_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
            reversed);
}

static void
fm_computer_view_set_directory_sort_by (FMComputerView *computer_view,
                                    PeonyFile *file,
                                    const char *sort_by)
{
    if (!fm_computer_view_supports_auto_layout (computer_view))
    {
        return;
    }

    EEL_CALL_METHOD (FM_COMPUTER_VIEW_CLASS, computer_view,
                     set_directory_sort_by, (computer_view, file, sort_by));
}


static gboolean
fm_computer_view_get_directory_sort_reversed (FMComputerView *computer_view,
        PeonyFile *file)
{
    if (!fm_computer_view_supports_auto_layout (computer_view))
    {
        return FALSE;
    }

    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, computer_view,
            get_directory_sort_reversed, (computer_view, file));
}


static GtkWidget *
fm_computer_view_get_background_widget (FMDirectoryView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), NULL);

    //return GTK_WIDGET (get_icon_container (FM_COMPUTER_VIEW (view)));
    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(FM_COMPUTER_VIEW (view)));
    return viewport;
}


static void
fm_computer_view_real_set_directory_sort_reversed (FMComputerView *computer_view,
        PeonyFile *file,
        gboolean sort_reversed)
{
    gboolean reversed;

    get_default_sort_order (file, &reversed);
    peony_file_set_boolean_metadata
    (file,
     PEONY_METADATA_KEY_ICON_VIEW_SORT_REVERSED,
     reversed, sort_reversed);
}

static void
fm_computer_view_set_directory_keep_aligned (FMComputerView *computer_view,
        PeonyFile *file,
        gboolean keep_aligned)
{
    if (!fm_computer_view_supports_keep_aligned (computer_view))
    {
        return;
    }

    peony_file_set_boolean_metadata
    (file, PEONY_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED,
     get_default_directory_keep_aligned (),
     keep_aligned);
}



static GList *
fm_computer_view_get_selection (FMDirectoryView *view)
{
    GList *list,*node,*node1,*node2;
    list = NULL;
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), NULL);

    node = peony_icon_container_get_selection
           (get_icon_container (FM_COMPUTER_VIEW (view),DISK));
    node1 = peony_icon_container_get_selection
           (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK));
    node2 = peony_icon_container_get_selection
           (get_icon_container (FM_COMPUTER_VIEW (view),OTHER));
    while(node!=NULL)
        {
        list = g_list_append(list,node->data);
        node=node->next;
        }
    while(node1!=NULL)
        {
        list = g_list_append(list,node1->data);
        node1=node1->next;
        }
    while(node2!=NULL)
        {
        list = g_list_append(list,node2->data);
        node2=node2->next;
        }

    peony_file_list_ref (list);
    return list;
}

static gboolean
fm_computer_view_supports_keep_aligned (FMComputerView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return EEL_CALL_METHOD_WITH_RETURN_VALUE
           (FM_COMPUTER_VIEW_CLASS, view,
            supports_keep_aligned, (view));
}


static gboolean
fm_computer_view_get_directory_keep_aligned (FMComputerView *computer_view,
        PeonyFile *file)
{
    if (!fm_computer_view_supports_keep_aligned (computer_view))
    {
        return FALSE;
    }

    return  peony_file_get_boolean_metadata
            (file,
             PEONY_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED,
             get_default_directory_keep_aligned ());
}


static void
count_item (PeonyIconData *icon_data,
            gpointer callback_data)
{
    guint *count;

    count = callback_data;
    (*count)++;
}

static guint
fm_computer_view_get_item_count (FMDirectoryView *view)
{
    guint count;

    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), 0);

    count = 0;

    peony_icon_container_for_each
    (get_icon_container (FM_COMPUTER_VIEW (view),DISK),
     count_item, &count);
    peony_icon_container_for_each
    (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK),
     count_item, &count);
    peony_icon_container_for_each
    (get_icon_container (FM_COMPUTER_VIEW (view),OTHER),
     count_item, &count);

    return count;
}


/*
static GList *
fm_computer_view_get_selection_for_file_transfer (FMDirectoryView *view)
{
    return NULL;
}

static guint
fm_computer_view_get_item_count (FMDirectoryView *view)
{
    return FM_COMPUTER_VIEW (view)->details->number_of_files;
}*/

static gboolean
fm_computer_view_is_empty (FMDirectoryView *view)
{
    g_assert (FM_IS_COMPUTER_VIEW (view));

    return peony_icon_container_is_empty
           (get_icon_container (FM_COMPUTER_VIEW (view),DISK))&&
           peony_icon_container_is_empty
           (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK))&&
           peony_icon_container_is_empty
           (get_icon_container (FM_COMPUTER_VIEW (view),OTHER));
}

static void
set_sort_criterion_by_sort_type (FMComputerView *computer_view,
                                 PeonyFileSortType  sort_type)
{
    const SortCriterion *sort;

    g_assert (FM_IS_COMPUTER_VIEW (computer_view));

    sort = get_sort_criterion_by_sort_type (sort_type);
    g_return_if_fail (sort != NULL);

    if (sort == computer_view->details->sort
            && fm_computer_view_using_auto_layout (computer_view))
    {
        return;
    }

    set_sort_criterion (computer_view, sort);
    peony_icon_container_sort (get_icon_container (computer_view,DISK));
    
    peony_icon_container_sort (get_icon_container (computer_view,MOVABLEDISK));
    
    peony_icon_container_sort (get_icon_container (computer_view,OTHER));
    fm_computer_view_reveal_selection (FM_DIRECTORY_VIEW (computer_view));
}


static void
action_reversed_order_callback (GtkAction *action,
                                gpointer user_data)
{
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (user_data);

    if (set_sort_reversed (computer_view,
                           gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action))))
    {
        peony_icon_container_sort (get_icon_container (computer_view,DISK));
        peony_icon_container_sort (get_icon_container (computer_view,MOVABLEDISK));
        peony_icon_container_sort (get_icon_container (computer_view,OTHER));


        fm_computer_view_reveal_selection (FM_DIRECTORY_VIEW (computer_view));
    }
}

static void
action_keep_aligned_callback (GtkAction *action,
                              gpointer user_data)
{
    FMComputerView *computer_view;
    PeonyFile *file;
    gboolean keep_aligned;

    computer_view = FM_COMPUTER_VIEW (user_data);

    keep_aligned = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

    file = fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (computer_view));
    fm_computer_view_set_directory_keep_aligned (computer_view,
            file,
            keep_aligned);

    peony_icon_container_set_keep_aligned (get_icon_container (computer_view,DISK),
                                          keep_aligned);
    peony_icon_container_set_keep_aligned (get_icon_container (computer_view,MOVABLEDISK),
                                          keep_aligned);
    peony_icon_container_set_keep_aligned (get_icon_container (computer_view,OTHER),
                                          keep_aligned);

}

static void
switch_to_manual_layout (FMComputerView *computer_view)
{
    if (!fm_computer_view_using_auto_layout (computer_view))
    {
        return;
    }

    computer_view->details->sort = &sort_criteria[0];

    peony_icon_container_set_auto_layout
    (get_icon_container (computer_view,DISK), FALSE);
    peony_icon_container_set_auto_layout
    (get_icon_container (computer_view,MOVABLEDISK), FALSE);
    peony_icon_container_set_auto_layout
    (get_icon_container (computer_view,OTHER), FALSE);


}


static void
layout_changed_callback (PeonyIconContainer *container,
                         FMComputerView *computer_view)
{
    PeonyFile *file;

    g_assert (FM_IS_COMPUTER_VIEW (computer_view));
    g_assert (container == get_icon_container (computer_view,container->name));

    file = fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (computer_view));

    if (file != NULL)
    {
        fm_computer_view_set_directory_auto_layout
        (computer_view,
         file,
         fm_computer_view_using_auto_layout (computer_view));
       // fm_computer_view_set_directory_tighter_layout
        //(computer_view,
        // file,
         //fm_computer_view_using_tighter_layout (computer_view));
    }

    update_layout_menus (computer_view);
}

static gboolean
fm_computer_view_can_rename_file (FMDirectoryView *view, PeonyFile *file)
{
    if (!(fm_computer_view_get_zoom_level (view) > PEONY_ZOOM_LEVEL_SMALLEST))
    {
        return FALSE;
    }

    return FM_DIRECTORY_VIEW_CLASS(fm_computer_view_parent_class)->can_rename_file (view, file);
}


static void
fm_computer_view_end_file_changes (FMDirectoryView *view)
{
}

static void
fm_computer_view_remove_file (FMDirectoryView *view, PeonyFile *file, PeonyDirectory *directory)
{
    FMComputerView *computer_view;

    /* This used to assert that 'directory == fm_directory_view_get_model (view)', but that
     * resulted in a lot of crash reports (bug #352592). I don't see how that trace happens.
     * It seems that somehow we get a files_changed event sent to the view from a directory
     * that isn't the model, but the code disables the monitor and signal callback handlers when
     * changing directories. Maybe we can get some more information when this happens.
     * Further discussion in bug #368178.
     */
    if (directory != fm_directory_view_get_model (view))
    {
        char *file_uri, *dir_uri, *model_uri;
        file_uri = peony_file_get_uri (file);
        dir_uri = peony_directory_get_uri (directory);
        model_uri = peony_directory_get_uri (fm_directory_view_get_model (view));
        g_warning ("fm_icon_view_remove_file() - directory not icon view model, shouldn't happen.\n"
                   "file: %p:%s, dir: %p:%s, model: %p:%s, view loading: %d\n"
                   "If you see this, please add this info to http://bugzilla.gnome.org/show_bug.cgi?id=368178",
                   file, file_uri, directory, dir_uri, fm_directory_view_get_model (view), model_uri, fm_directory_view_get_loading (view));
        g_free (file_uri);
        g_free (dir_uri);
        g_free (model_uri);
    }

    computer_view = FM_COMPUTER_VIEW (view);

    if (peony_icon_container_remove (get_icon_container (computer_view,DISK),
                                    PEONY_ICON_CONTAINER_ICON_DATA (file)))
    {
        if (file == computer_view->details->audio_preview_file)
        {
            preview_audio (computer_view, NULL, FALSE);
        }

        peony_file_unref (file);
    }
    else if (peony_icon_container_remove (get_icon_container (computer_view,MOVABLEDISK),
                                    PEONY_ICON_CONTAINER_ICON_DATA (file)))
    {
        if (file == computer_view->details->audio_preview_file)
        {
            preview_audio (computer_view, NULL, FALSE);
        }

        peony_file_unref (file);
    }
    else if (peony_icon_container_remove (get_icon_container (computer_view,OTHER),
                                    PEONY_ICON_CONTAINER_ICON_DATA (file)))
    {
        if (file == computer_view->details->audio_preview_file)
        {
            preview_audio (computer_view, NULL, FALSE);
        }

        peony_file_unref (file);
    }



}

static void
fm_computer_view_set_selection (FMDirectoryView *view, GList *selection)
{
    fm_directory_view_notify_selection_changed (view);
}

static void
fm_computer_view_select_all (FMDirectoryView *view)
{
    PeonyIconContainer *icon_container;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (view));

    //icon_container = get_icon_container (FM_COMPUTER_VIEW (view));
    //peony_icon_container_select_all (icon_container);

}

static void
fm_computer_view_reveal_selection (FMDirectoryView *view)
{
    GList *selection;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (view));

    selection = fm_directory_view_get_selection (view);

    /* Make sure at least one of the selected items is scrolled into view */
    if (selection != NULL)
    {
        peony_icon_container_reveal
        (get_icon_container (FM_COMPUTER_VIEW (view),DISK),
         selection->data);
        peony_icon_container_reveal
        (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK),
         selection->data);
        peony_icon_container_reveal
        (get_icon_container (FM_COMPUTER_VIEW (view),OTHER),
         selection->data);
    }

    peony_file_list_free (selection);

}

static const GtkActionEntry computer_view_entries[] =
{
    /* name, stock id, label */  { "Arrange Items", NULL, N_("Arran_ge Items") },
    /* name, stock id */         { "Stretch", NULL,
        /* label, accelerator */       N_("Resize Icon..."), NULL,
        /* tooltip */                  N_("Make the selected icon resizable"),
        G_CALLBACK (action_stretch_callback)
    },
    /* name, stock id */         { "Unstretch", NULL,
        /* label, accelerator */       N_("Restore Icons' Original Si_zes"), NULL,
        /* tooltip */                  N_("Restore each selected icon to its original size"),
        G_CALLBACK (action_unstretch_callback)
    },
    /* name, stock id */         { "Clean Up", NULL,
        /* label, accelerator */       N_("_Organize by Name"), NULL,
        /* tooltip */                  N_("Reposition icons to better fit in the window and avoid overlapping"),
        G_CALLBACK (action_clean_up_callback)
    },
};

static const GtkToggleActionEntry computer_view_toggle_entries[] =
{
    /* name, stock id */      { "Tighter Layout", NULL,
        /* label, accelerator */    N_("Compact _Layout"), NULL,
        /* tooltip */               N_("Toggle using a tighter layout scheme"),
        G_CALLBACK (action_tighter_layout_callback),
        0
    },
    /* name, stock id */      { "Reversed Order", NULL,
        /* label, accelerator */    N_("Re_versed Order"), NULL,
        /* tooltip */               N_("Display icons in the opposite order"),
        G_CALLBACK (action_reversed_order_callback),
        0
    },
    /* name, stock id */      { "Keep Aligned", NULL,
        /* label, accelerator */    N_("_Keep Aligned"), NULL,
        /* tooltip */               N_("Keep icons lined up on a grid"),
        G_CALLBACK (action_keep_aligned_callback),
        0
    },
};

static const GtkRadioActionEntry arrange_radio_entries[] =
{
    {
        "Manual Layout", NULL,
        N_("_Manually"), NULL,
        N_("Leave icons wherever they are dropped"),
        PEONY_FILE_SORT_NONE
    },
    {
        "Sort by Name", NULL,
        N_("By _Name"), NULL,
        N_("Keep icons sorted by name in rows"),
        PEONY_FILE_SORT_BY_DISPLAY_NAME
    },
    {
        "Sort by Size", NULL,
        N_("By _Size"), NULL,
        N_("Keep icons sorted by size in rows"),
        PEONY_FILE_SORT_BY_SIZE
    },
    {
        "Sort by Type", NULL,
        N_("By _Type"), NULL,
        N_("Keep icons sorted by type in rows"),
        PEONY_FILE_SORT_BY_TYPE
    },
    {
        "Sort by Modification Date", NULL,
        N_("By Modification _Date"), NULL,
        N_("Keep icons sorted by modification date in rows"),
        PEONY_FILE_SORT_BY_MTIME
    },
    {
        "Sort by Emblems", NULL,
        N_("By _Emblems"), NULL,
        N_("Keep icons sorted by emblems in rows"),
        PEONY_FILE_SORT_BY_EMBLEMS
    },
    {
        "Sort by Trash Time", NULL,
        N_("By T_rash Time"), NULL,
        N_("Keep icons sorted by trash time in rows"),
        PEONY_FILE_SORT_BY_TRASHED_TIME
    },
};


static void
fm_computer_view_merge_menus (FMDirectoryView *view)
{
    FMComputerView *computer_view;
    GtkUIManager *ui_manager;
    GtkActionGroup *action_group;
    GtkAction *action;
    const char *ui;

    g_assert (FM_IS_COMPUTER_VIEW (view));

    FM_DIRECTORY_VIEW_CLASS (fm_computer_view_parent_class)->merge_menus (view);

    computer_view = FM_COMPUTER_VIEW (view);

    ui_manager = fm_directory_view_get_ui_manager (FM_DIRECTORY_VIEW (computer_view));

    action_group = gtk_action_group_new ("IconViewActions");
    gtk_action_group_set_translation_domain (action_group, GETTEXT_PACKAGE);
    computer_view->details->icon_action_group = action_group;
    gtk_action_group_add_actions (action_group,
                                  computer_view_entries, G_N_ELEMENTS (computer_view_entries),
                                  computer_view);
    gtk_action_group_add_toggle_actions (action_group,
                                         computer_view_toggle_entries, G_N_ELEMENTS (computer_view_toggle_entries),
                                         computer_view);
    gtk_action_group_add_radio_actions (action_group,
                                        arrange_radio_entries,
                                        G_N_ELEMENTS (arrange_radio_entries),
                                        -1,
                                        G_CALLBACK (action_sort_radio_callback),
                                        computer_view);

    gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
    g_object_unref (action_group); /* owned by ui manager */

    ui = peony_ui_string_get ("peony-icon-view-ui.xml");
    computer_view->details->icon_merge_id =
        gtk_ui_manager_add_ui_from_string (ui_manager, ui, -1, NULL);

    /* Do one-time state-setting here; context-dependent state-setting
     * is done in update_menus.
     */
    if (!fm_computer_view_supports_auto_layout (computer_view))
    {
        action = gtk_action_group_get_action (action_group,
                                              FM_ACTION_ARRANGE_ITEMS);
        gtk_action_set_visible (action, FALSE);
    }

    if (fm_computer_view_supports_scaling (computer_view))
    {
        //hide scaling action

        gtk_ui_manager_add_ui (ui_manager,
                               computer_view->details->icon_merge_id,
                               POPUP_PATH_ICON_APPEARANCE,
                               FM_ACTION_STRETCH,
                               FM_ACTION_STRETCH,
                               GTK_UI_MANAGER_MENUITEM,
                               FALSE);
        gtk_ui_manager_add_ui (ui_manager,
                               computer_view->details->icon_merge_id,
                               POPUP_PATH_ICON_APPEARANCE,
                               FM_ACTION_UNSTRETCH,
                               FM_ACTION_UNSTRETCH,
                               GTK_UI_MANAGER_MENUITEM,
                               FALSE);
 
    }

    update_layout_menus (computer_view);
}

static void
fm_computer_view_update_menus (FMDirectoryView *view)
{
    FMComputerView *computer_view;
    GList *selection;
    int selection_count;
    GtkAction *action;
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    gboolean editable;

    computer_view = FM_COMPUTER_VIEW (view);

    FM_DIRECTORY_VIEW_CLASS (fm_computer_view_parent_class)->update_menus(view);

    selection = fm_directory_view_get_selection (view);
    selection_count = g_list_length (selection);
    icon_container = get_icon_container (computer_view,DISK);
    icon_container1 = get_icon_container (computer_view,MOVABLEDISK);
    icon_container2 = get_icon_container (computer_view,OTHER);
    action = gtk_action_group_get_action (computer_view->details->icon_action_group,
                                          FM_ACTION_STRETCH);
    /*
    gtk_action_set_sensitive (action,
                              selection_count == 1
                              && icon_container != NULL
                              && !peony_icon_container_has_stretch_handles (icon_container));
    gtk_action_set_sensitive (action,
                              selection_count == 1
                              && icon_container1 != NULL
                              && !peony_icon_container_has_stretch_handles (icon_container1));
    gtk_action_set_sensitive (action,
                              selection_count == 1
                              && icon_container2 != NULL
                              && !peony_icon_container_has_stretch_handles (icon_container2));

    gtk_action_set_visible (action,
                            fm_computer_view_supports_scaling (computer_view));
*/
    gtk_action_set_visible (action,FALSE);
                            
    action = gtk_action_group_get_action (computer_view->details->icon_action_group,
                                          FM_ACTION_UNSTRETCH);
 /*   g_object_set (action, "label",
                  eel_g_list_more_than_one_item (selection)
                  ? _("Restore Icons' Original Si_zes")
                  : _("Restore Icon's Original Si_ze"),
                  NULL);
    gtk_action_set_sensitive (action,
                              icon_container != NULL
                              && peony_icon_container_is_stretched (icon_container));
    gtk_action_set_sensitive (action,
                              icon_container1 != NULL
                              && peony_icon_container_is_stretched (icon_container1));
    gtk_action_set_sensitive (action,
                              icon_container2 != NULL
                              && peony_icon_container_is_stretched (icon_container2));

    gtk_action_set_visible (action,
                            fm_computer_view_supports_scaling (computer_view));
*/
    gtk_action_set_visible (action,FALSE);

    peony_file_list_free (selection);

    editable = fm_directory_view_is_editable (view);
    action = gtk_action_group_get_action (computer_view->details->icon_action_group,
                                          FM_ACTION_MANUAL_LAYOUT);
    gtk_action_set_sensitive (action, editable);
}

/* Reset sort criteria and zoom level to match defaults */
static void
fm_computer_view_reset_to_defaults (FMDirectoryView *view)
{
    PeonyIconContainer *icon_container;
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (view);
    icon_container = get_icon_container (computer_view,DISK);

    clear_sort_criterion (computer_view);
    peony_icon_container_set_keep_aligned
    (icon_container, get_default_directory_keep_aligned ());

    peony_icon_container_sort (icon_container);

    update_layout_menus (computer_view);

    fm_computer_view_restore_default_zoom_level (view);

}


static void
fm_computer_view_bump_zoom_level (FMDirectoryView *view, int zoom_increment)
{
    PeonyZoomLevel new_level;

    g_return_if_fail (FM_IS_COMPUTER_VIEW(view));

    new_level = fm_computer_view_get_zoom_level (view) + zoom_increment;

    if (new_level >= PEONY_ZOOM_LEVEL_SMALLEST &&
            new_level <= PEONY_ZOOM_LEVEL_LARGEST)
    {
        fm_directory_view_zoom_to_level (view, new_level);
    }
}

static PeonyZoomLevel
fm_computer_view_get_zoom_level (FMDirectoryView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), PEONY_ZOOM_LEVEL_STANDARD);
    
    return peony_icon_container_get_zoom_level (get_icon_container (FM_COMPUTER_VIEW (view),DISK));
}

static void
fm_computer_view_set_zoom_level (FMComputerView *view,
                             PeonyZoomLevel new_level,
                             gboolean always_emit)
{
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (view));
    g_return_if_fail (new_level >= PEONY_ZOOM_LEVEL_SMALLEST &&
                      new_level <= PEONY_ZOOM_LEVEL_LARGEST);

    icon_container = get_icon_container (view,DISK);
    icon_container1 = get_icon_container (view,MOVABLEDISK);
    icon_container2 = get_icon_container (view,OTHER);
    if (peony_icon_container_get_zoom_level (icon_container) == new_level)
    {
        if (always_emit)
        {
            g_signal_emit_by_name (view, "zoom_level_changed");
        }
        return;
    }
    if (peony_icon_container_get_zoom_level (icon_container1) == new_level)
    {
        if (always_emit)
        {
            g_signal_emit_by_name (view, "zoom_level_changed");
        }
        return;
    }
    if (peony_icon_container_get_zoom_level (icon_container2) == new_level)
    {
        if (always_emit)
        {
            g_signal_emit_by_name (view, "zoom_level_changed");
        }
        return;
    }


    peony_file_set_integer_metadata
        (fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (view)),
         PEONY_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL,
         PEONY_ZOOM_LEVEL_STANDARD,
         new_level);

    peony_icon_container_set_zoom_level (icon_container, new_level);
    peony_icon_container_set_zoom_level (icon_container1, new_level);
    peony_icon_container_set_zoom_level (icon_container2, new_level);

    g_signal_emit_by_name (view, "zoom_level_changed");

    if (fm_directory_view_get_active (FM_DIRECTORY_VIEW (view)))
    {
        fm_directory_view_update_menus (FM_DIRECTORY_VIEW (view));
    }
}


static void
fm_computer_view_zoom_to_level (FMDirectoryView *view,
                             PeonyZoomLevel zoom_level)
{
    FMComputerView *computer_view;

    g_assert (FM_IS_COMPUTER_VIEW (view));

    computer_view = FM_COMPUTER_VIEW (view);
    fm_computer_view_set_zoom_level (computer_view, PEONY_ZOOM_LEVEL_STANDARD, FALSE);

}

static void
fm_computer_view_restore_default_zoom_level (FMDirectoryView *view)
{
    FMComputerView *computer_view;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (view));

    computer_view = FM_COMPUTER_VIEW (view);
    fm_directory_view_zoom_to_level
    (view, PEONY_ZOOM_LEVEL_STANDARD);

}

static gboolean
fm_computer_view_can_zoom_in (FMDirectoryView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    //return fm_computer_view_get_zoom_level (view)
      //     < PEONY_ZOOM_LEVEL_LARGEST;
      return FALSE;
}

static gboolean
fm_computer_view_can_zoom_out (FMDirectoryView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    //return fm_computer_view_get_zoom_level (view)
      //     > PEONY_ZOOM_LEVEL_SMALLEST;
      return FALSE;
}


static void
fm_computer_view_start_renaming_file (FMDirectoryView *view,
                                   PeonyFile *file,
                                   gboolean select_all)
{
    /* call parent class to make sure the right icon is selected */
        FM_DIRECTORY_VIEW_CLASS(fm_computer_view_parent_class)->start_renaming_file (view, file, select_all);
    
        /* start renaming */
        peony_icon_container_start_renaming_selected_item
        (get_icon_container (FM_COMPUTER_VIEW (view),DISK), select_all);
        peony_icon_container_start_renaming_selected_item
        (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK), select_all);
        peony_icon_container_start_renaming_selected_item
        (get_icon_container (FM_COMPUTER_VIEW (view),OTHER), select_all);
}

static void
icon_container_activate_callback (PeonyIconContainer *container,
                                  GList *file_list,
                                  FMComputerView *computer_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));
    g_assert (container == get_icon_container (computer_view,container->name));

    fm_directory_view_activate_files (FM_DIRECTORY_VIEW (computer_view),
                                      file_list,
                                      PEONY_WINDOW_OPEN_ACCORDING_TO_MODE, 0,
                                      TRUE);
}

static void
icon_container_activate_alternate_callback (PeonyIconContainer *container,
        GList *file_list,
        FMComputerView *computer_view)
{
    GdkEvent *event;
    GdkEventButton *button_event;
    GdkEventKey *key_event;
    gboolean open_in_tab;
    PeonyWindowInfo *window_info;
    PeonyWindowOpenFlags flags;

    g_assert (FM_IS_COMPUTER_VIEW (computer_view));
    g_assert (container == get_icon_container (computer_view,container->name));

    open_in_tab = TRUE;

    window_info = fm_directory_view_get_peony_window (FM_DIRECTORY_VIEW (computer_view));

    if (peony_window_info_get_window_type (window_info) == PEONY_WINDOW_NAVIGATION)
    {
        event = gtk_get_current_event ();
        if (event->type == GDK_BUTTON_PRESS ||
                event->type == GDK_BUTTON_RELEASE ||
                event->type == GDK_2BUTTON_PRESS ||
                event->type == GDK_3BUTTON_PRESS)
        {
            button_event = (GdkEventButton *) event;
            open_in_tab = (button_event->state & GDK_SHIFT_MASK) == 0;
        }
        else if (event->type == GDK_KEY_PRESS ||
                 event->type == GDK_KEY_RELEASE)
        {
            key_event = (GdkEventKey *) event;
            open_in_tab = !((key_event->state & GDK_SHIFT_MASK) != 0 &&
                            (key_event->state & GDK_CONTROL_MASK) != 0);
        }
        else
        {
            open_in_tab = TRUE;
        }
    }

    flags = PEONY_WINDOW_OPEN_FLAG_CLOSE_BEHIND;
    if (open_in_tab)
    {
        flags |= PEONY_WINDOW_OPEN_FLAG_NEW_TAB;
    }
    else
    {
        flags |= PEONY_WINDOW_OPEN_FLAG_NEW_WINDOW;
    }

    fm_directory_view_activate_files (FM_DIRECTORY_VIEW (computer_view),
                                      file_list,
                                      PEONY_WINDOW_OPEN_ACCORDING_TO_MODE,
                                      flags,
                                      TRUE);
}

static void
band_select_started_callback (PeonyIconContainer *container,
                              FMComputerView *computer_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));
    g_assert (container == get_icon_container (computer_view,container->name));

    fm_directory_view_start_batching_selection_changes (FM_DIRECTORY_VIEW (computer_view));
}

static void
band_select_ended_callback (PeonyIconContainer *container,
                            FMComputerView *computer_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));
    g_assert (container == get_icon_container (computer_view,container->name));

    fm_directory_view_stop_batching_selection_changes (FM_DIRECTORY_VIEW (computer_view));
    peony_icon_container_unselect_all(get_icon_container(computer_view,DISK));
    peony_icon_container_unselect_all(get_icon_container(computer_view,MOVABLEDISK));
    peony_icon_container_unselect_all(get_icon_container(computer_view,OTHER));
}

/* handle the preview signal by inspecting the mime type.  For now, we only preview local sound files. */

static char **
get_preview_argv (char *uri)
{
    char *command;
    char **argv;
    int i;

    command = g_find_program_in_path ("totem-audio-preview");
    if (command)
    {
        argv = g_new (char *, 3);
        argv[0] = command;
        argv[1] = g_strdup (uri);
        argv[2] = NULL;

        return argv;
    }

    command = g_find_program_in_path ("gst-launch-0.10");
    if (command)
    {
        argv = g_new (char *, 10);
        i = 0;
        argv[i++] = command;
        argv[i++] = g_strdup ("playbin");
        argv[i++] = g_strconcat ("uri=", uri, NULL);
        /* do not display videos */
        argv[i++] = g_strdup ("current-video=-1");
        argv[i++] = NULL;
        return argv;
    }

    return NULL;
}

static void
audio_child_died (GPid     pid,
                  gint     status,
                  gpointer data)
{
    FMComputerView *computer_view;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (data));

    computer_view = FM_COMPUTER_VIEW (data);

    computer_view->details->audio_preview_child_watch = 0;
    computer_view->details->audio_preview_child_pid = 0;
}

/* here's the timer task that actually plays the file using mpg123, ogg123 or play. */
/* FIXME bugzilla.gnome.org 41258: we should get the application from our mime-type stuff */
static gboolean
play_file (gpointer callback_data)
{
    PeonyFile *file;
    FMComputerView *computer_view;
    GPid child_pid;
    char **argv;
    GError *error;
    char *uri;
    GFile *gfile;
    char *path;

    computer_view = FM_COMPUTER_VIEW (callback_data);

    /* Stop timeout */
    computer_view->details->audio_preview_timeout = 0;

    file = computer_view->details->audio_preview_file;
    gfile = peony_file_get_location (file);
    path = g_file_get_path (gfile);

    /* if we have a local path, use that instead of the native URI.
     * this can be useful for special GVfs mounts, such as cdda://
     */
    if (path) {
        uri = g_filename_to_uri (path, NULL, NULL);
    } else {
        uri = peony_file_get_uri (file);
    }

    g_object_unref (gfile);
    g_free (path);

    argv = get_preview_argv (uri);
    g_free (uri);
    if (argv == NULL)
    {
        return FALSE;
    }

    error = NULL;
    if (!g_spawn_async_with_pipes (NULL,
                                   argv,
                                   NULL,
                                   G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                                   NULL,
                                   NULL /* user_data */,
                                   &child_pid,
                                   NULL, NULL, NULL,
                                   &error))
    {
        g_strfreev (argv);
        g_warning ("Error spawning sound preview: %s\n", error->message);
        g_error_free (error);
        return FALSE;
    }
    g_strfreev (argv);

    computer_view->details->audio_preview_child_watch =
        g_child_watch_add (child_pid,
                           audio_child_died, NULL);
    computer_view->details->audio_preview_child_pid = child_pid;

    return FALSE;
}

/* FIXME bugzilla.gnome.org 42530: Hardcoding this here sucks. We should be using components
 * for open ended things like this.
 */

/* this routine is invoked from the preview signal handler to preview a sound file.  We
   want to wait a suitable delay until we actually do it, so set up a timer task to actually
   start playing.  If we move out before the task files, we remove it. */

static void
preview_audio (FMComputerView *computer_view, PeonyFile *file, gboolean start_flag)
{
    /* Stop current audio playback */
    if (computer_view->details->audio_preview_child_pid != 0)
    {
        kill (computer_view->details->audio_preview_child_pid, SIGTERM);
        g_source_remove (computer_view->details->audio_preview_child_watch);
        waitpid (computer_view->details->audio_preview_child_pid, NULL, 0);
        computer_view->details->audio_preview_child_pid = 0;
    }

    if (computer_view->details->audio_preview_timeout != 0)
    {
        g_source_remove (computer_view->details->audio_preview_timeout);
        computer_view->details->audio_preview_timeout = 0;
    }

    if (start_flag)
    {
        computer_view->details->audio_preview_file = file;
        computer_view->details->audio_preview_timeout = g_timeout_add_seconds (1, play_file, computer_view);
    }
}

static gboolean
sound_preview_type_supported (PeonyFile *file)
{
    char *mime_type;
    guint i;

    mime_type = peony_file_get_mime_type (file);
    if (mime_type == NULL)
    {
        return FALSE;
    }
    for (i = 0; i < G_N_ELEMENTS (audio_mime_types); i++)
    {
        if (g_content_type_is_a (mime_type, audio_mime_types[i]))
        {
            g_free (mime_type);
            return TRUE;
        }
    }

    g_free (mime_type);
    return FALSE;
}


static gboolean
should_preview_sound (PeonyFile *file)
{
    GFile *location;
    GFilesystemPreviewType use_preview;

    use_preview = peony_file_get_filesystem_use_preview (file);

    location = peony_file_get_location (file);
    if (g_file_has_uri_scheme (location, "burn"))
    {
        g_object_unref (location);
        return FALSE;
    }
    g_object_unref (location);

    /* Check user performance preference */
    if (preview_sound_auto_value == PEONY_SPEED_TRADEOFF_NEVER)
    {
        return FALSE;
    }

    if (preview_sound_auto_value == PEONY_SPEED_TRADEOFF_ALWAYS)
    {
        if (use_preview == G_FILESYSTEM_PREVIEW_TYPE_NEVER)
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    if (use_preview == G_FILESYSTEM_PREVIEW_TYPE_NEVER)
    {
        /* file system says to never preview anything */
        return FALSE;
    }
    else if (use_preview == G_FILESYSTEM_PREVIEW_TYPE_IF_LOCAL)
    {
        /* file system says we should treat file as if it's local */
        return TRUE;
    }
    else
    {
        /* only local files */
        return peony_file_is_local (file);
    }
}

static int
icon_container_preview_callback (PeonyIconContainer *container,
                                 PeonyFile *file,
                                 gboolean start_flag,
                                 FMComputerView *computer_view)
{
    int result;
    char *file_name, *message;

    result = 0;

    /* preview files based on the mime_type. */
    /* at first, we just handle sounds */
    if (should_preview_sound (file))
    {
        if (sound_preview_type_supported (file))
        {
            result = 1;
            preview_audio (computer_view, file, start_flag);
        }
    }

    /* Display file name in status area at low zoom levels, since
     * the name is not displayed or hard to read in the icon view.
     */
    if (fm_computer_view_get_zoom_level (FM_DIRECTORY_VIEW (computer_view)) <= PEONY_ZOOM_LEVEL_SMALLER)
    {
        if (start_flag)
        {
            file_name = peony_file_get_display_name (file);
            message = g_strdup_printf (_("pointing at \"%s\""), file_name);
            g_free (file_name);
            peony_window_slot_info_set_status
            (fm_directory_view_get_peony_window_slot (FM_DIRECTORY_VIEW (computer_view)),
             message);
            g_free (message);
        }
        else
        {
            fm_directory_view_display_selection_info (FM_DIRECTORY_VIEW(computer_view));
        }
    }

    return result;
}

static void
renaming_icon_callback (PeonyIconContainer *container,
                        GtkWidget *widget,
                        gpointer callback_data)
{
    FMDirectoryView *directory_view;

    directory_view = FM_DIRECTORY_VIEW (callback_data);
    peony_clipboard_set_up_editable
    (GTK_EDITABLE (widget),
     fm_directory_view_get_ui_manager (directory_view),
     FALSE);
}

int
fm_computer_view_compare_files (FMComputerView   *computer_view,
                            PeonyFile *a,
                            PeonyFile *b)
{
    return peony_file_compare_for_sort
           (a, b, computer_view->details->sort->sort_type,
            /* Use type-unsafe cast for performance */
            fm_directory_view_should_sort_directories_first ((FMDirectoryView *)computer_view),
            computer_view->details->sort_reversed);
}

static int
compare_files (FMDirectoryView   *computer_view,
               PeonyFile *a,
               PeonyFile *b)
{
    return fm_computer_view_compare_files ((FMComputerView *)computer_view, a, b);
}


void
fm_computer_view_filter_by_screen (FMComputerView *computer_view,
                               gboolean filter)
{
    computer_view->details->filter_by_screen = filter;
    computer_view->details->num_screens = gdk_display_get_n_screens (gtk_widget_get_display (GTK_WIDGET (computer_view)));
}

static void
fm_computer_view_screen_changed (GtkWidget *widget,
                             GdkScreen *previous_screen)
{
    FMDirectoryView *view;
    GList *files, *l;
    PeonyFile *file;
    PeonyDirectory *directory;
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;

    if (GTK_WIDGET_CLASS (fm_computer_view_parent_class)->screen_changed)
    {
        GTK_WIDGET_CLASS (fm_computer_view_parent_class)->screen_changed (widget, previous_screen);
    }

    view = FM_DIRECTORY_VIEW (widget);
    if (FM_COMPUTER_VIEW (view)->details->filter_by_screen)
    {
        icon_container = get_icon_container (FM_COMPUTER_VIEW (view),DISK);
        icon_container1 = get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK);
        icon_container2 = get_icon_container (FM_COMPUTER_VIEW (view),OTHER);

        directory = fm_directory_view_get_model (view);
        files = peony_directory_get_file_list (directory);

        for (l = files; l != NULL; l = l->next)
        {
            file = l->data;

            if (!should_show_file_on_screen (view, file))
            {
                fm_computer_view_remove_file (view, file, directory);
            }
            else
            {
                if (peony_icon_container_add (icon_container,
                                             PEONY_ICON_CONTAINER_ICON_DATA (file)))
                {
                    peony_file_ref (file);
                }
                if (peony_icon_container_add (icon_container1,
                                             PEONY_ICON_CONTAINER_ICON_DATA (file)))
                {
                    peony_file_ref (file);
                }
                if (peony_icon_container_add (icon_container2,
                                             PEONY_ICON_CONTAINER_ICON_DATA (file)))
                {
                    peony_file_ref (file);
                }
            }
        }

        peony_file_list_unref (files);
        g_list_free (files);
    }
}

static gboolean
fm_computer_view_scroll_event (GtkWidget *widget,
                           GdkEventScroll *scroll_event)
{
    FMComputerView *computer_view;
    GdkEvent *event_copy;
    GdkEventScroll *scroll_event_copy;
    gboolean ret;

    computer_view = FM_COMPUTER_VIEW (widget);

    if (computer_view->details->compact &&
            (scroll_event->direction == GDK_SCROLL_UP ||
             scroll_event->direction == GDK_SCROLL_DOWN))
    {
        ret = fm_directory_view_handle_scroll_event (FM_DIRECTORY_VIEW (computer_view), scroll_event);
        if (!ret)
        {
            /* in column-wise layout, re-emit vertical mouse scroll events as horizontal ones,
             * if they don't bump zoom */
            event_copy = gdk_event_copy ((GdkEvent *) scroll_event);

            scroll_event_copy = (GdkEventScroll *) event_copy;
            if (scroll_event_copy->direction == GDK_SCROLL_UP)
            {
                scroll_event_copy->direction = GDK_SCROLL_LEFT;
            }
            else
            {
                scroll_event_copy->direction = GDK_SCROLL_RIGHT;
            }

            ret = gtk_widget_event (widget, event_copy);
            gdk_event_free (event_copy);
        }

        return ret;
    }

    return GTK_WIDGET_CLASS (fm_computer_view_parent_class)->scroll_event (widget, scroll_event);
}

static void
selection_changed_callback (PeonyIconContainer *container,
                            FMComputerView *computer_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));
    g_assert (container == get_icon_container (computer_view,container->name));

    fm_directory_view_notify_selection_changed (FM_DIRECTORY_VIEW (computer_view));
}

static void
icon_container_context_click_selection_callback (PeonyIconContainer *container,
        GdkEventButton *event,
        FMComputerView *computer_view)
{
    g_assert (PEONY_IS_ICON_CONTAINER (container));
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));

    fm_directory_view_pop_up_selection_context_menu
    (FM_DIRECTORY_VIEW (computer_view), event);
}

static void
icon_container_context_click_background_callback (PeonyIconContainer *container,
        GdkEventButton *event,
        FMComputerView *computer_view)
{
    g_assert (PEONY_IS_ICON_CONTAINER (container));
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));

    fm_directory_view_pop_up_background_context_menu
    (FM_DIRECTORY_VIEW (computer_view), event);
}

static gboolean
fm_computer_view_react_to_icon_change_idle_callback (gpointer data)
{
    FMComputerView *computer_view;

    g_assert (FM_IS_COMPUTER_VIEW (data));

    computer_view = FM_COMPUTER_VIEW (data);
    computer_view->details->react_to_icon_change_idle_id = 0;

    /* Rebuild the menus since some of them (e.g. Restore Stretched Icons)
     * may be different now.
     */
    fm_directory_view_update_menus (FM_DIRECTORY_VIEW (computer_view));

    /* Don't call this again (unless rescheduled) */
    return FALSE;
}

static void
icon_position_changed_callback (PeonyIconContainer *container,
                                PeonyFile *file,
                                const PeonyIconPosition *position,
                                FMComputerView *computer_view)
{
    char *position_string;
    char scale_string[G_ASCII_DTOSTR_BUF_SIZE];

    g_assert (FM_IS_COMPUTER_VIEW (computer_view));
    g_assert (container == get_icon_container (computer_view,container->name));
    g_assert (PEONY_IS_FILE (file));

    /* Schedule updating menus for the next idle. Doing it directly here
     * noticeably slows down icon stretching.  The other work here to
     * store the icon position and scale does not seem to noticeably
     * slow down icon stretching. It would be trickier to move to an
     * idle call, because we'd have to keep track of potentially multiple
     * sets of file/geometry info.
     */
    if (fm_directory_view_get_active (FM_DIRECTORY_VIEW (computer_view)) &&
            computer_view->details->react_to_icon_change_idle_id == 0)
    {
        computer_view->details->react_to_icon_change_idle_id
            = g_idle_add (fm_computer_view_react_to_icon_change_idle_callback,
                          computer_view);
    }

    /* Store the new position of the icon in the metadata. */
    if (!fm_computer_view_using_auto_layout (computer_view))
    {
        position_string = g_strdup_printf
                          ("%d,%d", position->x, position->y);
        peony_file_set_metadata
        (file, PEONY_METADATA_KEY_ICON_POSITION,
         NULL, position_string);
        g_free (position_string);
    }


    g_ascii_dtostr (scale_string, sizeof (scale_string), position->scale);
    peony_file_set_metadata
    (file, PEONY_METADATA_KEY_ICON_SCALE,
     "1.0", scale_string);
}

/* Attempt to change the filename to the new text.  Notify user if operation fails. */
static void
fm_computer_view_icon_text_changed_callback (PeonyIconContainer *container,
        PeonyFile *file,
        char *new_name,
        FMComputerView *computer_view)
{
    g_assert (PEONY_IS_FILE (file));

    /* Don't allow a rename with an empty string. Revert to original
     * without notifying the user.
     */
    if ((new_name == NULL) || (new_name[0] == '\0'))
    {
        return;
    }
    fm_rename_file (file, new_name, NULL, NULL);
}

static char *
get_icon_uri_callback (PeonyIconContainer *container,
                       PeonyFile *file,
                       FMComputerView *computer_view)
{
    g_assert (PEONY_IS_ICON_CONTAINER (container));
    g_assert (PEONY_IS_FILE (file));
    g_assert (FM_IS_COMPUTER_VIEW (computer_view));

    return peony_file_get_uri (file);
}

static char *
get_icon_drop_target_uri_callback (PeonyIconContainer *container,
                                   PeonyFile *file,
                                   FMComputerView *computer_view)
{
    g_return_val_if_fail (PEONY_IS_ICON_CONTAINER (container), NULL);
    g_return_val_if_fail (PEONY_IS_FILE (file), NULL);
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (computer_view), NULL);

    return peony_file_get_drop_target_uri (file);
}

/* Preferences changed callbacks */
static void
fm_computer_view_text_attribute_names_changed (FMDirectoryView *directory_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (directory_view));

    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),DISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),MOVABLEDISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),OTHER));
}

static void
fm_computer_view_embedded_text_policy_changed (FMDirectoryView *directory_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (directory_view));

    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),DISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),MOVABLEDISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),OTHER));
}

static void
fm_computer_view_image_display_policy_changed (FMDirectoryView *directory_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (directory_view));

    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),DISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),MOVABLEDISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),OTHER));
}


static void
fm_computer_view_click_policy_changed (FMDirectoryView *directory_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (directory_view));
    
        fm_computer_view_update_click_mode (FM_COMPUTER_VIEW (directory_view));
    
}



static gboolean
fm_computer_view_using_manual_layout (FMDirectoryView *view)
{
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), FALSE);

    return !fm_computer_view_using_auto_layout (FM_COMPUTER_VIEW (view));
}


static void
fm_computer_view_update_click_mode (FMComputerView *computer_view)
{
    PeonyIconContainer   *icon_container,*icon_container1,*icon_container2;
    int         click_mode;

    icon_container = get_icon_container (computer_view,DISK);
    g_assert (icon_container != NULL);

    click_mode = g_settings_get_enum (peony_preferences, PEONY_PREFERENCES_CLICK_POLICY);

    peony_icon_container_set_single_click_mode (icon_container,
            click_mode == PEONY_CLICK_POLICY_SINGLE);
    
    icon_container1 = get_icon_container (computer_view,MOVABLEDISK);
    g_assert (icon_container1 != NULL);

    click_mode = g_settings_get_enum (peony_preferences, PEONY_PREFERENCES_CLICK_POLICY);

    peony_icon_container_set_single_click_mode (icon_container1,
            click_mode == PEONY_CLICK_POLICY_SINGLE);

    icon_container2 = get_icon_container (computer_view,DISK);
    g_assert (icon_container2 != NULL);

    click_mode = g_settings_get_enum (peony_preferences, PEONY_PREFERENCES_CLICK_POLICY);

    peony_icon_container_set_single_click_mode (icon_container2,
            click_mode == PEONY_CLICK_POLICY_SINGLE);


}


static void
default_sort_order_changed_callback (gpointer callback_data)
{
    FMComputerView *computer_view;
    PeonyFile *file;
    char *sort_name;
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (callback_data));

    computer_view = FM_COMPUTER_VIEW (callback_data);

    file = fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (computer_view));
    sort_name = fm_computer_view_get_directory_sort_by (computer_view, file);
    set_sort_criterion (computer_view, get_sort_criterion_by_metadata_text (sort_name));
    g_free (sort_name);

    icon_container = get_icon_container (computer_view,DISK);
    g_return_if_fail (PEONY_IS_ICON_CONTAINER (icon_container));
    peony_icon_container_request_update_all (icon_container);

    icon_container1 = get_icon_container (computer_view,MOVABLEDISK);
    g_return_if_fail (PEONY_IS_ICON_CONTAINER (icon_container1));
    peony_icon_container_request_update_all (icon_container1);

    icon_container2 = get_icon_container (computer_view,DISK);
    g_return_if_fail (PEONY_IS_ICON_CONTAINER (icon_container2));
    peony_icon_container_request_update_all (icon_container2);



    
}

static void
default_sort_in_reverse_order_changed_callback (gpointer callback_data)
{
    FMComputerView *computer_view;
    PeonyFile *file;
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (callback_data));

    computer_view = FM_COMPUTER_VIEW (callback_data);

    file = fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (computer_view));
    set_sort_reversed (computer_view, fm_computer_view_get_directory_sort_reversed (computer_view, file));
    icon_container = get_icon_container (computer_view,DISK);
    g_return_if_fail (PEONY_IS_ICON_CONTAINER (icon_container));

    peony_icon_container_request_update_all (icon_container);

    icon_container1 = get_icon_container (computer_view,MOVABLEDISK);
    g_return_if_fail (PEONY_IS_ICON_CONTAINER (icon_container1));

    peony_icon_container_request_update_all (icon_container1);

    icon_container2 = get_icon_container (computer_view,OTHER);
    g_return_if_fail (PEONY_IS_ICON_CONTAINER (icon_container2));

    peony_icon_container_request_update_all (icon_container2);   
}

static void
default_zoom_level_changed_callback (gpointer callback_data)
{
    FMComputerView *computer_view;
    PeonyFile *file;
    int level;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (callback_data));

    computer_view = FM_COMPUTER_VIEW (callback_data);

    if (fm_directory_view_supports_zooming (FM_DIRECTORY_VIEW (computer_view)))
    {
        file = fm_directory_view_get_directory_as_file (FM_DIRECTORY_VIEW (computer_view));

        if (fm_computer_view_is_compact (computer_view))
        {
            level = peony_file_get_integer_metadata (file,
                                                    PEONY_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL,
                                                    PEONY_ZOOM_LEVEL_STANDARD);
        }
        else
        {
            level = peony_file_get_integer_metadata (file,
                                                    PEONY_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL,
                                                    PEONY_ZOOM_LEVEL_STANDARD);
        }
        fm_directory_view_zoom_to_level (FM_DIRECTORY_VIEW (computer_view), level);
    }
}

static void
labels_beside_icons_changed_callback (gpointer callback_data)
{
    FMComputerView *computer_view;

    g_return_if_fail (FM_IS_COMPUTER_VIEW (callback_data));

    computer_view = FM_COMPUTER_VIEW (callback_data);

    set_labels_beside_icons (computer_view);
}

static void
all_columns_same_width_changed_callback (gpointer callback_data)
{
    FMComputerView *computer_view;

    g_assert (FM_IS_COMPUTER_VIEW (callback_data));

    computer_view = FM_COMPUTER_VIEW (callback_data);

    set_columns_same_width (computer_view);
}

static void
computer_view_notify_clipboard_info (PeonyClipboardMonitor *monitor,
                                 PeonyClipboardInfo *info,
                                 FMComputerView *computer_view)
{
    GList *icon_data;

    icon_data = NULL;
    if (info && info->cut)
    {
        icon_data = info->files;
    }

    peony_icon_container_set_highlighted_for_clipboard (
        get_icon_container (computer_view,DISK), icon_data);
    peony_icon_container_set_highlighted_for_clipboard (
        get_icon_container (computer_view,MOVABLEDISK), icon_data);
    peony_icon_container_set_highlighted_for_clipboard (
        get_icon_container (computer_view,OTHER), icon_data);
}


static void
fm_computer_view_end_loading (FMDirectoryView *view,
                           gboolean all_files_seen)
{
    FMComputerView *computer_view;
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    PeonyClipboardMonitor *monitor;
    PeonyClipboardInfo *info;
    GList *v[3],*vb,*e,*c,*c1,*c2,*w[3];
    computer_view = FM_COMPUTER_VIEW (view);
    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(computer_view));
    GtkWidget *scrollwindow[3];
    GtkViewport *viewport1[3];
    
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(viewport));

    e = gtk_container_get_children (GTK_CONTAINER(box));
    
    GtkWidget *expander = GTK_EXPANDER(e->data);
    GtkWidget *expander1 =GTK_EXPANDER(e->next->data);
    GtkWidget *expander2 =GTK_EXPANDER(e->next->next->data);
    const char *cc = gtk_expander_get_label(GTK_EXPANDER(expander));
    c =gtk_container_get_children(GTK_CONTAINER(expander));
    c1 = gtk_container_get_children(GTK_CONTAINER(expander1));
    c2 = gtk_container_get_children(GTK_CONTAINER(expander2));

    viewport1[0] = GTK_VIEWPORT(c->data);
    viewport1[1] = GTK_VIEWPORT(c1->data);
    viewport1[2] = GTK_VIEWPORT(c2->data);
    v[0] = gtk_container_get_children(GTK_CONTAINER(viewport1[0]));
    v[1] = gtk_container_get_children(GTK_CONTAINER(viewport1[1]));
    v[2] = gtk_container_get_children(GTK_CONTAINER(viewport1[2]));
    icon_container = PEONY_ICON_CONTAINER(v[0]->data);
    icon_container1 = PEONY_ICON_CONTAINER(v[1]->data);
    icon_container2 = PEONY_ICON_CONTAINER(v[2]->data);
    
    peony_icon_container_end_loading ( icon_container, all_files_seen);
    peony_icon_container_end_loading ( icon_container1, all_files_seen);
    peony_icon_container_end_loading ( icon_container2, all_files_seen);
   if(!icon_container->details->icons)
    gtk_widget_hide(expander);
   else
    gtk_widget_show(expander);  
   if(!icon_container1->details->icons)
    gtk_widget_hide(expander1);
   else
    gtk_widget_show(expander1);
   if(!icon_container2->details->icons)
    gtk_widget_hide(expander2);
   else
    gtk_widget_show(expander2);
    monitor = peony_clipboard_monitor_get ();
    info = peony_clipboard_monitor_get_clipboard_info (monitor);

    computer_view_notify_clipboard_info (monitor, info, computer_view);

}

static void
fm_computer_view_widget_to_file_operation_position (FMDirectoryView *view,
        GdkPoint *position)
{
    g_assert (FM_IS_ICON_VIEW (view));

    peony_icon_container_widget_to_file_operation_position
    (get_icon_container (FM_COMPUTER_VIEW (view),DISK), position);
    peony_icon_container_widget_to_file_operation_position
    (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK), position);
    peony_icon_container_widget_to_file_operation_position
    (get_icon_container (FM_COMPUTER_VIEW (view),OTHER), position);

}

static GList *
peony_icon_container_get_selected_icons (PeonyIconContainer *container)
{
    GList *list, *p;

    g_return_val_if_fail (PEONY_IS_ICON_CONTAINER (container), NULL);

    list = NULL;
    for (p = container->details->icons; p != NULL; p = p->next)
    {
        PeonyIcon *icon;

        icon = p->data;
        if (icon->is_selected)
        {
            list = g_list_prepend (list, icon);
        }
    }

    return g_list_reverse (list);
}


static GArray *
peony_icon_container_get_icon_locations (PeonyIconContainer *container,
                                        GList *icons)
{
    GArray *result;
    GList *node;
    int index;

    result = g_array_new (FALSE, TRUE, sizeof (GdkPoint));
    result = g_array_set_size (result, g_list_length (icons));

    for (index = 0, node = icons; node != NULL; index++, node = node->next)
    {
        g_array_index (result, GdkPoint, index).x =
            ((PeonyIcon *)node->data)->x;
        g_array_index (result, GdkPoint, index).y =
            ((PeonyIcon *)node->data)->y;
    }

    return result;
}


static GArray *
fm_computer_view_get_selected_icon_locations (FMDirectoryView *view)
{
    PeonyIconContainer *icon_container,*icon_container1,*icon_container2;
    g_return_val_if_fail (FM_IS_COMPUTER_VIEW (view), NULL);

    GArray *result;
    GList *icons=NULL,*icons1,*icons2,*icons3;

    icons1 = peony_icon_container_get_selected_icons
           (get_icon_container (FM_COMPUTER_VIEW (view),DISK));
    icons2 = peony_icon_container_get_selected_icons
           (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK));
    icons3 = peony_icon_container_get_selected_icons
           (get_icon_container (FM_COMPUTER_VIEW (view),OTHER));
    while(icons1!=NULL)
        {
        icons = g_list_append(icons,icons1->data);
        icons1=icons1->next;
        }
    while(icons2!=NULL)
        {
        icons = g_list_append(icons,icons2->data);
        icons2 = icons2->next;
        }
    while(icons3!=NULL)
        {
        icons3 = g_list_append(icons,icons3->data);
        icons3 = icons3->next;
        }

    result = peony_icon_container_get_icon_locations (icon_container1, icons);
    g_list_free (icons);
    g_list_free (icons1);
    g_list_free (icons2);
    g_list_free (icons3);

    return result;

}

static void
fm_computer_view_emblems_changed (FMDirectoryView *directory_view)
{
    g_assert (FM_IS_COMPUTER_VIEW (directory_view));

    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),DISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),MOVABLEDISK));
    peony_icon_container_request_update_all (get_icon_container (FM_COMPUTER_VIEW (directory_view),OTHER));
}

static char *
fm_computer_view_get_first_visible_file (PeonyView *view)
{
    PeonyFile *file,*file1,*file2;
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (view);

    file = PEONY_FILE (peony_icon_container_get_first_visible_icon (get_icon_container (computer_view,DISK)));
    file1 = PEONY_FILE (peony_icon_container_get_first_visible_icon (get_icon_container (computer_view,MOVABLEDISK)));
    file2 = PEONY_FILE (peony_icon_container_get_first_visible_icon (get_icon_container (computer_view,OTHER)));
    if (file)
    {
        return peony_file_get_uri (file);
    }
    if (file1)
    {
        return peony_file_get_uri (file1);
    }
    if (file2)
    {
        return peony_file_get_uri (file2);
    }

    return NULL;
}

static void
fm_computer_view_scroll_to_file (PeonyView *view,
                              const char *uri)
{
    PeonyFile *file;
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (view);

    if (uri != NULL)
    {
        /* Only if existing, since we don't want to add the file to
           the directory if it has been removed since then */
        file = peony_file_get_existing_by_uri (uri);
        if (file != NULL)
        {
            peony_icon_container_scroll_to_icon (get_icon_container (computer_view,DISK),
                                                PEONY_ICON_CONTAINER_ICON_DATA (file));
            peony_icon_container_scroll_to_icon (get_icon_container (computer_view,MOVABLEDISK),
                                                PEONY_ICON_CONTAINER_ICON_DATA (file));
            peony_icon_container_scroll_to_icon (get_icon_container (computer_view,OTHER),
                                                PEONY_ICON_CONTAINER_ICON_DATA (file));          
            
            
            peony_file_unref (file);

        }
    }

}

static void
fm_computer_view_set_property (GObject         *object,
                           guint            prop_id,
                           const GValue    *value,
                           GParamSpec      *pspec)
{
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (object);

    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
fm_computer_view_grab_focus (PeonyView *view)
{
    gtk_widget_grab_focus (GTK_WIDGET (view));
}
static void
fm_computer_view_invert_selection (FMDirectoryView *view)
{
    g_return_if_fail (FM_IS_COMPUTER_VIEW (view));

    peony_icon_container_invert_selection
    (get_icon_container (FM_COMPUTER_VIEW (view),DISK));
    peony_icon_container_invert_selection
    (get_icon_container (FM_COMPUTER_VIEW (view),MOVABLEDISK));
    peony_icon_container_invert_selection
    (get_icon_container (FM_COMPUTER_VIEW (view),OTHER));

}


static void
fm_computer_view_sort_directories_first_changed (FMDirectoryView *view)
{
    FMComputerView *computer_view;

    computer_view = FM_COMPUTER_VIEW (view);

    if (fm_computer_view_using_auto_layout (computer_view))
    {
        peony_icon_container_sort
        (get_icon_container (computer_view,DISK));
    }
    if (fm_computer_view_using_auto_layout (computer_view))
    {
        peony_icon_container_sort
        (get_icon_container (computer_view,MOVABLEDISK));
    }
    if (fm_computer_view_using_auto_layout (computer_view))
    {
        peony_icon_container_sort
        (get_icon_container (computer_view,OTHER));
    }



}

static void
fm_computer_view_class_init (FMComputerViewClass *class)
{
    FMDirectoryViewClass *fm_directory_view_class;
    

    fm_directory_view_class = FM_DIRECTORY_VIEW_CLASS (class);
    G_OBJECT_CLASS (class)->set_property = fm_computer_view_set_property;
    G_OBJECT_CLASS (class)->finalize = fm_computer_view_finalize;

#if !GTK_CHECK_VERSION (3, 0, 0)
    GTK_OBJECT_CLASS (class)->destroy = fm_computer_view_destroy;
#else
    GTK_WIDGET_CLASS (class)->destroy = fm_computer_view_destroy;
#endif

    GTK_WIDGET_CLASS (class)->screen_changed = fm_computer_view_screen_changed;
    GTK_WIDGET_CLASS (class)->scroll_event = fm_computer_view_scroll_event;

    fm_directory_view_class->add_file = fm_computer_view_add_file;  
    fm_directory_view_class->flush_added_files = fm_computer_view_flush_added_files;
    fm_directory_view_class->begin_loading = fm_computer_view_begin_loading;
    fm_directory_view_class->can_rename_file = fm_computer_view_can_rename_file;
    fm_directory_view_class->bump_zoom_level = fm_computer_view_bump_zoom_level;
    fm_directory_view_class->can_zoom_in = fm_computer_view_can_zoom_in;
    fm_directory_view_class->can_zoom_out = fm_computer_view_can_zoom_out;
    fm_directory_view_class->click_policy_changed = fm_computer_view_click_policy_changed;
    fm_directory_view_class->clear = fm_computer_view_clear;
    fm_directory_view_class->file_changed = fm_computer_view_file_changed;
    fm_directory_view_class->get_background_widget = fm_computer_view_get_background_widget;
    fm_directory_view_class->get_selected_icon_locations = fm_computer_view_get_selected_icon_locations;
    fm_directory_view_class->get_selection = fm_computer_view_get_selection;
    fm_directory_view_class->get_selection_for_file_transfer = fm_computer_view_get_selection;
    fm_directory_view_class->get_item_count = fm_computer_view_get_item_count;
    fm_directory_view_class->is_empty = fm_computer_view_is_empty;
    fm_directory_view_class->remove_file = fm_computer_view_remove_file;
    fm_directory_view_class->merge_menus = fm_computer_view_merge_menus;
    fm_directory_view_class->update_menus = fm_computer_view_update_menus;
    fm_directory_view_class->reset_to_defaults = fm_computer_view_reset_to_defaults;
    fm_directory_view_class->restore_default_zoom_level = fm_computer_view_restore_default_zoom_level;
    fm_directory_view_class->reveal_selection = fm_computer_view_reveal_selection;
    fm_directory_view_class->select_all = fm_computer_view_select_all;
    fm_directory_view_class->set_selection = fm_computer_view_set_selection;
    fm_directory_view_class->compare_files = compare_files;
    fm_directory_view_class->sort_directories_first_changed = fm_computer_view_sort_directories_first_changed;
    fm_directory_view_class->start_renaming_file = fm_computer_view_start_renaming_file;
    fm_directory_view_class->get_zoom_level = fm_computer_view_get_zoom_level;
    fm_directory_view_class->zoom_to_level = fm_computer_view_zoom_to_level;
    fm_directory_view_class->emblems_changed = fm_computer_view_emblems_changed;  
    fm_directory_view_class->using_manual_layout = fm_computer_view_using_manual_layout;
    fm_directory_view_class->end_loading = fm_computer_view_end_loading;
    fm_directory_view_class->text_attribute_names_changed = fm_computer_view_text_attribute_names_changed;
    fm_directory_view_class->embedded_text_policy_changed = fm_computer_view_embedded_text_policy_changed;
    fm_directory_view_class->image_display_policy_changed = fm_computer_view_image_display_policy_changed;
    fm_directory_view_class->click_policy_changed = fm_computer_view_click_policy_changed;
    fm_directory_view_class->widget_to_file_operation_position = fm_computer_view_widget_to_file_operation_position;
    fm_directory_view_class->invert_selection = fm_computer_view_invert_selection;


    class->clean_up = fm_computer_view_real_clean_up;
    class->supports_auto_layout = real_supports_auto_layout;
    class->supports_scaling = real_supports_scaling;
    class->supports_manual_layout = real_supports_manual_layout;
    class->supports_keep_aligned = real_supports_keep_aligned;
    class->supports_labels_beside_icons = real_supports_labels_beside_icons;
    class->get_directory_auto_layout = fm_computer_view_real_get_directory_auto_layout;
    class->get_directory_sort_by = fm_computer_view_real_get_directory_sort_by;
    class->get_directory_sort_reversed = fm_computer_view_real_get_directory_sort_reversed;
    class->set_directory_auto_layout = fm_computer_view_real_set_directory_auto_layout;
    class->set_directory_sort_by = fm_computer_view_real_set_directory_sort_by;
    class->set_directory_sort_reversed = fm_computer_view_real_set_directory_sort_reversed;
}

static const char *
fm_computer_view_get_id (PeonyView *view)
{
    return FM_COMPUTER_VIEW_ID;
}

static void
fm_computer_view_refresh (GtkWidget *menu_item,FMComputerView *computer_view)
{
    peony_window_reload (fm_directory_view_get_peony_window (FM_DIRECTORY_VIEW (computer_view)));
}

static void
computer_properties (GtkWidget *menu_item,FMComputerView *computer_view)
{
    /*need to write*/
}

static GtkMenu*
create_popup_menu(FMComputerView *computer_view,GdkEventButton *event)
{
    
    GtkWidget *popup, *menu_item, *menu_image;

    if (computer_view->details->popup != NULL)
    {
        /* already created */
        return;
    }

    popup = gtk_menu_new ();


    /* add the "refresh" menu item */
    menu_image = gtk_image_new_from_stock (GTK_STOCK_REFRESH,
                                           GTK_ICON_SIZE_MENU);
    gtk_widget_show (menu_image);
    menu_item = gtk_image_menu_item_new_with_mnemonic (_("_Refresh"));
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_item),
                                   menu_image);
    g_signal_connect (menu_item, "activate",
                      G_CALLBACK (fm_computer_view_refresh),
                      computer_view);
    gtk_widget_show (menu_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (popup), menu_item);

    /*add the "properties" menu item*/
    menu_image = gtk_image_new_from_stock (GTK_STOCK_PROPERTIES,
                                            GTK_ICON_SIZE_MENU);
    gtk_widget_show (menu_image);
    menu_item = gtk_image_menu_item_new_with_mnemonic (_("_Properties"));
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_item),
                                    menu_image);
    g_signal_connect (menu_item, "activate",
                       G_CALLBACK (computer_properties),
                       computer_view);
    gtk_widget_show (menu_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (popup), menu_item);
    

    return GTK_MENU (popup);
}

static void
button_press_callback(GtkWidget *widget, GdkEventButton *event,
                                FMComputerView *computer_view)
{
    peony_icon_container_unselect_all(get_icon_container(computer_view,DISK));
    peony_icon_container_unselect_all(get_icon_container(computer_view,MOVABLEDISK));
    peony_icon_container_unselect_all(get_icon_container(computer_view,OTHER));

}

static void
create_popup_menu_callback (GtkWidget *widget,GdkEventButton *event,
                                FMComputerView *computer_view)
{
    PeonyIconContainer* icon_container;
    GtkWidget *menu;
  
   /* if(event->button == 3)
    {
        menu = create_popup_menu(computer_view,event);
        gtk_menu_popup (menu,NULL, NULL, NULL, NULL,widget, 0);               
    }*/
}


static gboolean
get_stored_layout_timestamp (PeonyIconContainer *container,
                             PeonyIconData *icon_data,
                             time_t *timestamp,
                             FMComputerView *view)
{
    PeonyFile *file;
    PeonyDirectory *directory;

    if (icon_data == NULL)
    {
        directory = fm_directory_view_get_model (FM_DIRECTORY_VIEW (view));
        if (directory == NULL)
        {
            return FALSE;
        }

        file = peony_directory_get_corresponding_file (directory);
        *timestamp = peony_file_get_time_metadata (file,
                     PEONY_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP);
        peony_file_unref (file);
    }
    else
    {
        *timestamp = peony_file_get_time_metadata (PEONY_FILE (icon_data),
                     PEONY_METADATA_KEY_ICON_POSITION_TIMESTAMP);
    }

    return TRUE;
}



static gboolean
store_layout_timestamp (PeonyIconContainer *container,
                        PeonyIconData *icon_data,
                        const time_t *timestamp,
                        FMComputerView *view)
{
    PeonyFile *file;
    PeonyDirectory *directory;

    if (icon_data == NULL)
    {
        directory = fm_directory_view_get_model (FM_DIRECTORY_VIEW (view));
        if (directory == NULL)
        {
            return FALSE;
        }

        file = peony_directory_get_corresponding_file (directory);
        peony_file_set_time_metadata (file,
                                     PEONY_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP,
                                     (time_t) *timestamp);
        peony_file_unref (file);
    }
    else
    {
        peony_file_set_time_metadata (PEONY_FILE (icon_data),
                                     PEONY_METADATA_KEY_ICON_POSITION_TIMESTAMP,
                                     (time_t) *timestamp);
    }

    return TRUE;
}


static void
fm_computer_view_iface_init (PeonyViewIface *iface)
{
    fm_directory_view_init_view_iface (iface);

    iface->get_view_id = fm_computer_view_get_id;
    iface->get_first_visible_file = fm_computer_view_get_first_visible_file;
    iface->scroll_to_file = fm_computer_view_scroll_to_file;
    iface->get_title = NULL;
    iface->grab_focus = fm_computer_view_grab_focus;
}


static gboolean
computer_view_can_accept_item (PeonyIconContainer *container,
                           PeonyFile *target_item,
                           const char *item_uri,
                           FMDirectoryView *view)
{
    return fm_directory_view_can_accept_item (target_item, item_uri, view);
}

static char *
computer_view_get_container_uri (PeonyIconContainer *container,
                             FMDirectoryView *view)
{
    return /*container->name;*/fm_directory_view_get_uri (view);
}

static void
computer_view_move_copy_items (PeonyIconContainer *container,
                           const GList *item_uris,
                           GArray *relative_item_points,
                           const char *target_dir,
                           int copy_action,
                           int x, int y,
                           FMDirectoryView *view)
{
    peony_clipboard_clear_if_colliding_uris (GTK_WIDGET (view),
                                            item_uris,
                                            fm_directory_view_get_copied_files_atom (view));
    fm_directory_view_move_copy_items (item_uris, relative_item_points, target_dir,
                                       copy_action, x, y, view);
}

static gboolean
focus_in_event_callback (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
    PeonyWindowSlotInfo *slot_info;
    FMComputerView *computer_view = FM_COMPUTER_VIEW (user_data);

    /* make the corresponding slot (and the pane that contains it) active */
    slot_info = fm_directory_view_get_peony_window_slot (FM_DIRECTORY_VIEW (computer_view));
    peony_window_slot_info_make_hosting_pane_active (slot_info);

    return FALSE;
}

static PeonyIconContainer *
create_icon_container(FMComputerView *computer_view)
{
    PeonyIconContainer *icon_container;
    icon_container = fm_computer_container_new (computer_view);
    g_signal_connect_object (icon_container, "focus_in_event",
                             G_CALLBACK (focus_in_event_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "activate",
                             G_CALLBACK (icon_container_activate_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "activate_alternate",
                             G_CALLBACK (icon_container_activate_alternate_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "band_select_started",
                             G_CALLBACK (band_select_started_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "band_select_ended",
                             G_CALLBACK (band_select_ended_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "context_click_selection",
                             G_CALLBACK (icon_container_context_click_selection_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "context_click_background",
                             G_CALLBACK (icon_container_context_click_background_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "icon_position_changed",
                             G_CALLBACK (icon_position_changed_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "icon_text_changed",
                             G_CALLBACK (fm_computer_view_icon_text_changed_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "selection_changed",
                             G_CALLBACK (selection_changed_callback), computer_view, 0);
    /* FIXME: many of these should move into fm-icon-container as virtual methods */
    g_signal_connect_object (icon_container, "get_icon_uri",
                             G_CALLBACK (get_icon_uri_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "get_icon_drop_target_uri",
                             G_CALLBACK (get_icon_drop_target_uri_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "move_copy_items",
                             G_CALLBACK (computer_view_move_copy_items), computer_view, 0);
    g_signal_connect_object (icon_container, "get_container_uri",
                             G_CALLBACK (computer_view_get_container_uri), computer_view, 0);
    g_signal_connect_object (icon_container, "can_accept_item",
                             G_CALLBACK (computer_view_can_accept_item), computer_view, 0);
    g_signal_connect_object (icon_container, "get_stored_icon_position",
                             G_CALLBACK (get_stored_icon_position_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "layout_changed",
                             G_CALLBACK (layout_changed_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "preview",
                             G_CALLBACK (icon_container_preview_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "renaming_icon",
                             G_CALLBACK (renaming_icon_callback), computer_view, 0);
    g_signal_connect_object (icon_container, "icon_stretch_started",
                             G_CALLBACK (fm_directory_view_update_menus), computer_view,
                             G_CONNECT_SWAPPED);
    g_signal_connect_object (icon_container, "icon_stretch_ended",
                             G_CALLBACK (fm_directory_view_update_menus), computer_view,
                             G_CONNECT_SWAPPED);

    g_signal_connect_object (icon_container, "get_stored_layout_timestamp",
                             G_CALLBACK (get_stored_layout_timestamp), computer_view, 0);
    g_signal_connect_object (icon_container, "store_layout_timestamp",
                             G_CALLBACK (store_layout_timestamp), computer_view, 0);
    return icon_container;
}

static void
mount_added_callback(GVolumeMonitor * monitor,GMount *mount,FMComputerView * computer_view)
{
    peony_window_slot_reload(fm_directory_view_get_peony_window_slot(FM_DIRECTORY_VIEW(computer_view)));
}

static void
fm_computer_view_init (FMComputerView *computer_view)
{
    GtkWidget *expander[3],*box,*viewport,*viewport1[3];
    PeonyIconContainer *icon_container[3];
    gint i=0;
    static gboolean setup_sound_preview = FALSE;

    computer_view->details = g_new0 (FMComputerViewDetails, 1);
    
    computer_view->details->filter_by_screen = FALSE;
    computer_view->details->sort = &sort_criteria[0];
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (computer_view),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    GtkAdjustment *hadjustment = gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (computer_view));
    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (computer_view));
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (computer_view), GTK_SHADOW_IN);

    viewport = gtk_viewport_new(hadjustment,
                                  vadjustment);
    viewport1[0]=gtk_viewport_new (hadjustment, vadjustment);
    viewport1[1]=gtk_viewport_new (hadjustment, vadjustment);
    viewport1[2]=gtk_viewport_new (hadjustment, vadjustment);

/*create a box*/
    box = gtk_vbox_new(TRUE,0);
    
    icon_container[0] = create_icon_container (computer_view);
    icon_container[0]->name = "disk";
    icon_container[1] = create_icon_container (computer_view);
    icon_container[1]->name = "movable-disk";
    icon_container[2] = create_icon_container (computer_view);
    icon_container[2]->name = "other";

    GtkWidget *elabel_1,*elabel_2,*elabel_3;
    /*disk-expander*/
    expander[0] = gtk_expander_new(_("Disk"));
    elabel_1 = gtk_expander_get_label_widget(expander[0]);
   // g_object_set(GTK_CONTAINER(expander[0]), "border-width",8,NULL);
    //g_object_set(elabel_1, "xpad",8,NULL);

    /*movable-disk-expander*/
    expander[1] = gtk_expander_new(_("Movable-Disk"));
    elabel_2 = gtk_expander_get_label_widget(expander[1]);
   // g_object_set(GTK_CONTAINER(expander[1]), "border-width",8,NULL);
   // g_object_set(elabel_2, "xpad",8,NULL);

    /*other-expander*/  
    expander[2]= gtk_expander_new(_("Other"));
    elabel_3 = gtk_expander_get_label_widget(expander[2]);
//    g_object_set(GTK_CONTAINER(expander[2]), "border-width",8,NULL);
  //  g_object_set(elabel_3, "xpad",8,NULL);
// gtk_container_set_reallocate_redraws(GTK_CONTAINER(expander[0]),TRUE);
    //gtk_container_set_reallocate_redraws(GTK_CONTAINER(expander[1]),TRUE);
  //  gtk_container_set_reallocate_redraws(GTK_CONTAINER(expander[2]),TRUE);

    for(i=0;i<3;i++)
    {
        gtk_container_add(GTK_CONTAINER(viewport1[i]),GTK_WIDGET (icon_container[i]));
        gtk_container_add(GTK_CONTAINER(expander[i]),GTK_WIDGET (viewport1[i]));
        gtk_expander_set_expanded(GTK_EXPANDER(expander[i]),TRUE);
        gtk_box_pack_start(GTK_BOX(box),expander[i],FALSE,TRUE,0);
    }
    
    gtk_container_add(GTK_CONTAINER(viewport),box);
    gtk_container_add(GTK_CONTAINER(computer_view),viewport);
    fm_computer_view_update_click_mode (computer_view);
    gtk_widget_show(GTK_WIDGET(viewport1[0]));
    gtk_widget_show(GTK_WIDGET(viewport1[1]));
    gtk_widget_show(GTK_WIDGET(viewport1[2]));
    gtk_widget_show(box);
    gtk_widget_show(viewport);
    gtk_widget_show(GTK_WIDGET (icon_container[0]));
    gtk_widget_show(GTK_WIDGET (icon_container[1]));
    gtk_widget_show(GTK_WIDGET (icon_container[2]));
    
    for(i=0;i<3;i++)
    {
        peony_icon_container_set_layout_mode (icon_container[i],
                                                gtk_widget_get_direction (GTK_WIDGET(icon_container[i])) == GTK_TEXT_DIR_RTL ?
                                                PEONY_ICON_LAYOUT_R_L_T_B :
                                                PEONY_ICON_LAYOUT_L_R_T_B);
    }

    if (!setup_sound_preview)
    {
        eel_g_settings_add_auto_enum (peony_preferences,
                                      PEONY_PREFERENCES_PREVIEW_SOUND,
                                      &preview_sound_auto_value);
        setup_sound_preview = TRUE;
    }

        g_signal_connect_swapped (peony_preferences,
                              "changed::" PEONY_PREFERENCES_DEFAULT_SORT_ORDER,
                              G_CALLBACK (default_sort_order_changed_callback),
                              computer_view);
        g_signal_connect_swapped (peony_preferences,
                              "changed::" PEONY_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER,
                              G_CALLBACK (default_sort_in_reverse_order_changed_callback),
                              computer_view);
        g_signal_connect_swapped (peony_icon_view_preferences,
                              "changed::" PEONY_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL,
                              G_CALLBACK (default_zoom_level_changed_callback),
                              computer_view);
        g_signal_connect (G_OBJECT (computer_view),
                        "button_press_event", G_CALLBACK (button_press_callback),
                        computer_view);
        g_signal_connect (computer_view,"button_press_event",G_CALLBACK (create_popup_menu_callback),computer_view);
        GVolumeMonitor *volume_monitor;
        volume_monitor = g_volume_monitor_get ();
        computer_view->details->volume_monitor = volume_monitor;

        g_signal_connect_object (volume_monitor, "mount_added",
                                G_CALLBACK (mount_added_callback),computer_view, 0);   

        computer_view->details->clipboard_handler_id =
        g_signal_connect (peony_clipboard_monitor_get (),
                                "clipboard_info",
                                G_CALLBACK (computer_view_notify_clipboard_info), computer_view);
    
        set_labels_beside_icons (computer_view);

        computer_view->details->popup = NULL;
    
        gtk_widget_show(GTK_WIDGET(computer_view));
}

static PeonyView *
fm_computer_view_create (PeonyWindowSlotInfo *slot)
{
    FMComputerView *view;

    g_assert (PEONY_IS_WINDOW_SLOT_INFO (slot));

    view = g_object_new (FM_TYPE_COMPUTER_VIEW,
                         "window-slot", slot,
                         NULL);

    return PEONY_VIEW (view);
}

static gboolean
fm_computer_view_supports_uri (const char *uri,
                            GFileType file_type,
                            const char *mime_type)
{
    if (g_str_has_prefix (uri, "computer:"))
    {
        return TRUE;
    }
   if (g_str_has_prefix (uri, EEL_SEARCH_URI))
   {
       return FALSE;
   }

    return FALSE;
}

#define TRANSLATE_VIEW_INFO(view_info) \
    view_info.view_combo_label = _(view_info.view_combo_label); \
    view_info.view_menu_label_with_mnemonic = _(view_info.view_menu_label_with_mnemonic); \
    view_info.error_label = _(view_info.error_label); \
    view_info.startup_error_label = _(view_info.startup_error_label); \
    view_info.display_location_label = _(view_info.display_location_label); \

static PeonyViewInfo fm_computer_view =
{
    FM_COMPUTER_VIEW_ID,
    N_("Icon View"),
    //"peony-view-as-normal.svg",
    /* translators: this is used in the view menu */
    N_("_Icons"),
    N_("Computer View"),
    N_("The computer view encountered an error."),
    N_("Display this location with the computer view."),
    fm_computer_view_create,
    fm_computer_view_supports_uri
};

void
fm_computer_view_register (void)
{
    TRANSLATE_VIEW_INFO (fm_computer_view);
    peony_view_factory_register (&fm_computer_view);
}


