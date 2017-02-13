/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 2000, 2001 Eazel, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author: Andy Hertzfeld <andy@eazel.com>
 *
 */

/* notes sidebar panel -- allows editing per-directory notes */

#include <config.h>

#include "peony-notes-viewer.h"

#include <eel/eel-debug.h>
#include <eel/eel-gtk-extensions.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libpeony-private/peony-file-attributes.h>
#include <libpeony-private/peony-file.h>
#include <libpeony-private/peony-file-utilities.h>
#include <libpeony-private/peony-global-preferences.h>
#include <libpeony-private/peony-metadata.h>
#include <libpeony-private/peony-clipboard.h>
#include <libpeony-private/peony-module.h>
#include <libpeony-private/peony-sidebar-provider.h>
#include <libpeony-private/peony-window-info.h>
#include <libpeony-private/peony-window-slot-info.h>
#include <libpeony-extension/peony-property-page-provider.h>

#define SAVE_TIMEOUT 3

static void load_note_text_from_metadata             (PeonyFile                      *file,
        PeonyNotesViewer               *notes);
static void notes_save_metainfo                      (PeonyNotesViewer               *notes);
static void peony_notes_viewer_sidebar_iface_init (PeonySidebarIface              *iface);
static void on_changed                               (GtkEditable                       *editable,
        PeonyNotesViewer               *notes);
static void property_page_provider_iface_init        (PeonyPropertyPageProviderIface *iface);
static void sidebar_provider_iface_init              (PeonySidebarProviderIface       *iface);

typedef struct
{
    GtkScrolledWindowClass parent;
} PeonyNotesViewerClass;

typedef struct
{
    GObject parent;
} PeonyNotesViewerProvider;

typedef struct
{
    GObjectClass parent;
} PeonyNotesViewerProviderClass;


G_DEFINE_TYPE_WITH_CODE (PeonyNotesViewer, peony_notes_viewer, GTK_TYPE_SCROLLED_WINDOW,
                         G_IMPLEMENT_INTERFACE (PEONY_TYPE_SIDEBAR,
                                 peony_notes_viewer_sidebar_iface_init));

static GType peony_notes_viewer_provider_get_type (void);

G_DEFINE_TYPE_WITH_CODE (PeonyNotesViewerProvider, peony_notes_viewer_provider, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (PEONY_TYPE_PROPERTY_PAGE_PROVIDER,
                                 property_page_provider_iface_init);
                         G_IMPLEMENT_INTERFACE (PEONY_TYPE_SIDEBAR_PROVIDER,
                                 sidebar_provider_iface_init));


struct _PeonyNotesViewerDetails
{
    GtkWidget *note_text_field;
    GtkTextBuffer *text_buffer;
    char *uri;
    PeonyFile *file;
    guint save_timeout_id;
    char *previous_saved_text;
    GdkPixbuf *icon;
};

static gboolean
schedule_save_callback (gpointer data)
{
    PeonyNotesViewer *notes;

    notes = data;

    /* Zero out save_timeout_id so no one will try to cancel our
     * in-progress timeout callback.
         */
    notes->details->save_timeout_id = 0;

    notes_save_metainfo (notes);

    return FALSE;
}

static void
cancel_pending_save (PeonyNotesViewer *notes)
{
    if (notes->details->save_timeout_id != 0)
    {
        g_source_remove (notes->details->save_timeout_id);
        notes->details->save_timeout_id = 0;
    }
}

static void
schedule_save (PeonyNotesViewer *notes)
{
    cancel_pending_save (notes);

    notes->details->save_timeout_id = g_timeout_add_seconds (SAVE_TIMEOUT, schedule_save_callback, notes);
}

