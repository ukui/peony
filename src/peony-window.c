/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000, 2004 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
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
 *           Alexander Larsson <alexl@redhat.com>
 */

/* peony-window.c: Implementation of the main window object */

#include <config.h>
#include "peony-window-private.h"

#include "peony-actions.h"
#include "peony-application.h"
#include "peony-bookmarks-window.h"
#include "peony-information-panel.h"
#include "peony-window-manage-views.h"
#include "peony-window-bookmarks.h"
#include "peony-window-slot.h"
#include "peony-navigation-window-slot.h"
#include "peony-search-bar.h"
#include "peony-navigation-window-pane.h"
#include "peony-src-marshal.h"
#include <eel/eel-debug.h>
#include <eel/eel-gtk-macros.h>
#include <eel/eel-string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#ifdef HAVE_X11_XF86KEYSYM_H
#include <X11/XF86keysym.h>
#endif
#include <libpeony-private/peony-file-utilities.h>
#include <libpeony-private/peony-file-attributes.h>
#include <libpeony-private/peony-global-preferences.h>
#include <libpeony-private/peony-metadata.h>
#include <libpeony-private/peony-mime-actions.h>
#include <libpeony-private/peony-program-choosing.h>
#include <libpeony-private/peony-view-factory.h>
#include <libpeony-private/peony-clipboard.h>
#include <libpeony-private/peony-search-directory.h>
#include <libpeony-private/peony-signaller.h>

#define MAX_HISTORY_ITEMS 50

#define EXTRA_VIEW_WIDGETS_BACKGROUND "#a7c6e1"

/* dock items */

#define PEONY_MENU_PATH_EXTRA_VIEWER_PLACEHOLDER	"/MenuBar/View/View Choices/Extra Viewer"
#define PEONY_MENU_PATH_SHORT_LIST_PLACEHOLDER  	"/MenuBar/View/View Choices/Short List"

enum {
	ARG_0,
	ARG_APP
};

enum {
	GO_UP,
	RELOAD,
	PROMPT_FOR_LOCATION,
	ZOOM_CHANGED,
	VIEW_AS_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

typedef struct
{
    PeonyWindow *window;
    char *id;
} ActivateViewData;

static void cancel_view_as_callback         (PeonyWindowSlot      *slot);
static void peony_window_info_iface_init (PeonyWindowInfoIface *iface);
static void action_view_as_callback         (GtkAction               *action,
        ActivateViewData        *data);

static GList *history_list;

G_DEFINE_TYPE_WITH_CODE (PeonyWindow, peony_window, GTK_TYPE_WINDOW,
                         G_IMPLEMENT_INTERFACE (PEONY_TYPE_WINDOW_INFO,
                                 peony_window_info_iface_init));

static const struct
{
    unsigned int keyval;
    const char *action;
} extra_window_keybindings [] =
{
#ifdef HAVE_X11_XF86KEYSYM_H
    { XF86XK_AddFavorite,	PEONY_ACTION_ADD_BOOKMARK },
    { XF86XK_Favorites,	PEONY_ACTION_EDIT_BOOKMARKS },
    { XF86XK_Go,		PEONY_ACTION_GO_TO_LOCATION },
    /* TODO?{ XF86XK_History,	PEONY_ACTION_HISTORY }, */
    { XF86XK_HomePage,      PEONY_ACTION_GO_HOME },
    { XF86XK_OpenURL,	PEONY_ACTION_GO_TO_LOCATION },
    { XF86XK_Refresh,	PEONY_ACTION_RELOAD },
    { XF86XK_Reload,	PEONY_ACTION_RELOAD },
    { XF86XK_Search,	PEONY_ACTION_SEARCH },
    { XF86XK_Start,		PEONY_ACTION_GO_HOME },
    { XF86XK_Stop,		PEONY_ACTION_STOP },
    { XF86XK_ZoomIn,	PEONY_ACTION_ZOOM_IN },
    { XF86XK_ZoomOut,	PEONY_ACTION_ZOOM_OUT }
#endif
};

static void
peony_window_init (PeonyWindow *window)
{
    GtkWidget *grid;
    GtkWidget *menu;
    GtkWidget *statusbar;

#if GTK_CHECK_VERSION (3, 20, 0)
    static const gchar css_custom[] =
      "#peony-extra-view-widget {"
      "  background-color: " EXTRA_VIEW_WIDGETS_BACKGROUND ";"
      "}";
#else
    static const gchar css_custom[] =
      "#statusbar-no-border {"
      "  -GtkStatusbar-shadow-type: none;"
      "}"
      "#peony-extra-view-widget {"
      "  background-color: " EXTRA_VIEW_WIDGETS_BACKGROUND ";"
      "}";
#endif
    GError *error = NULL;
    GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (provider, css_custom, -1, &error);

    if (error != NULL) {
            g_warning ("Can't parse PeonyWindow's CSS custom description: %s\n", error->message);
            g_error_free (error);
    } else {
            gtk_style_context_add_provider (gtk_widget_get_style_context (GTK_WIDGET (window)),
                                            GTK_STYLE_PROVIDER (provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    g_object_unref (provider);
    window->details = G_TYPE_INSTANCE_GET_PRIVATE (window, PEONY_TYPE_WINDOW, PeonyWindowDetails);

    window->details->panes = NULL;
    window->details->active_pane = NULL;

    window->details->show_hidden_files_mode = PEONY_WINDOW_SHOW_HIDDEN_FILES_DEFAULT;

    /* Set initial window title */
    gtk_window_set_title (GTK_WINDOW (window), _("Peony"));

    grid = gtk_grid_new ();
    gtk_orientable_set_orientation (GTK_ORIENTABLE (grid), GTK_ORIENTATION_VERTICAL);
    window->details->grid = grid;
    gtk_widget_show (grid);
    gtk_container_add (GTK_CONTAINER (window), grid);

    statusbar = gtk_statusbar_new ();
    gtk_widget_set_name (statusbar, "statusbar-noborder");

/* set margin to zero to reduce size of statusbar */
	gtk_widget_set_margin_top (GTK_WIDGET (statusbar), 0);
	gtk_widget_set_margin_bottom (GTK_WIDGET (statusbar), 0);

    window->details->statusbar = statusbar;
    window->details->help_message_cid = gtk_statusbar_get_context_id
                                        (GTK_STATUSBAR (statusbar), "help_message");
    /* Statusbar is packed in the subclasses */

    peony_window_initialize_menus (window);

    menu = gtk_ui_manager_get_widget (window->details->ui_manager, "/MenuBar");
    window->details->menubar = menu;
    gtk_widget_set_hexpand (menu, TRUE);
    gtk_widget_show (menu);
    gtk_grid_attach (GTK_GRID (grid), menu, 0, 0, 1, 1);

    /* Register to menu provider extension signal managing menu updates */
    g_signal_connect_object (peony_signaller_get_current (), "popup_menu_changed",
                             G_CALLBACK (peony_window_load_extension_menus), window, G_CONNECT_SWAPPED);
}

/* Unconditionally synchronize the GtkUIManager of WINDOW. */
static void
peony_window_ui_update (PeonyWindow *window)
{
    g_assert (PEONY_IS_WINDOW (window));

    gtk_ui_manager_ensure_update (window->details->ui_manager);
}

static void
peony_window_push_status (PeonyWindow *window,
                         const char *text)
{
    g_return_if_fail (PEONY_IS_WINDOW (window));

    /* clear any previous message, underflow is allowed */
    gtk_statusbar_pop (GTK_STATUSBAR (window->details->statusbar), 0);

    if (text != NULL && text[0] != '\0')
    {
        gtk_statusbar_push (GTK_STATUSBAR (window->details->statusbar), 0, text);
    }
}

void
peony_window_sync_status (PeonyWindow *window)
{
    PeonyWindowSlot *slot;

    slot = window->details->active_pane->active_slot;
    peony_window_push_status (window, slot->status_text);
}

void
peony_window_go_to (PeonyWindow *window, GFile *location)
{
    printf("peony_window_go_to : %s\n",g_file_get_path(location));
    g_return_if_fail (PEONY_IS_WINDOW (window));

    peony_window_slot_go_to (window->details->active_pane->active_slot, location, FALSE);
}

void
peony_window_go_to_full (PeonyWindow *window,
                        GFile                 *location,
                        PeonyWindowGoToCallback callback,
                        gpointer               user_data)
{
    g_return_if_fail (PEONY_IS_WINDOW (window));

    peony_window_slot_go_to_full (window->details->active_pane->active_slot, location, FALSE, callback, user_data);
}

void
peony_window_go_to_with_selection (PeonyWindow *window, GFile *location, GList *new_selection)
{
    g_return_if_fail (PEONY_IS_WINDOW (window));

    peony_window_slot_go_to_with_selection (window->details->active_pane->active_slot, location, new_selection);
}

static gboolean
peony_window_go_up_signal (PeonyWindow *window, gboolean close_behind)
{
    peony_window_go_up (window, close_behind, FALSE);
    return TRUE;
}

void
peony_window_new_tab (PeonyWindow *window)
{
    PeonyWindowSlot *current_slot;
    PeonyWindowSlot *new_slot;
    PeonyWindowOpenFlags flags;
    GFile *location;
    int new_slot_position;
    char *scheme;

    current_slot = window->details->active_pane->active_slot;
    location = peony_window_slot_get_location (current_slot);

    if (location != NULL) {
    	flags = 0;

    	new_slot_position = g_settings_get_enum (peony_preferences, PEONY_PREFERENCES_NEW_TAB_POSITION);
    	if (new_slot_position == PEONY_NEW_TAB_POSITION_END) {
    		flags = PEONY_WINDOW_OPEN_SLOT_APPEND;
    	}

    	scheme = g_file_get_uri_scheme (location);
    	if (!strcmp (scheme, "x-peony-search")) {
    		g_object_unref (location);
    		location = g_file_new_for_path (g_get_home_dir ());
    	}
    	g_free (scheme);

    	new_slot = peony_window_open_slot (current_slot->pane, flags);
    	peony_window_set_active_slot (window, new_slot);
    	peony_window_slot_go_to (new_slot, location, FALSE);
    	g_object_unref (location);
    }
}

void
peony_window_go_up (PeonyWindow *window, gboolean close_behind, gboolean new_tab)
{
    PeonyWindowSlot *slot;
    GFile *parent;
    GList *selection;
    PeonyWindowOpenFlags flags;

    g_assert (PEONY_IS_WINDOW (window));

    slot = window->details->active_pane->active_slot;

    if (slot->location == NULL)
    {
        return;
    }

    parent = g_file_get_parent (slot->location);

    if (parent == NULL)
    {
        return;
    }

    selection = g_list_prepend (NULL, g_object_ref (slot->location));

    flags = 0;
    if (close_behind)
    {
        flags |= PEONY_WINDOW_OPEN_FLAG_CLOSE_BEHIND;
    }
    if (new_tab)
    {
        flags |= PEONY_WINDOW_OPEN_FLAG_NEW_TAB;
    }

    peony_window_slot_open_location_full (slot, parent,
                                         PEONY_WINDOW_OPEN_ACCORDING_TO_MODE,
                                         flags,
                                         selection,
                                         NULL, NULL);

    g_object_unref (parent);

    g_list_free_full (selection, g_object_unref);
}

static void
real_set_allow_up (PeonyWindow *window,
                   gboolean        allow)
{
    GtkAction *action;

    g_assert (PEONY_IS_WINDOW (window));

    action = gtk_action_group_get_action (window->details->main_action_group,
                                          PEONY_ACTION_UP);
    gtk_action_set_sensitive (action, allow);
    action = gtk_action_group_get_action (window->details->main_action_group,
                                          PEONY_ACTION_UP_ACCEL);
    gtk_action_set_sensitive (action, allow);
}

void
peony_window_allow_up (PeonyWindow *window, gboolean allow)
{
    g_return_if_fail (PEONY_IS_WINDOW (window));

    EEL_CALL_METHOD (PEONY_WINDOW_CLASS, window,
                     set_allow_up, (window, allow));
}

static void
update_cursor (PeonyWindow *window)
{
    PeonyWindowSlot *slot;
    GdkCursor *cursor;

    slot = window->details->active_pane->active_slot;

    if (slot->allow_stop)
    {
        GdkDisplay *display;
        GdkCursor * cursor;

        display = gtk_widget_get_display (GTK_WIDGET (window));
        cursor = gdk_cursor_new_for_display (display, GDK_WATCH);
        gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (window)), cursor);
        g_object_unref (cursor);
    }
    else
    {
        gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (window)), NULL);
    }
}

