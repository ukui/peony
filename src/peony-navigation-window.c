/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *  Copyright (C) 2003 Ximian, Inc.
 *  Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
 *
 *  Peony is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Peony is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *  	     John Sullivan <sullivan@eazel.com>
 *           Zuxun Yang <yangzuxun@kylinos.cn>
 *
 */

/* peony-window.c: Implementation of the main window object */

#include <config.h>
#include "peony-window-private.h"

#include "file-manager/test-widget.h"
#include "file-manager/mime-utils.h"
#include "file-manager/office-utils.h"
#include "file-manager/pdfviewer.h"
#include <webkit/webkit.h>

#include "peony-actions.h"
#include "peony-application.h"
#include "peony-bookmarks-window.h"
#include "peony-location-bar.h"
#include "peony-query-editor.h"
#include "peony-search-bar.h"
#include "peony-navigation-window-slot.h"
#include "peony-notebook.h"
#include "peony-window-manage-views.h"
#include "peony-navigation-window-pane.h"
#include <eel/eel-gtk-extensions.h>
#include <eel/eel-gtk-macros.h>
#include <eel/eel-string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#ifdef HAVE_X11_XF86KEYSYM_H
#include <X11/XF86keysym.h>
#endif
#include <libpeony-private/peony-file-utilities.h>
#include <libpeony-private/peony-file-attributes.h>
#include <libpeony-private/peony-global-preferences.h>
#include <libpeony-private/peony-icon-info.h>
#include <libpeony-private/peony-metadata.h>
#include <libpeony-private/peony-mime-actions.h>
#include <libpeony-private/peony-program-choosing.h>
#include <libpeony-private/peony-sidebar.h>
#include <libpeony-private/peony-view-factory.h>
#include <libpeony-private/peony-clipboard.h>
#include <libpeony-private/peony-module.h>
#include <libpeony-private/peony-sidebar-provider.h>
#include <libpeony-private/peony-search-directory.h>
#include <libpeony-private/peony-signaller.h>
#include <math.h>
#include <sys/time.h>

#define MAX_TITLE_LENGTH 180

#define MENU_PATH_BOOKMARKS_PLACEHOLDER			"/MenuBar/Other Menus/Bookmarks/Bookmarks Placeholder"

/* Forward and back buttons on the mouse */
static gboolean mouse_extra_buttons = TRUE;
static int mouse_forward_button = 9;
static int mouse_back_button = 8;

static void add_sidebar_panels                       (PeonyNavigationWindow *window);
static void side_panel_image_changed_callback        (PeonySidebar          *side_panel,
        gpointer                  callback_data);
static void always_use_location_entry_changed        (gpointer                  callback_data);
static void always_use_browser_changed               (gpointer                  callback_data);
static void mouse_back_button_changed		     (gpointer                  callback_data);
static void mouse_forward_button_changed	     (gpointer                  callback_data);
static void use_extra_mouse_buttons_changed          (gpointer                  callback_data);
static PeonyWindowSlot *create_extra_pane         (PeonyNavigationWindow *window);

static void preview_file_changed_callback(GObject *singaller, gpointer data);
static void office_format_trans_ready_callback(GObject *singaller, gpointer data);

G_DEFINE_TYPE (PeonyNavigationWindow, peony_navigation_window, PEONY_TYPE_WINDOW)
#define parent_class peony_navigation_window_parent_class

static const struct
{
    unsigned int keyval;
    const char *action;
} extra_navigation_window_keybindings [] =
{
#ifdef HAVE_X11_XF86KEYSYM_H
    { XF86XK_Back,		PEONY_ACTION_BACK },
    { XF86XK_Forward,	PEONY_ACTION_FORWARD }
#endif
};
static void
choose_icon_view (GtkButton *widget,gpointer data)
{
    PeonyWindow *window;
    PeonyWindowSlot *slot;
    char    *uri;
    
    window = PEONY_WINDOW (data);
    slot = peony_window_get_active_slot(window);
    uri = peony_window_slot_get_location_uri(slot);
    if (strcmp (uri, "computer:///") == 0)
        peony_window_slot_set_content_view (slot, "OAFIID:Peony_File_Manager_Computer_View");
    else
        peony_window_slot_set_content_view (slot,"OAFIID:Peony_File_Manager_Icon_View");
}

static void
choose_list_view (GtkButton *widget,gpointer data)
{
    PeonyWindow *window;
    PeonyWindowSlot *slot;
    window = PEONY_WINDOW (data);
    slot = peony_window_get_active_slot(window);
    peony_window_slot_set_content_view (slot,"OAFIID:Peony_File_Manager_List_View");
}

//as we use global signaller recieving the data of filename, 
//we couldn't get the handle of window in callback method.
//so, it seems that we have to use static val to get window globally.
static PeonyNavigationWindow *global_window;
static char* global_preview_filename;
static char* global_preview_office2pdf_filename;
static char* global_preview_excel2html_filename;
//static TestWidget *global_test_widget;
//static GtkWidget *global_pdf_view;
//static GtkWidget *empty_widget;