/* notifies event listeners if the notes data actually changed */
static void
set_saved_text (PeonyNotesViewer *notes, char *new_notes)
{
    char *old_text;

    old_text = notes->details->previous_saved_text;
    notes->details->previous_saved_text = new_notes;

    if (g_strcmp0 (old_text, new_notes) != 0)
    {
        g_signal_emit_by_name (PEONY_SIDEBAR (notes),
                               "tab_icon_changed");
    }

    g_free (old_text);
}

/* save the metainfo corresponding to the current uri, if any, into the text field */
static void
notes_save_metainfo (PeonyNotesViewer *notes)
{
    char *notes_text;
    GtkTextIter start_iter;
    GtkTextIter end_iter;

    if (notes->details->file == NULL)
    {
        return;
    }

    cancel_pending_save (notes);

    /* Block the handler, so we don't respond to our own change.
     */
    g_signal_handlers_block_matched (notes->details->file,
                                     G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                     0, 0, NULL,
                                     G_CALLBACK (load_note_text_from_metadata),
                                     notes);

    gtk_text_buffer_get_start_iter (notes->details->text_buffer, &start_iter);
    gtk_text_buffer_get_end_iter (notes->details->text_buffer, &end_iter);
    notes_text = gtk_text_buffer_get_text (notes->details->text_buffer,
                                           &start_iter,
                                           &end_iter,
                                           FALSE);

    peony_file_set_metadata (notes->details->file,
                            PEONY_METADATA_KEY_ANNOTATION,
                            NULL, notes_text);

    g_signal_handlers_unblock_matched (notes->details->file,
                                       G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                       0, 0, NULL,
                                       G_CALLBACK (load_note_text_from_metadata),
                                       notes);

    set_saved_text (notes, notes_text);
}

static void
load_note_text_from_metadata (PeonyFile *file,
                              PeonyNotesViewer *notes)
{
    char *saved_text;

    g_assert (PEONY_IS_FILE (file));
    g_assert (notes->details->file == file);

    saved_text = peony_file_get_metadata (file, PEONY_METADATA_KEY_ANNOTATION, "");

    /* This fn is called for any change signal on the file, so make sure that the
     * metadata has actually changed.
     */
    if (g_strcmp0 (saved_text, notes->details->previous_saved_text) != 0)
    {
        set_saved_text (notes, saved_text);
        cancel_pending_save (notes);

        /* Block the handler, so we don't respond to our own change.
         */
        g_signal_handlers_block_matched (notes->details->text_buffer,
                                         G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                         0, 0, NULL,
                                         G_CALLBACK (on_changed),
                                         notes);
        gtk_text_buffer_set_text (notes->details->text_buffer, saved_text, -1);
        g_signal_handlers_unblock_matched (notes->details->text_buffer,
                                           G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                           0, 0, NULL,
                                           G_CALLBACK (on_changed),
                                           notes);
    }
    else
    {
        g_free (saved_text);
    }
}

static void
done_with_file (PeonyNotesViewer *notes)
{
    cancel_pending_save (notes);

    if (notes->details->file != NULL)
    {
        peony_file_monitor_remove (notes->details->file, notes);
        g_signal_handlers_disconnect_matched (notes->details->file,
                                              G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                              0, 0, NULL,
                                              G_CALLBACK (load_note_text_from_metadata),
                                              notes);
        peony_file_unref (notes->details->file);
    }
}

static void
notes_load_metainfo (PeonyNotesViewer *notes)
{
    PeonyFileAttributes attributes;

    done_with_file (notes);
    notes->details->file = peony_file_get_by_uri (notes->details->uri);

    /* Block the handler, so we don't respond to our own change.
     */
    g_signal_handlers_block_matched (notes->details->text_buffer,
                                     G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                     0, 0, NULL,
                                     G_CALLBACK (on_changed),
                                     notes);
    gtk_text_buffer_set_text (notes->details->text_buffer, "", -1);
    g_signal_handlers_unblock_matched (notes->details->text_buffer,
                                       G_SIGNAL_MATCH_FUNC | G_SIGNAL_MATCH_DATA,
                                       0, 0, NULL,
                                       G_CALLBACK (on_changed),
                                       notes);

    if (notes->details->file == NULL)
    {
        return;
    }

    attributes = PEONY_FILE_ATTRIBUTE_INFO;
    peony_file_monitor_add (notes->details->file, notes, attributes);

    if (peony_file_check_if_ready (notes->details->file, attributes))
    {
        load_note_text_from_metadata (notes->details->file, notes);
    }

    g_signal_connect (notes->details->file, "changed",
                      G_CALLBACK (load_note_text_from_metadata), notes);
}

