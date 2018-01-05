/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* ukui-icon-container.h - Icon container widget.

   Copyright (C) 1999, 2000 Free Software Foundation
   Copyright (C) 2000 Eazel, Inc.
   copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.

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

   Authors: Ettore Perazzoli <ettore@gnu.org>, Darin Adler <darin@bentspoon.com>
*/

#ifndef PEONY_ICON_CONTAINER_H
#define PEONY_ICON_CONTAINER_H

#include <eel/eel-canvas.h>
#include <libpeony-private/peony-icon-info.h>

#define PEONY_TYPE_ICON_CONTAINER peony_icon_container_get_type()
#define PEONY_ICON_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_ICON_CONTAINER, PeonyIconContainer))
#define PEONY_ICON_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_ICON_CONTAINER, PeonyIconContainerClass))
#define PEONY_IS_ICON_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_ICON_CONTAINER))
#define PEONY_IS_ICON_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_ICON_CONTAINER))
#define PEONY_ICON_CONTAINER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_ICON_CONTAINER, PeonyIconContainerClass))


#define PEONY_ICON_CONTAINER_ICON_DATA(pointer) \
	((PeonyIconData *) (pointer))

typedef struct PeonyIconData PeonyIconData;

typedef void (* PeonyIconCallback) (PeonyIconData *icon_data,
                                   gpointer callback_data);

typedef struct
{
    int x;
    int y;
    double scale;
} PeonyIconPosition;

typedef enum
{
    PEONY_ICON_LAYOUT_L_R_T_B,
    PEONY_ICON_LAYOUT_R_L_T_B,
    PEONY_ICON_LAYOUT_T_B_L_R,
    PEONY_ICON_LAYOUT_T_B_R_L
} PeonyIconLayoutMode;

typedef enum
{
    PEONY_ICON_LABEL_POSITION_UNDER,
    PEONY_ICON_LABEL_POSITION_BESIDE
} PeonyIconLabelPosition;

#define	PEONY_ICON_CONTAINER_TYPESELECT_FLUSH_DELAY 1000000

typedef struct PeonyIconContainerDetails PeonyIconContainerDetails;

typedef struct
{
    EelCanvas canvas;
    PeonyIconContainerDetails *details;
    char* name;
    gboolean auto_layout_desktop;
} PeonyIconContainer;