static void
peony_navigation_window_init (PeonyNavigationWindow *window)
{
    GtkUIManager *ui_manager;
    GtkWidget *toolbar;
    PeonyWindow *win;
    PeonyNavigationWindowPane *pane;
    GtkWidget *hpaned;
    GtkWidget *vbox;
    GtkWidget *add_toolbar_vbox;
    GtkImage  *iconView_image;
    GtkImage *listView_image;
    GtkWidget *iconView_button;
    GtkWidget *listView_button;

    //pdf_viewer_init();
	
    win = PEONY_WINDOW (window);

    window->details = G_TYPE_INSTANCE_GET_PRIVATE (window, PEONY_TYPE_NAVIGATION_WINDOW, PeonyNavigationWindowDetails);

    GtkStyleContext *context;

    context = gtk_widget_get_style_context (GTK_WIDGET (window));
    gtk_style_context_add_class (context, "peony-navigation-window");

    pane = peony_navigation_window_pane_new (win);
    win->details->panes = g_list_prepend (win->details->panes, pane);

    window->details->header_size_group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);
    gtk_size_group_set_ignore_hidden (window->details->header_size_group, FALSE);

    window->details->content_paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand (window->details->content_paned, TRUE);
    gtk_widget_set_vexpand (window->details->content_paned, TRUE);
    gtk_grid_attach (GTK_GRID (PEONY_WINDOW (window)->details->grid),
                     window->details->content_paned,
                     0, 3, 1, 1);
    gtk_widget_show (window->details->content_paned);

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous (vbox,FALSE);
    gtk_paned_pack2 (GTK_PANED (window->details->content_paned), vbox,
    		     TRUE, FALSE);
    gtk_widget_show (vbox);
   
    hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start (GTK_BOX (vbox), hpaned, TRUE, TRUE, 0);
    gtk_widget_show (hpaned);

    window->toolbar_table= gtk_grid_new();
    window->details->split_view_hpane = hpaned;

    gtk_box_pack_start (GTK_BOX (vbox), win->details->statusbar, FALSE, FALSE, 0);
    gtk_widget_show (win->details->statusbar);

    peony_navigation_window_pane_setup (pane);

    gtk_paned_pack1 (GTK_PANED(hpaned), pane->widget, TRUE, FALSE);
    gtk_widget_show (pane->widget);

    //GtkWidget *test = test_widget_new();
    //open_file_cb(test,"/home/lanyue/ui");
    //window->details->test_widget = test_widget_new();
    //open_file_cb(window->details->test_widget, "/home/lanyue/ui");
    //window->details->test_widget = gtk_scrolled_window_new(NULL,NULL);
    //gtk_paned_pack2 (GTK_PANED(hpaned), window->details->test_widget, TRUE, FALSE);
    //gtk_widget_show (window->details->test_widget);

    /* this has to be done after the location bar has been set up,
     * but before menu stuff is being called */
    peony_window_set_active_pane (win, PEONY_WINDOW_PANE (pane));

    peony_navigation_window_initialize_actions (window);

    peony_navigation_window_initialize_menus (window);

    ui_manager = peony_window_get_ui_manager (PEONY_WINDOW (window));
    toolbar = gtk_ui_manager_get_widget (ui_manager, "/Toolbar");
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar),GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_widget_set_size_request(toolbar,-1,24);
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar),GTK_TOOLBAR_ICONS);
    gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar),FALSE);
    gtk_style_context_add_class (gtk_widget_get_style_context (toolbar), GTK_STYLE_CLASS_PRIMARY_TOOLBAR);
	#if 0
    window->toolbarViewAs = gtk_ui_manager_get_widget(ui_manager, "/ToolbarViewAs");
    gtk_toolbar_set_icon_size(GTK_TOOLBAR(window->toolbarViewAs),GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_toolbar_set_style(GTK_TOOLBAR(window->toolbarViewAs), GTK_TOOLBAR_ICONS);
    gtk_widget_set_size_request(window->toolbarViewAs, 50, 32);
    #else
    iconView_button = gtk_button_new();
	gtk_button_set_relief(iconView_button,GTK_RELIEF_NONE);
    iconView_image = gtk_image_new_from_file ("/usr/share/pixmaps/peony/peony-view-as-normal.png");
    gtk_button_set_image (iconView_button,iconView_image);

    listView_button =gtk_button_new ();
	gtk_button_set_relief(listView_button,GTK_RELIEF_NONE);
    listView_image = gtk_image_new_from_file ("/usr/share/pixmaps/peony/peony-view-as-list.png");
    gtk_button_set_image (listView_button,listView_image);

    window->viewAsbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_start (GTK_BOX (window->viewAsbox), iconView_button, FALSE, FALSE, 2);
	gtk_box_pack_start (GTK_BOX (window->viewAsbox), listView_button, FALSE, FALSE, 2);
	#endif
    window->details->toolbar = toolbar;
    gtk_widget_set_hexpand (toolbar, FALSE);
    add_toolbar_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,3);
    gtk_box_set_homogeneous (add_toolbar_vbox,FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(add_toolbar_vbox),3);
    gtk_box_pack_start(GTK_BOX(add_toolbar_vbox),toolbar,TRUE,TRUE,0);
    gtk_widget_show (add_toolbar_vbox);
    gtk_widget_set_size_request (add_toolbar_vbox,36,36);

    GtkWidget *separator= gtk_hseparator_new();
    gtk_grid_attach (window->toolbar_table,
                        separator,
	                    0,0,3,1);

    gtk_grid_attach (window->toolbar_table,
                        add_toolbar_vbox,
	                    0,1,1,1);
    gtk_grid_attach (window->toolbar_table,
                    window->viewAsbox,
                        2,1,1,1);

    gtk_grid_attach (GTK_GRID (PEONY_WINDOW (window)->details->grid),
                    // toolbar,
		    window->toolbar_table,
                     0, 1, 1, 1);
    gtk_widget_show (toolbar);
    gtk_widget_show_all (window->toolbar_table);
    gtk_widget_show (window->viewAsbox);
    g_signal_connect (iconView_button,"pressed",G_CALLBACK (choose_icon_view),window);

    g_signal_connect (listView_button,"pressed",G_CALLBACK (choose_list_view),window);


    peony_navigation_window_initialize_toolbars (window);

    /* Set initial sensitivity of some buttons & menu items
     * now that they're all created.
     */
    peony_navigation_window_allow_back (window, FALSE);
    peony_navigation_window_allow_forward (window, FALSE);

    g_signal_connect_swapped (peony_preferences,
                              "changed::" PEONY_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY,
                              G_CALLBACK(always_use_location_entry_changed),
                              window);

    g_signal_connect_swapped (peony_preferences,
                              "changed::" PEONY_PREFERENCES_ALWAYS_USE_BROWSER,
                              G_CALLBACK(always_use_browser_changed),
                              window);

    //pdf_viewer_init();
}

static void
always_use_location_entry_changed (gpointer callback_data)
{
    PeonyNavigationWindow *window;
    GList *walk;
    gboolean use_entry;

    window = PEONY_NAVIGATION_WINDOW (callback_data);

    use_entry = g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY);

    for (walk = PEONY_WINDOW(window)->details->panes; walk; walk = walk->next)
    {
        peony_navigation_window_pane_always_use_location_entry (walk->data, use_entry);
    }
}

static void
always_use_browser_changed (gpointer callback_data)
{
    PeonyNavigationWindow *window;

    window = PEONY_NAVIGATION_WINDOW (callback_data);

    peony_navigation_window_update_spatial_menu_item (window);
}

/* Sanity check: highest mouse button value I could find was 14. 5 is our
 * lower threshold (well-documented to be the one of the button events for the
 * scrollwheel), so it's hardcoded in the functions below. However, if you have
 * a button that registers higher and want to map it, file a bug and
 * we'll move the bar. Makes you wonder why the X guys don't have
 * defined values for these like the XKB stuff, huh?
 */
#define UPPER_MOUSE_LIMIT 14

static void
mouse_back_button_changed (gpointer callback_data)
{
    int new_back_button;

    new_back_button = g_settings_get_int (peony_preferences, PEONY_PREFERENCES_MOUSE_BACK_BUTTON);

    /* Bounds checking */
    if (new_back_button < 6 || new_back_button > UPPER_MOUSE_LIMIT)
        return;

    mouse_back_button = new_back_button;
}

static void
mouse_forward_button_changed (gpointer callback_data)
{
    int new_forward_button;

    new_forward_button = g_settings_get_int (peony_preferences, PEONY_PREFERENCES_MOUSE_FORWARD_BUTTON);

    /* Bounds checking */
    if (new_forward_button < 6 || new_forward_button > UPPER_MOUSE_LIMIT)
        return;

    mouse_forward_button = new_forward_button;
}

static void
use_extra_mouse_buttons_changed (gpointer callback_data)
{
    mouse_extra_buttons = g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_MOUSE_USE_EXTRA_BUTTONS);
}

void
peony_navigation_window_unset_focus_widget (PeonyNavigationWindow *window)
{
    if (window->details->last_focus_widget != NULL)
    {
        g_object_remove_weak_pointer (G_OBJECT (window->details->last_focus_widget),
                                      (gpointer *) &window->details->last_focus_widget);
        window->details->last_focus_widget = NULL;
    }
}