void
peony_window_sync_allow_stop (PeonyWindow *window,
                             PeonyWindowSlot *slot)
{
    GtkAction *action;
    gboolean allow_stop;

    g_assert (PEONY_IS_WINDOW (window));

    action = gtk_action_group_get_action (window->details->main_action_group,
                                          PEONY_ACTION_STOP);
    allow_stop = gtk_action_get_sensitive (action);

    if (slot != window->details->active_pane->active_slot ||
            allow_stop != slot->allow_stop)
    {
        if (slot == window->details->active_pane->active_slot)
        {
            gtk_action_set_sensitive (action, slot->allow_stop);
        }

        if (gtk_widget_get_realized (GTK_WIDGET (window)))
        {
            update_cursor (window);
        }

        EEL_CALL_METHOD (PEONY_WINDOW_CLASS, window,
                         sync_allow_stop, (window, slot));
    }
}

void
peony_window_allow_reload (PeonyWindow *window, gboolean allow)
{
    GtkAction *action;

    g_return_if_fail (PEONY_IS_WINDOW (window));

    action = gtk_action_group_get_action (window->details->main_action_group,
                                          PEONY_ACTION_RELOAD);
    gtk_action_set_sensitive (action, allow);
}

void
peony_window_go_home (PeonyWindow *window)
{
    g_return_if_fail (PEONY_IS_WINDOW (window));

    peony_window_slot_go_home (window->details->active_pane->active_slot, FALSE);
}

void
peony_window_prompt_for_location (PeonyWindow *window,
                                 const char     *initial)
{
    g_return_if_fail (PEONY_IS_WINDOW (window));

    EEL_CALL_METHOD (PEONY_WINDOW_CLASS, window,
                     prompt_for_location, (window, initial));
}

static char *
peony_window_get_location_uri (PeonyWindow *window)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));

    slot = window->details->active_pane->active_slot;

    if (slot->location)
    {
        return g_file_get_uri (slot->location);
    }
    return NULL;
}

void
peony_window_zoom_in (PeonyWindow *window)
{
    g_assert (window != NULL);

    peony_window_pane_zoom_in (window->details->active_pane);
}

void
peony_window_zoom_to_level (PeonyWindow *window,
                           PeonyZoomLevel level)
{
    g_assert (window != NULL);

    peony_window_pane_zoom_to_level (window->details->active_pane, level);
}

void
peony_window_zoom_out (PeonyWindow *window)
{
    g_assert (window != NULL);

    peony_window_pane_zoom_out (window->details->active_pane);
}

void
peony_window_zoom_to_default (PeonyWindow *window)
{
    g_assert (window != NULL);

    peony_window_pane_zoom_to_default (window->details->active_pane);
}

/* Code should never force the window taller than this size.
 * (The user can still stretch the window taller if desired).
 */
