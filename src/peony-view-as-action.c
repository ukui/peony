/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 2009 Red Hat, Inc.
 *
 *  Peony is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Peony is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Authors: Alexander Larsson <alexl@redhat.com>
 *
 */

#include <config.h>

#include "peony-view-as-action.h"
#include "peony-navigation-window.h"
#include "peony-window-private.h"
#include "peony-navigation-window-slot.h"
#include <gtk/gtk.h>
#include <eel/eel-gtk-extensions.h>
#include <libpeony-private/peony-view-factory.h>
#include <libpeony-private/peony-file-utilities.h>

G_DEFINE_TYPE (PeonyViewAsAction, peony_view_as_action, GTK_TYPE_ACTION)

static void peony_view_as_action_init       (PeonyViewAsAction *action);
static void peony_view_as_action_class_init (PeonyViewAsActionClass *class);

static GObjectClass *parent_class = NULL;

#define PEONY_VIEW_AS_ACTION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), PEONY_TYPE_VIEW_AS_ACTION, PeonyViewAsActionPrivate))

struct PeonyViewAsActionPrivate
{
    PeonyNavigationWindow *window;
};

typedef struct
{
    PeonyNavigationWindow *window;
    GtkButton *viewButton;
    GtkMenu *viewMenu;
}ViewData;

enum
{
    PROP_0,
    PROP_WINDOW
};


static void
activate_nth_short_list_item (PeonyWindow *window, guint index)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));

    slot = peony_window_get_active_slot (window);
    g_assert (index < g_list_length (window->details->short_list_viewers));

    peony_window_slot_set_content_view (slot,
                                       g_list_nth_data (window->details->short_list_viewers, index));
}

static void
activate_extra_viewer (PeonyWindow *window)
{
    PeonyWindowSlot *slot;

    g_assert (PEONY_IS_WINDOW (window));

    slot = peony_window_get_active_slot (window);
    g_assert (window->details->extra_viewer != NULL);

    peony_window_slot_set_content_view (slot, window->details->extra_viewer);
}

static void
view_as_menu_switch_views_callback (GtkCheckMenuItem *CheckMenuItem, ViewData * data)
{
    int active;
    GtkWidget *currentItem;
    gchar * currentItemLabel;
    ViewData  viewData;
    gtk_check_menu_item_set_active (CheckMenuItem,TRUE );
    viewData.viewButton= data->viewButton;
    viewData.window = data->window;
    viewData.viewMenu = data->viewMenu;
    gtk_menu_popdown (viewData.viewMenu);
    g_assert (PEONY_IS_NAVIGATION_WINDOW (viewData.window));
    currentItem = CheckMenuItem;
    currentItemLabel = gtk_menu_item_get_label(currentItem);
    if(0 == strcmp(currentItemLabel,_("Icon View")))
    {
      active = 0;
    }
    if(0 == strcmp(currentItemLabel,_("Compact View")))
    {
      active = 1;
    }
    if(0 == strcmp(currentItemLabel,_("List View")))
    {
      active = 2;
    }
    if (active < 0)
    {
      return;
    }
    else if (active < GPOINTER_TO_INT (g_object_get_data (G_OBJECT (viewData.viewButton), "num viewers")))
    {
      activate_nth_short_list_item (PEONY_WINDOW (viewData.window), active);
    }
    else
    {
      activate_extra_viewer (PEONY_WINDOW (viewData.window));
    }

}

static void
view_button_clicked_callback(GtkWidget* w,gpointer userdata)
{
   GtkMenu *menu;
   GtkButton *button;
   button=GTK_BUTTON(w);
   menu=GTK_MENU(userdata);

   g_assert(GTK_IS_BUTTON(button));
   g_assert(GTK_IS_MENU(menu));

   if(GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "num viewers"))>1)
   gtk_menu_popup(menu,NULL,NULL,NULL,NULL,button,0);
}