gboolean
peony_navigation_window_is_in_temporary_navigation_bar (GtkWidget *widget,
        PeonyNavigationWindow *window)
{
    GList *walk;
    gboolean is_in_any = FALSE;

    for (walk = PEONY_WINDOW(window)->details->panes; walk; walk = walk->next)
    {
        PeonyNavigationWindowPane *pane = walk->data;
        if(gtk_widget_get_ancestor (widget, PEONY_TYPE_LOCATION_BAR) != NULL &&
                pane->temporary_navigation_bar)
            is_in_any = TRUE;
    }
    return is_in_any;
}

gboolean
peony_navigation_window_is_in_temporary_search_bar (GtkWidget *widget,
        PeonyNavigationWindow *window)
{
    GList *walk;
    gboolean is_in_any = FALSE;

    for (walk = PEONY_WINDOW(window)->details->panes; walk; walk = walk->next)
    {
        PeonyNavigationWindowPane *pane = walk->data;
        if(gtk_widget_get_ancestor (widget, PEONY_TYPE_SEARCH_BAR) != NULL &&
                pane->temporary_search_bar)
            is_in_any = TRUE;
    }
    return is_in_any;
}

static void
remember_focus_widget (PeonyNavigationWindow *window)
{
    PeonyNavigationWindow *navigation_window;
    GtkWidget *focus_widget;

    navigation_window = PEONY_NAVIGATION_WINDOW (window);

    focus_widget = gtk_window_get_focus (GTK_WINDOW (window));
    if (focus_widget != NULL &&
            !peony_navigation_window_is_in_temporary_navigation_bar (focus_widget, navigation_window) &&
            !peony_navigation_window_is_in_temporary_search_bar (focus_widget, navigation_window))
    {
        peony_navigation_window_unset_focus_widget (navigation_window);

        navigation_window->details->last_focus_widget = focus_widget;
        g_object_add_weak_pointer (G_OBJECT (focus_widget),
                                   (gpointer *) &(PEONY_NAVIGATION_WINDOW (window)->details->last_focus_widget));
    }
}

void
peony_navigation_window_restore_focus_widget (PeonyNavigationWindow *window)
{
    if (window->details->last_focus_widget != NULL)
    {
        if (PEONY_IS_VIEW (window->details->last_focus_widget))
        {
            peony_view_grab_focus (PEONY_VIEW (window->details->last_focus_widget));
        }
        else
        {
            gtk_widget_grab_focus (window->details->last_focus_widget);
        }

        peony_navigation_window_unset_focus_widget (window);
    }
}

static void
side_pane_close_requested_callback (GtkWidget *widget,
                                    gpointer user_data)
{
    PeonyNavigationWindow *window;

    window = PEONY_NAVIGATION_WINDOW (user_data);

    peony_navigation_window_hide_sidebar (window);
}

static void
side_pane_size_allocate_callback (GtkWidget *widget,
                                  GtkAllocation *allocation,
                                  gpointer user_data)
{
    PeonyNavigationWindow *window;

    window = PEONY_NAVIGATION_WINDOW (user_data);

    if (allocation->width != window->details->side_pane_width)
    {
        window->details->side_pane_width = allocation->width;
        g_settings_set_int (peony_window_state,
                            PEONY_WINDOW_STATE_SIDEBAR_WIDTH,
                            allocation->width <= 1 ? 0 : allocation->width);
    }
}

static void
setup_side_pane_width (PeonyNavigationWindow *window)
{
    g_return_if_fail (window->sidebar != NULL);

    window->details->side_pane_width =
        g_settings_get_int (peony_window_state,
                            PEONY_WINDOW_STATE_SIDEBAR_WIDTH);

    gtk_paned_set_position (GTK_PANED (window->details->content_paned),
                            window->details->side_pane_width);
}

static void
set_current_side_panel (PeonyNavigationWindow *window,
                        PeonySidebar *panel)
{
    if (window->details->current_side_panel)
    {
        peony_sidebar_is_visible_changed (window->details->current_side_panel,
                                         FALSE);
        eel_remove_weak_pointer (&window->details->current_side_panel);
    }

    if (panel != NULL)
    {
        peony_sidebar_is_visible_changed (panel, TRUE);
    }
    window->details->current_side_panel = panel;
    eel_add_weak_pointer (&window->details->current_side_panel);
}

static void
side_pane_switch_page_callback (PeonySidePane *side_pane,
                                GtkWidget *widget,
                                PeonyNavigationWindow *window)
{
    const char *id;
    PeonySidebar *sidebar;

    sidebar = PEONY_SIDEBAR (widget);

    if (sidebar == NULL)
    {
        return;
    }

    set_current_side_panel (window, sidebar);

    id = peony_sidebar_get_sidebar_id (sidebar);
    g_settings_set_string (peony_window_state, PEONY_WINDOW_STATE_SIDE_PANE_VIEW, id);
}

static void
peony_navigation_window_set_up_sidebar (PeonyNavigationWindow *window)
{
    GtkWidget *title;

    window->sidebar = peony_side_pane_new ();

    title = peony_side_pane_get_title (window->sidebar);
    gtk_size_group_add_widget (window->details->header_size_group,
                               title);

    gtk_paned_pack1 (GTK_PANED (window->details->content_paned),
                     GTK_WIDGET (window->sidebar),
                     FALSE, FALSE);

    setup_side_pane_width (window);
    g_signal_connect (window->sidebar,
                      "size_allocate",
                      G_CALLBACK (side_pane_size_allocate_callback),
                      window);

    add_sidebar_panels (window);

    g_signal_connect (window->sidebar,
                      "close_requested",
                      G_CALLBACK (side_pane_close_requested_callback),
                      window);

    g_signal_connect (window->sidebar,
                      "switch_page",
                      G_CALLBACK (side_pane_switch_page_callback),
                      window);

    gtk_widget_show (GTK_WIDGET (window->sidebar));
}

static void
peony_navigation_window_tear_down_sidebar (PeonyNavigationWindow *window)
{
    GList *node, *next;
    PeonySidebar *sidebar_panel;

    g_signal_handlers_disconnect_by_func (window->sidebar,
                                          side_pane_switch_page_callback,
                                          window);

    for (node = window->sidebar_panels; node != NULL; node = next)
    {
        next = node->next;

        sidebar_panel = PEONY_SIDEBAR (node->data);

        peony_navigation_window_remove_sidebar_panel (window,
                sidebar_panel);
    }

    gtk_widget_destroy (GTK_WIDGET (window->sidebar));
    window->sidebar = NULL;
}

static gboolean
peony_navigation_window_state_event (GtkWidget *widget,
                                    GdkEventWindowState *event)
{
    if (event->changed_mask & GDK_WINDOW_STATE_MAXIMIZED)
    {
        g_settings_set_boolean (peony_window_state, PEONY_WINDOW_STATE_MAXIMIZED,
                                event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED);
    }

    if (GTK_WIDGET_CLASS (parent_class)->window_state_event != NULL)
    {
        return GTK_WIDGET_CLASS (parent_class)->window_state_event (widget, event);
    }

    return FALSE;
}