typedef struct
{
    EelCanvasClass parent_slot;

    /* Operations on the container. */
    int          (* button_press) 	          (PeonyIconContainer *container,
            GdkEventButton *event);
    void          (* rename_button_press) 	          (PeonyIconContainer *container);
    void         (* context_click_background) (PeonyIconContainer *container,
            GdkEventButton *event);
    void         (* middle_click) 		  (PeonyIconContainer *container,
                                           GdkEventButton *event);

    /* Operations on icons. */
    void         (* activate)	  	  (PeonyIconContainer *container,
                                       PeonyIconData *data);
    void         (* activate_alternate)       (PeonyIconContainer *container,
            PeonyIconData *data);
    void         (* context_click_selection)  (PeonyIconContainer *container,
            GdkEventButton *event);
    void	     (* move_copy_items)	  (PeonyIconContainer *container,
                                           const GList *item_uris,
                                           GdkPoint *relative_item_points,
                                           const char *target_uri,
                                           GdkDragAction action,
                                           int x,
                                           int y);
    void	     (* handle_netscape_url)	  (PeonyIconContainer *container,
            const char *url,
            const char *target_uri,
            GdkDragAction action,
            int x,
            int y);
    void	     (* handle_uri_list)    	  (PeonyIconContainer *container,
            const char *uri_list,
            const char *target_uri,
            GdkDragAction action,
            int x,
            int y);
    void	     (* handle_text)		  (PeonyIconContainer *container,
                                           const char *text,
                                           const char *target_uri,
                                           GdkDragAction action,
                                           int x,
                                           int y);
    void	     (* handle_raw)		  (PeonyIconContainer *container,
                                       char *raw_data,
                                       int length,
                                       const char *target_uri,
                                       const char *direct_save_uri,
                                       GdkDragAction action,
                                       int x,
                                       int y);

    /* Queries on the container for subclass/client.
     * These must be implemented. The default "do nothing" is not good enough.
     */
    char *	     (* get_container_uri)	  (PeonyIconContainer *container);

    /* Queries on icons for subclass/client.
     * These must be implemented. The default "do nothing" is not
     * good enough, these are _not_ signals.
     */
    PeonyIconInfo *(* get_icon_images)     (PeonyIconContainer *container,
                                           PeonyIconData *data,
                                           int icon_size,
                                           GList **emblem_pixbufs,
                                           char **embedded_text,
                                           gboolean for_drag_accept,
                                           gboolean need_large_embeddded_text,
                                           gboolean *embedded_text_needs_loading,
                                           gboolean *has_window_open);
    void         (* get_icon_text)            (PeonyIconContainer *container,
            PeonyIconData *data,
            char **editable_text,
            char **additional_text,
            gboolean include_invisible);
    char *       (* get_icon_description)     (PeonyIconContainer *container,
            PeonyIconData *data);
    int          (* compare_icons)            (PeonyIconContainer *container,
            PeonyIconData *icon_a,
            PeonyIconData *icon_b);
    int          (* compare_icons_by_name)    (PeonyIconContainer *container,
            PeonyIconData *icon_a,
            PeonyIconData *icon_b);
    void         (* freeze_updates)           (PeonyIconContainer *container);
    void         (* unfreeze_updates)         (PeonyIconContainer *container);
    void         (* start_monitor_top_left)   (PeonyIconContainer *container,
            PeonyIconData *data,
            gconstpointer client,
            gboolean large_text);
    void         (* stop_monitor_top_left)    (PeonyIconContainer *container,
            PeonyIconData *data,
            gconstpointer client);
    void         (* prioritize_thumbnailing)  (PeonyIconContainer *container,
            PeonyIconData *data);

    /* Queries on icons for subclass/client.
     * These must be implemented => These are signals !
     * The default "do nothing" is not good enough.
     */
    gboolean     (* can_accept_item)	  (PeonyIconContainer *container,
                                           PeonyIconData *target,
                                           const char *item_uri);
    gboolean     (* get_stored_icon_position) (PeonyIconContainer *container,
            PeonyIconData *data,
            PeonyIconPosition *position);
    char *       (* get_icon_uri)             (PeonyIconContainer *container,
            PeonyIconData *data);
    char *       (* get_icon_drop_target_uri) (PeonyIconContainer *container,
            PeonyIconData *data);

    /* If icon data is NULL, the layout timestamp of the container should be retrieved.
     * That is the time when the container displayed a fully loaded directory with
     * all icon positions assigned.
     *
     * If icon data is not NULL, the position timestamp of the icon should be retrieved.
     * That is the time when the file (i.e. icon data payload) was last displayed in a
     * fully loaded directory with all icon positions assigned.
     */
    gboolean     (* get_stored_layout_timestamp) (PeonyIconContainer *container,
            PeonyIconData *data,
            time_t *time);
    /* If icon data is NULL, the layout timestamp of the container should be stored.
     * If icon data is not NULL, the position timestamp of the container should be stored.
     */
    gboolean     (* store_layout_timestamp) (PeonyIconContainer *container,
            PeonyIconData *data,
            const time_t *time);

    /* Notifications for the whole container. */
    void	     (* band_select_started)	  (PeonyIconContainer *container);
    void	     (* band_select_ended)	  (PeonyIconContainer *container);
    void         (* selection_changed) 	  (PeonyIconContainer *container);
    void         (* layout_changed)           (PeonyIconContainer *container);

    /* Notifications for icons. */
    void         (* icon_position_changed)    (PeonyIconContainer *container,
            PeonyIconData *data,
            const PeonyIconPosition *position);
    void         (* icon_text_changed)        (PeonyIconContainer *container,
            PeonyIconData *data,
            const char *text);
    void         (* renaming_icon)            (PeonyIconContainer *container,
            GtkWidget *renaming_widget);
    void	     (* icon_stretch_started)     (PeonyIconContainer *container,
            PeonyIconData *data);
    void	     (* icon_stretch_ended)       (PeonyIconContainer *container,
            PeonyIconData *data);
    int	     (* preview)		  (PeonyIconContainer *container,
                                   PeonyIconData *data,
                                   gboolean start_flag);
    void         (* icon_added)               (PeonyIconContainer *container,
            PeonyIconData *data);
    void         (* icon_removed)             (PeonyIconContainer *container,
            PeonyIconData *data);
    void         (* cleared)                  (PeonyIconContainer *container);
    void          (* size_changed)              (PeonyIconContainer *container);
    gboolean     (* start_interactive_search) (PeonyIconContainer *container);
} PeonyIconContainerClass;

