/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
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
 *
 */
/* peony-window.h: Interface of the main window object */

#ifndef PEONY_WINDOW_H
#define PEONY_WINDOW_H

#include <gtk/gtk.h>
#include <eel/eel-glib-extensions.h>
#include <libpeony-private/peony-bookmark.h>
#include <libpeony-private/peony-entry.h>
#include <libpeony-private/peony-window-info.h>
#include <libpeony-private/peony-search-directory.h>
#include "peony-application.h"
#include "peony-information-panel.h"
#include "peony-side-pane.h"

#define PEONY_TYPE_WINDOW peony_window_get_type()
#define PEONY_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_WINDOW, PeonyWindow))
#define PEONY_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_WINDOW, PeonyWindowClass))
#define PEONY_IS_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_WINDOW))
#define PEONY_IS_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_WINDOW))
#define PEONY_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_WINDOW, PeonyWindowClass))

#ifndef PEONY_WINDOW_DEFINED
#define PEONY_WINDOW_DEFINED
typedef struct PeonyWindow PeonyWindow;
#endif

#ifndef PEONY_WINDOW_SLOT_DEFINED
#define PEONY_WINDOW_SLOT_DEFINED
typedef struct PeonyWindowSlot PeonyWindowSlot;
#endif

typedef struct _PeonyWindowPane      PeonyWindowPane;

typedef struct PeonyWindowSlotClass PeonyWindowSlotClass;
typedef enum PeonyWindowOpenSlotFlags PeonyWindowOpenSlotFlags;

GType          peony_window_slot_get_type (void);

typedef enum
{
    PEONY_WINDOW_NOT_SHOWN,
    PEONY_WINDOW_POSITION_SET,
    PEONY_WINDOW_SHOULD_SHOW
} PeonyWindowShowState;

enum PeonyWindowOpenSlotFlags
{
    PEONY_WINDOW_OPEN_SLOT_NONE = 0,
    PEONY_WINDOW_OPEN_SLOT_APPEND = 1
};

typedef struct PeonyWindowDetails PeonyWindowDetails;

typedef struct
{
    GtkWindowClass parent_spot;

    PeonyWindowType window_type;
    const char *bookmarks_placeholder;

    /* Function pointers for overriding, without corresponding signals */

    char * (* get_title) (PeonyWindow *window);
    void   (* sync_title) (PeonyWindow *window,
                           PeonyWindowSlot *slot);
    PeonyIconInfo * (* get_icon) (PeonyWindow *window,
                                 PeonyWindowSlot *slot);

    void   (* sync_allow_stop) (PeonyWindow *window,
                                PeonyWindowSlot *slot);
    void   (* set_allow_up) (PeonyWindow *window, gboolean allow);
    void   (* reload)              (PeonyWindow *window);
    void   (* prompt_for_location) (PeonyWindow *window, const char *initial);
    void   (* get_min_size) (PeonyWindow *window, guint *default_width, guint *default_height);
    void   (* get_default_size) (PeonyWindow *window, guint *default_width, guint *default_height);
    void   (* close) (PeonyWindow *window);

    PeonyWindowSlot * (* open_slot) (PeonyWindowPane *pane,
                                    PeonyWindowOpenSlotFlags flags);
    void                 (* close_slot) (PeonyWindowPane *pane,
                                         PeonyWindowSlot *slot);
    void                 (* set_active_slot) (PeonyWindowPane *pane,
            PeonyWindowSlot *slot);

    /* Signals used only for keybindings */
    gboolean (* go_up) (PeonyWindow *window, gboolean close);
} PeonyWindowClass;

struct PeonyWindow
{
    GtkWindow parent_object;

    PeonyWindowDetails *details;

    PeonyApplication *application;
};

GType            peony_window_get_type             (void);
void             peony_window_show_window          (PeonyWindow    *window);
void             peony_window_close                (PeonyWindow    *window);

void             peony_window_connect_content_view (PeonyWindow    *window,
        PeonyView      *view);
void             peony_window_disconnect_content_view (PeonyWindow    *window,
        PeonyView      *view);

void             peony_window_go_to                (PeonyWindow    *window,
        GFile             *location);
void             peony_window_go_to_full           (PeonyWindow    *window,
        GFile             *location,
        PeonyWindowGoToCallback callback,
        gpointer           user_data);
void             peony_window_go_to_with_selection (PeonyWindow    *window,
        GFile             *location,
        GList             *new_selection);
void             peony_window_go_home              (PeonyWindow    *window);
void             peony_window_new_tab              (PeonyWindow    *window);
void             peony_window_go_up                (PeonyWindow    *window,
        gboolean           close_behind,
        gboolean           new_tab);
void             peony_window_prompt_for_location  (PeonyWindow    *window,
        const char        *initial);
void             peony_window_display_error        (PeonyWindow    *window,
        const char        *error_msg);
void		 peony_window_reload		      (PeonyWindow	 *window);

void             peony_window_allow_reload         (PeonyWindow    *window,
        gboolean           allow);
void             peony_window_allow_up             (PeonyWindow    *window,
        gboolean           allow);
void             peony_window_allow_stop           (PeonyWindow    *window,
        gboolean           allow);
GtkUIManager *   peony_window_get_ui_manager       (PeonyWindow    *window);

#endif