static gboolean
peony_navigation_window_key_press_event (GtkWidget *widget,
                                        GdkEventKey *event)
{
    PeonyNavigationWindow *window;
    int i;

    window = PEONY_NAVIGATION_WINDOW (widget);

    for (i = 0; i < G_N_ELEMENTS (extra_navigation_window_keybindings); i++)
    {
        if (extra_navigation_window_keybindings[i].keyval == event->keyval)
        {
            GtkAction *action;

            action = gtk_action_group_get_action (window->details->navigation_action_group,
                                                  extra_navigation_window_keybindings[i].action);

            g_assert (action != NULL);
            if (gtk_action_is_sensitive (action))
            {
                gtk_action_activate (action);
                return TRUE;
            }

            break;
        }
    }

    return GTK_WIDGET_CLASS (peony_navigation_window_parent_class)->key_press_event (widget, event);
}

static gboolean
peony_navigation_window_button_press_event (GtkWidget *widget,
        GdkEventButton *event)
{
    PeonyNavigationWindow *window;
    gboolean handled;

    handled = FALSE;
    window = PEONY_NAVIGATION_WINDOW (widget);

    if (mouse_extra_buttons && (event->button == mouse_back_button))
    {
        peony_navigation_window_go_back (window);
        handled = TRUE;
    }
    else if (mouse_extra_buttons && (event->button == mouse_forward_button))
    {
        peony_navigation_window_go_forward (window);
        handled = TRUE;
    }
    else if (GTK_WIDGET_CLASS (peony_navigation_window_parent_class)->button_press_event)
    {
        handled = GTK_WIDGET_CLASS (peony_navigation_window_parent_class)->button_press_event (widget, event);
    }
    else
    {
        handled = FALSE;
    }
    return handled;
}

static void
peony_navigation_window_destroy (GtkWidget *object)
{
    g_signal_handlers_disconnect_by_func(peony_signaller_get_current (), G_CALLBACK (preview_file_changed_callback), global_preview_filename);
    g_signal_handlers_disconnect_by_func(peony_signaller_get_current (), G_CALLBACK (office_format_trans_ready_callback), global_preview_office2pdf_filename);

    PeonyNavigationWindow *window;

    window = PEONY_NAVIGATION_WINDOW (object);

    peony_navigation_window_unset_focus_widget (window);

    window->sidebar = NULL;
    g_list_foreach (window->sidebar_panels, (GFunc)g_object_unref, NULL);
    g_list_free (window->sidebar_panels);
    window->sidebar_panels = NULL;

    window->details->content_paned = NULL;
    window->details->split_view_hpane = NULL;

    //gtk_widget_destroy(window->details->test_widget);
    window->details->test_widget = NULL;

    global_window = NULL;
    global_preview_filename = NULL;
    global_preview_office2pdf_filename = NULL;
    global_preview_excel2html_filename = NULL;
    //global_test_widget = NULL;

    //pdf_viewer_shutdown();    

    GTK_WIDGET_CLASS (parent_class)->destroy (object);
}

static void
peony_navigation_window_finalize (GObject *object)
{
    PeonyNavigationWindow *window;

    window = PEONY_NAVIGATION_WINDOW (object);

    peony_navigation_window_remove_go_menu_callback (window);

    g_signal_handlers_disconnect_by_func (peony_preferences,
                                          always_use_browser_changed,
                                          window);
    g_signal_handlers_disconnect_by_func (peony_preferences,
                                          always_use_location_entry_changed,
                                          window);

    G_OBJECT_CLASS (parent_class)->finalize (object);
}

/*
 * Main API
 */

void
peony_navigation_window_add_sidebar_panel (PeonyNavigationWindow *window,
        PeonySidebar *sidebar_panel)
{
    const char *sidebar_id;
    char *label;
    char *tooltip;
    char *default_id;
    GdkPixbuf *icon;

    g_return_if_fail (PEONY_IS_NAVIGATION_WINDOW (window));
    g_return_if_fail (PEONY_IS_SIDEBAR (sidebar_panel));
    g_return_if_fail (PEONY_IS_SIDE_PANE (window->sidebar));
    g_return_if_fail (g_list_find (window->sidebar_panels, sidebar_panel) == NULL);

    label = peony_sidebar_get_tab_label (sidebar_panel);
    tooltip = peony_sidebar_get_tab_tooltip (sidebar_panel);
    peony_side_pane_add_panel (window->sidebar,
                              GTK_WIDGET (sidebar_panel),
                              label,
                              tooltip);
    g_free (tooltip);
    g_free (label);

    icon = peony_sidebar_get_tab_icon (sidebar_panel);
    peony_side_pane_set_panel_image (PEONY_NAVIGATION_WINDOW (window)->sidebar,
                                    GTK_WIDGET (sidebar_panel),
                                    icon);
    if (icon)
    {
        g_object_unref (icon);
    }

    g_signal_connect (sidebar_panel, "tab_icon_changed",
                      (GCallback)side_panel_image_changed_callback, window);

    window->sidebar_panels = g_list_prepend (window->sidebar_panels,
                             g_object_ref (sidebar_panel));

    /* Show if default */
    sidebar_id = peony_sidebar_get_sidebar_id (sidebar_panel);
    default_id = g_settings_get_string (peony_window_state, PEONY_WINDOW_STATE_SIDE_PANE_VIEW);
    if (sidebar_id && default_id && !strcmp (sidebar_id, default_id))
    {
        peony_side_pane_show_panel (window->sidebar,
                                   GTK_WIDGET (sidebar_panel));
    }
    g_free (default_id);
}

void
peony_navigation_window_remove_sidebar_panel (PeonyNavigationWindow *window,
        PeonySidebar *sidebar_panel)
{
    g_return_if_fail (PEONY_IS_NAVIGATION_WINDOW (window));
    g_return_if_fail (PEONY_IS_SIDEBAR (sidebar_panel));

    if (g_list_find (window->sidebar_panels, sidebar_panel) == NULL)
    {
        return;
    }

    g_signal_handlers_disconnect_by_func (sidebar_panel, side_panel_image_changed_callback, window);

    peony_side_pane_remove_panel (window->sidebar,
                                 GTK_WIDGET (sidebar_panel));
    window->sidebar_panels = g_list_remove (window->sidebar_panels, sidebar_panel);
    g_object_unref (sidebar_panel);
}

void
peony_navigation_window_go_back (PeonyNavigationWindow *window)
{
    peony_navigation_window_back_or_forward (window, TRUE, 0, FALSE);
}

void
peony_navigation_window_go_forward (PeonyNavigationWindow *window)
{
    peony_navigation_window_back_or_forward (window, FALSE, 0, FALSE);
}

void
peony_navigation_window_allow_back (PeonyNavigationWindow *window, gboolean allow)
{
    GtkAction *action;

    action = gtk_action_group_get_action (window->details->navigation_action_group,
                                          PEONY_ACTION_BACK);

    gtk_action_set_sensitive (action, allow);
}