static guint
get_max_forced_height (GdkScreen *screen)
{
    return (gdk_screen_get_height (screen) * 90) / 100;
}

/* Code should never force the window wider than this size.
 * (The user can still stretch the window wider if desired).
 */
static guint
get_max_forced_width (GdkScreen *screen)
{
    return (gdk_screen_get_width (screen) * 90) / 100;
}

/* This must be called when construction of PeonyWindow is finished,
 * since it depends on the type of the argument, which isn't decided at
 * construction time.
 */
static void
peony_window_set_initial_window_geometry (PeonyWindow *window)
{
    GdkScreen *screen;
    guint max_width_for_screen, max_height_for_screen;

    guint default_width, default_height;

    screen = gtk_window_get_screen (GTK_WINDOW (window));

    max_width_for_screen = get_max_forced_width (screen);
    max_height_for_screen = get_max_forced_height (screen);

    EEL_CALL_METHOD (PEONY_WINDOW_CLASS, window,
                     get_default_size, (window, &default_width, &default_height));

    gtk_window_set_default_size (GTK_WINDOW (window),
                                 MIN (default_width,
                                      max_width_for_screen),
                                 MIN (default_height,
                                      max_height_for_screen));
}

static void
peony_window_constructed (GObject *self)
{
    PeonyWindow *window;

    window = PEONY_WINDOW (self);

    peony_window_initialize_bookmarks_menu (window);
    peony_window_set_initial_window_geometry (window);
}

static void
peony_window_set_property (GObject *object,
                          guint arg_id,
                          const GValue *value,
                          GParamSpec *pspec)
{
    PeonyWindow *window;

    window = PEONY_WINDOW (object);

    switch (arg_id)
    {
    case ARG_APP:
        window->application = PEONY_APPLICATION (g_value_get_object (value));
        break;
    }
}

static void
peony_window_get_property (GObject *object,
                          guint arg_id,
                          GValue *value,
                          GParamSpec *pspec)
{
    switch (arg_id)
    {
    case ARG_APP:
        g_value_set_object (value, PEONY_WINDOW (object)->application);
        break;
    }
}

static void
free_stored_viewers (PeonyWindow *window)
{
    g_list_free_full (window->details->short_list_viewers, g_free);
    window->details->short_list_viewers = NULL;
    g_free (window->details->extra_viewer);
    window->details->extra_viewer = NULL;
}

static void
peony_window_destroy (GtkWidget *object)
{
    PeonyWindow *window;
    GList *panes_copy;

    window = PEONY_WINDOW (object);

    /* close all panes safely */
    panes_copy = g_list_copy (window->details->panes);
    g_list_free_full (panes_copy, (GDestroyNotify) peony_window_close_pane);

    /* the panes list should now be empty */
    g_assert (window->details->panes == NULL);
    g_assert (window->details->active_pane == NULL);

    GTK_WIDGET_CLASS (peony_window_parent_class)->destroy (object);
}

static void
peony_window_finalize (GObject *object)
{
    PeonyWindow *window;

    window = PEONY_WINDOW (object);

    peony_window_finalize_menus (window);
    free_stored_viewers (window);

    if (window->details->bookmark_list != NULL)
    {
        g_object_unref (window->details->bookmark_list);
    }

    /* peony_window_close() should have run */
    g_assert (window->details->panes == NULL);

    g_object_unref (window->details->ui_manager);

    G_OBJECT_CLASS (peony_window_parent_class)->finalize (object);
}

static GObject *
peony_window_constructor (GType type,
                         guint n_construct_properties,
                         GObjectConstructParam *construct_params)
{
    GObject *object;
    PeonyWindow *window;
    PeonyWindowSlot *slot;

    object = (* G_OBJECT_CLASS (peony_window_parent_class)->constructor) (type,
             n_construct_properties,
             construct_params);

    window = PEONY_WINDOW (object);

    slot = peony_window_open_slot (window->details->active_pane, 0);
    peony_window_set_active_slot (window, slot);

    return object;
}

void set_active_window(Window xid)
{
    Display *d = XOpenDisplay(NULL);
    Window root = DefaultRootWindow(d);

    Atom _NET_ACTIVE_WINDOW = XInternAtom(d, "_NET_ACTIVE_WINDOW", False);
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.send_event = True;
    xev.xclient.window = xid;
    xev.xclient.message_type = _NET_ACTIVE_WINDOW;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 2;
    XSendEvent(d, root, False, StructureNotifyMask | SubstructureNotifyMask, &xev);

    XCloseDisplay(d);
}

void * active_window_thead(void *);

void
peony_window_show_window (PeonyWindow    *window)
{
    PeonyWindowSlot *slot;
    PeonyWindowPane *pane;
    GList *l, *walk;

    for (walk = window->details->panes; walk; walk = walk->next)
    {
        pane = walk->data;
        for (l = pane->slots; l != NULL; l = l->next)
        {
            slot = l->data;

            peony_window_slot_update_title (slot);
            peony_window_slot_update_icon (slot);
        }
    }

    gtk_widget_show (GTK_WIDGET (window));

    slot = window->details->active_pane->active_slot;

    if (slot->viewed_file)
    {
        if (PEONY_IS_SPATIAL_WINDOW (window))
        {
            peony_file_set_has_open_window (slot->viewed_file, TRUE);
        }
    }
    if (g_settings_get_boolean(peony_preferences,"dbus-show-peony-window")){
	GdkWindow *gdk_window = gtk_widget_get_window(GTK_WINDOW (window));
	Window win=gdk_x11_window_get_xid (gdk_window);
	pthread_t t0;
	if(pthread_create(&t0, NULL, active_window_thead, (void *)win) == -1){
	    puts("fail to create pthread t0");
	}
	g_settings_set_boolean(peony_preferences,"dbus-show-peony-window",FALSE);
    }
}

void * active_window_thead(void *a){
    char buff[30]={};
    sleep(0.5);
    Window win = (long)a;
    sprintf(buff,"wmctrl -i -a %ld",(long)a);
    system(buff);
//    set_active_window (win);
    return NULL;
}


static void
peony_window_view_visible (PeonyWindow *window,
                          PeonyView *view)
{
    PeonyWindowSlot *slot;
    PeonyWindowPane *pane;
    GList *l, *walk;

    g_return_if_fail (PEONY_IS_WINDOW (window));

    slot = peony_window_get_slot_for_view (window, view);

    /* Ensure we got the right active state for newly added panes */
    peony_window_slot_is_in_active_pane (slot, slot->pane->is_active);

    if (slot->visible)
    {
        return;
    }

    slot->visible = TRUE;

    pane = slot->pane;

    if (pane->visible)
    {
        return;
    }

    /* Look for other non-visible slots */
    for (l = pane->slots; l != NULL; l = l->next)
    {
        slot = l->data;

        if (!slot->visible)
        {
            return;
        }
    }

    /* None, this pane is visible */
    peony_window_pane_show (pane);

    /* Look for other non-visible panes */
    for (walk = window->details->panes; walk; walk = walk->next)
    {
        pane = walk->data;

        if (!pane->visible)
        {
            return;
        }
    }

    peony_window_pane_grab_focus (window->details->active_pane);

    /* All slots and panes visible, show window */
    peony_window_show_window (window);
}

void
peony_window_close (PeonyWindow *window)
{
    g_return_if_fail (PEONY_IS_WINDOW (window));

    EEL_CALL_METHOD (PEONY_WINDOW_CLASS, window,
                     close, (window));

    gtk_widget_destroy (GTK_WIDGET (window));
}