static void
loading_uri_callback (PeonyWindowInfo *window,
                      const char *location,
                      PeonyNotesViewer *notes)
{
    if (strcmp (notes->details->uri, location) != 0)
    {
        notes_save_metainfo (notes);
        g_free (notes->details->uri);
        notes->details->uri = g_strdup (location);
        notes_load_metainfo (notes);
    }
}

static gboolean
on_text_field_focus_out_event (GtkWidget *widget,
                               GdkEventFocus *event,
                               gpointer callback_data)
{
    PeonyNotesViewer *notes;

    notes = callback_data;
    notes_save_metainfo (notes);
    return FALSE;
}

static void
on_changed (GtkEditable *editable, PeonyNotesViewer *notes)
{
    schedule_save (notes);
}

static void
peony_notes_viewer_init (PeonyNotesViewer *sidebar)
{
    PeonyNotesViewerDetails *details;
    PeonyIconInfo *info;

    details = g_new0 (PeonyNotesViewerDetails, 1);
    sidebar->details = details;

    details->uri = g_strdup ("");

    info = peony_icon_info_lookup_from_name ("emblem-note", 16);
    details->icon = peony_icon_info_get_pixbuf (info);

    /* create the text container */
    details->text_buffer = gtk_text_buffer_new (NULL);
    details->note_text_field = gtk_text_view_new_with_buffer (details->text_buffer);

    gtk_text_view_set_editable (GTK_TEXT_VIEW (details->note_text_field), TRUE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (details->note_text_field),
                                 GTK_WRAP_WORD);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sidebar),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sidebar),
                                         GTK_SHADOW_IN);
    gtk_scrolled_window_set_hadjustment (GTK_SCROLLED_WINDOW (sidebar), NULL);
    gtk_scrolled_window_set_vadjustment (GTK_SCROLLED_WINDOW (sidebar), NULL);
    gtk_container_add (GTK_CONTAINER (sidebar), details->note_text_field);

    g_signal_connect (details->note_text_field, "focus_out_event",
                      G_CALLBACK (on_text_field_focus_out_event), sidebar);
    g_signal_connect (details->text_buffer, "changed",
                      G_CALLBACK (on_changed), sidebar);

    gtk_widget_show_all (GTK_WIDGET (sidebar));

}

static void
peony_notes_viewer_finalize (GObject *object)
{
    PeonyNotesViewer *sidebar;

    sidebar = PEONY_NOTES_VIEWER (object);

    done_with_file (sidebar);
    if (sidebar->details->icon != NULL)
    {
        g_object_unref (sidebar->details->icon);
    }
    g_free (sidebar->details->uri);
    g_free (sidebar->details->previous_saved_text);
    g_free (sidebar->details);

    G_OBJECT_CLASS (peony_notes_viewer_parent_class)->finalize (object);
}


static void
peony_notes_viewer_class_init (PeonyNotesViewerClass *class)
{
    G_OBJECT_CLASS (class)->finalize = peony_notes_viewer_finalize;
}

static const char *
peony_notes_viewer_get_sidebar_id (PeonySidebar *sidebar)
{
    return PEONY_NOTES_SIDEBAR_ID;
}

static char *
peony_notes_viewer_get_tab_label (PeonySidebar *sidebar)
{
    return g_strdup (_("Notes"));
}