void
peony_navigation_window_allow_forward (PeonyNavigationWindow *window, gboolean allow)
{
    GtkAction *action;

    action = gtk_action_group_get_action (window->details->navigation_action_group,
                                          PEONY_ACTION_FORWARD);

    gtk_action_set_sensitive (action, allow);
}

static void
real_sync_title (PeonyWindow *window,
                 PeonyWindowSlot *slot)
{
    PeonyNavigationWindowPane *pane;
    PeonyNotebook *notebook;
    char *full_title;
    char *window_title;

    EEL_CALL_PARENT (PEONY_WINDOW_CLASS,
                     sync_title, (window, slot));

    if (slot == window->details->active_pane->active_slot)
    {
        /* if spatial mode is default, we keep "File Browser" in the window title
         * to recognize browser windows. Otherwise, we default to the directory name.
         */
        if (!g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_ALWAYS_USE_BROWSER))
        {
            full_title = g_strdup_printf (_("%s - File Browser"), slot->title);
            window_title = eel_str_middle_truncate (full_title, MAX_TITLE_LENGTH);
            g_free (full_title);
        }
        else
        {
            window_title = eel_str_middle_truncate (slot->title, MAX_TITLE_LENGTH);
        }

        gtk_window_set_title (GTK_WINDOW (window), window_title);
        g_free (window_title);
    }

    pane = PEONY_NAVIGATION_WINDOW_PANE (slot->pane);
    notebook = PEONY_NOTEBOOK (pane->notebook);
    peony_notebook_sync_tab_label (notebook, slot);
}

static PeonyIconInfo *
real_get_icon (PeonyWindow *window,
               PeonyWindowSlot *slot)
{
    return peony_file_get_icon (slot->viewed_file, 48,
                               PEONY_FILE_ICON_FLAGS_IGNORE_VISITING |
                               PEONY_FILE_ICON_FLAGS_USE_MOUNT_ICON);
}

static void
real_sync_allow_stop (PeonyWindow *window,
                      PeonyWindowSlot *slot)
{
    PeonyNavigationWindow *navigation_window;
    PeonyNotebook *notebook;

    navigation_window = PEONY_NAVIGATION_WINDOW (window);
    peony_navigation_window_set_spinner_active (navigation_window, slot->allow_stop);

    notebook = PEONY_NOTEBOOK (PEONY_NAVIGATION_WINDOW_PANE (slot->pane)->notebook);
    peony_notebook_sync_loading (notebook, slot);
}

static void
real_prompt_for_location (PeonyWindow *window, const char *initial)
{
    PeonyNavigationWindowPane *pane;

    remember_focus_widget (PEONY_NAVIGATION_WINDOW (window));

    pane = PEONY_NAVIGATION_WINDOW_PANE (window->details->active_pane);

    peony_navigation_window_pane_show_location_bar_temporarily (pane);
    peony_navigation_window_pane_show_navigation_bar_temporarily (pane);

    if (initial)
    {
		peony_set_location_bar_emit_flag(PEONY_LOCATION_BAR (pane->navigation_bar),FALSE);
        peony_location_bar_set_location (PEONY_LOCATION_BAR (pane->navigation_bar),
                                        initial);
    }
}

void
peony_navigation_window_show_search (PeonyNavigationWindow *window)
{
    PeonyNavigationWindowPane *pane;

    pane = PEONY_NAVIGATION_WINDOW_PANE (PEONY_WINDOW (window)->details->active_pane);
    if (!peony_navigation_window_pane_search_bar_showing (pane))
    {
        remember_focus_widget (window);

        peony_navigation_window_pane_show_location_bar_temporarily (pane);
        peony_navigation_window_pane_set_bar_mode (pane, PEONY_BAR_SEARCH);
        pane->temporary_search_bar = TRUE;
        peony_search_bar_clear (PEONY_SEARCH_BAR (pane->search_bar));
    }

    peony_search_bar_grab_focus (PEONY_SEARCH_BAR (pane->search_bar));
}

void
peony_navigation_window_hide_search (PeonyNavigationWindow *window)
{
    PeonyNavigationWindowPane *pane = PEONY_NAVIGATION_WINDOW_PANE (PEONY_WINDOW (window)->details->active_pane);
    if (peony_navigation_window_pane_search_bar_showing (pane))
    {
        if (peony_navigation_window_pane_hide_temporary_bars (pane))
        {
            peony_navigation_window_restore_focus_widget (window);
        }
    }
}

/* This updates the UI state of the search button, but does not
   in itself trigger a search action */
void
peony_navigation_window_set_search_button (PeonyNavigationWindow *window,
        gboolean state)
{
    GtkAction *action;

    action = gtk_action_group_get_action (window->details->navigation_action_group,
                                          "Search");

    /* Block callback so we don't activate the action and thus focus the
       search entry */
    g_object_set_data (G_OBJECT (action), "blocked", GINT_TO_POINTER (1));
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), state);
    g_object_set_data (G_OBJECT (action), "blocked", NULL);
}

static void
side_panel_image_changed_callback (PeonySidebar *side_panel,
                                   gpointer callback_data)
{
    PeonyWindow *window;
    GdkPixbuf *icon;

    window = PEONY_WINDOW (callback_data);

    icon = peony_sidebar_get_tab_icon (side_panel);
    peony_side_pane_set_panel_image (PEONY_NAVIGATION_WINDOW (window)->sidebar,
                                    GTK_WIDGET (side_panel),
                                    icon);
    if (icon != NULL)
    {
        g_object_unref (icon);
    }
}

/**
 * add_sidebar_panels:
 * @window:	A PeonyNavigationWindow
 *
 * Adds all sidebars available
 *
 */
static void
add_sidebar_panels (PeonyNavigationWindow *window)
{
    GtkWidget *current;
    GList *providers;
    GList *p;
    PeonySidebar *sidebar_panel;

    g_assert (PEONY_IS_NAVIGATION_WINDOW (window));

    if (window->sidebar == NULL)
    {
        return;
    }

    providers = peony_module_get_extensions_for_type (PEONY_TYPE_SIDEBAR_PROVIDER);

    for (p = providers; p != NULL; p = p->next)
    {
        PeonySidebarProvider *provider;

        provider = PEONY_SIDEBAR_PROVIDER (p->data);

        sidebar_panel = peony_sidebar_provider_create (provider,
                        PEONY_WINDOW_INFO (window));
        peony_navigation_window_add_sidebar_panel (window,
                sidebar_panel);

        g_object_unref (sidebar_panel);
    }

    peony_module_extension_list_free (providers);

    current = peony_side_pane_get_current_panel (window->sidebar);
    set_current_side_panel
    (window,
     PEONY_SIDEBAR (current));
}

gboolean
peony_navigation_window_toolbar_showing (PeonyNavigationWindow *window)
{
    if (window->details->toolbar != NULL)
    {
        return gtk_widget_get_visible (window->details->toolbar);
    }
    /* If we're not visible yet we haven't changed visibility, so its TRUE */
    return TRUE;
}

