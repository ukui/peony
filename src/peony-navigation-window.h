/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *  Copyright (C) 2003 Ximian, Inc.
 *  Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
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
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *           Darin Adler <darin@bentspoon.com>
 *           Zuxun Yang <yangzuxun@kylinos.cn>
 *
 */
/* peony-navigation-window.h: Interface of the navigation window object */

#ifndef PEONY_NAVIGATION_WINDOW_H
#define PEONY_NAVIGATION_WINDOW_H

#include <gtk/gtk.h>
#include <eel/eel-glib-extensions.h>
#include <libpeony-private/peony-bookmark.h>
#include <libpeony-private/peony-sidebar.h>
#include "peony-application.h"
#include "peony-information-panel.h"
#include "peony-side-pane.h"
#include "peony-window.h"

#define PEONY_TYPE_NAVIGATION_WINDOW peony_navigation_window_get_type()
#define PEONY_NAVIGATION_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_NAVIGATION_WINDOW, PeonyNavigationWindow))
#define PEONY_NAVIGATION_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_NAVIGATION_WINDOW, PeonyNavigationWindowClass))
#define PEONY_IS_NAVIGATION_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_NAVIGATION_WINDOW))
#define PEONY_IS_NAVIGATION_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_NAVIGATION_WINDOW))
#define PEONY_NAVIGATION_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_NAVIGATION_WINDOW, PeonyNavigationWindowClass))

typedef struct _PeonyNavigationWindow        PeonyNavigationWindow;
typedef struct _PeonyNavigationWindowClass   PeonyNavigationWindowClass;
typedef struct _PeonyNavigationWindowDetails PeonyNavigationWindowDetails;


struct _PeonyNavigationWindow
{
    PeonyWindow parent_object;
    //PeonyWindow is aslo PeonyWindowInfo
    
    PeonyNavigationWindowDetails *details;

    /** UI stuff **/
    PeonySidePane *sidebar;

    /* Current views stuff */
    GList *sidebar_panels;
    GtkWidget *toolbar_table;
    GtkWidget *toolbarViewAs;
	GtkWidget *viewAsbox;
};


struct _PeonyNavigationWindowClass
{
    PeonyWindowClass parent_spot;
};

GType    peony_navigation_window_get_type             (void);
void     peony_navigation_window_allow_back           (PeonyNavigationWindow *window,
        gboolean                  allow);
void     peony_navigation_window_allow_forward        (PeonyNavigationWindow *window,
        gboolean                  allow);
void     peony_navigation_window_clear_back_list      (PeonyNavigationWindow *window);
void     peony_navigation_window_clear_forward_list   (PeonyNavigationWindow *window);
void     peony_forget_history                         (void);
gint     peony_navigation_window_get_base_page_index  (PeonyNavigationWindow *window);
void     peony_navigation_window_hide_toolbar         (PeonyNavigationWindow *window);
void     peony_navigation_window_show_toolbar         (PeonyNavigationWindow *window);
gboolean peony_navigation_window_toolbar_showing      (PeonyNavigationWindow *window);
void     peony_navigation_window_hide_sidebar         (PeonyNavigationWindow *window);
void     peony_navigation_window_show_sidebar         (PeonyNavigationWindow *window);
gboolean peony_navigation_window_sidebar_showing      (PeonyNavigationWindow *window);
void     peony_navigation_window_add_sidebar_panel    (PeonyNavigationWindow *window,
        PeonySidebar          *sidebar_panel);
void     peony_navigation_window_remove_sidebar_panel (PeonyNavigationWindow *window,
        PeonySidebar          *sidebar_panel);
void     peony_navigation_window_hide_status_bar      (PeonyNavigationWindow *window);
void     peony_navigation_window_show_status_bar      (PeonyNavigationWindow *window);
gboolean peony_navigation_window_status_bar_showing   (PeonyNavigationWindow *window);
void     peony_navigation_window_back_or_forward      (PeonyNavigationWindow *window,
        gboolean                  back,
        guint                     distance,
        gboolean                  new_tab);
void     peony_navigation_window_show_search          (PeonyNavigationWindow *window);
void     peony_navigation_window_unset_focus_widget   (PeonyNavigationWindow *window);
void     peony_navigation_window_hide_search          (PeonyNavigationWindow *window);
void     peony_navigation_window_set_search_button	 (PeonyNavigationWindow *window,
        gboolean		    state);
void     peony_navigation_window_restore_focus_widget (PeonyNavigationWindow *window);
void     peony_navigation_window_split_view_on        (PeonyNavigationWindow *window);
void     peony_navigation_window_split_view_off       (PeonyNavigationWindow *window);
gboolean peony_navigation_window_split_view_showing   (PeonyNavigationWindow *window);

gboolean peony_navigation_window_is_in_temporary_navigation_bar (GtkWidget *widget,
        PeonyNavigationWindow *window);
gboolean peony_navigation_window_is_in_temporary_search_bar (GtkWidget *widget,
        PeonyNavigationWindow *window);

#endif