/* GtkObject */
GType             peony_icon_container_get_type                      (void);
GtkWidget *       peony_icon_container_new                           (void);


/* adding, removing, and managing icons */
void              peony_icon_container_clear                         (PeonyIconContainer  *view);
gboolean          peony_icon_container_add                           (PeonyIconContainer  *view,
        PeonyIconData       *data);
void              peony_icon_container_layout_now                    (PeonyIconContainer *container);
gboolean          peony_icon_container_remove                        (PeonyIconContainer  *view,
        PeonyIconData       *data);
void              peony_icon_container_for_each                      (PeonyIconContainer  *view,
        PeonyIconCallback    callback,
        gpointer                callback_data);
void              peony_icon_container_request_update                (PeonyIconContainer  *view,
        PeonyIconData       *data);
void              peony_icon_container_request_update_all            (PeonyIconContainer  *container);
void              peony_icon_container_reveal                        (PeonyIconContainer  *container,
        PeonyIconData       *data);
gboolean          peony_icon_container_is_empty                      (PeonyIconContainer  *container);
PeonyIconData *peony_icon_container_get_first_visible_icon        (PeonyIconContainer  *container);
void              peony_icon_container_scroll_to_icon                (PeonyIconContainer  *container,
        PeonyIconData       *data);

void              peony_icon_container_begin_loading                 (PeonyIconContainer  *container);
void              peony_icon_container_end_loading                   (PeonyIconContainer  *container,
        gboolean                all_icons_added);

/* control the layout */
gboolean          peony_icon_container_is_auto_layout                (PeonyIconContainer  *container);
void              peony_icon_container_set_auto_layout               (PeonyIconContainer  *container,
        gboolean                auto_layout);
gboolean          peony_icon_container_is_tighter_layout             (PeonyIconContainer  *container);
void              peony_icon_container_set_tighter_layout            (PeonyIconContainer  *container,
        gboolean                tighter_layout);

gboolean          peony_icon_container_is_keep_aligned               (PeonyIconContainer  *container);
void              peony_icon_container_set_keep_aligned              (PeonyIconContainer  *container,
        gboolean                keep_aligned);
void              peony_icon_container_set_layout_mode               (PeonyIconContainer  *container,
        PeonyIconLayoutMode  mode);
void              peony_icon_container_set_label_position            (PeonyIconContainer  *container,
        PeonyIconLabelPosition pos);
void              peony_icon_container_sort                          (PeonyIconContainer  *container);
void              peony_icon_container_freeze_icon_positions         (PeonyIconContainer  *container);

int               peony_icon_container_get_max_layout_lines           (PeonyIconContainer  *container);
int               peony_icon_container_get_max_layout_lines_for_pango (PeonyIconContainer  *container);

void              peony_icon_container_set_highlighted_for_clipboard (PeonyIconContainer  *container,
        GList                  *clipboard_icon_data);

/* operations on all icons */
void              peony_icon_container_unselect_all                  (PeonyIconContainer  *view);
void              peony_icon_container_select_all                    (PeonyIconContainer  *view);


/* operations on the selection */
GList     *       peony_icon_container_get_selection                 (PeonyIconContainer  *view);
void			  peony_icon_container_invert_selection				(PeonyIconContainer  *view);
void              peony_icon_container_set_selection                 (PeonyIconContainer  *view,
        GList                  *selection);