void
peony_navigation_window_hide_status_bar (PeonyNavigationWindow *window)
{
    gtk_widget_hide (PEONY_WINDOW (window)->details->statusbar);

    peony_navigation_window_update_show_hide_menu_items (window);

    g_settings_set_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_STATUS_BAR, FALSE);
}

void
peony_navigation_window_show_status_bar (PeonyNavigationWindow *window)
{
    gtk_widget_show (PEONY_WINDOW (window)->details->statusbar);

    peony_navigation_window_update_show_hide_menu_items (window);

    g_settings_set_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_STATUS_BAR, TRUE);
}

gboolean
peony_navigation_window_status_bar_showing (PeonyNavigationWindow *window)
{
    if (PEONY_WINDOW (window)->details->statusbar != NULL)
    {
        return gtk_widget_get_visible (PEONY_WINDOW (window)->details->statusbar);
    }
    /* If we're not visible yet we haven't changed visibility, so its TRUE */
    return TRUE;
}


void
peony_navigation_window_hide_toolbar (PeonyNavigationWindow *window)
{
    gtk_widget_hide (window->details->toolbar);
    peony_navigation_window_update_show_hide_menu_items (window);
    g_settings_set_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_TOOLBAR, FALSE);
}

void
peony_navigation_window_show_toolbar (PeonyNavigationWindow *window)
{
    gtk_widget_show (window->details->toolbar);
    peony_navigation_window_update_show_hide_menu_items (window);
    g_settings_set_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_TOOLBAR, TRUE);
}

void
peony_navigation_window_hide_sidebar (PeonyNavigationWindow *window)
{
    if (window->sidebar == NULL)
    {
        return;
    }

    peony_navigation_window_tear_down_sidebar (window);
    peony_navigation_window_update_show_hide_menu_items (window);

    g_settings_set_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_SIDEBAR, FALSE);
}

void
peony_navigation_window_show_sidebar (PeonyNavigationWindow *window)
{
    if (window->sidebar != NULL)
    {
        return;
    }

    peony_navigation_window_set_up_sidebar (window);
    peony_navigation_window_update_show_hide_menu_items (window);
    g_settings_set_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_SIDEBAR, TRUE);
}

gboolean
peony_navigation_window_sidebar_showing (PeonyNavigationWindow *window)
{
    g_return_val_if_fail (PEONY_IS_NAVIGATION_WINDOW (window), FALSE);

    return (window->sidebar != NULL)
           && gtk_widget_get_visible (gtk_paned_get_child1 (GTK_PANED (window->details->content_paned)));
}

/**
 * peony_navigation_window_get_base_page_index:
 * @window:	Window to get index from
 *
 * Returns the index of the base page in the history list.
 * Base page is not the currently displayed page, but the page
 * that acts as the base from which the back and forward commands
 * navigate from.
 */
gint
peony_navigation_window_get_base_page_index (PeonyNavigationWindow *window)
{
    PeonyNavigationWindowSlot *slot;
    gint forward_count;

    slot = PEONY_NAVIGATION_WINDOW_SLOT (PEONY_WINDOW (window)->details->active_pane->active_slot);

    forward_count = g_list_length (slot->forward_list);

    /* If forward is empty, the base it at the top of the list */
    if (forward_count == 0)
    {
        return 0;
    }

    /* The forward count indicate the relative postion of the base page
     * in the history list
     */
    return forward_count;
}

/**
 * peony_navigation_window_show:
 * @widget: a #GtkWidget.
 *
 * Call parent and then show/hide window items
 * base on user prefs.
 */
static void
peony_navigation_window_show (GtkWidget *widget)
{
    PeonyNavigationWindow *window;
    gboolean show_location_bar;
    gboolean always_use_location_entry;
    GList *walk;

    window = PEONY_NAVIGATION_WINDOW (widget);

    /* Initially show or hide views based on preferences; once the window is displayed
     * these can be controlled on a per-window basis from View menu items.
     */

    if (g_settings_get_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_TOOLBAR))
    {
        peony_navigation_window_show_toolbar (window);
    }
    else
    {
        peony_navigation_window_hide_toolbar (window);
    }

    show_location_bar = g_settings_get_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_LOCATION_BAR);
    always_use_location_entry = g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY);
    for (walk = PEONY_WINDOW(window)->details->panes; walk; walk = walk->next)
    {
        PeonyNavigationWindowPane *pane = walk->data;
        if (show_location_bar)
        {
            peony_navigation_window_pane_show_location_bar (pane, FALSE);
        }
        else
        {
            peony_navigation_window_pane_hide_location_bar (pane, FALSE);
        }

        if (always_use_location_entry)
        {
            peony_navigation_window_pane_set_bar_mode (pane, PEONY_BAR_NAVIGATION);
        }
        else
        {
            peony_navigation_window_pane_set_bar_mode (pane, PEONY_BAR_PATH);
        }
    }

    if (g_settings_get_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_SIDEBAR))
    {
        peony_navigation_window_show_sidebar (window);
    }
    else
    {
        peony_navigation_window_hide_sidebar (window);
    }

    if (g_settings_get_boolean (peony_window_state, PEONY_WINDOW_STATE_START_WITH_STATUS_BAR))
    {
        peony_navigation_window_show_status_bar (window);
    }
    else
    {
        peony_navigation_window_hide_status_bar (window);
    }

    GTK_WIDGET_CLASS (parent_class)->show (widget);
}

static void
peony_navigation_window_save_geometry (PeonyNavigationWindow *window)
{
    char *geometry_string;
    gboolean is_maximized;

    g_assert (PEONY_IS_WINDOW (window));

    if (gtk_widget_get_window (GTK_WIDGET (window)))
    {
        geometry_string = eel_gtk_window_get_geometry_string (GTK_WINDOW (window));
        is_maximized = gdk_window_get_state (gtk_widget_get_window (GTK_WIDGET (window)))
                       & GDK_WINDOW_STATE_MAXIMIZED;

        if (!is_maximized)
        {
            g_settings_set_string (peony_window_state,
                                   PEONY_WINDOW_STATE_GEOMETRY,
                                   geometry_string);
        }
        g_free (geometry_string);

        g_settings_set_boolean (peony_window_state,
                                PEONY_WINDOW_STATE_MAXIMIZED,
                                is_maximized);
    }
}

static void
real_window_close (PeonyWindow *window)
{
    peony_navigation_window_save_geometry (PEONY_NAVIGATION_WINDOW (window));
}

static void
real_get_min_size (PeonyWindow *window,
                   guint *min_width, guint *min_height)
{
    if (min_width)
    {
        *min_width = PEONY_NAVIGATION_WINDOW_MIN_WIDTH;
    }
    if (min_height)
    {
        *min_height = PEONY_NAVIGATION_WINDOW_MIN_HEIGHT;
    }
}

static void
real_get_default_size (PeonyWindow *window,
                       guint *default_width, guint *default_height)
{
    if (default_width)
    {
        *default_width = PEONY_NAVIGATION_WINDOW_DEFAULT_WIDTH;
    }

    if (default_height)
    {
        *default_height = PEONY_NAVIGATION_WINDOW_DEFAULT_HEIGHT;
    }
}