PeonyWindowSlot *
peony_window_open_slot (PeonyWindowPane *pane,
                       PeonyWindowOpenSlotFlags flags)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW_PANE (pane));
    g_assert (PEONY_IS_WINDOW (pane->window));

    slot = EEL_CALL_METHOD_WITH_RETURN_VALUE (PEONY_WINDOW_CLASS, pane->window,
            open_slot, (pane, flags));

    g_assert (PEONY_IS_WINDOW_SLOT (slot));
    g_assert (pane->window == slot->pane->window);

    pane->slots = g_list_append (pane->slots, slot);

    return slot;
}

void
peony_window_close_pane (PeonyWindowPane *pane)
{
    PeonyWindow *window;

    g_assert (PEONY_IS_WINDOW_PANE (pane));
    g_assert (PEONY_IS_WINDOW (pane->window));
    g_assert (g_list_find (pane->window->details->panes, pane) != NULL);

    while (pane->slots != NULL)
    {
        PeonyWindowSlot *slot = pane->slots->data;

        peony_window_close_slot (slot);
    }

    window = pane->window;

    /* If the pane was active, set it to NULL. The caller is responsible
     * for setting a new active pane with peony_window_pane_switch_to()
     * if it wants to continue using the window. */
    if (window->details->active_pane == pane)
    {
        window->details->active_pane = NULL;
    }

    window->details->panes = g_list_remove (window->details->panes, pane);

    g_object_unref (pane);
}

static void
real_close_slot (PeonyWindowPane *pane,
                 PeonyWindowSlot *slot)
{
    peony_window_manage_views_close_slot (pane, slot);
    cancel_view_as_callback (slot);
}

void
peony_window_close_slot (PeonyWindowSlot *slot)
{
    PeonyWindowPane *pane;

    g_assert (PEONY_IS_WINDOW_SLOT (slot));
    g_assert (PEONY_IS_WINDOW_PANE(slot->pane));
    g_assert (g_list_find (slot->pane->slots, slot) != NULL);

    /* save pane because slot is not valid anymore after this call */
    pane = slot->pane;

    EEL_CALL_METHOD (PEONY_WINDOW_CLASS, slot->pane->window,
                     close_slot, (slot->pane, slot));

    g_object_run_dispose (G_OBJECT (slot));
    slot->pane = NULL;
    g_object_unref (slot);
    pane->slots = g_list_remove (pane->slots, slot);
    pane->active_slots = g_list_remove (pane->active_slots, slot);

}

PeonyWindowPane*
peony_window_get_active_pane (PeonyWindow *window)
{
    g_assert (PEONY_IS_WINDOW (window));
    return window->details->active_pane;
}

static void
real_set_active_pane (PeonyWindow *window, PeonyWindowPane *new_pane)
{
    /* make old pane inactive, and new one active.
     * Currently active pane may be NULL (after init). */
    if (window->details->active_pane &&
            window->details->active_pane != new_pane)
    {
        peony_window_pane_set_active (new_pane->window->details->active_pane, FALSE);
    }
    peony_window_pane_set_active (new_pane, TRUE);

    window->details->active_pane = new_pane;
}

/* Make the given pane the active pane of its associated window. This
 * always implies making the containing active slot the active slot of
 * the window. */
void
peony_window_set_active_pane (PeonyWindow *window,
                             PeonyWindowPane *new_pane)
{
    g_assert (PEONY_IS_WINDOW_PANE (new_pane));
    if (new_pane->active_slot)
    {
        peony_window_set_active_slot (window, new_pane->active_slot);
    }
    else if (new_pane != window->details->active_pane)
    {
        real_set_active_pane (window, new_pane);
    }
}

/* Make both, the given slot the active slot and its corresponding
 * pane the active pane of the associated window.
 * new_slot may be NULL. */
void
peony_window_set_active_slot (PeonyWindow *window, PeonyWindowSlot *new_slot)
{
    PeonyWindowSlot *old_slot;

    g_assert (PEONY_IS_WINDOW (window));

    if (new_slot)
    {
        g_assert (PEONY_IS_WINDOW_SLOT (new_slot));
        g_assert (PEONY_IS_WINDOW_PANE (new_slot->pane));
        g_assert (window == new_slot->pane->window);
        g_assert (g_list_find (new_slot->pane->slots, new_slot) != NULL);
    }

    if (window->details->active_pane != NULL)
    {
        old_slot = window->details->active_pane->active_slot;
    }
    else
    {
        old_slot = NULL;
    }

    if (old_slot == new_slot)
    {
        return;
    }

    /* make old slot inactive if it exists (may be NULL after init, for example) */
    if (old_slot != NULL)
    {
        /* inform window */
        if (old_slot->content_view != NULL)
        {
            peony_window_slot_disconnect_content_view (old_slot, old_slot->content_view);
        }

        /* inform slot & view */
        g_signal_emit_by_name (old_slot, "inactive");
    }

    /* deal with panes */
    if (new_slot &&
            new_slot->pane != window->details->active_pane)
    {
        real_set_active_pane (window, new_slot->pane);
    }

    window->details->active_pane->active_slot = new_slot;

    /* make new slot active, if it exists */
    if (new_slot)
    {
        window->details->active_pane->active_slots =
            g_list_remove (window->details->active_pane->active_slots, new_slot);
        window->details->active_pane->active_slots =
            g_list_prepend (window->details->active_pane->active_slots, new_slot);

        /* inform sidebar panels */
        peony_window_report_location_change (window);
        /* TODO decide whether "selection-changed" should be emitted */

        if (new_slot->content_view != NULL)
        {
            /* inform window */
            peony_window_slot_connect_content_view (new_slot, new_slot->content_view);
        }

        /* inform slot & view */
        g_signal_emit_by_name (new_slot, "active");
    }
}

void
peony_window_slot_close (PeonyWindowSlot *slot)
{
    peony_window_pane_slot_close (slot->pane, slot);
}

static void
peony_window_size_request (GtkWidget		*widget,
                          GtkRequisition	*requisition)
{
    GdkScreen *screen;
    guint max_width;
    guint max_height;

    g_assert (PEONY_IS_WINDOW (widget));
    g_assert (requisition != NULL);

    screen = gtk_window_get_screen (GTK_WINDOW (widget));

    /* Limit the requisition to be within 90% of the available screen
     * real state.
     *
     * This way the user will have a fighting chance of getting
     * control of their window back if for whatever reason one of the
     * window's descendants decide they want to be 4000 pixels wide.
     *
     * Note that the user can still make the window really huge by hand.
     *
     * Bugs in components or other widgets that cause such huge geometries
     * to be requested, should still be fixed.  This code is here only to
     * prevent the extremely frustrating consequence of such bugs.
     */
    max_width = get_max_forced_width (screen);
    max_height = get_max_forced_height (screen);

    if (requisition->width > (int) max_width)
    {
        requisition->width = max_width;
    }

    if (requisition->height > (int) max_height)
    {
        requisition->height = max_height;
    }
}

static void
peony_window_realize (GtkWidget *widget)
{
    GTK_WIDGET_CLASS (peony_window_parent_class)->realize (widget);
    update_cursor (PEONY_WINDOW (widget));
}

static gboolean
peony_window_key_press_event (GtkWidget *widget,
                             GdkEventKey *event)
{
    PeonyWindow *window;
    int i;

    window = PEONY_WINDOW (widget);

    for (i = 0; i < G_N_ELEMENTS (extra_window_keybindings); i++)
    {
        if (extra_window_keybindings[i].keyval == event->keyval)
        {
            const GList *action_groups;
            GtkAction *action;

            action = NULL;

            action_groups = gtk_ui_manager_get_action_groups (window->details->ui_manager);
            while (action_groups != NULL && action == NULL)
            {
                action = gtk_action_group_get_action (action_groups->data, extra_window_keybindings[i].action);
                action_groups = action_groups->next;
            }

            g_assert (action != NULL);
            if (gtk_action_is_sensitive (action))
            {
                gtk_action_activate (action);
                return TRUE;
            }

            break;
        }
    }

    return GTK_WIDGET_CLASS (peony_window_parent_class)->key_press_event (widget, event);
}