static void
view_as_changed_callback (PeonyWindow *window,
                          ViewData * data)
{
   PeonyWindowSlot *slot;
   GList *node;
   int index;
   int selected_index = -1;
   GtkTreeModel *model;
   GtkListStore *store;
   const PeonyViewInfo *info;
   GtkCheckMenuItem *CheckMenuItem;
   GList * children ,*it;
   GtkWidget * imageWidget;
   GtkMenu *menu;
   GtkWidget *view_as_icon;
   char * imagePath;
   GdkPixbuf *pixBuf;

   menu = data->viewMenu;
   g_assert(GTK_IS_MENU(menu));

   /* Clear the contents of ComboBox in a wacky way because there
    *      * is no function to clear all items and also no function to obtain
    *           * the number of items in a combobox.
    *                */
   children = gtk_container_get_children (GTK_CONTAINER (menu));
   for (it = children; it; it = it->next)
   {
       gtk_container_remove (GTK_CONTAINER (menu), it->data);
   }
   g_list_free (children);
   slot = peony_window_get_active_slot (window);
   /* Add a menu item for each view in the preferred list for this location. */
   for (node = window->details->short_list_viewers, index = 0;
              node != NULL;
              node = node->next, ++index)
   {
      if (peony_window_slot_content_view_matches_iid (slot, (char *)node->data))
      {
          selected_index = index;
      }
   }
      g_object_set_data (G_OBJECT (data->viewButton), "num viewers", GINT_TO_POINTER (index));

      for (node = window->details->short_list_viewers, index = 0;
	               node != NULL;
	               node = node->next, ++index)
      {
          info = peony_view_factory_lookup (node->data);
          CheckMenuItem = gtk_check_menu_item_new_with_label(info->view_combo_label);
          gtk_menu_shell_append(GTK_MENU_SHELL(menu),CheckMenuItem);
          gtk_check_menu_item_set_draw_as_radio(CheckMenuItem,TRUE );
          gtk_widget_show (GTK_WIDGET (CheckMenuItem));
          if(selected_index==index)
               gtk_check_menu_item_set_active (CheckMenuItem,TRUE );
          g_signal_connect (G_OBJECT (CheckMenuItem), "activate",
               G_CALLBACK (view_as_menu_switch_views_callback),data);
     }
     if (selected_index == -1)
     {
          const char *id;
          /* We're using an extra viewer, add a menu item for it */
          id = peony_window_slot_get_content_view_id (slot);
          info = peony_view_factory_lookup (id);
          selected_index = index;
     }
     if (index == 1)
     {
          view_as_icon = gtk_image_new_from_icon_name ("computer", GTK_ICON_SIZE_BUTTON);
          gtk_button_set_image (GTK_BUTTON(data->viewButton),view_as_icon);
          gtk_widget_show (GTK_WIDGET (view_as_icon));
     }
     if(index == 3)
     {
          if(selected_index == 0)
          {
             imagePath = peony_pixmap_file ("peony-view-menu-normal.png");
             pixBuf = gdk_pixbuf_new_from_file (imagePath, NULL);
             view_as_icon = gtk_image_new_from_pixbuf(pixBuf);
             gtk_button_set_image (GTK_BUTTON(data->viewButton),view_as_icon);
             gtk_widget_show (GTK_WIDGET (view_as_icon));
          }
          else if(selected_index == 1)
          {
             imagePath = peony_pixmap_file ("peony-view-menu-compact.png");
             pixBuf = gdk_pixbuf_new_from_file (imagePath, NULL);
             view_as_icon = gtk_image_new_from_pixbuf(pixBuf);
             gtk_button_set_image (GTK_BUTTON(data->viewButton),view_as_icon);
             gtk_widget_show (GTK_WIDGET (view_as_icon));
          }
          else if(selected_index == 2)
          {
             imagePath = peony_pixmap_file ("peony-view-menu-list.png");
             pixBuf = gdk_pixbuf_new_from_file (imagePath, NULL);
             view_as_icon = gtk_image_new_from_pixbuf(pixBuf);
             gtk_button_set_image (GTK_BUTTON(data->viewButton),view_as_icon);
             gtk_widget_show (GTK_WIDGET (view_as_icon));
          }
     }
}