static PeonyWindowSlot *
real_open_slot (PeonyWindowPane *pane,
                PeonyWindowOpenSlotFlags flags)
{
    PeonyWindowSlot *slot;

    slot = (PeonyWindowSlot *) g_object_new (PEONY_TYPE_NAVIGATION_WINDOW_SLOT, NULL);
    slot->pane = pane;

    peony_navigation_window_pane_add_slot_in_tab (PEONY_NAVIGATION_WINDOW_PANE (pane), slot, flags);
    gtk_widget_show (slot->content_box);

    return slot;
}

static void
real_close_slot (PeonyWindowPane *pane,
                 PeonyWindowSlot *slot)
{
    int page_num;
    GtkNotebook *notebook;

    notebook = GTK_NOTEBOOK (PEONY_NAVIGATION_WINDOW_PANE (pane)->notebook);

    page_num = gtk_notebook_page_num (notebook, slot->content_box);
    g_assert (page_num >= 0);

    peony_navigation_window_pane_remove_page (PEONY_NAVIGATION_WINDOW_PANE (pane), page_num);

    gtk_notebook_set_show_tabs (notebook,
                                gtk_notebook_get_n_pages (notebook) > 1);

    EEL_CALL_PARENT (PEONY_WINDOW_CLASS,
                     close_slot, (pane, slot));
}

static void
peony_navigation_window_class_init (PeonyNavigationWindowClass *class)
{
    PEONY_WINDOW_CLASS (class)->window_type = PEONY_WINDOW_NAVIGATION;
    PEONY_WINDOW_CLASS (class)->bookmarks_placeholder = MENU_PATH_BOOKMARKS_PLACEHOLDER;

    G_OBJECT_CLASS (class)->finalize = peony_navigation_window_finalize;

    GTK_WIDGET_CLASS (class)->destroy = peony_navigation_window_destroy;
    GTK_WIDGET_CLASS (class)->show = peony_navigation_window_show;
    GTK_WIDGET_CLASS (class)->window_state_event = peony_navigation_window_state_event;
    GTK_WIDGET_CLASS (class)->key_press_event = peony_navigation_window_key_press_event;
    GTK_WIDGET_CLASS (class)->button_press_event = peony_navigation_window_button_press_event;
    //GTK_WIDGET_CLASS (class)->begin_loading = peony_navigation_window_class_begin_loading;
    PEONY_WINDOW_CLASS (class)->sync_allow_stop = real_sync_allow_stop;
    PEONY_WINDOW_CLASS (class)->prompt_for_location = real_prompt_for_location;
    PEONY_WINDOW_CLASS (class)->sync_title = real_sync_title;
    PEONY_WINDOW_CLASS (class)->get_icon = real_get_icon;
    PEONY_WINDOW_CLASS (class)->get_min_size = real_get_min_size;
    PEONY_WINDOW_CLASS (class)->get_default_size = real_get_default_size;
    PEONY_WINDOW_CLASS (class)->close = real_window_close;

    PEONY_WINDOW_CLASS (class)->open_slot = real_open_slot;
    PEONY_WINDOW_CLASS (class)->close_slot = real_close_slot;

    //PEONY_WINDOW_CLASS (class)->begin_loading = peony_navigation_window_class_begin_loading;

    g_type_class_add_private (G_OBJECT_CLASS (class), sizeof (PeonyNavigationWindowDetails));

    g_signal_connect_swapped (peony_preferences,
                              "changed::" PEONY_PREFERENCES_MOUSE_BACK_BUTTON,
                              G_CALLBACK(mouse_back_button_changed),
                              NULL);

    g_signal_connect_swapped (peony_preferences,
                              "changed::" PEONY_PREFERENCES_MOUSE_FORWARD_BUTTON,
                              G_CALLBACK(mouse_forward_button_changed),
                              NULL);

    g_signal_connect_swapped (peony_preferences,
                              "changed::" PEONY_PREFERENCES_MOUSE_USE_EXTRA_BUTTONS,
                              G_CALLBACK(use_extra_mouse_buttons_changed),
                              NULL);
}

static PeonyWindowSlot *
create_extra_pane (PeonyNavigationWindow *window)
{
    PeonyWindow *win;
    PeonyNavigationWindowPane *pane;
    PeonyWindowSlot *slot;
    GtkPaned *paned;

    win = PEONY_WINDOW (window);

    /* New pane */
    pane = peony_navigation_window_pane_new (win);
    win->details->panes = g_list_append (win->details->panes, pane);

    peony_navigation_window_pane_setup (pane);

    paned = GTK_PANED (window->details->split_view_hpane);
    if (gtk_paned_get_child1 (paned) == NULL)
    {
        gtk_paned_pack1 (paned, pane->widget, TRUE, FALSE);
    }
    else
    {
        gtk_paned_pack2 (paned, pane->widget, TRUE, FALSE);
    }

    /* slot */
    slot = peony_window_open_slot (PEONY_WINDOW_PANE (pane),
                                  PEONY_WINDOW_OPEN_SLOT_APPEND);
    PEONY_WINDOW_PANE (pane)->active_slot = slot;

    return slot;
}

static void office_format_trans_ready_callback(GObject *singaller, gpointer data){
    printf("office2pdf/html done\n");
    //global_preview_office2pdf_filename = (char*) data;
    printf("cb: %s, global: %s\n", data, global_preview_filename);

    if(filename_has_suffix(data, ".pdf")){
        printf("cmp: %s, %s\n",data,global_preview_filename);
        if (g_str_equal(data,global_preview_filename) == TRUE) {
            printf("should show office pdf file\n");
            set_pdf_preview_widget_file_by_filename(global_window->details->pdf_view,(char*)data);
            gtk_widget_hide(global_window->details->empty_window);
            gtk_widget_show_all(global_window->details->pdf_swindow);
            gtk_widget_hide(global_window->details->test_widget);
            gtk_widget_hide(global_window->details->web_swindow);
        } 
    } 
    else if (filename_has_suffix(data,".html")) {
        printf("should show html file\n");
        if (g_str_equal(data,global_preview_filename) == TRUE) {
            gchar *uri;
            uri = g_strdup_printf("file://%s", global_preview_excel2html_filename);
            printf("load uri: %s",uri);
            webkit_web_view_load_uri (WEBKIT_WEB_VIEW(global_window->details->web_view), uri);
            g_free(uri);
            gtk_widget_hide(global_window->details->empty_window);
            gtk_widget_hide(global_window->details->pdf_swindow);
            gtk_widget_hide(global_window->details->test_widget);
            gtk_widget_show_all(global_window->details->web_swindow);
        }
    }
    //g_remove(data);
}