/*
 * Main API
 */

static void
free_activate_view_data (gpointer data)
{
    ActivateViewData *activate_data;

    activate_data = data;

    g_free (activate_data->id);

    g_slice_free (ActivateViewData, activate_data);
}

static void
action_view_as_callback (GtkAction *action,
                         ActivateViewData *data)
{
    PeonyWindow *window;
    PeonyWindowSlot *slot;

    window = data->window;

    if (gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)))
    {
        slot = window->details->active_pane->active_slot;
        peony_window_slot_set_content_view (slot,
                                           data->id);
    }
}

static GtkRadioAction *
add_view_as_menu_item (PeonyWindow *window,
                       const char *placeholder_path,
                       const char *identifier,
                       int index, /* extra_viewer is always index 0 */
                       guint merge_id)
{
    const PeonyViewInfo *info;
    GtkRadioAction *action;
    char action_name[32];
    ActivateViewData *data;

    char accel[32];
    char accel_path[48];
    unsigned int accel_keyval;

    info = peony_view_factory_lookup (identifier);

    g_snprintf (action_name, sizeof (action_name), "view_as_%d", index);
    action = gtk_radio_action_new (action_name,
                                   _(info->view_menu_label_with_mnemonic),
                                   _(info->display_location_label),
                                   NULL,
                                   0);

    if (index >= 1 && index <= 9)
    {
        g_snprintf (accel, sizeof (accel), "%d", index);
        g_snprintf (accel_path, sizeof (accel_path), "<Peony-Window>/%s", action_name);

        accel_keyval = gdk_keyval_from_name (accel);
		g_assert (accel_keyval != GDK_KEY_VoidSymbol);

        gtk_accel_map_add_entry (accel_path, accel_keyval, GDK_CONTROL_MASK);
        gtk_action_set_accel_path (GTK_ACTION (action), accel_path);
    }

    if (window->details->view_as_radio_action != NULL)
    {
        gtk_radio_action_set_group (action,
                                    gtk_radio_action_get_group (window->details->view_as_radio_action));
    }
    else if (index != 0)
    {
        /* Index 0 is the extra view, and we don't want to use that here,
           as it can get deleted/changed later */
        window->details->view_as_radio_action = action;
    }

    data = g_slice_new (ActivateViewData);
    data->window = window;
    data->id = g_strdup (identifier);
    g_signal_connect_data (action, "activate",
                           G_CALLBACK (action_view_as_callback),
                           data, (GClosureNotify) free_activate_view_data, 0);

    gtk_action_group_add_action (window->details->view_as_action_group,
                                 GTK_ACTION (action));
    g_object_unref (action);

    gtk_ui_manager_add_ui (window->details->ui_manager,
                           merge_id,
                           placeholder_path,
                           action_name,
                           action_name,
                           GTK_UI_MANAGER_MENUITEM,
                           FALSE);

    return action; /* return value owned by group */
}

/* Make a special first item in the "View as" option menu that represents
 * the current content view. This should only be called if the current
 * content view isn't already in the "View as" option menu.
 */
static void
update_extra_viewer_in_view_as_menus (PeonyWindow *window,
                                      const char *id)
{
    gboolean had_extra_viewer;

    had_extra_viewer = window->details->extra_viewer != NULL;

    if (id == NULL)
    {
        if (!had_extra_viewer)
        {
            return;
        }
    }
    else
    {
        if (had_extra_viewer
                && strcmp (window->details->extra_viewer, id) == 0)
        {
            return;
        }
    }
    g_free (window->details->extra_viewer);
    window->details->extra_viewer = g_strdup (id);

    if (window->details->extra_viewer_merge_id != 0)
    {
        gtk_ui_manager_remove_ui (window->details->ui_manager,
                                  window->details->extra_viewer_merge_id);
        window->details->extra_viewer_merge_id = 0;
    }

    if (window->details->extra_viewer_radio_action != NULL)
    {
        gtk_action_group_remove_action (window->details->view_as_action_group,
                                        GTK_ACTION (window->details->extra_viewer_radio_action));
        window->details->extra_viewer_radio_action = NULL;
    }

    if (id != NULL)
    {
        window->details->extra_viewer_merge_id = gtk_ui_manager_new_merge_id (window->details->ui_manager);
        window->details->extra_viewer_radio_action =
            add_view_as_menu_item (window,
                                   PEONY_MENU_PATH_EXTRA_VIEWER_PLACEHOLDER,
                                   window->details->extra_viewer,
                                   0,
                                   window->details->extra_viewer_merge_id);
    }
}

static void
remove_extra_viewer_in_view_as_menus (PeonyWindow *window)
{
    update_extra_viewer_in_view_as_menus (window, NULL);
}

static void
replace_extra_viewer_in_view_as_menus (PeonyWindow *window)
{
    PeonyWindowSlot *slot;
    const char *id;

    slot = window->details->active_pane->active_slot;

    id = peony_window_slot_get_content_view_id (slot);
    update_extra_viewer_in_view_as_menus (window, id);
}

/**
 * peony_window_synch_view_as_menus:
 *
 * Set the visible item of the "View as" option menu and
 * the marked "View as" item in the View menu to
 * match the current content view.
 *
 * @window: The PeonyWindow whose "View as" option menu should be synched.
 */
static void
peony_window_synch_view_as_menus (PeonyWindow *window)
{
    PeonyWindowSlot *slot;
    int index;
    char action_name[32];
    GList *node;
    GtkAction *action;

    g_assert (PEONY_IS_WINDOW (window));

    slot = window->details->active_pane->active_slot;

    if (slot->content_view == NULL)
    {
        return;
    }
    for (node = window->details->short_list_viewers, index = 1;
            node != NULL;
            node = node->next, ++index)
    {
        if (peony_window_slot_content_view_matches_iid (slot, (char *)node->data))
        {
            break;
        }
    }
    if (node == NULL)
    {
        replace_extra_viewer_in_view_as_menus (window);
        index = 0;
    }
    else
    {
        remove_extra_viewer_in_view_as_menus (window);
    }

    g_snprintf (action_name, sizeof (action_name), "view_as_%d", index);
    action = gtk_action_group_get_action (window->details->view_as_action_group,
                                          action_name);

    /* Don't trigger the action callback when we're synchronizing */
    g_signal_handlers_block_matched (action,
                                     G_SIGNAL_MATCH_FUNC,
                                     0, 0,
                                     NULL,
                                     action_view_as_callback,
                                     NULL);
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
    g_signal_handlers_unblock_matched (action,
                                       G_SIGNAL_MATCH_FUNC,
                                       0, 0,
                                       NULL,
                                       action_view_as_callback,
                                       NULL);
}

static void
refresh_stored_viewers (PeonyWindow *window)
{
    PeonyWindowSlot *slot;
    GList *viewers;
    char *uri, *mimetype;

    slot = window->details->active_pane->active_slot;

    uri = peony_file_get_uri (slot->viewed_file);
    mimetype = peony_file_get_mime_type (slot->viewed_file);
    viewers = peony_view_factory_get_views_for_uri (uri,
              peony_file_get_file_type (slot->viewed_file),
              mimetype);
    g_free (uri);
    g_free (mimetype);

    free_stored_viewers (window);
    window->details->short_list_viewers = viewers;
}