GArray    *       peony_icon_container_get_selected_icon_locations   (PeonyIconContainer  *view);
gboolean          peony_icon_container_has_stretch_handles           (PeonyIconContainer  *container);
gboolean          peony_icon_container_is_stretched                  (PeonyIconContainer  *container);
void              peony_icon_container_show_stretch_handles          (PeonyIconContainer  *container);
void              peony_icon_container_unstretch                     (PeonyIconContainer  *container);
void              peony_icon_container_start_renaming_selected_item  (PeonyIconContainer  *container,
        gboolean                select_all);

/* options */
PeonyZoomLevel peony_icon_container_get_zoom_level                (PeonyIconContainer  *view);
void              peony_icon_container_set_zoom_level                (PeonyIconContainer *container, int new_level,gboolean bDesktopChange);
void              peony_icon_container_set_single_click_mode         (PeonyIconContainer  *container,
        gboolean                single_click_mode);
void              peony_icon_container_enable_linger_selection       (PeonyIconContainer  *view,
        gboolean                enable);
gboolean          peony_icon_container_get_is_fixed_size             (PeonyIconContainer  *container);
void              peony_icon_container_set_is_fixed_size             (PeonyIconContainer  *container,
        gboolean                is_fixed_size);
gboolean          peony_icon_container_get_is_desktop                (PeonyIconContainer  *container);
void              peony_icon_container_set_is_desktop                (PeonyIconContainer  *container,
        gboolean                is_desktop);
void              peony_icon_container_reset_scroll_region           (PeonyIconContainer  *container);
void              peony_icon_container_set_font                      (PeonyIconContainer  *container,
        const char             *font);
void              peony_icon_container_set_font_size_table           (PeonyIconContainer  *container,
        const int               font_size_table[PEONY_ZOOM_LEVEL_LARGEST + 1]);
void              peony_icon_container_set_margins                   (PeonyIconContainer  *container,
        int                     left_margin,
        int                     right_margin,
        int                     top_margin,
        int                     bottom_margin);
void              peony_icon_container_set_use_drop_shadows          (PeonyIconContainer  *container,
        gboolean                use_drop_shadows);
char*             peony_icon_container_get_icon_description          (PeonyIconContainer  *container,
        PeonyIconData       *data);
gboolean          peony_icon_container_get_allow_moves               (PeonyIconContainer  *container);
void              peony_icon_container_set_allow_moves               (PeonyIconContainer  *container,
        gboolean                allow_moves);
void		  peony_icon_container_set_forced_icon_size		(PeonyIconContainer  *container,
        int                     forced_icon_size);
void		  peony_icon_container_set_all_columns_same_width	(PeonyIconContainer  *container,
        gboolean                all_columns_same_width);

gboolean	  peony_icon_container_is_layout_rtl			(PeonyIconContainer  *container);
gboolean	  peony_icon_container_is_layout_vertical		(PeonyIconContainer  *container);

gboolean          peony_icon_container_get_store_layout_timestamps   (PeonyIconContainer  *container);
void              peony_icon_container_set_store_layout_timestamps   (PeonyIconContainer  *container,
        gboolean                store_layout);

void              peony_icon_container_widget_to_file_operation_position (PeonyIconContainer *container,
        GdkPoint              *position);


#define CANVAS_WIDTH(container,allocation) ((allocation.width	  \
				- container->details->left_margin \
				- container->details->right_margin) \
				/  EEL_CANVAS (container)->pixels_per_unit)

#define CANVAS_HEIGHT(container,allocation) ((allocation.height \
			 - container->details->top_margin \
			 - container->details->bottom_margin) \
			 / EEL_CANVAS (container)->pixels_per_unit)
typedef struct
{
	int x;
	int y;
	gboolean has_icon;
}UnitGrid;

typedef struct
{
	UnitGrid **icon_grid;
	UnitGrid *grid_memory;
	int num_rows;
	int num_columns;
	int iStartRows;
	int iStartColumns;
}DesktopGrid;

void peony_icon_container_set_zoom_position (PeonyIconContainer *container);

#endif /* PEONY_ICON_CONTAINER_H */