static void preview_file_changed_callback(GObject *singaller, gpointer data){
    if((char*)data == "null"){
	    gtk_widget_show(global_window->details->empty_window);
	    gtk_widget_hide(global_window->details->pdf_swindow);
	    gtk_widget_hide(global_window->details->test_widget);
        gtk_widget_hide(global_window->details->web_swindow);
        return;
    }

    global_preview_filename = (char*) data;

    printf("recieved filename:%s\n",(char*)data);
    printf("mime type: %s\n",get_mime_type_string_by_filename((char*)data));
    if(is_text_type((char*)data)){
	    open_file_cb(global_window->details->gtk_source_widget,(char*)data);
        gtk_widget_hide(global_window->details->empty_window);
        gtk_widget_hide(global_window->details->pdf_swindow);
        gtk_widget_hide (global_window->details->web_swindow);
        gtk_widget_show_all(global_window->details->test_widget);
    } else if (is_image_type((char*)data)) {
        printf("is image type\n");
        gchar *uri;
        uri = g_strdup_printf("file://%s", data);
        printf("load uri: %s",uri);
        webkit_web_view_load_uri (WEBKIT_WEB_VIEW(global_window->details->web_view), uri);
        g_free(uri);
        gtk_widget_hide(global_window->details->empty_window);
        gtk_widget_hide(global_window->details->pdf_swindow);
        gtk_widget_hide(global_window->details->test_widget);
        gtk_widget_show_all (global_window->details->web_swindow);
    }  else if (is_pdf_type((char*)data)) {
        printf("is pdf type\n");
        set_pdf_preview_widget_file_by_filename(global_window->details->pdf_view,(char*)data);
        gtk_widget_hide(global_window->details->empty_window);
        gtk_widget_show_all(global_window->details->pdf_swindow);
        gtk_widget_hide(global_window->details->test_widget);
        gtk_widget_hide (global_window->details->web_swindow);
    } else if (is_office_file((char*) data)) {
        printf("is office file\n");
        if (!is_excel_doc((char*)data)){
            printf("doc & ppt\n");
            global_preview_office2pdf_filename = office2pdf((char*)data);
            if(global_preview_office2pdf_filename)
                global_preview_filename = global_preview_office2pdf_filename;
                global_preview_excel2html_filename = NULL;
        } else {
            printf("excel\n");
            global_preview_excel2html_filename = excel2html((char*)data);
            if(global_preview_excel2html_filename)
                global_preview_filename = global_preview_excel2html_filename;
                global_preview_office2pdf_filename = NULL;
        }
        
        //we will wait for child progress finished.

	    gtk_widget_show(global_window->details->empty_window);
	    gtk_widget_hide(global_window->details->pdf_swindow);
	    gtk_widget_hide(global_window->details->test_widget);
        gtk_widget_hide (global_window->details->web_swindow);
    } else {
        printf("can't preview this file\n");
	    gtk_widget_show(global_window->details->empty_window);
	    gtk_widget_hide(global_window->details->pdf_swindow);
	    gtk_widget_hide(global_window->details->test_widget);
        gtk_widget_hide (global_window->details->web_swindow);
    }
}

void
peony_navigation_window_split_view_on (PeonyNavigationWindow *window)
{
    pdf_viewer_init();
    //this may happend when first open the extra view only.
    if(global_window != window){
        global_window = window;

	//cause we may switch to the old window, we should sure that if the hbox is existed.
	if(window->details->preview_hbox)
		goto last;

        //create a hbox for preview views
        window->details->preview_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

        //add text widget to preview_hbox
        window->details->test_widget = gtk_scrolled_window_new(NULL,NULL);
        //global_test_widget = test_widget_new();
        window->details->gtk_source_widget = test_widget_new();//global_test_widget;
        gtk_container_add (GTK_CONTAINER (window->details->test_widget), GTK_WIDGET (window->details->gtk_source_widget));
        gtk_box_pack_start (GTK_BOX(window->details->preview_hbox), GTK_WIDGET(window->details->test_widget), TRUE, TRUE, 0);
        
        //add pdf widget to preview_hbox
        window->details->pdf_swindow = gtk_scrolled_window_new(NULL,NULL);
        window->details->pdf_view = pdf_preview_widget_new();        
        gtk_container_add (GTK_CONTAINER (window->details->pdf_swindow), window->details->pdf_view);        
        gtk_box_pack_start (GTK_BOX(window->details->preview_hbox), GTK_WIDGET(window->details->pdf_swindow), TRUE, TRUE, 0);

        //add web widget to preview_hbox
        window->details->web_swindow = gtk_scrolled_window_new(NULL,NULL);
        window->details->web_view = webkit_web_view_new();
        webkit_web_view_set_zoom_level (window->details->web_view, 1.50);
        gtk_container_add (GTK_CONTAINER(window->details->web_swindow), GTK_WIDGET(window->details->web_view));
        gtk_box_pack_start (GTK_BOX(window->details->preview_hbox), GTK_WIDGET(window->details->web_swindow), TRUE, TRUE, 0);

        //add empty widget to split_view_hpane
        window->details->empty_window = gtk_scrolled_window_new(NULL,NULL);
        gtk_box_pack_start (GTK_BOX(window->details->preview_hbox), GTK_WIDGET(window->details->empty_window), TRUE, TRUE, 0);

        //pack the preview_hbox to the split_view_hpane
        gtk_paned_pack2 (GTK_PANED(window->details->split_view_hpane), window->details->preview_hbox, TRUE, FALSE);
    }

last:
    gtk_widget_show_all (window->details->preview_hbox);
    gtk_widget_hide (window->details->test_widget);
    gtk_widget_hide (window->details->pdf_swindow);
    gtk_widget_hide (window->details->web_swindow);
    
    
    //char *signal_filename;
    char* preview_filename; 
    char* child_cb_filename;

    g_signal_connect (peony_signaller_get_current (),
                          "preview_file_changed",
                          G_CALLBACK (preview_file_changed_callback), preview_filename
                          );

    g_signal_connect (peony_signaller_get_current (),
                          "office2pdf_ready",
                          G_CALLBACK (office_format_trans_ready_callback), child_cb_filename
                          );

    window->details->is_split_view_showing = TRUE;
}

void
peony_navigation_window_split_view_off (PeonyNavigationWindow *window)
{
    printf("split_view_off\n");
    gtk_widget_hide (window->details->preview_hbox);
    g_signal_handlers_disconnect_by_func(peony_signaller_get_current (), G_CALLBACK (preview_file_changed_callback), global_preview_filename);
    g_signal_handlers_disconnect_by_func(peony_signaller_get_current (), G_CALLBACK (office_format_trans_ready_callback), global_preview_filename);

    window->details->is_split_view_showing = FALSE;
    peony_navigation_window_update_split_view_actions_sensitivity (window);

    g_remove("/home/lanyue/.cache/peony");
}

gboolean
peony_navigation_window_split_view_showing (PeonyNavigationWindow *window)
{
    return window->details->is_split_view_showing ;
}

void
peony_find_duplicate_files (PeonyNavigationWindow *window,GList *pListRes)
{
    PeonyWindow *win;
    PeonyNavigationWindowPane *pane;
    PeonyWindowSlot *old_active_slot;
    GFile *location;
    GtkAction *action;
	PeonySearchBar *pSearchBar = NULL;
	
    win = PEONY_WINDOW (window);

    old_active_slot = peony_window_get_active_slot (win);
    pane = PEONY_NAVIGATION_WINDOW_PANE (old_active_slot->pane);
	pSearchBar = PEONY_SEARCH_BAR(pane->search_bar);
	find_duplicate_signal(pane->search_bar);
	
	return ;
}