static void
load_view_as_menu (PeonyWindow *window)
{
    GList *node;
    int index;
    guint merge_id;

    if (window->details->short_list_merge_id != 0)
    {
        gtk_ui_manager_remove_ui (window->details->ui_manager,
                                  window->details->short_list_merge_id);
        window->details->short_list_merge_id = 0;
    }
    if (window->details->extra_viewer_merge_id != 0)
    {
        gtk_ui_manager_remove_ui (window->details->ui_manager,
                                  window->details->extra_viewer_merge_id);
        window->details->extra_viewer_merge_id = 0;
        window->details->extra_viewer_radio_action = NULL;
    }
    if (window->details->view_as_action_group != NULL)
    {
        gtk_ui_manager_remove_action_group (window->details->ui_manager,
                                            window->details->view_as_action_group);
        window->details->view_as_action_group = NULL;
    }


    refresh_stored_viewers (window);

    merge_id = gtk_ui_manager_new_merge_id (window->details->ui_manager);
    window->details->short_list_merge_id = merge_id;
    window->details->view_as_action_group = gtk_action_group_new ("ViewAsGroup");
    gtk_action_group_set_translation_domain (window->details->view_as_action_group, GETTEXT_PACKAGE);
    window->details->view_as_radio_action = NULL;

    /* Add a menu item for each view in the preferred list for this location. */
    /* Start on 1, because extra_viewer gets index 0 */
    for (node = window->details->short_list_viewers, index = 1;
            node != NULL;
            node = node->next, ++index)
    {
        /* Menu item in View menu. */
        add_view_as_menu_item (window,
                               PEONY_MENU_PATH_SHORT_LIST_PLACEHOLDER,
                               node->data,
                               index,
                               merge_id);
    }
    gtk_ui_manager_insert_action_group (window->details->ui_manager,
                                        window->details->view_as_action_group,
                                        -1);
    g_object_unref (window->details->view_as_action_group); /* owned by ui_manager */

    peony_window_synch_view_as_menus (window);

    g_signal_emit (window, signals[VIEW_AS_CHANGED], 0);

}

static void
load_view_as_menus_callback (PeonyFile *file,
                             gpointer callback_data)
{
    PeonyWindow *window;
    PeonyWindowSlot *slot;

    slot = callback_data;
    window = PEONY_WINDOW (slot->pane->window);

    if (slot == window->details->active_pane->active_slot)
    {
        load_view_as_menu (window);
    }
}

static void
cancel_view_as_callback (PeonyWindowSlot *slot)
{
    peony_file_cancel_call_when_ready (slot->viewed_file,
                                      load_view_as_menus_callback,
                                      slot);
}

void
peony_window_load_view_as_menus (PeonyWindow *window)
{
    PeonyWindowSlot *slot;
    PeonyFileAttributes attributes;

    g_return_if_fail (PEONY_IS_WINDOW (window));

    attributes = peony_mime_actions_get_required_file_attributes ();

    slot = window->details->active_pane->active_slot;

    cancel_view_as_callback (slot);
    peony_file_call_when_ready (slot->viewed_file,
                               attributes,
                               load_view_as_menus_callback,
                               slot);
}

void
peony_window_display_error (PeonyWindow *window, const char *error_msg)
{
    GtkWidget *dialog;

    g_return_if_fail (PEONY_IS_WINDOW (window));

    dialog = gtk_message_dialog_new (GTK_WINDOW (window), 0, GTK_MESSAGE_ERROR,
                                     GTK_BUTTONS_OK, error_msg, NULL);
    gtk_widget_show (dialog);
}

static char *
real_get_title (PeonyWindow *window)
{
    g_assert (PEONY_IS_WINDOW (window));

    return peony_window_slot_get_title (window->details->active_pane->active_slot);
}

static void
real_sync_title (PeonyWindow *window,
                 PeonyWindowSlot *slot)
{
    char *copy;

    if (slot == window->details->active_pane->active_slot)
    {
        copy = g_strdup (slot->title);
        g_signal_emit_by_name (window, "title_changed",
                               slot->title);
        g_free (copy);
    }
}

void
peony_window_sync_title (PeonyWindow *window,
                        PeonyWindowSlot *slot)
{
    EEL_CALL_METHOD (PEONY_WINDOW_CLASS, window,
                     sync_title, (window, slot));
}

void
peony_window_sync_zoom_widgets (PeonyWindow *window)
{
    PeonyWindowSlot *slot;
    PeonyView *view;
    GtkAction *action;
    gboolean supports_zooming;
    gboolean can_zoom, can_zoom_in, can_zoom_out;
    PeonyZoomLevel zoom_level;

    slot = window->details->active_pane->active_slot;
    view = slot->content_view;

    if (view != NULL)
    {
        supports_zooming = peony_view_supports_zooming (view);
        zoom_level = peony_view_get_zoom_level (view);
        can_zoom = supports_zooming &&
                   zoom_level >= PEONY_ZOOM_LEVEL_SMALLEST &&
                   zoom_level <= PEONY_ZOOM_LEVEL_LARGEST;
        can_zoom_in = can_zoom && peony_view_can_zoom_in (view);
        can_zoom_out = can_zoom && peony_view_can_zoom_out (view);
    }
    else
    {
        zoom_level = PEONY_ZOOM_LEVEL_STANDARD;
        supports_zooming = FALSE;
        can_zoom = FALSE;
        can_zoom_in = FALSE;
        can_zoom_out = FALSE;
    }

    action = gtk_action_group_get_action (window->details->main_action_group,
                                          PEONY_ACTION_ZOOM_IN);
    gtk_action_set_visible (action, supports_zooming);
    gtk_action_set_sensitive (action, can_zoom_in);

    action = gtk_action_group_get_action (window->details->main_action_group,
                                          PEONY_ACTION_ZOOM_OUT);
    gtk_action_set_visible (action, supports_zooming);
    gtk_action_set_sensitive (action, can_zoom_out);

    action = gtk_action_group_get_action (window->details->main_action_group,
                                          PEONY_ACTION_ZOOM_NORMAL);
    gtk_action_set_visible (action, supports_zooming);
    gtk_action_set_sensitive (action, can_zoom);

    g_signal_emit (window, signals[ZOOM_CHANGED], 0,
                   zoom_level, supports_zooming, can_zoom,
                   can_zoom_in, can_zoom_out);
}

static void
zoom_level_changed_callback (PeonyView *view,
                             PeonyWindow *window)
{
    g_assert (PEONY_IS_WINDOW (window));

    /* This is called each time the component in
     * the active slot successfully completed
     * a zooming operation.
     */
    peony_window_sync_zoom_widgets (window);
}


/* These are called
 *   A) when switching the view within the active slot
 *   B) when switching the active slot
 *   C) when closing the active slot (disconnect)
*/
void
peony_window_connect_content_view (PeonyWindow *window,
                                  PeonyView *view)
{
	printf("slot-changed");
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));
    g_assert (PEONY_IS_VIEW (view));

    slot = peony_window_get_slot_for_view (window, view);
    g_assert (slot == peony_window_get_active_slot (window));

    g_signal_connect (view, "zoom-level-changed",
                      G_CALLBACK (zoom_level_changed_callback),
                      window);

    /* Update displayed view in menu. Only do this if we're not switching
     * locations though, because if we are switching locations we'll
     * install a whole new set of views in the menu later (the current
     * views in the menu are for the old location).
     */
    if (slot->pending_location == NULL)
    {
        peony_window_load_view_as_menus (window);
    }

    peony_view_grab_focus (view);
}

void
peony_window_disconnect_content_view (PeonyWindow *window,
                                     PeonyView *view)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));
    g_assert (PEONY_IS_VIEW (view));

    slot = peony_window_get_slot_for_view (window, view);
    g_assert (slot == peony_window_get_active_slot (window));

    g_signal_handlers_disconnect_by_func (view, G_CALLBACK (zoom_level_changed_callback), window);
}

