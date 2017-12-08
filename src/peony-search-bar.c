/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Copyright (C) 2005 Novell, Inc.
 * Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * Peony is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Anders Carlsson <andersca@imendio.com>
           Zuxun Yang <yangzuxun@kylinos.cn>
 *
 */

#include <config.h>
#include "peony-search-bar.h"

#include <glib/gi18n.h>
#include <eel/eel-gtk-macros.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

struct PeonySearchBarDetails
{
    GtkWidget *entry;
    gboolean entry_borrowed;
	gboolean bDuplicate;
};

enum
{
    ACTIVATE,
    CANCEL,
    FOCUS_IN,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void  peony_search_bar_class_init       (PeonySearchBarClass *class);
static void  peony_search_bar_init             (PeonySearchBar      *bar);

EEL_CLASS_BOILERPLATE (PeonySearchBar,
                       peony_search_bar,
                       GTK_TYPE_EVENT_BOX)


static void
finalize (GObject *object)
{
    PeonySearchBar *bar;

    bar = PEONY_SEARCH_BAR (object);

    g_free (bar->details);

    EEL_CALL_PARENT (G_OBJECT_CLASS, finalize, (object));
}

static void
peony_search_bar_class_init (PeonySearchBarClass *class)
{
    GObjectClass *gobject_class;
    GtkBindingSet *binding_set;

    gobject_class = G_OBJECT_CLASS (class);
    gobject_class->finalize = finalize;

    signals[ACTIVATE] =
        g_signal_new ("activate",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (PeonySearchBarClass, activate),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[FOCUS_IN] =
        g_signal_new ("focus-in",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (PeonySearchBarClass, focus_in),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[CANCEL] =
        g_signal_new ("cancel",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (PeonySearchBarClass, cancel),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    binding_set = gtk_binding_set_by_class (class);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_Escape, 0, "cancel", 0);
}

static gboolean
entry_has_text (PeonySearchBar *bar)
{
    const char *text;

    text = gtk_entry_get_text (GTK_ENTRY (bar->details->entry));

    return text != NULL && text[0] != '\0';
}

static void
entry_icon_release_cb (GtkEntry *entry,
                       GtkEntryIconPosition position,
                       GdkEvent *event,
                       PeonySearchBar *bar)
{
	set_search_bar_duplicate(bar,FALSE);
    g_signal_emit_by_name (entry, "activate", 0);
}

static void
entry_changed_cb (GtkWidget *entry, PeonySearchBar *bar)
{
	//peony_debug_log(TRUE,"search_bar","changed");
	set_search_bar_duplicate(bar,FALSE);
    g_signal_emit_by_name (entry, "activate", 0);
}

static void
entry_activate_cb (GtkWidget *entry, PeonySearchBar *bar)
{
    if (entry_has_text (bar) && !bar->details->entry_borrowed)
    {
    	set_search_bar_duplicate(bar,FALSE);
        g_signal_emit (bar, signals[ACTIVATE], 0);
    }
}

static gboolean
focus_in_event_callback (GtkWidget *widget,
                         GdkEventFocus *event,
                         gpointer user_data)
{
    PeonySearchBar *bar;

    bar = PEONY_SEARCH_BAR (user_data);

    g_signal_emit (bar, signals[FOCUS_IN], 0);

    return FALSE;
}

static void
peony_search_bar_init (PeonySearchBar *bar)
{
    GtkWidget *hbox;
    GtkWidget *label;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context (GTK_WIDGET (bar));
    gtk_style_context_add_class (context, "peony-search-bar");

    bar->details = g_new0 (PeonySearchBarDetails, 1);

    gtk_event_box_set_visible_window (GTK_EVENT_BOX (bar), FALSE);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_margin_start (hbox, 6);
    gtk_widget_set_margin_end (hbox, 6);
    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (bar), hbox);

    label = gtk_label_new (_("Search:"));
    gtk_widget_show (label);

   // gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

    bar->details->entry = gtk_entry_new ();
    gtk_entry_set_icon_from_icon_name (GTK_ENTRY (bar->details->entry),
                                   GTK_ENTRY_ICON_SECONDARY,
                                   "find");
    gtk_box_pack_start (GTK_BOX (hbox), bar->details->entry, TRUE, TRUE, 0);

    g_signal_connect (bar->details->entry, "activate",
                      G_CALLBACK (entry_activate_cb), bar);
    g_signal_connect (bar->details->entry, "icon-release",
                      G_CALLBACK (entry_icon_release_cb), bar);
    g_signal_connect (bar->details->entry, "focus-in-event",
                      G_CALLBACK (focus_in_event_callback), bar);
    g_signal_connect (bar->details->entry, "changed",
                      G_CALLBACK (entry_changed_cb), bar);

    gtk_widget_show (bar->details->entry);
}

GtkWidget *
peony_search_bar_borrow_entry (PeonySearchBar *bar)
{
    GtkBindingSet *binding_set;

    bar->details->entry_borrowed = TRUE;

    binding_set = gtk_binding_set_by_class (G_OBJECT_GET_CLASS (bar));
	gtk_binding_entry_remove (binding_set, GDK_KEY_Escape, 0);
    return bar->details->entry;
}

void
peony_search_bar_return_entry (PeonySearchBar *bar)
{
    GtkBindingSet *binding_set;

    bar->details->entry_borrowed = FALSE;

    binding_set = gtk_binding_set_by_class (G_OBJECT_GET_CLASS (bar));
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_Escape, 0, "cancel", 0);
}

GtkWidget *
peony_search_bar_new (void)
{
    GtkWidget *bar;

    bar = g_object_new (PEONY_TYPE_SEARCH_BAR, NULL);

    return bar;
}

PeonyQuery *
peony_search_bar_get_query (PeonySearchBar *bar,gboolean bDuplicate)
{
    const char *query_text;
    PeonyQuery *query;

    query_text = gtk_entry_get_text (GTK_ENTRY (bar->details->entry));

    /* Empty string is a NULL query */
    if (FALSE == bDuplicate && query_text && query_text[0] == '\0')
    {
        return NULL;
    }

    query = peony_query_new ();
    peony_query_set_text (query, query_text);

    return query;
}

void
peony_search_bar_grab_focus (PeonySearchBar *bar)
{
    gtk_widget_grab_focus (bar->details->entry);
}

void
peony_search_bar_clear (PeonySearchBar *bar)
{
    gtk_entry_set_text (GTK_ENTRY (bar->details->entry), "");
}

void find_duplicate_signal (gpointer user_data)
{
	PeonySearchBar *search_bar = NULL;
	if(NULL == user_data)
	{
		peony_debug_log(TRUE,"_find_","find_duplicate_signal param null.");
		return;
	}

	search_bar = PEONY_SEARCH_BAR(user_data);
	set_search_bar_duplicate(search_bar,TRUE);
	g_signal_emit_by_name (search_bar, "activate", 0);
	return;
}

void
set_search_bar_duplicate (PeonySearchBar *bar,gboolean bDuplicate)
{
	if (bar == NULL)
    {
        return;
    }
	
    bar->details->bDuplicate = bDuplicate;
}

gboolean
get_search_bar_duplicate (PeonySearchBar *bar)
{
	if (bar == NULL)
    {
        return FALSE;
    }
	
    return bar->details->bDuplicate;
}

