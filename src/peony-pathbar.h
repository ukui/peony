/* peony-pathbar.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */

#ifndef PEONY_PATHBAR_H
#define PEONY_PATHBAR_H

#include <gtk/gtk.h>
#include <gio/gio.h>

typedef struct _PeonyPathBar      PeonyPathBar;
typedef struct _PeonyPathBarClass PeonyPathBarClass;


#define PEONY_TYPE_PATH_BAR                 (peony_path_bar_get_type ())
#define PEONY_PATH_BAR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_PATH_BAR, PeonyPathBar))
#define PEONY_PATH_BAR_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_PATH_BAR, PeonyPathBarClass))
#define PEONY_IS_PATH_BAR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_PATH_BAR))
#define PEONY_IS_PATH_BAR_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_PATH_BAR))
#define PEONY_PATH_BAR_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_PATH_BAR, PeonyPathBarClass))

struct _PeonyPathBar
{
    GtkContainer parent;

    GFile *root_path;
    GFile *home_path;
    GFile *desktop_path;

    GFile *current_path;
    gpointer current_button_data;

    GList *button_list;
    GList *first_scrolled_button;
    GList *fake_root;
    GtkWidget *up_slider_button;
    GtkWidget *down_slider_button;
    guint settings_signal_id;
    gint icon_size;
    gint16 slider_width;
    gint16 spacing;
    gint16 button_offset;
    guint timer;
    guint slider_visible : 1;
    guint need_timer : 1;
    guint ignore_click : 1;

    unsigned int drag_slider_timeout;
    gboolean drag_slider_timeout_for_up_button;
};

struct _PeonyPathBarClass
{
    GtkContainerClass parent_class;

    void (* path_clicked)   (PeonyPathBar  *path_bar,
                             GFile             *location);
    void (* path_set)       (PeonyPathBar  *path_bar,
                             GFile             *location);
};

GType    peony_path_bar_get_type (void) G_GNUC_CONST;

gboolean peony_path_bar_set_path    (PeonyPathBar *path_bar, GFile *file);
GFile *  peony_path_bar_get_path_for_button (PeonyPathBar *path_bar,
        GtkWidget       *button);
void     peony_path_bar_clear_buttons (PeonyPathBar *path_bar);

void     peony_path_bar_up              (PeonyPathBar *path_bar);
void     peony_path_bar_down            (PeonyPathBar *path_bar);

GtkWidget * peony_path_bar_get_button_from_button_list_entry (gpointer entry);

#endif /* PEONY_PATHBAR_H */