/**
 * peony_window_show:
 * @widget:	GtkWidget
 *
 * Call parent and then show/hide window items
 * base on user prefs.
 */
static void
peony_window_show (GtkWidget *widget)
{
    PeonyWindow *window;

    window = PEONY_WINDOW (widget);

    GTK_WIDGET_CLASS (peony_window_parent_class)->show (widget);

    peony_window_ui_update (window);
}

GtkUIManager *
peony_window_get_ui_manager (PeonyWindow *window)
{
    g_return_val_if_fail (PEONY_IS_WINDOW (window), NULL);

    return window->details->ui_manager;
}

PeonyWindowPane *
peony_window_get_next_pane (PeonyWindow *window)
{
    PeonyWindowPane *next_pane;
    GList *node;

    /* return NULL if there is only one pane */
    if (!window->details->panes || !window->details->panes->next)
    {
        return NULL;
    }

    /* get next pane in the (wrapped around) list */
    node = g_list_find (window->details->panes, window->details->active_pane);
    g_return_val_if_fail (node, NULL);
    if (node->next)
    {
        next_pane = node->next->data;
    }
    else
    {
        next_pane =  window->details->panes->data;
    }

    return next_pane;
}


void
peony_window_slot_set_viewed_file (PeonyWindowSlot *slot,
                                  PeonyFile *file)
{
    PeonyWindow *window;
    PeonyFileAttributes attributes;

    if (slot->viewed_file == file)
    {
        return;
    }

    peony_file_ref (file);

    cancel_view_as_callback (slot);

    if (slot->viewed_file != NULL)
    {
        window = slot->pane->window;

        if (PEONY_IS_SPATIAL_WINDOW (window))
        {
            peony_file_set_has_open_window (slot->viewed_file,
                                           FALSE);
        }
        peony_file_monitor_remove (slot->viewed_file,
                                  slot);
    }

    if (file != NULL)
    {
        attributes =
            PEONY_FILE_ATTRIBUTE_INFO |
            PEONY_FILE_ATTRIBUTE_LINK_INFO;
        peony_file_monitor_add (file, slot, attributes);
    }

    peony_file_unref (slot->viewed_file);
    slot->viewed_file = file;
}

void
peony_send_history_list_changed (void)
{
    g_signal_emit_by_name (peony_signaller_get_current (),
                           "history_list_changed");
}

static void
free_history_list (void)
{
    g_list_free_full (history_list, g_object_unref);
    history_list = NULL;
}

/* Remove the this URI from the history list.
 * Do not sent out a change notice.
 * We pass in a bookmark for convenience.
 */
static void
remove_from_history_list (PeonyBookmark *bookmark)
{
    GList *node;

    /* Compare only the uris here. Comparing the names also is not
     * necessary and can cause problems due to the asynchronous
     * nature of when the title of the window is set.
     */
    node = g_list_find_custom (history_list,
                               bookmark,
                               peony_bookmark_compare_uris);

    /* Remove any older entry for this same item. There can be at most 1. */
    if (node != NULL)
    {
        history_list = g_list_remove_link (history_list, node);
        g_object_unref (node->data);
        g_list_free_1 (node);
    }
}

gboolean
peony_add_bookmark_to_history_list (PeonyBookmark *bookmark)
{
    /* Note that the history is shared amongst all windows so
     * this is not a PeonyNavigationWindow function. Perhaps it belongs
     * in its own file.
     */
    int i;
    GList *l, *next;
    static gboolean free_history_list_is_set_up;

    g_assert (PEONY_IS_BOOKMARK (bookmark));

    if (!free_history_list_is_set_up)
    {
        eel_debug_call_at_shutdown (free_history_list);
        free_history_list_is_set_up = TRUE;
    }

    /*	g_warning ("Add to history list '%s' '%s'",
    		   peony_bookmark_get_name (bookmark),
    		   peony_bookmark_get_uri (bookmark)); */

    if (!history_list ||
            peony_bookmark_compare_uris (history_list->data, bookmark))
    {
        g_object_ref (bookmark);
        remove_from_history_list (bookmark);
        history_list = g_list_prepend (history_list, bookmark);

        for (i = 0, l = history_list; l; l = next)
        {
            next = l->next;

            if (i++ >= MAX_HISTORY_ITEMS)
            {
                g_object_unref (l->data);
                history_list = g_list_delete_link (history_list, l);
            }
        }

        return TRUE;
    }

    return FALSE;
}

void
peony_remove_from_history_list_no_notify (GFile *location)
{
    PeonyBookmark *bookmark;

    bookmark = peony_bookmark_new (location, "", FALSE, NULL);
    remove_from_history_list (bookmark);
    g_object_unref (bookmark);
}

gboolean
peony_add_to_history_list_no_notify (GFile *location,
                                    const char *name,
                                    gboolean has_custom_name,
                                    GIcon *icon)
{
    PeonyBookmark *bookmark;
    gboolean ret;

    bookmark = peony_bookmark_new (location, name, has_custom_name, icon);
    ret = peony_add_bookmark_to_history_list (bookmark);
    g_object_unref (bookmark);

    return ret;
}

PeonyWindowSlot *
peony_window_get_slot_for_view (PeonyWindow *window,
                               PeonyView *view)
{
    PeonyWindowSlot *slot;
    GList *l, *walk;

    for (walk = window->details->panes; walk; walk = walk->next)
    {
        PeonyWindowPane *pane = walk->data;

        for (l = pane->slots; l != NULL; l = l->next)
        {
            slot = l->data;
            if (slot->content_view == view ||
                    slot->new_content_view == view)
            {
                return slot;
            }
        }
    }

    return NULL;
}

void
peony_forget_history (void)
{
    PeonyWindowSlot *slot;
    PeonyNavigationWindowSlot *navigation_slot;
    GList *window_node, *l, *walk;
    PeonyApplication *app;

    app = PEONY_APPLICATION (g_application_get_default ());
    /* Clear out each window's back & forward lists. Also, remove
     * each window's current location bookmark from history list
     * so it doesn't get clobbered.
     */
    for (window_node = gtk_application_get_windows (GTK_APPLICATION (app));
            window_node != NULL;
            window_node = window_node->next)
    {

        if (PEONY_IS_NAVIGATION_WINDOW (window_node->data))
        {
            PeonyNavigationWindow *window;

            window = PEONY_NAVIGATION_WINDOW (window_node->data);

            for (walk = PEONY_WINDOW (window_node->data)->details->panes; walk; walk = walk->next)
            {
                PeonyWindowPane *pane = walk->data;
                for (l = pane->slots; l != NULL; l = l->next)
                {
                    navigation_slot = l->data;

                    peony_navigation_window_slot_clear_back_list (navigation_slot);
                    peony_navigation_window_slot_clear_forward_list (navigation_slot);
                }
            }

            peony_navigation_window_allow_back (window, FALSE);
            peony_navigation_window_allow_forward (window, FALSE);
        }

        for (walk = PEONY_WINDOW (window_node->data)->details->panes; walk; walk = walk->next)
        {
            PeonyWindowPane *pane = walk->data;
            for (l = pane->slots; l != NULL; l = l->next)
            {
                slot = l->data;
                history_list = g_list_remove (history_list,
                                              slot->current_location_bookmark);
            }
        }
    }

    /* Clobber history list. */
    free_history_list ();

    /* Re-add each window's current location to history list. */
    for (window_node = gtk_application_get_windows (GTK_APPLICATION (app));
            window_node != NULL;
            window_node = window_node->next)
    {
        PeonyWindow *window;
        PeonyWindowSlot *slot;
        GList *l;

        window = PEONY_WINDOW (window_node->data);
        for (walk = window->details->panes; walk; walk = walk->next)
        {
            PeonyWindowPane *pane = walk->data;
            for (l = pane->slots; l != NULL; l = l->next)
            {
                slot = PEONY_WINDOW_SLOT (l->data);
                peony_window_slot_add_current_location_to_history_list (slot);
            }
        }
    }
}