static void
connect_proxy (GtkAction *action,
               GtkWidget *proxy)
{
    if (GTK_IS_TOOL_ITEM (proxy))
        {
        GtkToolItem *item = GTK_TOOL_ITEM (proxy);
        PeonyViewAsAction *vaction = PEONY_VIEW_AS_ACTION (action);
        PeonyNavigationWindow *window = vaction->priv->window;
        GtkWidget *view_as_menu_vbox;
        GtkWidget *view_button;
        ViewData *viewData;
        GtkMenu *menu;

        /* Option menu for content view types; it's empty here, filled in when a uri is set.
	 *          * Pack it into vbox so it doesn't grow vertically when location bar does.
	 *                   */
        view_as_menu_vbox = gtk_vbox_new (FALSE, 4);
        gtk_widget_show (view_as_menu_vbox);

        gtk_container_add (GTK_CONTAINER (item), view_as_menu_vbox);
        view_button = gtk_button_new();
        gtk_button_set_relief(GTK_BUTTON(view_button),GTK_RELIEF_NONE);
        gtk_widget_show_all(GTK_WIDGET(view_button));

        gtk_widget_set_size_request (view_button, 44, 32);
        gtk_box_pack_end (GTK_BOX (view_as_menu_vbox), view_button, TRUE, FALSE, 0);

        menu = gtk_menu_new();
        viewData = (ViewData *)malloc(sizeof(ViewData *));
        viewData->window = window;
        viewData->viewButton= view_button;
        viewData->viewMenu = menu;
        g_signal_connect(G_OBJECT(viewData->viewButton),"clicked",
		                         G_CALLBACK(view_button_clicked_callback),viewData->viewMenu);
        g_signal_connect (window, "view-as-changed",
		                          G_CALLBACK (view_as_changed_callback),
				                            viewData);
    }

    (* GTK_ACTION_CLASS (parent_class)->connect_proxy) (action, proxy);

}

static void
disconnect_proxy (GtkAction *action,
                  GtkWidget *proxy)
{
    if (GTK_IS_TOOL_ITEM (proxy))
    {
        PeonyViewAsAction *vaction = PEONY_VIEW_AS_ACTION (action);
        PeonyNavigationWindow *window = vaction->priv->window;

        g_signal_handlers_disconnect_matched (window,
                                              G_SIGNAL_MATCH_FUNC,
                                              0, 0, NULL, G_CALLBACK (view_as_changed_callback), NULL);
    }

    (* GTK_ACTION_CLASS (parent_class)->disconnect_proxy) (action, proxy);
}

static void
peony_view_as_action_finalize (GObject *object)
{
    (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
peony_view_as_action_set_property (GObject *object,
                                  guint prop_id,
                                  const GValue *value,
                                  GParamSpec *pspec)
{
    PeonyViewAsAction *zoom;

    zoom = PEONY_VIEW_AS_ACTION (object);

    switch (prop_id)
    {
    case PROP_WINDOW:
        zoom->priv->window = PEONY_NAVIGATION_WINDOW (g_value_get_object (value));
        break;
    }
}

static void
peony_view_as_action_get_property (GObject *object,
                                  guint prop_id,
                                  GValue *value,
                                  GParamSpec *pspec)
{
    PeonyViewAsAction *zoom;

    zoom = PEONY_VIEW_AS_ACTION (object);

    switch (prop_id)
    {
    case PROP_WINDOW:
        g_value_set_object (value, zoom->priv->window);
        break;
    }
}

static void
peony_view_as_action_class_init (PeonyViewAsActionClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);
    GtkActionClass *action_class = GTK_ACTION_CLASS (class);

    object_class->finalize = peony_view_as_action_finalize;
    object_class->set_property = peony_view_as_action_set_property;
    object_class->get_property = peony_view_as_action_get_property;

    parent_class = g_type_class_peek_parent (class);

    action_class->toolbar_item_type = GTK_TYPE_TOOL_ITEM;
    action_class->connect_proxy = connect_proxy;
    action_class->disconnect_proxy = disconnect_proxy;

    g_object_class_install_property (object_class,
                                     PROP_WINDOW,
                                     g_param_spec_object ("window",
                                             "Window",
                                             "The navigation window",
                                             G_TYPE_OBJECT,
                                             G_PARAM_READWRITE));

    g_type_class_add_private (object_class, sizeof(PeonyViewAsActionPrivate));
}

static void
peony_view_as_action_init (PeonyViewAsAction *action)
{
    action->priv = PEONY_VIEW_AS_ACTION_GET_PRIVATE (action);
}