static char *
peony_notes_viewer_get_tab_tooltip (PeonySidebar *sidebar)
{
    return g_strdup (_("Show Notes"));
}

static GdkPixbuf *
peony_notes_viewer_get_tab_icon (PeonySidebar *sidebar)
{
    PeonyNotesViewer *notes;

    notes = PEONY_NOTES_VIEWER (sidebar);

    if (notes->details->previous_saved_text != NULL &&
            notes->details->previous_saved_text[0] != '\0')
    {
        return g_object_ref (notes->details->icon);
    }

    return NULL;
}

static void
peony_notes_viewer_is_visible_changed (PeonySidebar *sidebar,
                                      gboolean         is_visible)
{
    /* Do nothing */
}

static void
peony_notes_viewer_sidebar_iface_init (PeonySidebarIface *iface)
{
    iface->get_sidebar_id = peony_notes_viewer_get_sidebar_id;
    iface->get_tab_label = peony_notes_viewer_get_tab_label;
    iface->get_tab_tooltip = peony_notes_viewer_get_tab_tooltip;
    iface->get_tab_icon = peony_notes_viewer_get_tab_icon;
    iface->is_visible_changed = peony_notes_viewer_is_visible_changed;
}

static void
peony_notes_viewer_set_parent_window (PeonyNotesViewer *sidebar,
                                     PeonyWindowInfo *window)
{
    PeonyWindowSlotInfo *slot;

    slot = peony_window_info_get_active_slot (window);

    g_signal_connect_object (window, "loading_uri",
                             G_CALLBACK (loading_uri_callback), sidebar, 0);

    g_free (sidebar->details->uri);
    sidebar->details->uri = peony_window_slot_info_get_current_location (slot);
    notes_load_metainfo (sidebar);

    peony_clipboard_set_up_text_view
    (GTK_TEXT_VIEW (sidebar->details->note_text_field),
     peony_window_info_get_ui_manager (window));
}

static PeonySidebar *
peony_notes_viewer_create_sidebar (PeonySidebarProvider *provider,
                                  PeonyWindowInfo *window)
{
    PeonyNotesViewer *sidebar;

    sidebar = g_object_new (peony_notes_viewer_get_type (), NULL);
    peony_notes_viewer_set_parent_window (sidebar, window);
    g_object_ref_sink (sidebar);

    return PEONY_SIDEBAR (sidebar);
}

static GList *
get_property_pages (PeonyPropertyPageProvider *provider,
                    GList *files)
{
    GList *pages;
    PeonyPropertyPage *page;
    PeonyFileInfo *file;
    char *uri;
    PeonyNotesViewer *viewer;


    /* Only show the property page if 1 file is selected */
    if (!files || files->next != NULL)
    {
        return NULL;
    }

    pages = NULL;

    file = PEONY_FILE_INFO (files->data);
    uri = peony_file_info_get_uri (file);

    viewer = g_object_new (peony_notes_viewer_get_type (), NULL);
    g_free (viewer->details->uri);
    viewer->details->uri = uri;
    notes_load_metainfo (viewer);

    page = peony_property_page_new
           ("PeonyNotesViewer::property_page",
            gtk_label_new (_("Notes")),
            GTK_WIDGET (viewer));
    pages = g_list_append (pages, page);

    return pages;
}

static void
property_page_provider_iface_init (PeonyPropertyPageProviderIface *iface)
{
    iface->get_pages = get_property_pages;
}

static void
sidebar_provider_iface_init (PeonySidebarProviderIface *iface)
{
    iface->create = peony_notes_viewer_create_sidebar;
}

static void
peony_notes_viewer_provider_init (PeonyNotesViewerProvider *sidebar)
{
}

static void
peony_notes_viewer_provider_class_init (PeonyNotesViewerProviderClass *class)
{
}

void
peony_notes_viewer_register (void)
{
    peony_module_add_type (peony_notes_viewer_provider_get_type ());
}