GList *
peony_get_history_list (void)
{
    return history_list;
}

static GList *
peony_window_get_history (PeonyWindow *window)
{
    return eel_g_object_list_copy (history_list);
}


static PeonyWindowType
peony_window_get_window_type (PeonyWindow *window)
{
    g_assert (PEONY_IS_WINDOW (window));

    return PEONY_WINDOW_GET_CLASS (window)->window_type;
}

static int
peony_window_get_selection_count (PeonyWindow *window)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));

    slot = window->details->active_pane->active_slot;

    if (slot->content_view != NULL)
    {
        return peony_view_get_selection_count (slot->content_view);
    }

    return 0;
}

static GList *
peony_window_get_selection (PeonyWindow *window)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));

    slot = window->details->active_pane->active_slot;

    if (slot->content_view != NULL)
    {
        return peony_view_get_selection (slot->content_view);
    }
    return NULL;
}

static PeonyWindowShowHiddenFilesMode
peony_window_get_hidden_files_mode (PeonyWindowInfo *window)
{
    return window->details->show_hidden_files_mode;
}

static void
peony_window_set_hidden_files_mode (PeonyWindowInfo *window,
                                   PeonyWindowShowHiddenFilesMode  mode)
{
    window->details->show_hidden_files_mode = mode;

    g_signal_emit_by_name (window, "hidden_files_mode_changed");
}

static gboolean
peony_window_get_initiated_unmount (PeonyWindowInfo *window)
{
    return window->details->initiated_unmount;
}

static void
peony_window_set_initiated_unmount (PeonyWindowInfo *window,
                                   gboolean initiated_unmount)
{
    window->details->initiated_unmount = initiated_unmount;
}

static char *
peony_window_get_cached_title (PeonyWindow *window)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));

    slot = window->details->active_pane->active_slot;

    return g_strdup (slot->title);
}

PeonyWindowSlot *
peony_window_get_active_slot (PeonyWindow *window)
{
    g_assert (PEONY_IS_WINDOW (window));

    return window->details->active_pane->active_slot;
}

PeonyWindowSlot *
peony_window_get_extra_slot (PeonyWindow *window)
{
    PeonyWindowPane *extra_pane;
    GList *node;

    g_assert (PEONY_IS_WINDOW (window));


    /* return NULL if there is only one pane */
    if (window->details->panes == NULL ||
            window->details->panes->next == NULL)
    {
        return NULL;
    }

    /* get next pane in the (wrapped around) list */
    node = g_list_find (window->details->panes,
                        window->details->active_pane);
    g_return_val_if_fail (node, FALSE);

    if (node->next)
    {
        extra_pane = node->next->data;
    }
    else
    {
        extra_pane =  window->details->panes->data;
    }

    return extra_pane->active_slot;
}

GList *
peony_window_get_slots (PeonyWindow *window)
{
    GList *walk,*list;

    g_assert (PEONY_IS_WINDOW (window));

    list = NULL;
    for (walk = window->details->panes; walk; walk = walk->next)
    {
        PeonyWindowPane *pane = walk->data;
        list  = g_list_concat (list, g_list_copy(pane->slots));
    }
    return list;
}

static void
peony_window_info_iface_init (PeonyWindowInfoIface *iface)
{
    iface->report_load_underway = peony_window_report_load_underway;
    iface->report_load_complete = peony_window_report_load_complete;
    iface->report_selection_changed = peony_window_report_selection_changed;
    iface->report_view_failed = peony_window_report_view_failed;
    iface->view_visible = peony_window_view_visible;
    iface->close_window = peony_window_close;
    iface->push_status = peony_window_push_status;
    iface->get_window_type = peony_window_get_window_type;
    iface->get_title = peony_window_get_cached_title;
    iface->get_history = peony_window_get_history;
    iface->get_current_location = peony_window_get_location_uri;
    iface->get_ui_manager = peony_window_get_ui_manager;
    iface->get_selection_count = peony_window_get_selection_count;
    iface->get_selection = peony_window_get_selection;
    iface->get_hidden_files_mode = peony_window_get_hidden_files_mode;
    iface->set_hidden_files_mode = peony_window_set_hidden_files_mode;
    iface->get_active_slot = peony_window_get_active_slot;
    iface->get_extra_slot = peony_window_get_extra_slot;
    iface->get_initiated_unmount = peony_window_get_initiated_unmount;
    iface->set_initiated_unmount = peony_window_set_initiated_unmount;
}

static void
peony_window_class_init (PeonyWindowClass *class)
{
    GtkBindingSet *binding_set;

    G_OBJECT_CLASS (class)->constructor = peony_window_constructor;
    G_OBJECT_CLASS (class)->constructed = peony_window_constructed;
    G_OBJECT_CLASS (class)->get_property = peony_window_get_property;
    G_OBJECT_CLASS (class)->set_property = peony_window_set_property;
    G_OBJECT_CLASS (class)->finalize = peony_window_finalize;

    GTK_WIDGET_CLASS (class)->destroy = peony_window_destroy;

    GTK_WIDGET_CLASS (class)->show = peony_window_show;

    GTK_WIDGET_CLASS (class)->realize = peony_window_realize;
    GTK_WIDGET_CLASS (class)->key_press_event = peony_window_key_press_event;
    class->get_title = real_get_title;
    class->sync_title = real_sync_title;
    class->set_allow_up = real_set_allow_up;
    class->close_slot = real_close_slot;

    g_object_class_install_property (G_OBJECT_CLASS (class),
                                     ARG_APP,
                                     g_param_spec_object ("app",
                                             "Application",
                                             "The PeonyApplication associated with this window.",
                                             PEONY_TYPE_APPLICATION,
                                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    signals[GO_UP] =
        g_signal_new ("go_up",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (PeonyWindowClass, go_up),
                      g_signal_accumulator_true_handled, NULL,
                      peony_src_marshal_BOOLEAN__BOOLEAN,
                      G_TYPE_BOOLEAN, 1, G_TYPE_BOOLEAN);
    signals[RELOAD] =
        g_signal_new ("reload",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (PeonyWindowClass, reload),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);
    signals[PROMPT_FOR_LOCATION] =
        g_signal_new ("prompt-for-location",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (PeonyWindowClass, prompt_for_location),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__STRING,
                      G_TYPE_NONE, 1, G_TYPE_STRING);
    signals[ZOOM_CHANGED] =
        g_signal_new ("zoom-changed",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST,
                      0,
                      NULL, NULL,
                      peony_src_marshal_VOID__INT_BOOLEAN_BOOLEAN_BOOLEAN_BOOLEAN,
                      G_TYPE_NONE, 5,
                      G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN,
                      G_TYPE_BOOLEAN, G_TYPE_BOOLEAN);
    signals[VIEW_AS_CHANGED] =
        g_signal_new ("view-as-changed",
                      G_TYPE_FROM_CLASS (class),
                      G_SIGNAL_RUN_LAST,
                      0,
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    binding_set = gtk_binding_set_by_class (class);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_BackSpace, 0,
                                  "go_up", 1,
                                  G_TYPE_BOOLEAN, FALSE);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_F5, 0,
                                  "reload", 0);
	gtk_binding_entry_add_signal (binding_set, GDK_KEY_slash, 0,
                                  "prompt-for-location", 1,
                                  G_TYPE_STRING, "/");

    class->reload = peony_window_reload;
    class->go_up = peony_window_go_up_signal;



    g_type_class_add_private (G_OBJECT_CLASS (class), sizeof (PeonyWindowDetails));
}
