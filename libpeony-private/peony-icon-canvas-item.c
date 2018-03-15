/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Peony - Icon canvas item class for icon container.
 *
 * Copyright (C) 2000 Eazel, Inc
 * Copyright (C) 2018, Tianjin KYLIN Information Technology Co., Ltd.
 * 
 * Author: Andy Hertzfeld <andy@eazel.com>
 * Modified by: liupeng <liupeng@kylinos.cn>
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
 */

#include <config.h>
#include <math.h>
#include "peony-icon-canvas-item.h"

#include <glib/gi18n.h>

#include <libpeony-private/peony-file.h>
#include "peony-file-utilities.h"
#include "peony-global-preferences.h"
#include "peony-icon-private.h"
#include <eel/eel-art-extensions.h>
#include <eel/eel-gdk-extensions.h>
#include <eel/eel-gdk-pixbuf-extensions.h>
#include <eel/eel-glib-extensions.h>
#include <eel/eel-graphic-effects.h>
#include <eel/eel-gtk-macros.h>
#include <eel/eel-string.h>
#include <eel/eel-accessibility.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib/gi18n.h>
#include <atk/atkimage.h>
#include <atk/atkcomponent.h>
#include <atk/atknoopobject.h>
#include <stdio.h>
#include <string.h>

#define EMBLEM_SPACING 2
#define MY_COMPUTER_EXTRA_RECT 12 

/* gap between bottom of icon and start of text box */
#define LABEL_OFFSET 1
#define LABEL_LINE_SPACING 0

//#define MAX_TEXT_WIDTH_STANDARD 135
#define MAX_TEXT_WIDTH_STANDARD 78
#define MAX_TEXT_WIDTH_TIGHTER 80
#define MAX_TEXT_WIDTH_BESIDE 225
#define MAX_TEXT_WIDTH_BESIDE_TOP_TO_BOTTOM 150

#define LAYOUT_HEIGHT 24

#define BURN "burn:///"
#define FTP "ftp://"
#define AFP "afp://"
#define SMB "smb://"
#define SFTP "sftp://"
#define HTTP "http://"
#define HTTPS "https://"


/* special text height handling
 * each item has three text height variables:
 *  + text_height: actual height of the displayed (i.e. on-screen) PangoLayout.
 *  + text_height_for_layout: height used in icon grid layout algorithms.
 *       		      “sane amount” of text.
 *   “sane amount“ as of
 *      + hard-coded to three lines in text-below-icon mode.
 *      + unlimited in text-besides-icon mode (see VOODOO-TODO)
 *
 *  This layout height is used by grid layout algorithms, even
 *  though the actually displayed and/or requested text size may be larger
 *  and overlap adjacent icons, if an icon is selected.
 *
 *  + text_height_for_entire_text: height needed to display the entire PangoLayout,
 *    if it wasn't ellipsized.
 */

/* Private part of the PeonyIconCanvasItem structure. */
struct PeonyIconCanvasItemDetails
{
    /* The image, text, font. */
    double x, y;
    GdkPixbuf *pixbuf;
    GdkPixbuf *rendered_pixbuf;
    GList *emblem_pixbufs;
    char *editable_text;		/* Text that can be modified by a renaming function */
    char *additional_text;		/* Text that cannot be modifed, such as file size, etc. */
    GdkPoint *attach_points;
    int n_attach_points;

    /* Size of the text at current font. */
    int text_dx;
    int text_width;

    /* actual size required for rendering the text to display */
    int text_height;
    /* actual size that would be required for rendering the entire text if it wasn't ellipsized */
    int text_height_for_entire_text;
    /* actual size needed for rendering a “sane amount” of text */
    int text_height_for_layout;

    int editable_text_height;

    /* whether the entire text must always be visible. In that case,
     * text_height_for_layout will always be equal to text_height.
     * Used for the last line of a line-wise icon layout. */
    guint entire_text : 1;

    /* preview state */
    guint is_active : 1;

    /* Highlight state. */
    guint is_highlighted_for_selection : 1;
    guint is_highlighted_as_keyboard_focus: 1;
    guint is_highlighted_for_drop : 1;
    guint is_highlighted_for_clipboard : 1;
    guint show_stretch_handles : 1;
    guint is_prelit : 1;

    guint rendered_is_active : 1;
    guint rendered_is_highlighted_for_selection : 1;
    guint rendered_is_highlighted_for_drop : 1;
    guint rendered_is_highlighted_for_clipboard : 1;
    guint rendered_is_prelit : 1;
    guint rendered_is_focused : 1;

    guint is_renaming : 1;

    guint bounds_cached : 1;

    guint is_visible : 1;

    GdkRectangle embedded_text_rect;
    char *embedded_text;

    /* Cached PangoLayouts. Only used if the icon is visible */
    PangoLayout *editable_text_layout;
    PangoLayout *additional_text_layout;
    PangoLayout *embedded_text_layout;

    /* Cached rectangle in canvas coordinates */
    EelIRect canvas_rect;
    EelIRect text_rect;
    EelIRect emblem_rect;

    EelIRect bounds_cache;
    EelIRect bounds_cache_for_layout;
    EelIRect bounds_cache_for_entire_item;

    GdkWindow *cursor_window;

    /* Accessibility bits */
    GailTextUtil *text_util;
};

/* Object argument IDs. */
enum
{
    PROP_0,
    PROP_EDITABLE_TEXT,
    PROP_ADDITIONAL_TEXT,
    PROP_HIGHLIGHTED_FOR_SELECTION,
    PROP_HIGHLIGHTED_AS_KEYBOARD_FOCUS,
    PROP_HIGHLIGHTED_FOR_DROP,
    PROP_HIGHLIGHTED_FOR_CLIPBOARD
};

typedef enum
{
    RIGHT_SIDE,
    BOTTOM_SIDE,
    LEFT_SIDE,
    TOP_SIDE
} RectangleSide;

typedef struct
{
    PeonyIconCanvasItem *icon_item;
    EelIRect icon_rect;
    RectangleSide side;
    int position;
    int index;
    GList *emblem;
} EmblemLayout;

static int click_policy_auto_value;

static void peony_icon_canvas_item_text_interface_init (EelAccessibleTextIface *iface);
static GType peony_icon_canvas_item_accessible_factory_get_type (void);

G_DEFINE_TYPE_WITH_CODE (PeonyIconCanvasItem, peony_icon_canvas_item, EEL_TYPE_CANVAS_ITEM,
                         G_IMPLEMENT_INTERFACE (EEL_TYPE_ACCESSIBLE_TEXT,
                                 peony_icon_canvas_item_text_interface_init));

/* private */
static void     draw_label_text                      (PeonyIconCanvasItem        *item,
    						      cairo_t                   *cr,
    						      gboolean                  create_mask,
    						      EelIRect                  icon_rect);
static void     measure_label_text                   (PeonyIconCanvasItem        *item);
static void     get_icon_canvas_rectangle            (PeonyIconCanvasItem        *item,
    						      EelIRect                  *rect);
static void     emblem_layout_reset                  (EmblemLayout              *layout,
    						      PeonyIconCanvasItem        *icon_item,
    						      EelIRect                  icon_rect,
    						      gboolean			is_rtl);
static gboolean emblem_layout_next                   (EmblemLayout              *layout,
    						      GdkPixbuf                 **emblem_pixbuf,
    						      EelIRect                  *emblem_rect,
    						      gboolean			is_rtl);
static void     draw_pixbuf                          (GdkPixbuf                 *pixbuf,
    						      cairo_t                   *cr,
    						      int                       x,
    						      int                       y);
static PangoLayout *get_label_layout                 (PangoLayout               **layout,
    						      PeonyIconCanvasItem        *item,
    						      const char                *text);

static gboolean hit_test_stretch_handle              (PeonyIconCanvasItem        *item,
    						      EelIRect                  canvas_rect,
    						      GtkCornerType *corner);
static void      draw_embedded_text                  (PeonyIconCanvasItem        *icon_item,
    						      cairo_t                   *cr,
    						      int                       x,
    						      int                       y);

static void       peony_icon_canvas_item_ensure_bounds_up_to_date (PeonyIconCanvasItem *icon_item);


/* Object initialization function for the icon item. */
static void
peony_icon_canvas_item_init (PeonyIconCanvasItem *icon_item)
{
    static gboolean setup_auto_enums = FALSE;

    if (!setup_auto_enums)
    {
        eel_g_settings_add_auto_enum
             (peony_preferences,
             PEONY_PREFERENCES_CLICK_POLICY,
             &click_policy_auto_value);
        setup_auto_enums = TRUE;
    }

    icon_item->details = G_TYPE_INSTANCE_GET_PRIVATE ((icon_item), PEONY_TYPE_ICON_CANVAS_ITEM, PeonyIconCanvasItemDetails);
    peony_icon_canvas_item_invalidate_label_size (icon_item);
}

gint
get_disk_full (PeonyIconCanvasItem *item)
{
	GList list;
	goffset block_num;
	char *uri,*device;
	GDrive *gdrive;
	guint64 freedisk,totaldisk;
	gint track_num ;
	int size;
    	gint df_percent;
	float fraction;
	BraseroDeviceHandle *handle;
	BraseroScsiTrackInfo track_info;
	BraseroScsiResult result;

	PeonyIcon *icon = item->user_data;
	list.data = icon->data;
	PeonyFile *file = PEONY_FILE(list.data);
	char *name = peony_file_get_name(file);
	char *text_string = _("available space,total");
	
#define disk_additional_text_max_len 255
#define disk_show_tb 1000
	
	char *sum = (char*)malloc(disk_additional_text_max_len * sizeof(char));

	if(strcmp(name,"root.link")!=0)
	{
		GMount *mount =peony_file_get_mount(file);
		if(!mount)
		{
			g_free (name);
			return NULL;
		}
		GFile *root = g_mount_get_default_location (mount);
	  
		if(root )
		{
			uri = g_file_get_uri(root);
			if(g_str_has_prefix(uri,SMB)||
			g_str_has_prefix(uri,AFP)||
			g_str_has_prefix(uri,HTTP)||
			g_str_has_prefix(uri,HTTPS)||
			g_str_has_prefix(uri,SFTP)||
			g_str_has_prefix(uri,FTP))
			{
				g_free(uri);
				g_object_unref(mount);
				return NULL;
			}
	  		if(g_str_has_prefix(uri,BURN))
	  		{
				gdrive = g_mount_get_drive (mount);
				if(gdrive)
					device = g_drive_get_identifier (gdrive, G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
				else
				{
					g_object_unref(mount); 
					return  NULL;
				}
				if(device)
				{
					handle = brasero_device_handle_open(device,FALSE);
					track_num =1;
					size =36;
					brasero_mmc1_read_track_info(handle,track_num,& track_info,& size);
                                                        if(&track_info !=NULL)
                                                            block_num = BRASERO_GET_32(track_info.free_blocks);
                                                        else
                                                            block_num = 0;
					freedisk = block_num*2048;
					totaldisk=block_num*2048;
                                                        if(handle!=NULL)
					brasero_device_handle_close( handle);
				}
				if(gdrive)	g_object_unref(gdrive);
				if(!device) return NULL;
				g_free(device);
			}
	 		char *path = g_file_get_path(root);	  
	 		if(!g_file_test(path,G_FILE_TEST_EXISTS))
	 		{
	 			if(path)	g_free(path);
				g_object_unref(mount);
	 			return NULL;
	 		}
	 		else{
				freedisk = interface_get_disk_free_full (g_file_new_for_uri (uri));
				totaldisk = interface_get_disk_total_full(g_file_new_for_uri (uri));
				if(!interface_get_disk_free_full (root)&&!interface_get_disk_total_full(root))
				{					
					gdrive = g_mount_get_drive (mount);
					device = g_drive_get_identifier (gdrive,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
					goffset used_space;
					used_space = process_one_device (device);
					BraseroScsiDiscInfoStd *info = NULL;
					handle = brasero_device_handle_open(device,FALSE);
					result = brasero_mmc1_read_disc_information_std (handle,
																&info,
																&size);
                                                        if (info!=NULL)
					{
					    track_num = BRASERO_FIRST_TRACK_IN_LAST_SESSION (info);
					    brasero_mmc1_read_track_info(handle,track_num,& track_info,& size);
                                                            block_num = BRASERO_GET_32(track_info.free_blocks);
                                                        }
                                                        else
                                                            block_num =0;
					freedisk = block_num*2048;
					totaldisk = used_space+block_num*2048;
                                                        if(handle!=NULL)
					brasero_device_handle_close( handle);
					g_object_unref(gdrive);
					g_free(device);
                                                        if(info!=NULL)
					g_free(info);
				}				
			 }
			g_free(path);
		}	
		 g_object_unref (mount);
	}
	else
	{
		uri = "file:///";
		freedisk = interface_get_disk_free_full (g_file_new_for_uri (uri));
		totaldisk = interface_get_disk_total_full(g_file_new_for_uri (uri));
	}
	g_free (name);
            fraction = ((float) (totaldisk-freedisk) / (float) totaldisk) * 100.0;
            df_percent = (gint)rintf(fraction);
	return (df_percent > -1 && df_percent < 101) ? df_percent : 0;
}
guint64
interface_get_disk_free_full(GFile * file)
{
	GFileInfo *info = g_file_query_filesystem_info (file,
											"filesystem::*",
											NULL,
											NULL);
	return g_file_info_get_attribute_uint64 (info, G_FILE_ATTRIBUTE_FILESYSTEM_FREE);
}
 
guint64
interface_get_disk_total_full(GFile * file)
{

	GFileInfo *info = g_file_query_filesystem_info (file,
											"filesystem::*",
											NULL,
											NULL);
	return g_file_info_get_attribute_uint64 (info, G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
 }

static void
peony_icon_canvas_item_finalize (GObject *object)
{
    PeonyIconCanvasItemDetails *details;

    g_assert (PEONY_IS_ICON_CANVAS_ITEM (object));

    details = PEONY_ICON_CANVAS_ITEM (object)->details;

    if (details->cursor_window != NULL)
    {
        gdk_window_set_cursor (details->cursor_window, NULL);
        g_object_unref (details->cursor_window);
    }

    if (details->pixbuf != NULL)
    {
        g_object_unref (details->pixbuf);
    }

    if (details->text_util != NULL)
    {
        g_object_unref (details->text_util);
    }

    g_list_free_full (details->emblem_pixbufs, g_object_unref);
    g_free (details->editable_text);
    g_free (details->additional_text);
    g_free (details->attach_points);

    if (details->rendered_pixbuf != NULL)
    {
        g_object_unref (details->rendered_pixbuf);
    }

    if (details->editable_text_layout != NULL)
    {
        g_object_unref (details->editable_text_layout);
    }

    if (details->additional_text_layout != NULL)
    {
        g_object_unref (details->additional_text_layout);
    }

    if (details->embedded_text_layout != NULL)
    {
        g_object_unref (details->embedded_text_layout);
    }

    g_free (details->embedded_text);

    G_OBJECT_CLASS (peony_icon_canvas_item_parent_class)->finalize (object);
}
static char*
get_disk_additional_text(PeonyIconCanvasItem *item)
{
	GList list;
	goffset block_num;
	char *uri,*device;
	GDrive *gdrive;
	gchar* freedisk=NULL,*totaldisk=NULL;
	gint track_num ;
	int size;
	BraseroDeviceHandle *handle;
	BraseroScsiTrackInfo track_info;
	BraseroScsiResult result;

	PeonyIcon *icon = item->user_data;
	list.data = icon->data;
	PeonyFile *file = PEONY_FILE(list.data);
	char *name = peony_file_get_name(file);
	char *text_string = _("available space,total");
	
	#define disk_additional_text_max_len 255
	#define disk_show_tb 1000
	
	char *sum = (char*)malloc(disk_additional_text_max_len * sizeof(char));

	if(strcmp(name,"root.link")!=0)
	{
		GMount *mount =peony_file_get_mount(file);
		if(!mount)
		{
			g_free (name);
			return NULL;
		}
		GFile *root = g_mount_get_default_location (mount);
	  
		if(root )
		{
			uri = g_file_get_uri(root);
			if(g_str_has_prefix(uri,SMB)||
			g_str_has_prefix(uri,AFP)||
			g_str_has_prefix(uri,HTTP)||
			g_str_has_prefix(uri,HTTPS)||
			g_str_has_prefix(uri,SFTP)||
			g_str_has_prefix(uri,FTP))
			{
				g_free(uri);
				g_object_unref(mount);
				return NULL;
			}
	  		if(g_str_has_prefix(uri,BURN))
	  		{
				gdrive = g_mount_get_drive (mount);
				if(gdrive)
					device = g_drive_get_identifier (gdrive, G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
				else
				{
					g_object_unref(mount); 
					return  NULL;
				}
				if(device)
				{
					handle = brasero_device_handle_open(device,FALSE);
					track_num =1;
					size =36;
					brasero_mmc1_read_track_info(handle,track_num,& track_info,& size);
                                                        if(&track_info !=NULL)
                                                            block_num = BRASERO_GET_32(track_info.free_blocks);
                                                        else
                                                            block_num = 0;
					#if 0
					freedisk = g_format_size(block_num*2048);
					totaldisk=g_format_size(block_num*2048);
					#else
					if (g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_USE_IEC_UNITS))
					{
						freedisk = g_format_size_full(block_num*2048,G_FORMAT_SIZE_IEC_UNITS);
						totaldisk=g_format_size_full(block_num*2048,G_FORMAT_SIZE_IEC_UNITS);
					}
					else
					{
						freedisk = g_format_size(block_num*2048);
						totaldisk=g_format_size(block_num*2048);
					}
					#endif
                                                        if(handle!=NULL)
					brasero_device_handle_close( handle);
				}
				if(gdrive)	g_object_unref(gdrive);
				if(!device) return NULL;
				g_free(device);
			}
	 		char *path = g_file_get_path(root);	  
	 		if(!g_file_test(path,G_FILE_TEST_EXISTS))
	 		{
	 			if(path)	g_free(path);
				g_object_unref(mount);
	 			return NULL;
	 		}
	 		else{
				#if 0
				freedisk = g_format_size(interface_get_disk_free_full (g_file_new_for_uri (uri)));
				totaldisk = g_format_size(interface_get_disk_total_full(g_file_new_for_uri (uri)));
				#else
				if (g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_USE_IEC_UNITS))
				{
					freedisk = g_format_size_full(interface_get_disk_free_full (g_file_new_for_uri (uri)),G_FORMAT_SIZE_IEC_UNITS);
					totaldisk=g_format_size_full(interface_get_disk_total_full (g_file_new_for_uri (uri)),G_FORMAT_SIZE_IEC_UNITS);
				}
				else
				{
					freedisk = g_format_size(interface_get_disk_free_full (g_file_new_for_uri (uri)));
					totaldisk = g_format_size(interface_get_disk_total_full(g_file_new_for_uri (uri)));
				}
				#endif
				if(!interface_get_disk_free_full (root)&&!interface_get_disk_total_full(root))
				{					
					gdrive = g_mount_get_drive (mount);
					device = g_drive_get_identifier (gdrive,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
					goffset used_space;
					used_space = process_one_device (device);
					BraseroScsiDiscInfoStd *info = NULL;
					handle = brasero_device_handle_open(device,FALSE);
					result = brasero_mmc1_read_disc_information_std (handle,
																&info,
																&size);
                                                        if (info!=NULL)
					{
					    track_num = BRASERO_FIRST_TRACK_IN_LAST_SESSION (info);
					    brasero_mmc1_read_track_info(handle,track_num,& track_info,& size);
                                                            block_num = BRASERO_GET_32(track_info.free_blocks);
                                                        }
                                                        else
                                                            block_num =0;
					#if 0
					freedisk = g_format_size(block_num*2048);
					totaldisk = g_format_size(used_space+block_num*2048);
					#else
					if (g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_USE_IEC_UNITS))
					{
						freedisk = g_format_size_full(block_num*2048,G_FORMAT_SIZE_IEC_UNITS);
						totaldisk=g_format_size_full(used_space+block_num*2048,G_FORMAT_SIZE_IEC_UNITS);
					}
					else
					{
						freedisk = g_format_size(block_num*2048);
						totaldisk = g_format_size(used_space+block_num*2048);
					}
					#endif
                                                        if(handle!=NULL)
					brasero_device_handle_close( handle);
					g_object_unref(gdrive);
					g_free(device);
                                                        if(info!=NULL)
					g_free(info);
				}				
			 }
			g_free(path);
			freedisk = (char*)realloc(freedisk,strlen(freedisk)+strlen(text_string)+strlen(totaldisk)+1);
			strcat (freedisk,text_string);	
			sum = g_strdup(strcat(freedisk,totaldisk));
		}	
		 g_object_unref (mount);
	}
	else
	{
		uri = "file:///";
		#if 0
		freedisk = g_format_size(interface_get_disk_free_full (g_file_new_for_uri (uri)));
		totaldisk = g_format_size(interface_get_disk_total_full(g_file_new_for_uri (uri)));
		#else
		if (g_settings_get_boolean (peony_preferences, PEONY_PREFERENCES_USE_IEC_UNITS))
		{
			freedisk = g_format_size_full(interface_get_disk_free_full (g_file_new_for_uri (uri)),G_FORMAT_SIZE_IEC_UNITS);
			totaldisk=g_format_size_full(interface_get_disk_total_full(g_file_new_for_uri (uri)),G_FORMAT_SIZE_IEC_UNITS);
		}
		else
		{
			freedisk = g_format_size(interface_get_disk_free_full (g_file_new_for_uri (uri)));
			totaldisk = g_format_size(interface_get_disk_total_full(g_file_new_for_uri (uri)));
		}
		#endif
		freedisk = (char*)realloc(freedisk,strlen(freedisk)+strlen(text_string)+strlen(totaldisk)+1);
		strcat (freedisk,text_string);	
		sum = g_strdup(strcat(freedisk,totaldisk));
	}
	g_free (name);
	if(freedisk)	g_free (freedisk);
	if(totaldisk)	g_free (totaldisk);
	return sum;
}


/* Currently we require pixbufs in this format (for hit testing).
 * Perhaps gdk-pixbuf will be changed so it can do the hit testing
 * and we won't have this requirement any more.
 */
static gboolean
pixbuf_is_acceptable (GdkPixbuf *pixbuf)
{
    return gdk_pixbuf_get_colorspace (pixbuf) == GDK_COLORSPACE_RGB
           && ((!gdk_pixbuf_get_has_alpha (pixbuf)
                && gdk_pixbuf_get_n_channels (pixbuf) == 3)
               || (gdk_pixbuf_get_has_alpha (pixbuf)
                   && gdk_pixbuf_get_n_channels (pixbuf) == 4))
           && gdk_pixbuf_get_bits_per_sample (pixbuf) == 8;
}

static void
peony_icon_canvas_item_invalidate_bounds_cache (PeonyIconCanvasItem *item)
{
    item->details->bounds_cached = FALSE;
}

/* invalidate the text width and height cached in the item details. */
void
peony_icon_canvas_item_invalidate_label_size (PeonyIconCanvasItem *item)
{
    if (item->details->editable_text_layout != NULL)
    {
        pango_layout_context_changed (item->details->editable_text_layout);
    }
    if (item->details->additional_text_layout != NULL)
    {
        pango_layout_context_changed (item->details->additional_text_layout);
    }
    if (item->details->embedded_text_layout != NULL)
    {
        pango_layout_context_changed (item->details->embedded_text_layout);
    }
    peony_icon_canvas_item_invalidate_bounds_cache (item);
    item->details->text_width = -1;
    item->details->text_height = -1;
    item->details->text_height_for_layout = -1;
    item->details->text_height_for_entire_text = -1;
    item->details->editable_text_height = -1;
}

/* Set property handler for the icon item. */
static void
peony_icon_canvas_item_set_property (GObject        *object,
                                    guint           property_id,
                                    const GValue   *value,
                                    GParamSpec     *pspec)
{
    PeonyIconCanvasItem *item;
    PeonyIconCanvasItemDetails *details;
    AtkObject *accessible;

    item = PEONY_ICON_CANVAS_ITEM (object);
    details = item->details;
    accessible = atk_gobject_accessible_for_object (G_OBJECT (item));

    switch (property_id)
    {

    case PROP_EDITABLE_TEXT:
        if (g_strcmp0 (details->editable_text,
                        g_value_get_string (value)) == 0)
        {
            return;
        }

        g_free (details->editable_text);
        details->editable_text = g_strdup (g_value_get_string (value));
        if (details->text_util)
        {
            gail_text_util_text_setup (details->text_util,
                                       details->editable_text);
            g_object_notify (G_OBJECT(accessible), "accessible-name");
        }

        peony_icon_canvas_item_invalidate_label_size (item);
        if (details->editable_text_layout)
        {
            g_object_unref (details->editable_text_layout);
            details->editable_text_layout = NULL;
        }
        break;

    case PROP_ADDITIONAL_TEXT:
	if(PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas)->name)
	{	 
			details->additional_text = get_disk_additional_text(item); 
			peony_icon_canvas_item_invalidate_label_size (item);
			if (details->additional_text_layout)
			{
				g_object_unref (details->additional_text_layout);
				details->additional_text_layout = NULL;
			}
			break;
	}
	else		
        if (g_strcmp0 (details->additional_text,
                        g_value_get_string (value)) == 0)
        {
            return;
        }

        g_free (details->additional_text);
        details->additional_text = g_strdup (g_value_get_string (value));

        peony_icon_canvas_item_invalidate_label_size (item);
        if (details->additional_text_layout)
        {
            g_object_unref (details->additional_text_layout);
            details->additional_text_layout = NULL;
        }
        break;

    case PROP_HIGHLIGHTED_FOR_SELECTION:
        if (!details->is_highlighted_for_selection == !g_value_get_boolean (value))
        {
            return;
        }
        details->is_highlighted_for_selection = g_value_get_boolean (value);
        peony_icon_canvas_item_invalidate_label_size (item);

        atk_object_notify_state_change (accessible, ATK_STATE_SELECTED,
                                        details->is_highlighted_for_selection);
        break;

    case PROP_HIGHLIGHTED_AS_KEYBOARD_FOCUS:
        if (!details->is_highlighted_as_keyboard_focus == !g_value_get_boolean (value))
        {
            return;
        }
        details->is_highlighted_as_keyboard_focus = g_value_get_boolean (value);
        atk_object_notify_state_change (accessible, ATK_STATE_FOCUSED,
                                        details->is_highlighted_as_keyboard_focus);
        break;

    case PROP_HIGHLIGHTED_FOR_DROP:
        if (!details->is_highlighted_for_drop == !g_value_get_boolean (value))
        {
            return;
        }
        details->is_highlighted_for_drop = g_value_get_boolean (value);
        break;

    case PROP_HIGHLIGHTED_FOR_CLIPBOARD:
        if (!details->is_highlighted_for_clipboard == !g_value_get_boolean (value))
        {
            return;
        }
        details->is_highlighted_for_clipboard = g_value_get_boolean (value);
        break;

    default:
        g_warning ("peony_icons_view_item_item_set_arg on unknown argument");
        return;
    }

    eel_canvas_item_request_update (EEL_CANVAS_ITEM (object));
}

/* Get property handler for the icon item */
static void
peony_icon_canvas_item_get_property (GObject        *object,
                                    guint           property_id,
                                    GValue         *value,
                                    GParamSpec     *pspec)
{
    PeonyIconCanvasItemDetails *details;

    details = PEONY_ICON_CANVAS_ITEM (object)->details;

    switch (property_id)
    {

    case PROP_EDITABLE_TEXT:
        g_value_set_string (value, details->editable_text);
        break;

    case PROP_ADDITIONAL_TEXT:
        g_value_set_string (value, details->additional_text);
        break;

    case PROP_HIGHLIGHTED_FOR_SELECTION:
        g_value_set_boolean (value, details->is_highlighted_for_selection);
        break;

    case PROP_HIGHLIGHTED_AS_KEYBOARD_FOCUS:
        g_value_set_boolean (value, details->is_highlighted_as_keyboard_focus);
        break;

    case PROP_HIGHLIGHTED_FOR_DROP:
        g_value_set_boolean (value, details->is_highlighted_for_drop);
        break;

    case PROP_HIGHLIGHTED_FOR_CLIPBOARD:
        g_value_set_boolean (value, details->is_highlighted_for_clipboard);
        break;

    default:
        g_warning ("invalid property %d", property_id);
        break;
    }
}

cairo_surface_t *
peony_icon_canvas_item_get_drag_surface (PeonyIconCanvasItem *item)
{
    cairo_surface_t *surface;

    EelCanvas *canvas;
    GdkScreen *screen;
    int width, height;
    int item_offset_x, item_offset_y;
    EelIRect icon_rect;
    EelIRect emblem_rect;
    GdkPixbuf *emblem_pixbuf;
    EmblemLayout emblem_layout;
    double item_x, item_y;
    gboolean is_rtl;
    cairo_t *cr;
    GtkStyleContext *context;

    g_return_val_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item), NULL);

    canvas = EEL_CANVAS_ITEM (item)->canvas;
    screen = gtk_widget_get_screen (GTK_WIDGET (canvas));
    context = gtk_widget_get_style_context (GTK_WIDGET (canvas));

    gtk_style_context_save (context);
    gtk_style_context_add_class (context, "peony-canvas-item");

    /* Assume we're updated so canvas item data is right */

    /* Calculate the offset from the top-left corner of the
       new image to the item position (where the pixmap is placed) */
    eel_canvas_world_to_window (canvas,
                                item->details->x, item->details->y,
                                &item_x, &item_y);

    item_offset_x = item_x - EEL_CANVAS_ITEM (item)->x1;
    item_offset_y = item_y - EEL_CANVAS_ITEM (item)->y1;

    /* Calculate the width of the item */
    width = EEL_CANVAS_ITEM (item)->x2 - EEL_CANVAS_ITEM (item)->x1;
    height = EEL_CANVAS_ITEM (item)->y2 - EEL_CANVAS_ITEM (item)->y1;

    surface = gdk_window_create_similar_surface (gdk_screen_get_root_window (screen),
    						 CAIRO_CONTENT_COLOR_ALPHA,
    						 width, height);

    cr = cairo_create (surface);

    gtk_render_icon (context, cr, item->details->pixbuf,
                     item_offset_x, item_offset_y);


    icon_rect.x0 = item_offset_x;
    icon_rect.y0 = item_offset_y;
    icon_rect.x1 = item_offset_x + gdk_pixbuf_get_width (item->details->pixbuf);
    icon_rect.y1 = item_offset_y + gdk_pixbuf_get_height (item->details->pixbuf);

    is_rtl = peony_icon_container_is_layout_rtl (PEONY_ICON_CONTAINER (canvas));

    emblem_layout_reset (&emblem_layout, item, icon_rect, is_rtl);

    while (emblem_layout_next (&emblem_layout, &emblem_pixbuf, &emblem_rect, is_rtl))
    {
        gdk_cairo_set_source_pixbuf (cr, emblem_pixbuf, emblem_rect.x0, emblem_rect.y0);
        cairo_rectangle (cr, emblem_rect.x0, emblem_rect.y0,
                         gdk_pixbuf_get_width (emblem_pixbuf),
                         gdk_pixbuf_get_height (emblem_pixbuf));
        cairo_fill (cr);
    }

    draw_embedded_text (item, cr,
    			item_offset_x, item_offset_y);
    draw_label_text (item, cr, FALSE, icon_rect);
    cairo_destroy (cr);

    gtk_style_context_restore (context);

    return surface;

}

void
peony_icon_canvas_item_set_image (PeonyIconCanvasItem *item,
                                 GdkPixbuf *image)
{
    PeonyIconCanvasItemDetails *details;

    g_return_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item));
    g_return_if_fail (image == NULL || pixbuf_is_acceptable (image));

    details = item->details;
    if (details->pixbuf == image)
    {
        return;
    }

    if (image != NULL)
    {
        g_object_ref (image);
    }
    if (details->pixbuf != NULL)
    {
        g_object_unref (details->pixbuf);
    }
    if (details->rendered_pixbuf != NULL)
    {
        g_object_unref (details->rendered_pixbuf);
        details->rendered_pixbuf = NULL;
    }

    details->pixbuf = image;

    peony_icon_canvas_item_invalidate_bounds_cache (item);
    eel_canvas_item_request_update (EEL_CANVAS_ITEM (item));
}

void
peony_icon_canvas_item_set_emblems (PeonyIconCanvasItem *item,
                                   GList *emblem_pixbufs)
{
    GList *p;

    g_return_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item));

    g_assert (item->details->emblem_pixbufs != emblem_pixbufs || emblem_pixbufs == NULL);

    /* The case where the emblems are identical is fairly common,
     * so lets take the time to check for it.
     */
    if (eel_g_list_equal (item->details->emblem_pixbufs, emblem_pixbufs))
    {
        return;
    }

    /* Check if they are acceptable. */
    for (p = emblem_pixbufs; p != NULL; p = p->next)
    {
        g_return_if_fail (pixbuf_is_acceptable (p->data));
    }

    /* Take in the new list of emblems. */
    eel_g_object_list_ref (emblem_pixbufs);
    g_list_free_full (item->details->emblem_pixbufs, g_object_unref);
    item->details->emblem_pixbufs = g_list_copy (emblem_pixbufs);

    peony_icon_canvas_item_invalidate_bounds_cache (item);
    eel_canvas_item_request_update (EEL_CANVAS_ITEM (item));
}

void
peony_icon_canvas_item_set_attach_points (PeonyIconCanvasItem *item,
        GdkPoint *attach_points,
        int n_attach_points)
{
    g_free (item->details->attach_points);
    item->details->attach_points = NULL;
    item->details->n_attach_points = 0;

    if (attach_points != NULL && n_attach_points != 0)
    {
        item->details->attach_points = g_memdup (attach_points, n_attach_points * sizeof (GdkPoint));
        item->details->n_attach_points = n_attach_points;
    }

    peony_icon_canvas_item_invalidate_bounds_cache (item);
}

void
peony_icon_canvas_item_set_embedded_text_rect (PeonyIconCanvasItem       *item,
        const GdkRectangle           *text_rect)
{
    item->details->embedded_text_rect = *text_rect;

    peony_icon_canvas_item_invalidate_bounds_cache (item);
    eel_canvas_item_request_update (EEL_CANVAS_ITEM (item));
}

void
peony_icon_canvas_item_set_embedded_text (PeonyIconCanvasItem       *item,
        const char                   *text)
{
    g_free (item->details->embedded_text);
    item->details->embedded_text = g_strdup (text);

    if (item->details->embedded_text_layout != NULL)
    {
        if (text != NULL)
        {
            pango_layout_set_text (item->details->embedded_text_layout, text, -1);
        }
        else
        {
            pango_layout_set_text (item->details->embedded_text_layout, "", -1);
        }
    }

    eel_canvas_item_request_update (EEL_CANVAS_ITEM (item));
}


/* Recomputes the bounding box of a icon canvas item.
 * This is a generic implementation that could be used for any canvas item
 * class, it has no assumptions about how the item is used.
 */
static void
recompute_bounding_box (PeonyIconCanvasItem *icon_item,
                        double i2w_dx, double i2w_dy)
{
    /* The bounds stored in the item is the same as what get_bounds
     * returns, except it's in canvas coordinates instead of the item's
     * parent's coordinates.
     */

    EelCanvasItem *item;
    EelDPoint top_left, bottom_right;

    item = EEL_CANVAS_ITEM (icon_item);

    eel_canvas_item_get_bounds (item,
                                &top_left.x, &top_left.y,
                                &bottom_right.x, &bottom_right.y);

    top_left.x += i2w_dx;
    top_left.y += i2w_dy;
    bottom_right.x += i2w_dx;
    bottom_right.y += i2w_dy;
    eel_canvas_w2c_d (item->canvas,
                      top_left.x, top_left.y,
                      &item->x1, &item->y1);
    eel_canvas_w2c_d (item->canvas,
                      bottom_right.x, bottom_right.y,
                      &item->x2, &item->y2);
}

static EelIRect
compute_text_rectangle (const PeonyIconCanvasItem *item,
                        EelIRect icon_rectangle,
                        gboolean canvas_coords,
                        PeonyIconCanvasItemBoundsUsage usage)
{
    EelIRect text_rectangle;
    double pixels_per_unit;
    double text_width, text_height, text_height_for_layout, text_height_for_entire_text, real_text_height, text_dx;

    pixels_per_unit = EEL_CANVAS_ITEM (item)->canvas->pixels_per_unit;
    if (canvas_coords)
    {
        text_width = item->details->text_width;
        text_height = item->details->text_height;
        text_height_for_layout = item->details->text_height_for_layout;
        text_height_for_entire_text = item->details->text_height_for_entire_text;
        text_dx = item->details->text_dx;
    }
    else
    {
        text_width = item->details->text_width / pixels_per_unit;
        text_height = item->details->text_height / pixels_per_unit;
        text_height_for_layout = item->details->text_height_for_layout / pixels_per_unit;
        text_height_for_entire_text = item->details->text_height_for_entire_text / pixels_per_unit;
        text_dx = item->details->text_dx / pixels_per_unit;
    }

    if (PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas)->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
    {
        if (!peony_icon_container_is_layout_rtl (PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas)))
        {
            text_rectangle.x0 = icon_rectangle.x1;
            text_rectangle.x1 = text_rectangle.x0 + text_dx + text_width;
        }
        else
        {
            text_rectangle.x1 = icon_rectangle.x0;
            text_rectangle.x0 = text_rectangle.x1 - text_dx - text_width;
        }

        /* VOODOO-TODO */
#if 0
        if (for_layout)
        {
            /* in this case, we should be more smart and calculate the size according to the maximum
             * number of lines fitting next to the icon. However, this requires a more complex layout logic.
             * It would mean that when measuring the label, the icon dimensions must be known already,
             * and we
             *   1. start with an unlimited layout
             *   2. measure how many lines of this layout fit next to the icon
             *   3. limit the number of lines to the given number of fitting lines
             */
            real_text_height = VOODOO();
        }
        else
        {
#endif
            real_text_height = text_height_for_entire_text;
#if 0
        }
#endif

        text_rectangle.y0 = (icon_rectangle.y0 + icon_rectangle.y1) / 2- (int) real_text_height / 2;
        text_rectangle.y1 = text_rectangle.y0 + real_text_height;
    }
    else
    {
        text_rectangle.x0 = (icon_rectangle.x0 + icon_rectangle.x1) / 2 - (int) text_width / 2;
        text_rectangle.y0 = icon_rectangle.y1;
        text_rectangle.x1 = text_rectangle.x0 + text_width;

        if (usage == BOUNDS_USAGE_FOR_LAYOUT)
        {
            real_text_height = text_height_for_layout;
        }
        else if (usage == BOUNDS_USAGE_FOR_ENTIRE_ITEM)
        {
            real_text_height = text_height_for_entire_text;
        }
        else if (usage == BOUNDS_USAGE_FOR_DISPLAY)
        {
            real_text_height = text_height;
        }
        else
        {
            g_assert_not_reached ();
        }

        text_rectangle.y1 = text_rectangle.y0 + real_text_height + LABEL_OFFSET / pixels_per_unit;
    }

    return text_rectangle;
}

EelIRect
get_compute_text_rectangle (const PeonyIconCanvasItem *item,
						gboolean canvas_coords,
						PeonyIconCanvasItemBoundsUsage usage)
{
	return compute_text_rectangle (item, item->details->canvas_rect, TRUE, BOUNDS_USAGE_FOR_DISPLAY);
}

double
get_pixels_per_unit (const PeonyIconCanvasItem *item)
{
	return EEL_CANVAS_ITEM (item)->canvas->pixels_per_unit;;
}

static EelIRect
get_current_canvas_bounds (EelCanvasItem *item)
{
    EelIRect bounds;

    g_assert (EEL_IS_CANVAS_ITEM (item));

    bounds.x0 = item->x1;
    bounds.y0 = item->y1;
    bounds.x1 = item->x2;
    bounds.y1 = item->y2;

    return bounds;
}

void
peony_icon_canvas_item_update_bounds (PeonyIconCanvasItem *item,
                                     double i2w_dx, double i2w_dy)
{
    EelIRect before, after, emblem_rect;
    EmblemLayout emblem_layout;
    EelCanvasItem *canvas_item;
    GdkPixbuf *emblem_pixbuf;
    gboolean is_rtl;

    canvas_item = EEL_CANVAS_ITEM (item);

    /* Update canvas and text rect cache */
    get_icon_canvas_rectangle (item, &item->details->canvas_rect);
    item->details->text_rect = compute_text_rectangle (item, item->details->canvas_rect,
                               TRUE, BOUNDS_USAGE_FOR_DISPLAY);
    /* Compute new bounds. */
    before = get_current_canvas_bounds (canvas_item);
    recompute_bounding_box (item, i2w_dx, i2w_dy);
    after = get_current_canvas_bounds (canvas_item);

    /* If the bounds didn't change, we are done. */
    if (eel_irect_equal (before, after))
    {
        return;
    }

    is_rtl = peony_icon_container_is_layout_rtl (PEONY_ICON_CONTAINER (canvas_item->canvas));


    /* Update emblem rect cache */
    item->details->emblem_rect.x0 = 0;
    item->details->emblem_rect.x1 = 0;
    item->details->emblem_rect.y0 = 0;
    item->details->emblem_rect.y1 = 0;
    emblem_layout_reset (&emblem_layout, item, item->details->canvas_rect, is_rtl);
    while (emblem_layout_next (&emblem_layout, &emblem_pixbuf, &emblem_rect, is_rtl))
    {
        eel_irect_union (&item->details->emblem_rect, &item->details->emblem_rect, &emblem_rect);
    }

    /* queue a redraw. */
    eel_canvas_request_redraw (canvas_item->canvas,
                               before.x0, before.y0,
                               before.x1 + 1, before.y1 + 1);
}

/* Update handler for the icon canvas item. */
static void
peony_icon_canvas_item_update (EelCanvasItem *item,
                              double i2w_dx, double i2w_dy,
                              gint flags)
{
    peony_icon_canvas_item_update_bounds (PEONY_ICON_CANVAS_ITEM (item), i2w_dx, i2w_dy);

    eel_canvas_item_request_redraw (EEL_CANVAS_ITEM (item));

    EEL_CANVAS_ITEM_CLASS (peony_icon_canvas_item_parent_class)->update (item, i2w_dx, i2w_dy, flags);
}

/* Rendering */
static gboolean
in_single_click_mode (void)
{
    return click_policy_auto_value == PEONY_CLICK_POLICY_SINGLE;
}


/* Keep these for a bit while we work on performance of draw_or_measure_label_text. */
/*
  #define PERFORMANCE_TEST_DRAW_DISABLE
  #define PERFORMANCE_TEST_MEASURE_DISABLE
*/

/* This gets the size of the layout from the position of the layout.
 * This means that if the layout is right aligned we get the full width
 * of the layout, not just the width of the text snippet on the right side
 */
static void
layout_get_full_size (PangoLayout *layout,
                      int         *width,
                      int         *height,
                      int         *dx)
{
    PangoRectangle logical_rect;
    int the_width, total_width;

    pango_layout_get_extents (layout, NULL, &logical_rect);
    the_width = (logical_rect.width + PANGO_SCALE / 2) / PANGO_SCALE;
    total_width = (logical_rect.x + logical_rect.width + PANGO_SCALE / 2) / PANGO_SCALE;

    if (width != NULL)
    {
        *width = the_width;
    }

    if (height != NULL)
    {
        *height = (logical_rect.height + PANGO_SCALE / 2) / PANGO_SCALE;
    }

    if (dx != NULL)
    {
        *dx = total_width - the_width;
    }
}

static void
layout_get_size_for_layout (PangoLayout *layout,
                            int          max_layout_line_count,
                            int          height_for_entire_text,
                            int         *height_for_layout)
{
    PangoLayoutIter *iter;
    PangoRectangle logical_rect;
    int i;

    /* only use the first max_layout_line_count lines for the gridded auto layout */
    if (pango_layout_get_line_count (layout) <= max_layout_line_count)
    {
        *height_for_layout = height_for_entire_text;
    }
    else
    {
        *height_for_layout = 0;
        iter = pango_layout_get_iter (layout);
        /* VOODOO-TODO, determine number of lines based on the icon size for text besides icon.
         * cf. compute_text_rectangle() */
        for (i = 0; i < max_layout_line_count; i++)
        {
            pango_layout_iter_get_line_extents (iter, NULL, &logical_rect);
            *height_for_layout += (logical_rect.height + PANGO_SCALE / 2) / PANGO_SCALE;

            if (!pango_layout_iter_next_line (iter))
            {
                break;
            }

            *height_for_layout += pango_layout_get_spacing (layout);
        }
        pango_layout_iter_free (iter);
    }
}

#define IS_COMPACT_VIEW(container) \
        ((container->details->layout_mode == PEONY_ICON_LAYOUT_T_B_L_R || \
	  container->details->layout_mode == PEONY_ICON_LAYOUT_T_B_R_L) && \
	 container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)

#define TEXT_BACK_PADDING_X 4
#define TEXT_BACK_PADDING_Y 1
#define disk_full 165

static void
prepare_pango_layout_width (PeonyIconCanvasItem *item,
                            PangoLayout *layout)
{
    if (peony_icon_canvas_item_get_max_text_width (item) < 0)
    {
        pango_layout_set_width (layout, -1);
    }
    else
    {
        pango_layout_set_width (layout, floor (peony_icon_canvas_item_get_max_text_width (item)) * PANGO_SCALE);
        pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
    }
}

static void
prepare_pango_layout_for_measure_entire_text (PeonyIconCanvasItem *item,
        PangoLayout *layout)
{
    PeonyIconContainer *container;

    prepare_pango_layout_width (item, layout);

    container = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas);

    if (IS_COMPACT_VIEW (container))
    {
        pango_layout_set_height (layout, -1);
    }
    else
    {
		pango_layout_set_height (layout, LAYOUT_HEIGHT);
    }
}

static void
prepare_pango_layout_for_draw (PeonyIconCanvasItem *item,
                               PangoLayout *layout)
{
    PeonyIconCanvasItemDetails *details;
    PeonyIconContainer *container;
    gboolean needs_highlight;

    prepare_pango_layout_width (item, layout);

    container = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas);
    details = item->details;

    needs_highlight = details->is_highlighted_for_selection || details->is_highlighted_for_drop;

    if (IS_COMPACT_VIEW (container))
    {
        pango_layout_set_height (layout, -1);
    }
    else if (needs_highlight ||
             details->is_prelit ||
             details->is_highlighted_as_keyboard_focus ||
             details->entire_text ||
             container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
    {
        /* VOODOO-TODO, cf. compute_text_rectangle() */
        pango_layout_set_height (layout, G_MININT);
    }
    else
    {
        /* TODO? we might save some resources, when the re-layout is not neccessary in case
         * the layout height already fits into max. layout lines. But pango should figure this
         * out itself (which it doesn't ATM).
         */
        pango_layout_set_height (layout,
                                 peony_icon_container_get_max_layout_lines_for_pango (container));
	}
	if(container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
	{
		pango_layout_set_height (layout, -1);
    }
}

static void
measure_label_text (PeonyIconCanvasItem *item)
{
    PeonyIconCanvasItemDetails *details;
    PeonyIconContainer *container;
    gint editable_height, editable_height_for_layout, editable_height_for_entire_text, editable_width, editable_dx;
    gint additional_height, additional_width, additional_dx;
    PangoLayout *editable_layout;
    PangoLayout *additional_layout;
    gboolean have_editable, have_additional;

    /* check to see if the cached values are still valid; if so, there's
     * no work necessary
     */

    if (item->details->text_width >= 0 && item->details->text_height >= 0)
    {
        return;
    }

    details = item->details;

    have_editable = details->editable_text != NULL && details->editable_text[0] != '\0';
    have_additional = details->additional_text != NULL && details->additional_text[0] != '\0';

    /* No font or no text, then do no work. */
    if (!have_editable && !have_additional)
    {
        details->text_height = 0;
        details->text_height_for_layout = 0;
        details->text_height_for_entire_text = 0;
        details->text_width = 0;
        return;
    }

#ifdef PERFORMANCE_TEST_MEASURE_DISABLE
    /* fake out the width */
    details->text_width = 80;
    details->text_height = 20;
    details->text_height_for_layout = 20;
    details->text_height_for_entire_text = 20;
    return;
#endif

    editable_width = 0;
    editable_height = 0;
    editable_height_for_layout = 0;
    editable_height_for_entire_text = 0;
    editable_dx = 0;
    additional_width = 0;
    additional_height = 0;
    additional_dx = 0;

    container = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas);
    editable_layout = NULL;
    additional_layout = NULL;

    if (have_editable)
    {
        /* first, measure required text height: editable_height_for_entire_text
         * then, measure text height applicable for layout: editable_height_for_layout
         * next, measure actually displayed height: editable_height
         */
        editable_layout = get_label_layout (&details->editable_text_layout, item, details->editable_text);

        prepare_pango_layout_for_measure_entire_text (item, editable_layout);
        layout_get_full_size (editable_layout,
                              NULL,
                              &editable_height_for_entire_text,
                              NULL);
        layout_get_size_for_layout (editable_layout,
                                    peony_icon_container_get_max_layout_lines (container),
                                    editable_height_for_entire_text,
                                    &editable_height_for_layout);

        prepare_pango_layout_for_draw (item, editable_layout);
        layout_get_full_size (editable_layout,
                              &editable_width,
                              &editable_height,
                              &editable_dx);
    }

    if (have_additional)
    {
        additional_layout = get_label_layout (&details->additional_text_layout, item, details->additional_text);
        prepare_pango_layout_for_draw (item, additional_layout);
        layout_get_full_size (additional_layout,
                              &additional_width, &additional_height, &additional_dx);
    }

    details->editable_text_height = editable_height;

    if (editable_width > additional_width)
    {
		 if(container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
		 {
			details->text_width = (editable_width<disk_full)?disk_full:editable_width;
		 }
		 else
		 {
			details->text_width = editable_width;
		 }
		details->text_dx = editable_dx+10;
    }
    else
    {
		 if(container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
		 {
			details->text_width = (additional_width<disk_full)?disk_full:additional_width;
		 }
		 else
		 {
			details->text_width = additional_width;
		   }
		details->text_dx = additional_dx+10;
    }

    if (have_additional)
    {
        details->text_height = editable_height + LABEL_LINE_SPACING + additional_height;
        details->text_height_for_layout = editable_height_for_layout + LABEL_LINE_SPACING + additional_height;
		details->text_height_for_entire_text = editable_height_for_entire_text + LABEL_LINE_SPACING + additional_height+MY_COMPUTER_EXTRA_RECT;
    }
    else
    {
        details->text_height = editable_height;
        details->text_height_for_layout = editable_height_for_layout;
        details->text_height_for_entire_text = editable_height_for_entire_text;
    }

    /* add some extra space for highlighting even when we don't highlight so things won't move */

    /* extra slop for nicer highlighting */
    details->text_height += TEXT_BACK_PADDING_Y*2;
    details->text_height_for_layout += TEXT_BACK_PADDING_Y*2;
    details->text_height_for_entire_text += TEXT_BACK_PADDING_Y*2;
    details->editable_text_height += TEXT_BACK_PADDING_Y*2;

    /* extra to make it look nicer */
    details->text_width += TEXT_BACK_PADDING_X*2;

    if (editable_layout)
    {
        g_object_unref (editable_layout);
    }

    if (additional_layout)
    {
        g_object_unref (additional_layout);
    }
}

static void
draw_label_text (PeonyIconCanvasItem *item,
                 cairo_t *cr,
                 gboolean create_mask,
                 EelIRect icon_rect)
{
    PeonyIconCanvasItemDetails *details;
    PeonyIconContainer *container;
    PangoLayout *editable_layout;
    PangoLayout *additional_layout;
    GtkStyleContext *context;
    GtkStateFlags state, base_state;
    gboolean have_editable, have_additional;
    gboolean needs_highlight, prelight_label, is_rtl_label_beside;
    EelIRect text_rect;
    int x;
    int max_text_width;
    gdouble frame_w, frame_h, frame_x, frame_y;
    gboolean draw_frame = TRUE;
	gboolean is_beside;
	int cr_line_width = 0;

#ifdef PERFORMANCE_TEST_DRAW_DISABLE
    return;
#endif

    details = item->details;

    measure_label_text (item);
    if (details->text_height == 0 ||
            details->text_width == 0)
    {
        return;
    }

    container = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas);
    context = gtk_widget_get_style_context (GTK_WIDGET (container));

    text_rect = compute_text_rectangle (item, icon_rect, TRUE, BOUNDS_USAGE_FOR_DISPLAY);

    needs_highlight = details->is_highlighted_for_selection || details->is_highlighted_for_drop;
    is_rtl_label_beside = peony_icon_container_is_layout_rtl (container) &&
                          container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE;
	if(container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
		is_beside = TRUE;
	else
		is_beside = FALSE;

    editable_layout = NULL;
    additional_layout = NULL;

    have_editable = details->editable_text != NULL && details->editable_text[0] != '\0';
    have_additional = details->additional_text != NULL && details->additional_text[0] != '\0';
    g_assert (have_editable || have_additional);

    max_text_width = floor (peony_icon_canvas_item_get_max_text_width (item));

    base_state = gtk_widget_get_state_flags (GTK_WIDGET (container));
    base_state &= ~(GTK_STATE_FLAG_SELECTED | GTK_STATE_FLAG_PRELIGHT);
    state = base_state;

    gtk_widget_style_get (GTK_WIDGET (container),
                          "activate_prelight_icon_label", &prelight_label,
                          NULL);

    /* if the icon is highlighted, do some set-up */
    if (needs_highlight &&
        !details->is_renaming) {
			draw_frame = FALSE;
    } else if (!needs_highlight && have_editable &&
               details->text_width > 0 && details->text_height > 0 &&
               prelight_label && item->details->is_prelit) {
            state |= GTK_STATE_FLAG_PRELIGHT;

            frame_x = text_rect.x0;
            frame_y = text_rect.y0;
            frame_w = text_rect.x1 - text_rect.x0;
            frame_h = text_rect.y1 - text_rect.y0;
    } else {
            draw_frame = FALSE;
    }

    if (draw_frame) {
            gtk_style_context_save (context);
            gtk_style_context_set_state (context, state);

            gtk_render_frame (context, cr,
                              frame_x, frame_y,
                              frame_w, frame_h);
            gtk_render_background (context, cr,
                                   frame_x, frame_y,
                                   frame_w, frame_h);

            gtk_style_context_restore (context);
    }

    if (container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
    {
		x = text_rect.x0 + 10;
    }
    else
    {
        x = text_rect.x0 + ((text_rect.x1 - text_rect.x0) - max_text_width) / 2;
    }

    if (have_editable &&
        !details->is_renaming)
    {
        state = base_state;

        if (prelight_label && item->details->is_prelit) {
                state |= GTK_STATE_FLAG_PRELIGHT;
        }

        if (needs_highlight) {
                state |= GTK_STATE_FLAG_SELECTED;
        }

        editable_layout = get_label_layout (&item->details->editable_text_layout, item, item->details->editable_text);
        prepare_pango_layout_for_draw (item, editable_layout);

        gtk_style_context_save (context);
        gtk_style_context_set_state (context, state);

        gtk_render_layout (context, cr,
                           x, text_rect.y0 + TEXT_BACK_PADDING_Y,
                           editable_layout);

        gtk_style_context_restore (context);
    }
	if (container->name)
	{
		GList list;
		char *uri;
		PeonyIcon *icon = item->user_data;
		list.data = icon->data;
		PeonyFile *file = PEONY_FILE(list.data);
		char *name = peony_file_get_name(file);
		if(strcmp(name,"root.link")!=0)
		{
			GMount *mount= peony_file_get_mount(file);
			if(mount)
			{
				GFile *root = g_mount_get_default_location (mount);	
				uri = g_file_get_uri(root);
				double full = get_disk_full (item);
				double wid = full/100*disk_full;
				if (full > 0)
				{
					cr_line_width = 12;
					cairo_set_line_width(cr,12);
					cairo_set_source_rgb(cr,0.19,0.58,0.95);
					cairo_move_to(cr,x,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);
					cairo_line_to(cr,x+wid,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);
					cairo_stroke(cr);
					cairo_set_source_rgb(cr,0.937,0.941,0.945);
					cairo_move_to(cr,x+wid,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);
					cairo_line_to(cr,x+disk_full,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);
					cairo_stroke(cr);
					cairo_set_line_width(cr,0.1);
					cairo_set_source_rgb(cr,0.866,0.866,0.866);
					cairo_rectangle(cr,x,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height-1,180,12);
					cairo_stroke(cr);
				}
				g_object_unref(mount);
			}
	}	
	else
	{
		uri = "file:///";
		GFile *diskfile = g_file_new_for_uri (uri);
		double full = get_disk_full (item);
		double wid = full/100*disk_full;
		if (full > 0)
		{
			cr_line_width = 12;
			cairo_set_line_width(cr,12);
			cairo_set_source_rgb(cr,0.19,0.58,0.95);
			cairo_move_to(cr,x,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);
			cairo_line_to(cr,x+wid,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);

			cairo_stroke(cr);
			cairo_set_source_rgb(cr,0.937,0.941,0.945);
			cairo_move_to(cr,x+wid,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);
			cairo_line_to(cr,x+disk_full,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height+5);
			cairo_stroke(cr);
			cairo_set_line_width(cr,0.1);
			cairo_set_source_rgb(cr,0.866,0.866,0.866);
			cairo_rectangle(cr,x,text_rect.y0 + TEXT_BACK_PADDING_Y+details->editable_text_height-1,180,12);
			cairo_stroke(cr);
		}
		g_object_unref(diskfile);
	}
}
    if (have_additional &&
        !details->is_renaming)
    {
        state = base_state;

        if (needs_highlight) {
                state |= GTK_STATE_FLAG_SELECTED;
        }

        additional_layout = get_label_layout (&item->details->additional_text_layout, item, item->details->additional_text);
        prepare_pango_layout_for_draw (item, additional_layout);

        gtk_style_context_save (context);
        gtk_style_context_set_state (context, state);
        gtk_style_context_add_class (context, "dim-label");

        gtk_render_layout (context, cr,
                           x, text_rect.y0 + details->editable_text_height + LABEL_LINE_SPACING + TEXT_BACK_PADDING_Y+cr_line_width,
                           additional_layout);

        gtk_style_context_restore (context);
    }

    if (!create_mask && item->details->is_highlighted_as_keyboard_focus)
    {
        if (needs_highlight) {
                state = GTK_STATE_FLAG_SELECTED;
        }

        gtk_style_context_save (context);
        gtk_style_context_set_state (context, state);

        gtk_render_focus (context,
                          cr,
                         text_rect.x0,
                         text_rect.y0,
                         text_rect.x1 - text_rect.x0,
                         text_rect.y1 - text_rect.y0);

        gtk_style_context_restore (context);
    }

    if (editable_layout != NULL)
    {
        g_object_unref (editable_layout);
    }

    if (additional_layout != NULL)
    {
        g_object_unref (additional_layout);
    }
}

void
peony_icon_canvas_item_set_is_visible (PeonyIconCanvasItem       *item,
                                      gboolean                      visible)
{
    if (item->details->is_visible == visible)
        return;

    item->details->is_visible = visible;

    if (!visible)
    {
        peony_icon_canvas_item_invalidate_label (item);
    }
}

void
peony_icon_canvas_item_invalidate_label (PeonyIconCanvasItem     *item)
{
    peony_icon_canvas_item_invalidate_label_size (item);

    if (item->details->editable_text_layout)
    {
        g_object_unref (item->details->editable_text_layout);
        item->details->editable_text_layout = NULL;
    }

    if (item->details->additional_text_layout)
    {
        g_object_unref (item->details->additional_text_layout);
        item->details->additional_text_layout = NULL;
    }

    if (item->details->embedded_text_layout)
    {
        g_object_unref (item->details->embedded_text_layout);
        item->details->embedded_text_layout = NULL;
    }
}


static GdkPixbuf *
get_knob_pixbuf (void)
{
    GdkPixbuf *knob_pixbuf;
    char *knob_filename;

    knob_pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                            "stock-peony-knob",
                                            8, 0, NULL);
    if (!knob_pixbuf)
    {
        knob_filename = peony_pixmap_file ("ukui-knob.png");
        knob_pixbuf = gdk_pixbuf_new_from_file (knob_filename, NULL);
        g_free (knob_filename);
    }

    return knob_pixbuf;
}

static void
draw_stretch_handles (PeonyIconCanvasItem *item,
                      cairo_t *cr,
                      const EelIRect *rect)
{
    GtkWidget *widget;
    GdkPixbuf *knob_pixbuf;
    int knob_width, knob_height;
    double dash = { 2.0 };
    GtkStyleContext *style;
    GdkRGBA color;

    if (!item->details->show_stretch_handles)
    {
        return;
    }

    widget = GTK_WIDGET (EEL_CANVAS_ITEM (item)->canvas);
    style = gtk_widget_get_style_context (widget);

    cairo_save (cr);

    knob_pixbuf = get_knob_pixbuf ();
    knob_width = gdk_pixbuf_get_width (knob_pixbuf);
    knob_height = gdk_pixbuf_get_height (knob_pixbuf);

    /* first draw the box */
    gtk_style_context_get_color (style, GTK_STATE_FLAG_SELECTED, &color);
    gdk_cairo_set_source_rgba (cr, &color);

    cairo_set_dash (cr, &dash, 1, 0);
    cairo_set_line_width (cr, 1.0);
    cairo_rectangle (cr,
             rect->x0 + 0.5,
             rect->y0 + 0.5,
             rect->x1 - rect->x0 - 1,
             rect->y1 - rect->y0 - 1);
    cairo_stroke (cr);

    cairo_restore (cr);

    /* draw the stretch handles themselves */
    draw_pixbuf (knob_pixbuf, cr, rect->x0, rect->y0);
    draw_pixbuf (knob_pixbuf, cr, rect->x0, rect->y1 - knob_height);
    draw_pixbuf (knob_pixbuf, cr, rect->x1 - knob_width, rect->y0);
    draw_pixbuf (knob_pixbuf, cr, rect->x1 - knob_width, rect->y1 - knob_height);

    g_object_unref (knob_pixbuf);
}

static void
emblem_layout_reset (EmblemLayout *layout, PeonyIconCanvasItem *icon_item, EelIRect icon_rect, gboolean is_rtl)
{
    layout->icon_item = icon_item;
    layout->icon_rect = icon_rect;
    layout->side = is_rtl ? LEFT_SIDE : RIGHT_SIDE;
    layout->position = 0;
    layout->index = 0;
    layout->emblem = icon_item->details->emblem_pixbufs;
}

static gboolean
emblem_layout_next (EmblemLayout *layout,
                    GdkPixbuf **emblem_pixbuf,
                    EelIRect *emblem_rect,
                    gboolean is_rtl)
{
    GdkPixbuf *pixbuf;
    int width, height, x, y;
    GdkPoint *attach_points;

    /* Check if we have layed out all of the pixbufs. */
    if (layout->emblem == NULL)
    {
        return FALSE;
    }

    /* Get the pixbuf. */
    pixbuf = layout->emblem->data;
    width = gdk_pixbuf_get_width (pixbuf);
    height = gdk_pixbuf_get_height (pixbuf);


    /* Advance to the next emblem. */
    layout->emblem = layout->emblem->next;

    attach_points = layout->icon_item->details->attach_points;
    if (attach_points != NULL)
    {
        if (layout->index >= layout->icon_item->details->n_attach_points)
        {
            return FALSE;
        }

        x = layout->icon_rect.x0 + attach_points[layout->index].x;
        y = layout->icon_rect.y0 + attach_points[layout->index].y;

        layout->index += 1;

        /* Return the rectangle and pixbuf. */
        *emblem_pixbuf = pixbuf;
        emblem_rect->x0 = x - width / 2;
        emblem_rect->y0 = y - height / 2;
        emblem_rect->x1 = emblem_rect->x0 + width;
        emblem_rect->y1 = emblem_rect->y0 + height;

        return TRUE;

    }

    for (;;)
    {

        /* Find the side to lay out along. */
        switch (layout->side)
        {
        case RIGHT_SIDE:
            x = layout->icon_rect.x1;
            y = is_rtl ? layout->icon_rect.y1 : layout->icon_rect.y0;
            break;
        case BOTTOM_SIDE:
            x = is_rtl ? layout->icon_rect.x0 : layout->icon_rect.x1;
            y = layout->icon_rect.y1;
            break;
        case LEFT_SIDE:
            x = layout->icon_rect.x0;
            y = is_rtl ? layout->icon_rect.y0 : layout->icon_rect.y1;
            break;
        case TOP_SIDE:
            x = is_rtl ? layout->icon_rect.x1 : layout->icon_rect.x0;
            y = layout->icon_rect.y0;
            break;
        default:
            g_assert_not_reached ();
            x = 0;
            y = 0;
            break;
        }
        if (layout->position != 0)
        {
            switch (layout->side)
            {
            case RIGHT_SIDE:
                y += (is_rtl ? -1 : 1) * (layout->position + height / 2);
                break;
            case BOTTOM_SIDE:
                x += (is_rtl ? 1 : -1 ) * (layout->position + width / 2);
                break;
            case LEFT_SIDE:
                y += (is_rtl ? 1 : -1) * (layout->position + height / 2);
                break;
            case TOP_SIDE:
                x += (is_rtl ? -1 : 1) * (layout->position + width / 2);
                break;
            }
        }

        /* Check to see if emblem fits in current side. */
        if (x >= layout->icon_rect.x0 && x <= layout->icon_rect.x1
                && y >= layout->icon_rect.y0 && y <= layout->icon_rect.y1)
        {

            /* It fits. */

            /* Advance along the side. */
            switch (layout->side)
            {
            case RIGHT_SIDE:
            case LEFT_SIDE:
                layout->position += height + EMBLEM_SPACING;
                break;
            case BOTTOM_SIDE:
            case TOP_SIDE:
                layout->position += width + EMBLEM_SPACING;
                break;
            }

            /* Return the rectangle and pixbuf. */
            *emblem_pixbuf = pixbuf;
            emblem_rect->x0 = x - width / 2;
            emblem_rect->y0 = y - height / 2;
            emblem_rect->x1 = emblem_rect->x0 + width;
            emblem_rect->y1 = emblem_rect->y0 + height;

            return TRUE;
        }

        /* It doesn't fit, so move to the next side. */
        switch (layout->side)
        {
        case RIGHT_SIDE:
            layout->side = is_rtl ? TOP_SIDE : BOTTOM_SIDE;
            break;
        case BOTTOM_SIDE:
            layout->side = is_rtl ? RIGHT_SIDE : LEFT_SIDE;
            break;
        case LEFT_SIDE:
            layout->side = is_rtl ? BOTTOM_SIDE : TOP_SIDE;
            break;
        case TOP_SIDE:
        default:
            return FALSE;
        }
        layout->position = 0;
    }
}

static void
draw_pixbuf (GdkPixbuf *pixbuf,
             cairo_t *cr,
             int x, int y)
{
    cairo_save (cr);
    gdk_cairo_set_source_pixbuf (cr, pixbuf, x, y);
    cairo_paint (cr);
    cairo_restore (cr);
}

/* shared code to highlight or dim the passed-in pixbuf */
static GdkPixbuf *
real_map_pixbuf (PeonyIconCanvasItem *icon_item)
{
    EelCanvas *canvas;
    char *audio_filename;
    PeonyIconContainer *container;
    GdkPixbuf *temp_pixbuf, *old_pixbuf, *audio_pixbuf;
    int emblem_size;
    GtkStyleContext *style;
    GdkRGBA color;

    temp_pixbuf = icon_item->details->pixbuf;
    canvas = EEL_CANVAS_ITEM(icon_item)->canvas;
    container = PEONY_ICON_CONTAINER (canvas);

    g_object_ref (temp_pixbuf);

    if (icon_item->details->is_prelit ||
            icon_item->details->is_highlighted_for_clipboard)
    {
        old_pixbuf = temp_pixbuf;

        temp_pixbuf = eel_create_spotlight_pixbuf (temp_pixbuf);
        g_object_unref (old_pixbuf);

        /* FIXME bugzilla.gnome.org 42471: This hard-wired image is inappropriate to
         * this level of code, which shouldn't know that the
         * preview is audio, nor should it have an icon
         * hard-wired in.
         */

        /* if the icon is currently being previewed, superimpose an image to indicate that */
        /* audio is the only kind of previewing right now, so this code isn't as general as it could be */
        if (icon_item->details->is_active)
        {
            emblem_size = peony_icon_get_emblem_size_for_icon_size (gdk_pixbuf_get_width (temp_pixbuf));
            /* Load the audio symbol. */
            audio_filename = peony_pixmap_file ("ukui-audio.svg");
            if (audio_filename != NULL)
            {
                audio_pixbuf = gdk_pixbuf_new_from_file_at_scale (audio_filename,
                               emblem_size, emblem_size,
                               TRUE,
                               NULL);
            }
            else
            {
                audio_pixbuf = NULL;
            }

            /* Composite it onto the icon. */
            if (audio_pixbuf != NULL)
            {
                gdk_pixbuf_composite
                (audio_pixbuf,
                 temp_pixbuf,
                 0, 0,
                 gdk_pixbuf_get_width (audio_pixbuf),
                 gdk_pixbuf_get_height (audio_pixbuf),
                 0, 0,
                 1.0, 1.0,
                 GDK_INTERP_BILINEAR, 0xFF);

                g_object_unref (audio_pixbuf);
            }

            g_free (audio_filename);
        }
    }

    return temp_pixbuf;
}

static GdkPixbuf *
map_pixbuf (PeonyIconCanvasItem *icon_item)
{
    if (!(icon_item->details->rendered_pixbuf != NULL
            && icon_item->details->rendered_is_active == icon_item->details->is_active
            && icon_item->details->rendered_is_prelit == icon_item->details->is_prelit
            && icon_item->details->rendered_is_highlighted_for_selection == icon_item->details->is_highlighted_for_selection
            && icon_item->details->rendered_is_highlighted_for_drop == icon_item->details->is_highlighted_for_drop
            && icon_item->details->rendered_is_highlighted_for_clipboard == icon_item->details->is_highlighted_for_clipboard
            && (icon_item->details->is_highlighted_for_selection && icon_item->details->rendered_is_focused == gtk_widget_has_focus (GTK_WIDGET (EEL_CANVAS_ITEM (icon_item)->canvas)))))
    {
        if (icon_item->details->rendered_pixbuf != NULL)
        {
            g_object_unref (icon_item->details->rendered_pixbuf);
        }
        icon_item->details->rendered_pixbuf = real_map_pixbuf (icon_item);
        icon_item->details->rendered_is_active = icon_item->details->is_active;
        icon_item->details->rendered_is_prelit = icon_item->details->is_prelit;
        icon_item->details->rendered_is_highlighted_for_selection = icon_item->details->is_highlighted_for_selection;
        icon_item->details->rendered_is_highlighted_for_drop = icon_item->details->is_highlighted_for_drop;
        icon_item->details->rendered_is_highlighted_for_clipboard = icon_item->details->is_highlighted_for_clipboard;
        icon_item->details->rendered_is_focused = gtk_widget_has_focus (GTK_WIDGET (EEL_CANVAS_ITEM (icon_item)->canvas));
    }

    g_object_ref (icon_item->details->rendered_pixbuf);

    return icon_item->details->rendered_pixbuf;
}

static void
draw_embedded_text (PeonyIconCanvasItem *item,
                    cairo_t *cr,
                    int x, int y)
{
    PangoLayout *layout;
    PangoContext *context;
    PangoFontDescription *desc;
    GtkWidget *widget;
    GtkStyleContext *style_context;

    if (item->details->embedded_text == NULL ||
            item->details->embedded_text_rect.width == 0 ||
            item->details->embedded_text_rect.height == 0)
    {
        return;
    }

    widget = GTK_WIDGET (EEL_CANVAS_ITEM (item)->canvas);

    if (item->details->embedded_text_layout != NULL)
    {
        layout = g_object_ref (item->details->embedded_text_layout);
    }
    else
    {
        context = gtk_widget_get_pango_context (widget);
        layout = pango_layout_new (context);
        pango_layout_set_text (layout, item->details->embedded_text, -1);

        desc = pango_font_description_from_string ("monospace 6");
        pango_layout_set_font_description (layout, desc);
        pango_font_description_free (desc);

        if (item->details->is_visible)
        {
            item->details->embedded_text_layout = g_object_ref (layout);
        }
    }

    style_context = gtk_widget_get_style_context (widget);
    gtk_style_context_save (style_context);
    gtk_style_context_add_class (style_context, "icon-embedded-text");

    cairo_save (cr);

    cairo_rectangle (cr,
                     x + item->details->embedded_text_rect.x,
                     y + item->details->embedded_text_rect.y,
                     item->details->embedded_text_rect.width,
                     item->details->embedded_text_rect.height);
    cairo_clip (cr);

    gtk_render_layout (style_context, cr,
                       x + item->details->embedded_text_rect.x,
                       y + item->details->embedded_text_rect.y,
                       layout);

    gtk_style_context_restore (style_context);
    cairo_restore (cr);
}

typedef enum
{
	SELECT_TARGET = 1,
	HOVER_TARGET  = 2,  
	NORMAL_TARGET = 3	
}TARGET_ACTION;

#define BACKGROUND_MAX(a,b) ((a)>(b)?(a):(b))
#define BACKGROUND_MIN(a,b) ((a)<(b)?(a):(b))

static void
draw_target_background (cairo_t *cr,
                        int x, int y, int width, int height,TARGET_ACTION eAction)
{
	GdkColor label_color;
	GdkRectangle stRect = {0};

	if(SELECT_TARGET == eAction)
	{
		label_color.red   = 50115;
		label_color.green = 57331;
		label_color.blue  = 65278;
	}
	else if(HOVER_TARGET == eAction)
	{
		label_color.red   = 56540;
		label_color.green = 60395;
		label_color.blue  = 64507;
	}
	else if(NORMAL_TARGET == eAction)
	{
		label_color.red   = 65535;
		label_color.green = 65535;
		label_color.blue  = 65535;
	}

	stRect.x 	  = x;
	stRect.y 	  = y;
	stRect.width  = width;
	stRect.height = height;
	/* By default, we use the style background. */
    gdk_cairo_set_source_color (cr, &label_color);
    //gdk_cairo_rectangle(cr, &stRect);
    cairo_rectangle(cr, x,y,width,height);
    cairo_fill (cr);

}

static int get_can_redraw_rect(EelCanvasItem *item,RECT_PARAM_T *pRect)
{
	EelIRect icon_rect				   = {0};
	EelIRect *pTotalRect			   = {0};
	gboolean bLabelBeside			   = FALSE;
	EelIRect stTextRect				   = {0};
	EelIRect stDisTextRect			   = {0};
	double 	 iBackGroundx			   = 0.0;
	double 	 iBackGroundy			   = 0.0;
	double 	 iBackGroundwidth		   = 0.0;
	double 	 iBackGroundheight	       = 0.0;
	double 	 iBackGroundMidx	       = 0.0;
	double 	 iBackGroundMidy	       = 0.0;
	int 	 iIconZoomLevel			   = 0.0;
	double 	 iTemp				       = 0.0;
	PeonyIconContainer  *pstContainer   = NULL;
	PeonyIconCanvasItem *icon_item	   = NULL;
	PeonyIconCanvasItemDetails *details = NULL;
	double 			   pixels_per_unit = 0.0;

	if((NULL == item) || (NULL == pRect))
	{
		return -1;
	}
	pixels_per_unit = item->canvas->pixels_per_unit;
	icon_item = PEONY_ICON_CANVAS_ITEM (item);
	details = icon_item->details;

	/* Draw the pixbuf. */
	if (NULL == details->pixbuf || NULL == icon_item || NULL == details)
	{
		return -1;
	}

	icon_rect 			  = icon_item->details->canvas_rect;
	pstContainer 		  = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (icon_item)->canvas);
	if(NULL == pstContainer)
	{
		return -1;
	}
	
	iIconZoomLevel 		  = peony_get_icon_size_for_zoom_level(pstContainer->details->zoom_level);
	bLabelBeside		  = pstContainer->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE;

	stTextRect			  = compute_text_rectangle (icon_item, icon_rect, TRUE, BOUNDS_USAGE_FOR_LAYOUT);
	iBackGroundwidth	  = (double)(bLabelBeside ? (MAX((MAX(stTextRect.x1,icon_rect.x1) - icon_rect.x0),(250*iIconZoomLevel/48))) : (MAX((icon_rect.x1 - icon_rect.x0),(105*iIconZoomLevel/64))));
	iBackGroundheight	  = (double)(bLabelBeside ? (MAX((MAX(stTextRect.y1,icon_rect.y1) - icon_rect.y0),(60*iIconZoomLevel/48)))  : (MAX((icon_rect.y1 - icon_rect.y0),(118*iIconZoomLevel/64))));

	if(FALSE == bLabelBeside)
	{
		/*Here need to consider the case of screenshots, x is relatively narrow but centered, y shorter but bottom*/
		iBackGroundMidx	  = icon_rect.x0 + ((icon_rect.x1 - icon_rect.x0)/2);
		iBackGroundMidy	  = (icon_rect.y1 > (iIconZoomLevel/2)) ? (icon_rect.y1 - (iIconZoomLevel/2)) : 0;
	}
	else
	{
		//pTotalRect 		  = &(icon_item->details->bounds_cache);
		//iBackGroundMidx	  = pTotalRect->x0 + ((BACKGROUND_MAX(stTextRect.x1,pTotalRect->x1) - pTotalRect->x0)/2) + icon_item->details->x;
		//iBackGroundMidy	  = pTotalRect->y0 + ((BACKGROUND_MAX(stTextRect.y1,pTotalRect->y1) - pTotalRect->y0)/2) + icon_item->details->y;
		iBackGroundMidx	  = icon_rect.x0 + ((BACKGROUND_MAX(stTextRect.x1,icon_rect.x1) - icon_rect.x0)/2);
		iBackGroundMidy	  = icon_rect.y0 + ((BACKGROUND_MAX(stTextRect.y1,icon_rect.y1) - icon_rect.y0)/2);
	}
	//peony_debug_log(TRUE,"_background_","iBackGroundMidx[%lf] iBackGroundMidy[%lf] icon_rect.x0[%d]icon_rect.y0[%d]icon_rect.x1[%d]icon_rect.y1[%d]stTextRect.x1[%d]stTextRect.y1[%d]bLabelBeside[%d]",iBackGroundMidx,iBackGroundMidy,
	//	icon_rect.x0,icon_rect.y0,icon_rect.x1,icon_rect.y1,stTextRect.x1,stTextRect.y1,bLabelBeside);
	//peony_debug_log(TRUE,"_background_","iBackGroundwidth[%lf] iBackGroundheight[%lf]",
	//	iBackGroundwidth,iBackGroundheight);

	iBackGroundx		  = (iBackGroundMidx > (iBackGroundwidth/2 + LAYOUT_PAD_LEFT)) ? (iBackGroundMidx - (iBackGroundwidth/2)) : LAYOUT_PAD_LEFT;
	iBackGroundy		  = (iBackGroundMidy > (iBackGroundheight/2 + LAYOUT_PAD_TOP)) ? (iBackGroundMidy - (iBackGroundheight/2)) : LAYOUT_PAD_TOP;
	iTemp			      = iBackGroundy + iBackGroundheight;
	if((FALSE == bLabelBeside) && (stTextRect.y1 > iTemp))
	{
		iBackGroundheight += stTextRect.y1 - iTemp;
	}

	
	stDisTextRect = compute_text_rectangle (icon_item, icon_rect, TRUE, BOUNDS_USAGE_FOR_DISPLAY);//!!!
	
	pRect->x      = iBackGroundx;///pixels_per_unit;
	pRect->y      = iBackGroundy;///pixels_per_unit;
	pRect->width  = iBackGroundwidth;///pixels_per_unit;
	pRect->height = (iBackGroundheight + MAX(stDisTextRect.y1 - stTextRect.y1,0));///pixels_per_unit;
	//peony_debug_log(TRUE,"_background_","iIconZoomLevel[%d] w[%d]h[%d] x0[%d]y0[%d]",iIconZoomLevel,icon_rect.x1-icon_rect.x0,
	//	icon_rect.y1-icon_rect.y0,icon_rect.x0,icon_rect.y0);
	//peony_debug_log(TRUE,"_background_","pRect->x[%d] pRect->y[%d]pRect->width[%d] pRect->height[%d]",
	//	pRect->x,pRect->y,pRect->width,pRect->height);
	return 0;
}

/* Draw the icon item for non-anti-aliased mode. */
static void
peony_icon_canvas_item_draw (EelCanvasItem *item,
                            cairo_t *cr,
                            cairo_region_t *region)
{
    PeonyIconContainer *container;
    PeonyIconCanvasItem *icon_item;
    PeonyIconCanvasItemDetails *details;
    EelIRect icon_rect, emblem_rect;
    EmblemLayout emblem_layout;
    GdkPixbuf *emblem_pixbuf, *temp_pixbuf;
    GtkStyleContext *context;

    container = PEONY_ICON_CONTAINER (item->canvas);
    gboolean is_rtl;
	gboolean     		bNeedsHighlight;
	int          		iRet   		 = 0;
	PeonyIconContainer *	pstContainer = NULL;

	icon_item 	 = PEONY_ICON_CANVAS_ITEM (item);
	details 	 = icon_item->details;
	pstContainer = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (icon_item)->canvas);
    /* Draw the pixbuf. */
    if (details->pixbuf == NULL)
    {
        return;
    }

    context = gtk_widget_get_style_context (GTK_WIDGET (container));
    gtk_style_context_save (context);
    gtk_style_context_add_class (context, "peony-canvas-item");

    icon_rect = icon_item->details->canvas_rect;

    temp_pixbuf = map_pixbuf (icon_item);

	bNeedsHighlight = details->is_highlighted_for_selection || details->is_highlighted_for_drop;
	if(bNeedsHighlight)
	{		
		draw_target_background(cr,item->x1,\
				item->y1,(item->x2)-(item->x1),(item->y2)-(item->y1),SELECT_TARGET);
	}
	else if(details->is_prelit)
	{
		draw_target_background(cr,item->x1,\
				item->y1,(item->x2)-(item->x1),(item->y2)-(item->y1),HOVER_TARGET);
	}

    gtk_render_icon (context, cr,
                     temp_pixbuf,
                     icon_rect.x0, icon_rect.y0);
    g_object_unref (temp_pixbuf);

    draw_embedded_text (icon_item, cr, icon_rect.x0, icon_rect.y0);

    is_rtl = peony_icon_container_is_layout_rtl (PEONY_ICON_CONTAINER (item->canvas));

    /* Draw the emblem pixbufs. */
    emblem_layout_reset (&emblem_layout, icon_item, icon_rect, is_rtl);
    while (emblem_layout_next (&emblem_layout, &emblem_pixbuf, &emblem_rect, is_rtl))
    {
        draw_pixbuf (emblem_pixbuf, cr, emblem_rect.x0, emblem_rect.y0);
    }

    /* Draw stretching handles (if necessary). */
    draw_stretch_handles (icon_item, cr, &icon_rect);

    /* Draw the label text. */
    draw_label_text (icon_item, cr, FALSE, icon_rect);

    gtk_style_context_restore (context);
}

#define ZERO_WIDTH_SPACE "\xE2\x80\x8B"

#define ZERO_OR_THREE_DIGITS(p) \
	(!g_ascii_isdigit (*p) || \
	 (g_ascii_isdigit (*(p+1)) && \
	  g_ascii_isdigit (*(p+2))))


static PangoLayout *
create_label_layout (PeonyIconCanvasItem *item,
                     const char *text)
{
    PangoLayout *layout;
    PangoContext *context;
    PangoFontDescription *desc;
    PeonyIconContainer *container;
    EelCanvasItem *canvas_item;
    GString *str;
    char *zeroified_text;
    const char *p;

    canvas_item = EEL_CANVAS_ITEM (item);

    container = PEONY_ICON_CONTAINER (canvas_item->canvas);
    context = gtk_widget_get_pango_context (GTK_WIDGET (canvas_item->canvas));
    layout = pango_layout_new (context);

    zeroified_text = NULL;

    if (text != NULL)
    {
        str = g_string_new (NULL);

        for (p = text; *p != '\0'; p++)
        {
            str = g_string_append_c (str, *p);

            if (*p == '_' || *p == '-' || (*p == '.' && ZERO_OR_THREE_DIGITS (p+1)))
            {
                /* Ensure that we allow to break after '_' or '.' characters,
                 * if they are not likely to be part of a version information, to
                 * not break wrapping of foobar-0.0.1.
                 * Wrap before IPs and long numbers, though. */
                str = g_string_append (str, ZERO_WIDTH_SPACE);
            }
        }

        zeroified_text = g_string_free (str, FALSE);
    }

    pango_layout_set_text (layout, zeroified_text, -1);
    pango_layout_set_auto_dir (layout, FALSE);

    if (container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
    {
        if (!peony_icon_container_is_layout_rtl (container))
        {
            pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
        }
        else
        {
            pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
        }
    }
    else
    {
        pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
    }

    pango_layout_set_spacing (layout, LABEL_LINE_SPACING);
    pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);

    /* Create a font description */
    if (container->details->font)
    {
        desc = pango_font_description_from_string (container->details->font);
    }
    else
    {
        desc = pango_font_description_copy (pango_context_get_font_description (context));
        pango_font_description_set_size (desc,
                                         pango_font_description_get_size (desc) +
                                         container->details->font_size_table [container->details->zoom_level]);
    }
    pango_layout_set_font_description (layout, desc);
    pango_font_description_free (desc);
    g_free (zeroified_text);

    return layout;
}

static PangoLayout *
get_label_layout (PangoLayout **layout_cache,
                  PeonyIconCanvasItem *item,
                  const char *text)
{
    PangoLayout *layout;

    if (*layout_cache != NULL)
    {
        return g_object_ref (*layout_cache);
    }

    layout = create_label_layout (item, text);

    if (item->details->is_visible)
    {
        *layout_cache = g_object_ref (layout);
    }

    return layout;
}


/* handle events */
static int
peony_icon_canvas_item_event (EelCanvasItem *item, GdkEvent *event)
{
    PeonyIconCanvasItem *icon_item;
    GdkCursor *cursor;
    GdkWindow *cursor_window;

    icon_item = PEONY_ICON_CANVAS_ITEM (item);
    cursor_window = ((GdkEventAny *) event)->window;

    switch (event->type)
    {
    case GDK_ENTER_NOTIFY:
        if (!icon_item->details->is_prelit)
        {
            icon_item->details->is_prelit = TRUE;
            peony_icon_canvas_item_invalidate_label_size (icon_item);
            eel_canvas_item_request_update (item);
            eel_canvas_item_send_behind (item,
                                         PEONY_ICON_CONTAINER (item->canvas)->details->rubberband_info.selection_rectangle);

            /* show a hand cursor */
            if (in_single_click_mode ())
            {
                cursor = gdk_cursor_new_for_display (gdk_display_get_default(),
                                                     GDK_HAND2);
                gdk_window_set_cursor (cursor_window, cursor);
                g_object_unref (cursor);

                icon_item->details->cursor_window = g_object_ref (cursor_window);
            }

            /* FIXME bugzilla.gnome.org 42473:
             * We should emit our own signal here,
             * not one from the container; it could hook
             * up to that signal and emit one of its
             * own. Doing it this way hard-codes what
             * "user_data" is. Also, the two signals
             * should be separate. The "unpreview" signal
             * does not have a return value.
             */
            icon_item->details->is_active = peony_icon_container_emit_preview_signal
                                            (PEONY_ICON_CONTAINER (item->canvas),
                                             PEONY_ICON_CANVAS_ITEM (item)->user_data,
                                             TRUE);
        }
        return TRUE;

    case GDK_LEAVE_NOTIFY:
        if (icon_item->details->is_prelit
                || icon_item->details->is_highlighted_for_drop)
        {
            /* When leaving, turn of the prelight state and the
             * higlighted for drop. The latter gets turned on
             * by the drag&drop motion callback.
             */
            /* FIXME bugzilla.gnome.org 42473:
             * We should emit our own signal here,
             * not one from the containe; it could hook up
             * to that signal and emit one of its
             * ownr. Doing it this way hard-codes what
             * "user_data" is. Also, the two signals
             * should be separate. The "unpreview" signal
             * does not have a return value.
             */
            peony_icon_container_emit_preview_signal
            (PEONY_ICON_CONTAINER (item->canvas),
             PEONY_ICON_CANVAS_ITEM (item)->user_data,
             FALSE);
            icon_item->details->is_prelit = FALSE;
            icon_item->details->is_active = 0;
            icon_item->details->is_highlighted_for_drop = FALSE;
            peony_icon_canvas_item_invalidate_label_size (icon_item);
            eel_canvas_item_request_update (item);

            /* show default cursor */
            gdk_window_set_cursor (cursor_window, NULL);
            g_clear_object (&icon_item->details->cursor_window);
        }
        return TRUE;

    default:
        /* Don't eat up other events; icon container might use them. */
        return FALSE;
    }
}

static gboolean
hit_test_pixbuf (GdkPixbuf *pixbuf, EelIRect pixbuf_location, EelIRect probe_rect)
{
    EelIRect relative_rect, pixbuf_rect;
    int x, y;
    guint8 *pixel;

    /* You can get here without a pixbuf in some strange cases. */
    if (pixbuf == NULL)
    {
        return FALSE;
    }

    /* Check to see if it's within the rectangle at all. */
    relative_rect.x0 = probe_rect.x0 - pixbuf_location.x0;
    relative_rect.y0 = probe_rect.y0 - pixbuf_location.y0;
    relative_rect.x1 = probe_rect.x1 - pixbuf_location.x0;
    relative_rect.y1 = probe_rect.y1 - pixbuf_location.y0;
    pixbuf_rect.x0 = 0;
    pixbuf_rect.y0 = 0;
    pixbuf_rect.x1 = gdk_pixbuf_get_width (pixbuf);
    pixbuf_rect.y1 = gdk_pixbuf_get_height (pixbuf);
    eel_irect_intersect (&relative_rect, &relative_rect, &pixbuf_rect);
    if (eel_irect_is_empty (&relative_rect))
    {
        return FALSE;
    }

    /* If there's no alpha channel, it's opaque and we have a hit. */
    if (!gdk_pixbuf_get_has_alpha (pixbuf))
    {
        return TRUE;
    }
    g_assert (gdk_pixbuf_get_n_channels (pixbuf) == 4);

    /* Check the alpha channel of the pixel to see if we have a hit. */
    for (x = relative_rect.x0; x < relative_rect.x1; x++)
    {
        for (y = relative_rect.y0; y < relative_rect.y1; y++)
        {
            pixel = gdk_pixbuf_get_pixels (pixbuf)
                    + y * gdk_pixbuf_get_rowstride (pixbuf)
                    + x * 4;
            if (pixel[3] > 1)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static gboolean
hit_test (PeonyIconCanvasItem *icon_item, EelIRect canvas_rect)
{
    PeonyIconCanvasItemDetails *details;
    EelIRect emblem_rect;
    EmblemLayout emblem_layout;
    GdkPixbuf *emblem_pixbuf;
    gboolean is_rtl;
	EelIRect stRectBounds = {0};
    details = icon_item->details;
	stRectBounds.x0 = icon_item->item.x1;
	stRectBounds.y0 = icon_item->item.y1;
	stRectBounds.x1 = icon_item->item.x2;
	stRectBounds.y1 = icon_item->item.y2;
	/* Quick check to see if the rect hits the icon, text or emblems at all. */
	if (!eel_irect_hits_irect (stRectBounds, canvas_rect)
			&& !eel_irect_hits_irect (icon_item->details->canvas_rect, canvas_rect)
			&& (!eel_irect_hits_irect (details->text_rect, canvas_rect))
			&& (!eel_irect_hits_irect (details->emblem_rect, canvas_rect))
			)
	{
		return FALSE;
	}

	
	if (eel_irect_hits_irect (stRectBounds, canvas_rect))
	{
		return TRUE;
	}


    /* Check for hits in the stretch handles. */
    if (hit_test_stretch_handle (icon_item, canvas_rect, NULL))
    {
        return TRUE;
    }

    /* Check for hit in the icon. */
    if (eel_irect_hits_irect (icon_item->details->canvas_rect, canvas_rect))
    {
        return TRUE;
    }

    /* Check for hit in the text. */
    if (eel_irect_hits_irect (details->text_rect, canvas_rect)
            && !icon_item->details->is_renaming)
    {
        return TRUE;
    }

    is_rtl = peony_icon_container_is_layout_rtl (PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (icon_item)->canvas));

    /* Check for hit in the emblem pixbufs. */
    emblem_layout_reset (&emblem_layout, icon_item, icon_item->details->canvas_rect, is_rtl);
    while (emblem_layout_next (&emblem_layout, &emblem_pixbuf, &emblem_rect, is_rtl))
    {
        if (hit_test_pixbuf (emblem_pixbuf, emblem_rect, canvas_rect))
        {
            return TRUE;
        }
    }

    return FALSE;
}

/* Point handler for the icon canvas item. */
static double
peony_icon_canvas_item_point (EelCanvasItem *item, double x, double y, int cx, int cy,
                             EelCanvasItem **actual_item)
{
    EelIRect canvas_rect;

    *actual_item = item;
    canvas_rect.x0 = cx;
    canvas_rect.y0 = cy;
    canvas_rect.x1 = cx + 1;
    canvas_rect.y1 = cy + 1;
    if (hit_test (PEONY_ICON_CANVAS_ITEM (item), canvas_rect))
    {
        return 0.0;
    }
    else
    {
        /* This value means not hit.
         * It's kind of arbitrary. Can we do better?
         */
        return item->canvas->pixels_per_unit * 2 + 10;
    }
}

static void
peony_icon_canvas_item_translate (EelCanvasItem *item, double dx, double dy)
{
    PeonyIconCanvasItem *icon_item;
    PeonyIconCanvasItemDetails *details;

    icon_item = PEONY_ICON_CANVAS_ITEM (item);
    details = icon_item->details;

    details->x += dx;
    details->y += dy;
}

void
peony_icon_canvas_item_get_bounds_for_layout (PeonyIconCanvasItem *icon_item,
        double *x1, double *y1, double *x2, double *y2)
{
    PeonyIconCanvasItemDetails *details;
    EelIRect *total_rect;

    details = icon_item->details;

    peony_icon_canvas_item_ensure_bounds_up_to_date (icon_item);
    g_assert (details->bounds_cached);

    total_rect = &details->bounds_cache_for_layout;

    /* Return the result. */
    if (x1 != NULL)
    {
        *x1 = (int)details->x + total_rect->x0;
    }
    if (y1 != NULL)
    {
        *y1 = (int)details->y + total_rect->y0;
    }
    if (x2 != NULL)
    {
        *x2 = (int)details->x + total_rect->x1 + 1;
    }
    if (y2 != NULL)
    {
        *y2 = (int)details->y + total_rect->y1 + 1;
    }
}

void
peony_icon_canvas_item_get_bounds_for_entire_item (PeonyIconCanvasItem *icon_item,
        double *x1, double *y1, double *x2, double *y2)
{
    PeonyIconCanvasItemDetails *details;
    EelIRect *total_rect;

    details = icon_item->details;

    peony_icon_canvas_item_ensure_bounds_up_to_date (icon_item);
    g_assert (details->bounds_cached);

    total_rect = &details->bounds_cache_for_entire_item;

    /* Return the result. */
    if (x1 != NULL)
    {
        *x1 = (int)details->x + total_rect->x0;
    }
    if (y1 != NULL)
    {
        *y1 = (int)details->y + total_rect->y0;
    }
    if (x2 != NULL)
    {
        *x2 = (int)details->x + total_rect->x1 + 1;
    }
    if (y2 != NULL)
    {
        *y2 = (int)details->y + total_rect->y1 + 1;
    }
}

/* Bounds handler for the icon canvas item. */
static void
peony_icon_canvas_item_bounds (EelCanvasItem *item,
                              double *x1, double *y1, double *x2, double *y2)
{
    PeonyIconCanvasItem *icon_item;
    PeonyIconCanvasItemDetails *details;
    EelIRect *total_rect;
	RECT_PARAM_T stRect = {0};
	int          iRet   = 0;

    icon_item = PEONY_ICON_CANVAS_ITEM (item);
    details = icon_item->details;

    g_assert (x1 != NULL);
    g_assert (y1 != NULL);
    g_assert (x2 != NULL);
    g_assert (y2 != NULL);

    peony_icon_canvas_item_ensure_bounds_up_to_date (icon_item);
	g_assert (details->bounds_cached);
	
	total_rect = &details->bounds_cache;
	
	iRet = get_can_redraw_rect(item,&stRect);
	if(0 == iRet)
	{
		*x1 = ((double)stRect.x - item->canvas->zoom_xofs)/item->canvas->pixels_per_unit + item->canvas->scroll_x1;
		*y1 = ((double)stRect.y - item->canvas->zoom_yofs)/item->canvas->pixels_per_unit + item->canvas->scroll_y1;
		*x2 = ((double)(stRect.x + stRect.width) - item->canvas->zoom_xofs)/item->canvas->pixels_per_unit + item->canvas->scroll_x1;
		*y2 = ((double)(stRect.y + stRect.height) - item->canvas->zoom_yofs)/item->canvas->pixels_per_unit + item->canvas->scroll_y1;
	}
	else
	{
		*x1 = (int)details->x + total_rect->x0;
		*y1 = (int)details->y + total_rect->y0;
		*x2 = (int)details->x + total_rect->x1 + 1;
		*y2 = (int)details->y + total_rect->y1 + 1;		
	}
}

static void
peony_icon_canvas_item_ensure_bounds_up_to_date (PeonyIconCanvasItem *icon_item)
{
    PeonyIconCanvasItemDetails *details;
    EelIRect icon_rect, emblem_rect, icon_rect_raw;
    EelIRect text_rect, text_rect_for_layout, text_rect_for_entire_text;
    EelIRect total_rect, total_rect_for_layout, total_rect_for_entire_text;
    EelCanvasItem *item;
    double pixels_per_unit;
    EmblemLayout emblem_layout;
    GdkPixbuf *emblem_pixbuf;
    gboolean is_rtl;

    details = icon_item->details;
    item = EEL_CANVAS_ITEM (icon_item);

    if (!details->bounds_cached)
    {
        measure_label_text (icon_item);

        pixels_per_unit = EEL_CANVAS_ITEM (item)->canvas->pixels_per_unit;

        /* Compute raw and scaled icon rectangle. */
        icon_rect.x0 = 0;
        icon_rect.y0 = 0;
        icon_rect_raw.x0 = 0;
        icon_rect_raw.y0 = 0;
        if (details->pixbuf == NULL)
        {
            icon_rect.x1 = icon_rect.x0;
            icon_rect.y1 = icon_rect.y0;
            icon_rect_raw.x1 = icon_rect_raw.x0;
            icon_rect_raw.y1 = icon_rect_raw.y0;
        }
        else
        {
            icon_rect_raw.x1 = icon_rect_raw.x0 + gdk_pixbuf_get_width (details->pixbuf);
            icon_rect_raw.y1 = icon_rect_raw.y0 + gdk_pixbuf_get_height (details->pixbuf);
            icon_rect.x1 = icon_rect_raw.x1 / pixels_per_unit;
			icon_rect.y1 = icon_rect_raw.y1 / pixels_per_unit+MY_COMPUTER_EXTRA_RECT;
        }

        /* Compute text rectangle. */
        text_rect = compute_text_rectangle (icon_item, icon_rect, FALSE, BOUNDS_USAGE_FOR_DISPLAY);
        text_rect_for_layout = compute_text_rectangle (icon_item, icon_rect, FALSE, BOUNDS_USAGE_FOR_LAYOUT);
        text_rect_for_entire_text = compute_text_rectangle (icon_item, icon_rect, FALSE, BOUNDS_USAGE_FOR_ENTIRE_ITEM);

        is_rtl = peony_icon_container_is_layout_rtl (PEONY_ICON_CONTAINER (item->canvas));

        /* Compute total rectangle, adding in emblem rectangles. */
        eel_irect_union (&total_rect, &icon_rect, &text_rect);
        eel_irect_union (&total_rect_for_layout, &icon_rect, &text_rect_for_layout);
        eel_irect_union (&total_rect_for_entire_text, &icon_rect, &text_rect_for_entire_text);
        emblem_layout_reset (&emblem_layout, icon_item, icon_rect_raw, is_rtl);
        while (emblem_layout_next (&emblem_layout, &emblem_pixbuf, &emblem_rect, is_rtl))
        {
            emblem_rect.x0 = floor (emblem_rect.x0 / pixels_per_unit);
            emblem_rect.y0 = floor (emblem_rect.y0 / pixels_per_unit);
            emblem_rect.x1 = ceil (emblem_rect.x1 / pixels_per_unit);
            emblem_rect.y1 = ceil (emblem_rect.y1 / pixels_per_unit);

            eel_irect_union (&total_rect, &total_rect, &emblem_rect);
            eel_irect_union (&total_rect_for_layout, &total_rect_for_layout, &emblem_rect);
            eel_irect_union (&total_rect_for_entire_text, &total_rect_for_entire_text, &emblem_rect);
        }

        details->bounds_cache = total_rect;
        details->bounds_cache_for_layout = total_rect_for_layout;
        details->bounds_cache_for_entire_item = total_rect_for_entire_text;
        details->bounds_cached = TRUE;
    }
}

/* Get the rectangle of the icon only, in world coordinates. */
EelDRect
peony_icon_canvas_item_get_icon_rectangle (const PeonyIconCanvasItem *item)
{
    EelDRect rectangle;
    double pixels_per_unit;
    GdkPixbuf *pixbuf;

    g_return_val_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item), eel_drect_empty);

    rectangle.x0 = item->details->x;
    rectangle.y0 = item->details->y;

    pixbuf = item->details->pixbuf;

    pixels_per_unit = EEL_CANVAS_ITEM (item)->canvas->pixels_per_unit;
    rectangle.x1 = rectangle.x0 + (pixbuf == NULL ? 0 : gdk_pixbuf_get_width (pixbuf)) / pixels_per_unit;
    rectangle.y1 = rectangle.y0 + (pixbuf == NULL ? 0 : gdk_pixbuf_get_height (pixbuf)) / pixels_per_unit;

    eel_canvas_item_i2w (EEL_CANVAS_ITEM (item),
                         &rectangle.x0,
                         &rectangle.y0);
    eel_canvas_item_i2w (EEL_CANVAS_ITEM (item),
                         &rectangle.x1,
                         &rectangle.y1);

    return rectangle;
}

EelDRect
peony_icon_canvas_item_get_text_rectangle (PeonyIconCanvasItem *item,
        gboolean for_layout)
{
    /* FIXME */
    EelIRect icon_rectangle;
    EelIRect text_rectangle;
    EelDRect ret;
    double pixels_per_unit;
    GdkPixbuf *pixbuf;

    g_return_val_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item), eel_drect_empty);

    icon_rectangle.x0 = item->details->x;
    icon_rectangle.y0 = item->details->y;

    pixbuf = item->details->pixbuf;

    pixels_per_unit = EEL_CANVAS_ITEM (item)->canvas->pixels_per_unit;
    icon_rectangle.x1 = icon_rectangle.x0 + (pixbuf == NULL ? 0 : gdk_pixbuf_get_width (pixbuf)) / pixels_per_unit;
    icon_rectangle.y1 = icon_rectangle.y0 + (pixbuf == NULL ? 0 : gdk_pixbuf_get_height (pixbuf)) / pixels_per_unit;

    measure_label_text (item);

    text_rectangle = compute_text_rectangle (item, icon_rectangle, FALSE,
                     for_layout ? BOUNDS_USAGE_FOR_LAYOUT : BOUNDS_USAGE_FOR_DISPLAY);

    ret.x0 = text_rectangle.x0;
    ret.y0 = text_rectangle.y0;
    ret.x1 = text_rectangle.x1;
    ret.y1 = text_rectangle.y1;

    eel_canvas_item_i2w (EEL_CANVAS_ITEM (item),
                         &ret.x0,
                         &ret.y0);
    eel_canvas_item_i2w (EEL_CANVAS_ITEM (item),
                         &ret.x1,
                         &ret.y1);

    return ret;
}


/* Get the rectangle of the icon only, in canvas coordinates. */
static void
get_icon_canvas_rectangle (PeonyIconCanvasItem *item,
                           EelIRect *rect)
{
    GdkPixbuf *pixbuf;

    g_assert (PEONY_IS_ICON_CANVAS_ITEM (item));
    g_assert (rect != NULL);

    eel_canvas_w2c (EEL_CANVAS_ITEM (item)->canvas,
                    item->details->x,
                    item->details->y,
                    &rect->x0,
                    &rect->y0);

    pixbuf = item->details->pixbuf;

    rect->x1 = rect->x0 + (pixbuf == NULL ? 0 : gdk_pixbuf_get_width (pixbuf));
    rect->y1 = rect->y0 + (pixbuf == NULL ? 0 : gdk_pixbuf_get_height (pixbuf));
}

void
peony_icon_canvas_item_set_show_stretch_handles (PeonyIconCanvasItem *item,
        gboolean show_stretch_handles)
{
    g_return_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item));
    g_return_if_fail (show_stretch_handles == FALSE || show_stretch_handles == TRUE);

    if (!item->details->show_stretch_handles == !show_stretch_handles)
    {
        return;
    }

    item->details->show_stretch_handles = show_stretch_handles;
    eel_canvas_item_request_update (EEL_CANVAS_ITEM (item));
}

/* Check if one of the stretch handles was hit. */
static gboolean
hit_test_stretch_handle (PeonyIconCanvasItem *item,
                         EelIRect probe_canvas_rect,
                         GtkCornerType *corner)
{
    EelIRect icon_rect;
    GdkPixbuf *knob_pixbuf;
    int knob_width, knob_height;
    int hit_corner;

    g_assert (PEONY_IS_ICON_CANVAS_ITEM (item));

    /* Make sure there are handles to hit. */
    if (!item->details->show_stretch_handles)
    {
        return FALSE;
    }

    /* Quick check to see if the rect hits the icon at all. */
    icon_rect = item->details->canvas_rect;
    if (!eel_irect_hits_irect (probe_canvas_rect, icon_rect))
    {
        return FALSE;
    }

    knob_pixbuf = get_knob_pixbuf ();
    knob_width = gdk_pixbuf_get_width (knob_pixbuf);
    knob_height = gdk_pixbuf_get_height (knob_pixbuf);
    g_object_unref (knob_pixbuf);

    /* Check for hits in the stretch handles. */
    hit_corner = -1;
    if (probe_canvas_rect.x0 < icon_rect.x0 + knob_width)
    {
        if (probe_canvas_rect.y0 < icon_rect.y0 + knob_height)
            hit_corner = GTK_CORNER_TOP_LEFT;
        else if (probe_canvas_rect.y1 >= icon_rect.y1 - knob_height)
            hit_corner = GTK_CORNER_BOTTOM_LEFT;
    }
    else if (probe_canvas_rect.x1 >= icon_rect.x1 - knob_width)
    {
        if (probe_canvas_rect.y0 < icon_rect.y0 + knob_height)
            hit_corner = GTK_CORNER_TOP_RIGHT;
        else if (probe_canvas_rect.y1 >= icon_rect.y1 - knob_height)
            hit_corner = GTK_CORNER_BOTTOM_RIGHT;
    }
    if (corner)
        *corner = hit_corner;

    return hit_corner != -1;
}

gboolean
peony_icon_canvas_item_hit_test_stretch_handles (PeonyIconCanvasItem *item,
        EelDPoint world_point,
        GtkCornerType *corner)
{
    EelIRect canvas_rect;

    g_return_val_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item), FALSE);

    eel_canvas_w2c (EEL_CANVAS_ITEM (item)->canvas,
                    world_point.x,
                    world_point.y,
                    &canvas_rect.x0,
                    &canvas_rect.y0);
    canvas_rect.x1 = canvas_rect.x0 + 1;
    canvas_rect.y1 = canvas_rect.y0 + 1;
    return hit_test_stretch_handle (item, canvas_rect, corner);
}

/* peony_icon_canvas_item_hit_test_rectangle
 *
 * Check and see if there is an intersection between the item and the
 * canvas rect.
 */
gboolean
peony_icon_canvas_item_hit_test_rectangle (PeonyIconCanvasItem *item, EelIRect canvas_rect)
{
    g_return_val_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item), FALSE);

    return hit_test (item, canvas_rect);
}

const char *
peony_icon_canvas_item_get_editable_text (PeonyIconCanvasItem *icon_item)
{
    g_return_val_if_fail (PEONY_IS_ICON_CANVAS_ITEM (icon_item), NULL);

    return icon_item->details->editable_text;
}

void
peony_icon_canvas_item_set_renaming (PeonyIconCanvasItem *item, gboolean state)
{
    g_return_if_fail (PEONY_IS_ICON_CANVAS_ITEM (item));
    g_return_if_fail (state == FALSE || state == TRUE);

    if (!item->details->is_renaming == !state)
    {
        return;
    }

    item->details->is_renaming = state;
    eel_canvas_item_request_update (EEL_CANVAS_ITEM (item));
}

double
peony_icon_canvas_item_get_max_text_width (PeonyIconCanvasItem *item)
{
    EelCanvasItem *canvas_item;
    PeonyIconContainer *container;

    canvas_item = EEL_CANVAS_ITEM (item);
    container = PEONY_ICON_CONTAINER (canvas_item->canvas);

    if (peony_icon_container_is_tighter_layout (container))
    {
        return MAX_TEXT_WIDTH_TIGHTER * canvas_item->canvas->pixels_per_unit;
    }
    else
    {

        if (container->details->label_position == PEONY_ICON_LABEL_POSITION_BESIDE)
        {
            if (container->details->layout_mode == PEONY_ICON_LAYOUT_T_B_L_R ||
                    container->details->layout_mode == PEONY_ICON_LAYOUT_T_B_R_L)
            {
                if (container->details->all_columns_same_width)
                {
                    return MAX_TEXT_WIDTH_BESIDE_TOP_TO_BOTTOM * canvas_item->canvas->pixels_per_unit;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                return MAX_TEXT_WIDTH_BESIDE * canvas_item->canvas->pixels_per_unit;
            }
        }
        else
        {
            return MAX_TEXT_WIDTH_STANDARD * canvas_item->canvas->pixels_per_unit;
        }


    }

}

void
peony_icon_canvas_item_set_entire_text (PeonyIconCanvasItem       *item,
					   gboolean                      entire_text)
{
	if (item->details->entire_text != entire_text) {
		item->details->entire_text = entire_text;

		peony_icon_canvas_item_invalidate_label_size (item);
		eel_canvas_item_request_update (EEL_CANVAS_ITEM (item));
	}
}

/* Class initialization function for the icon canvas item. */
static void
peony_icon_canvas_item_class_init (PeonyIconCanvasItemClass *class)
{
	GObjectClass *object_class;
	EelCanvasItemClass *item_class;

	object_class = G_OBJECT_CLASS (class);
	item_class = EEL_CANVAS_ITEM_CLASS (class);

	object_class->finalize = peony_icon_canvas_item_finalize;
	object_class->set_property = peony_icon_canvas_item_set_property;
	object_class->get_property = peony_icon_canvas_item_get_property;

        g_object_class_install_property (
		object_class,
		PROP_EDITABLE_TEXT,
		g_param_spec_string ("editable_text",
				     "editable text",
				     "the editable label",
				     "", G_PARAM_READWRITE));

        g_object_class_install_property (
		object_class,
		PROP_ADDITIONAL_TEXT,
		g_param_spec_string ("additional_text",
				     "additional text",
				     "some more text",
				     "", G_PARAM_READWRITE));

        g_object_class_install_property (
		object_class,
		PROP_HIGHLIGHTED_FOR_SELECTION,
		g_param_spec_boolean ("highlighted_for_selection",
				      "highlighted for selection",
				      "whether we are highlighted for a selection",
				      FALSE, G_PARAM_READWRITE)); 

        g_object_class_install_property (
		object_class,
		PROP_HIGHLIGHTED_AS_KEYBOARD_FOCUS,
		g_param_spec_boolean ("highlighted_as_keyboard_focus",
				      "highlighted as keyboard focus",
				      "whether we are highlighted to render keyboard focus",
				      FALSE, G_PARAM_READWRITE)); 


        g_object_class_install_property (
		object_class,
		PROP_HIGHLIGHTED_FOR_DROP,
		g_param_spec_boolean ("highlighted_for_drop",
				      "highlighted for drop",
				      "whether we are highlighted for a D&D drop",
				      FALSE, G_PARAM_READWRITE));

	g_object_class_install_property (
		object_class,
		PROP_HIGHLIGHTED_FOR_CLIPBOARD,
		g_param_spec_boolean ("highlighted_for_clipboard",
				      "highlighted for clipboard",
				      "whether we are highlighted for a clipboard paste (after we have been cut)",
 				      FALSE, G_PARAM_READWRITE));

	item_class->update = peony_icon_canvas_item_update;
	item_class->draw = peony_icon_canvas_item_draw;
	item_class->point = peony_icon_canvas_item_point;
	item_class->translate = peony_icon_canvas_item_translate;
	item_class->bounds = peony_icon_canvas_item_bounds;
	item_class->event = peony_icon_canvas_item_event;

	atk_registry_set_factory_type (atk_get_default_registry (),
				       PEONY_TYPE_ICON_CANVAS_ITEM,
				       peony_icon_canvas_item_accessible_factory_get_type ());

	g_type_class_add_private (class, sizeof (PeonyIconCanvasItemDetails));
}

static GailTextUtil *
peony_icon_canvas_item_get_text (GObject *text)
{
	return PEONY_ICON_CANVAS_ITEM (text)->details->text_util;
}

static void
peony_icon_canvas_item_text_interface_init (EelAccessibleTextIface *iface)
{
	iface->get_text = peony_icon_canvas_item_get_text;
}

/* ============================= a11y interfaces =========================== */

static const char *peony_icon_canvas_item_accessible_action_names[] = {
        "open",
        "menu",
        NULL
};

static const char *peony_icon_canvas_item_accessible_action_descriptions[] = {
        "Open item",
        "Popup context menu",
        NULL
};

enum {
	ACTION_OPEN,
	ACTION_MENU,
	LAST_ACTION
};

typedef struct {
        char *action_descriptions[LAST_ACTION];
	char *image_description;
	char *description;
} PeonyIconCanvasItemAccessiblePrivate;

typedef struct {
	PeonyIconCanvasItem *item;
	gint action_number;
} PeonyIconCanvasItemAccessibleActionContext;

static GType peony_icon_canvas_item_accessible_get_type (void);

#define GET_PRIV(o) G_TYPE_INSTANCE_GET_PRIVATE(o, peony_icon_canvas_item_accessible_get_type (), PeonyIconCanvasItemAccessiblePrivate);

/* accessible AtkAction interface */

static gboolean
peony_icon_canvas_item_accessible_idle_do_action (gpointer data)
{
    PeonyIconCanvasItem *item;
    PeonyIconCanvasItemAccessibleActionContext *ctx;
    PeonyIcon *icon;
    PeonyIconContainer *container;
    GList* selection;
    GList file_list;
    GdkEventButton button_event = { 0 };
    gint action_number;

    container = PEONY_ICON_CONTAINER (data);
    container->details->a11y_item_action_idle_handler = 0;
    while (!g_queue_is_empty (container->details->a11y_item_action_queue))
    {
        ctx = g_queue_pop_head (container->details->a11y_item_action_queue);
        action_number = ctx->action_number;
        item = ctx->item;
        g_free (ctx);
        icon = item->user_data;

        switch (action_number)
        {
        case ACTION_OPEN:
            file_list.data = icon->data;
            file_list.next = NULL;
            file_list.prev = NULL;
            g_signal_emit_by_name (container, "activate", &file_list);
            break;
        case ACTION_MENU:
            selection = peony_icon_container_get_selection (container);
            if (selection == NULL ||
                    g_list_length (selection) != 1 ||
                    selection->data != icon->data)
            {
                g_list_free (selection);
                return FALSE;
            }
            g_list_free (selection);
            g_signal_emit_by_name (container, "context_click_selection", &button_event);
            break;
        default :
            g_assert_not_reached ();
            break;
        }
    }
    return FALSE;
}

static gboolean
peony_icon_canvas_item_accessible_do_action (AtkAction *accessible, int i)
{
    PeonyIconCanvasItem *item;
    PeonyIconCanvasItemAccessibleActionContext *ctx;
    PeonyIconContainer *container;

    g_assert (i < LAST_ACTION);

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (accessible)));
    if (!item)
    {
        return FALSE;
    }
    container = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas);
    switch (i)
    {
    case ACTION_OPEN:
    case ACTION_MENU:
        if (container->details->a11y_item_action_queue == NULL)
        {
            container->details->a11y_item_action_queue = g_queue_new ();
        }
        ctx = g_new (PeonyIconCanvasItemAccessibleActionContext, 1);
        ctx->action_number = i;
        ctx->item = item;
        g_queue_push_head (container->details->a11y_item_action_queue, ctx);
        if (container->details->a11y_item_action_idle_handler == 0)
        {
            container->details->a11y_item_action_idle_handler = g_idle_add (peony_icon_canvas_item_accessible_idle_do_action, container);
        }
        break;
    default :
        g_warning ("Invalid action passed to PeonyIconCanvasItemAccessible::do_action");
        return FALSE;
    }

    return TRUE;
}

static int
peony_icon_canvas_item_accessible_get_n_actions (AtkAction *accessible)
{
    return LAST_ACTION;
}

static const char *
peony_icon_canvas_item_accessible_action_get_description (AtkAction *accessible,
        int i)
{
    PeonyIconCanvasItemAccessiblePrivate *priv;

    g_assert (i < LAST_ACTION);

    priv = GET_PRIV (accessible);
    if (priv->action_descriptions[i])
    {
        return priv->action_descriptions[i];
    }
    else
    {
        return peony_icon_canvas_item_accessible_action_descriptions[i];
    }
}

static const char *
peony_icon_canvas_item_accessible_action_get_name (AtkAction *accessible, int i)
{
    g_assert (i < LAST_ACTION);

    return peony_icon_canvas_item_accessible_action_names[i];
}

static const char *
peony_icon_canvas_item_accessible_action_get_keybinding (AtkAction *accessible,
        int i)
{
    g_assert (i < LAST_ACTION);

    return NULL;
}

static gboolean
peony_icon_canvas_item_accessible_action_set_description (AtkAction *accessible,
        int i,
        const char *description)
{
    PeonyIconCanvasItemAccessiblePrivate *priv;

    g_assert (i < LAST_ACTION);

    priv = GET_PRIV (accessible);

    if (priv->action_descriptions[i])
    {
        g_free (priv->action_descriptions[i]);
    }
    priv->action_descriptions[i] = g_strdup (description);

    return TRUE;
}

static void
peony_icon_canvas_item_accessible_action_interface_init (AtkActionIface *iface)
{
    iface->do_action = peony_icon_canvas_item_accessible_do_action;
    iface->get_n_actions = peony_icon_canvas_item_accessible_get_n_actions;
    iface->get_description = peony_icon_canvas_item_accessible_action_get_description;
    iface->get_keybinding = peony_icon_canvas_item_accessible_action_get_keybinding;
    iface->get_name = peony_icon_canvas_item_accessible_action_get_name;
    iface->set_description = peony_icon_canvas_item_accessible_action_set_description;
}

static const gchar* peony_icon_canvas_item_accessible_get_name(AtkObject* accessible)
{
    PeonyIconCanvasItem* item;

    if (accessible->name)
    {
        return accessible->name;
    }

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (accessible)));

    if (!item)
    {
        return NULL;
    }

    return item->details->editable_text;
}

static const gchar* peony_icon_canvas_item_accessible_get_description(AtkObject* accessible)
{
    PeonyIconCanvasItem* item;

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (accessible)));

    if (!item)
    {
        return NULL;
    }

    return item->details->additional_text;
}

static AtkObject *
peony_icon_canvas_item_accessible_get_parent (AtkObject *accessible)
{
    PeonyIconCanvasItem *item;

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (accessible)));
    if (!item)
    {
        return NULL;
    }

    return gtk_widget_get_accessible (GTK_WIDGET (EEL_CANVAS_ITEM (item)->canvas));
}

static int
peony_icon_canvas_item_accessible_get_index_in_parent (AtkObject *accessible)
{
    PeonyIconCanvasItem *item;
    PeonyIconContainer *container;
    GList *l;
    PeonyIcon *icon;
    int i;

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (accessible)));
    if (!item)
    {
        return -1;
    }

    container = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas);

    l = container->details->icons;
    i = 0;
    while (l)
    {
        icon = l->data;

        if (icon->item == item)
        {
            return i;
        }

        i++;
        l = l->next;
    }

    return -1;
}


static const gchar* peony_icon_canvas_item_accessible_get_image_description(AtkImage* image)
{
    PeonyIconCanvasItemAccessiblePrivate* priv;
    PeonyIconCanvasItem* item;
    PeonyIcon* icon;
    PeonyIconContainer* container;
    char* description;

    priv = GET_PRIV (image);

    if (priv->image_description)
    {
        return priv->image_description;
    }
    else
    {
        item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (image)));

        if (item == NULL)
        {
            return NULL;
        }

        icon = item->user_data;
        container = PEONY_ICON_CONTAINER(EEL_CANVAS_ITEM(item)->canvas);
        description = peony_icon_container_get_icon_description(container, icon->data);
        g_free(priv->description);
        priv->description = description;

        return priv->description;
    }
}

static void
peony_icon_canvas_item_accessible_get_image_size
(AtkImage *image,
 gint     *width,
 gint     *height)
{
    PeonyIconCanvasItem *item;

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (image)));

    if (!item || !item->details->pixbuf)
    {
        *width = *height = 0;
    }
    else
    {
        *width = gdk_pixbuf_get_width (item->details->pixbuf);
        *height = gdk_pixbuf_get_height (item->details->pixbuf);
    }
}

static void
peony_icon_canvas_item_accessible_get_image_position
(AtkImage		 *image,
 gint                    *x,
 gint	                 *y,
 AtkCoordType	         coord_type)
{
    PeonyIconCanvasItem *item;
    gint x_offset, y_offset, itmp;

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (image)));
    if (!item)
    {
        return;
    }
    if (!item->details->canvas_rect.x0 && !item->details->canvas_rect.x1)
    {
        return;
    }
    else
    {
        x_offset = 0;
        y_offset = 0;
        if (item->details->text_width)
        {
            itmp = item->details->canvas_rect.x0 -
                   item->details->text_rect.x0;
            if (itmp > x_offset)
            {
                x_offset = itmp;
            }
            itmp = item->details->canvas_rect.y0 -
                   item->details->text_rect.y0;
            if (itmp > y_offset)
            {
                y_offset = itmp;
            }
        }
        if (item->details->emblem_pixbufs)
        {
            itmp = item->details->canvas_rect.x0 -
                   item->details->emblem_rect.x0;
            if (itmp > x_offset)
            {
                x_offset = itmp;
            }
            itmp = item->details->canvas_rect.y0 -
                   item->details->emblem_rect.y0;
            if (itmp > y_offset)
            {
                y_offset = itmp;
            }
        }
    }
    atk_component_get_position (ATK_COMPONENT (image), x, y, coord_type);
    *x += x_offset;
    *y += y_offset;
}

static gboolean
peony_icon_canvas_item_accessible_set_image_description
(AtkImage    *image,
 const gchar *description)
{
    PeonyIconCanvasItemAccessiblePrivate *priv;

    priv = GET_PRIV (image);

    g_free (priv->image_description);
    priv->image_description = g_strdup (description);

    return TRUE;
}

static void
peony_icon_canvas_item_accessible_image_interface_init (AtkImageIface *iface)
{
    iface->get_image_description = peony_icon_canvas_item_accessible_get_image_description;
    iface->set_image_description = peony_icon_canvas_item_accessible_set_image_description;
    iface->get_image_size        = peony_icon_canvas_item_accessible_get_image_size;
    iface->get_image_position    = peony_icon_canvas_item_accessible_get_image_position;
}

static gint
peony_icon_canvas_item_accessible_get_offset_at_point (AtkText	 *text,
        gint           x,
        gint           y,
        AtkCoordType coords)
{
    gint real_x, real_y, real_width, real_height;
    PeonyIconCanvasItem *item;
    gint editable_height;
    gint offset = 0;
    gint index;
    PangoLayout *layout, *editable_layout, *additional_layout;
    PangoRectangle rect0;
    char *icon_text;
    gboolean have_editable;
    gboolean have_additional;
    gint text_offset;

    atk_component_get_extents (ATK_COMPONENT (text), &real_x, &real_y,
                               &real_width, &real_height, coords);

    x -= real_x;
    y -= real_y;

    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (text)));

    if (item->details->pixbuf)
    {
        y -= gdk_pixbuf_get_height (item->details->pixbuf);
    }
    have_editable = item->details->editable_text != NULL &&
                    item->details->editable_text[0] != '\0';
    have_additional = item->details->additional_text != NULL &&item->details->additional_text[0] != '\0';

    editable_layout = NULL;
    additional_layout = NULL;
    if (have_editable)
    {
        editable_layout = get_label_layout (&item->details->editable_text_layout, item, item->details->editable_text);
        prepare_pango_layout_for_draw (item, editable_layout);
        pango_layout_get_pixel_size (editable_layout, NULL, &editable_height);
        if (y >= editable_height &&
                have_additional)
        {
            prepare_pango_layout_for_draw (item, editable_layout);
            additional_layout = get_label_layout (&item->details->additional_text_layout, item, item->details->additional_text);
            layout = additional_layout;
            icon_text = item->details->additional_text;
            y -= editable_height + LABEL_LINE_SPACING;
        }
        else
        {
            layout = editable_layout;
            icon_text = item->details->editable_text;
        }
    }
    else if (have_additional)
    {
        additional_layout = get_label_layout (&item->details->additional_text_layout, item, item->details->additional_text);
        prepare_pango_layout_for_draw (item, additional_layout);
        layout = additional_layout;
        icon_text = item->details->additional_text;
    }
    else
    {
        return 0;
    }

    text_offset = 0;
    if (have_editable)
    {
        pango_layout_index_to_pos (editable_layout, 0, &rect0);
        text_offset = PANGO_PIXELS (rect0.x);
    }
    if (have_additional)
    {
        gint itmp;

        pango_layout_index_to_pos (additional_layout, 0, &rect0);
        itmp = PANGO_PIXELS (rect0.x);
        if (itmp < text_offset)
        {
            text_offset = itmp;
        }
    }
    pango_layout_index_to_pos (layout, 0, &rect0);
    x += text_offset;
    if (!pango_layout_xy_to_index (layout,
                                   x * PANGO_SCALE,
                                   y * PANGO_SCALE,
                                   &index, NULL))
    {
        if (x < 0 || y < 0)
        {
            index = 0;
        }
        else
        {
            index = -1;
        }
    }
    if (index == -1)
    {
        offset = g_utf8_strlen (icon_text, -1);
    }
    else
    {
        offset = g_utf8_pointer_to_offset (icon_text, icon_text + index);
    }
    if (layout == additional_layout)
    {
        offset += g_utf8_strlen (item->details->editable_text, -1);
    }

    if (editable_layout != NULL)
    {
        g_object_unref (editable_layout);
    }

    if (additional_layout != NULL)
    {
        g_object_unref (additional_layout);
    }

    return offset;
}

static void
peony_icon_canvas_item_accessible_get_character_extents (AtkText	   *text,
        gint	   offset,
        gint	   *x,
        gint	   *y,
        gint	   *width,
        gint	   *height,
        AtkCoordType coords)
{
    gint pos_x, pos_y;
    gint len, byte_offset;
    gint editable_height;
    gchar *icon_text;
    PeonyIconCanvasItem *item;
    PangoLayout *layout, *editable_layout, *additional_layout;
    PangoRectangle rect;
    PangoRectangle rect0;
    gboolean have_editable;
    gint text_offset;

    atk_component_get_position (ATK_COMPONENT (text), &pos_x, &pos_y, coords);
    item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (text)));

    if (item->details->pixbuf)
    {
        pos_y += gdk_pixbuf_get_height (item->details->pixbuf);
    }

    have_editable = item->details->editable_text != NULL &&
                    item->details->editable_text[0] != '\0';
    if (have_editable)
    {
        len = g_utf8_strlen (item->details->editable_text, -1);
    }
    else
    {
        len = 0;
    }

    editable_layout = get_label_layout (&item->details->editable_text_layout, item, item->details->editable_text);
    additional_layout = get_label_layout (&item->details->additional_text_layout, item, item->details->additional_text);

    if (offset < len)
    {
        icon_text = item->details->editable_text;
        layout = editable_layout;
    }
    else
    {
        offset -= len;
        icon_text = item->details->additional_text;
        layout = additional_layout;
        pos_y += LABEL_LINE_SPACING;
        if (have_editable)
        {
            pango_layout_get_pixel_size (editable_layout, NULL, &editable_height);
            pos_y += editable_height;
        }
    }
    byte_offset = g_utf8_offset_to_pointer (icon_text, offset) - icon_text;
    pango_layout_index_to_pos (layout, byte_offset, &rect);
    text_offset = 0;
    if (have_editable)
    {
        pango_layout_index_to_pos (editable_layout, 0, &rect0);
        text_offset = PANGO_PIXELS (rect0.x);
    }
    if (item->details->additional_text != NULL &&
            item->details->additional_text[0] != '\0')
    {
        gint itmp;

        pango_layout_index_to_pos (additional_layout, 0, &rect0);
        itmp = PANGO_PIXELS (rect0.x);
        if (itmp < text_offset)
        {
            text_offset = itmp;
        }
    }

    g_object_unref (editable_layout);
    g_object_unref (additional_layout);

    *x = pos_x + PANGO_PIXELS (rect.x) - text_offset;
    *y = pos_y + PANGO_PIXELS (rect.y);
    *width = PANGO_PIXELS (rect.width);
    *height = PANGO_PIXELS (rect.height);
}

static void
peony_icon_canvas_item_accessible_text_interface_init (AtkTextIface *iface)
{
    iface->get_text                = eel_accessibility_text_get_text;
    iface->get_character_at_offset = eel_accessibility_text_get_character_at_offset;
    iface->get_text_before_offset  = eel_accessibility_text_get_text_before_offset;
    iface->get_text_at_offset      = eel_accessibility_text_get_text_at_offset;
    iface->get_text_after_offset   = eel_accessibility_text_get_text_after_offset;
    iface->get_character_count     = eel_accessibility_text_get_character_count;
    iface->get_character_extents   = peony_icon_canvas_item_accessible_get_character_extents;
    iface->get_offset_at_point     = peony_icon_canvas_item_accessible_get_offset_at_point;
}

typedef struct {
	EelCanvasItemAccessible parent;
} PeonyIconCanvasItemAccessible;

typedef struct {
	EelCanvasItemAccessibleClass parent_class;
} PeonyIconCanvasItemAccessibleClass;

G_DEFINE_TYPE_WITH_CODE (PeonyIconCanvasItemAccessible,
			 peony_icon_canvas_item_accessible,
			 eel_canvas_item_accessible_get_type (),
			 G_IMPLEMENT_INTERFACE (ATK_TYPE_IMAGE,
						peony_icon_canvas_item_accessible_image_interface_init)
			 G_IMPLEMENT_INTERFACE (ATK_TYPE_TEXT,
						peony_icon_canvas_item_accessible_text_interface_init)
			 G_IMPLEMENT_INTERFACE (ATK_TYPE_ACTION,
						peony_icon_canvas_item_accessible_action_interface_init));

static AtkStateSet*
peony_icon_canvas_item_accessible_ref_state_set (AtkObject *accessible)
{
	AtkStateSet *state_set;
	PeonyIconCanvasItem *item;
	PeonyIconContainer *container;
	PeonyIcon *icon;
	GList *l;
	gboolean one_item_selected;

	state_set = ATK_OBJECT_CLASS (peony_icon_canvas_item_accessible_parent_class)->ref_state_set (accessible);

	item = PEONY_ICON_CANVAS_ITEM (atk_gobject_accessible_get_object (ATK_GOBJECT_ACCESSIBLE (accessible)));
	if (!item) {
		atk_state_set_add_state (state_set, ATK_STATE_DEFUNCT);
		return state_set;
	}
	container = PEONY_ICON_CONTAINER (EEL_CANVAS_ITEM (item)->canvas);
	if (item->details->is_highlighted_as_keyboard_focus) {
		atk_state_set_add_state (state_set, ATK_STATE_FOCUSED);
	} else if (!container->details->keyboard_focus) {

		one_item_selected = FALSE;
		l = container->details->icons;
		while (l) {
			icon = l->data;
		
			if (icon->item == item) {
				if (icon->is_selected) {
					one_item_selected = TRUE;
				} else {
					break;
				}
			} else if (icon->is_selected) {
				one_item_selected = FALSE;
				break;
			}

			l = l->next;
		}

		if (one_item_selected) {
			atk_state_set_add_state (state_set, ATK_STATE_FOCUSED);
		}
	}

	return state_set;
}

static void
peony_icon_canvas_item_accessible_finalize (GObject *object)
{
	PeonyIconCanvasItemAccessiblePrivate *priv;
	int i;

	priv = GET_PRIV (object);

	for (i = 0; i < LAST_ACTION; i++) {
		g_free (priv->action_descriptions[i]);
	}
	g_free (priv->image_description);
	g_free (priv->description);

        G_OBJECT_CLASS (peony_icon_canvas_item_accessible_parent_class)->finalize (object);
}

static void
peony_icon_canvas_item_accessible_initialize (AtkObject *accessible,
						 gpointer widget)
{
	ATK_OBJECT_CLASS (peony_icon_canvas_item_accessible_parent_class)->initialize (accessible, widget);

	atk_object_set_role (accessible, ATK_ROLE_ICON);
}

static void
peony_icon_canvas_item_accessible_class_init (PeonyIconCanvasItemAccessibleClass *klass)
{
	AtkObjectClass *aclass = ATK_OBJECT_CLASS (klass);
	GObjectClass *oclass = G_OBJECT_CLASS (klass);

	oclass->finalize = peony_icon_canvas_item_accessible_finalize;

	aclass->initialize = peony_icon_canvas_item_accessible_initialize;

	aclass->get_name = peony_icon_canvas_item_accessible_get_name;
	aclass->get_description = peony_icon_canvas_item_accessible_get_description;
	aclass->get_parent = peony_icon_canvas_item_accessible_get_parent;
	aclass->get_index_in_parent = peony_icon_canvas_item_accessible_get_index_in_parent;
	aclass->ref_state_set = peony_icon_canvas_item_accessible_ref_state_set;

	g_type_class_add_private (klass, sizeof (PeonyIconCanvasItemAccessiblePrivate));
}

static void
peony_icon_canvas_item_accessible_init (PeonyIconCanvasItemAccessible *self)
{
}

/* dummy typedef */
typedef AtkObjectFactory      PeonyIconCanvasItemAccessibleFactory;
typedef AtkObjectFactoryClass PeonyIconCanvasItemAccessibleFactoryClass;

G_DEFINE_TYPE (PeonyIconCanvasItemAccessibleFactory, peony_icon_canvas_item_accessible_factory,
	       ATK_TYPE_OBJECT_FACTORY);


static AtkObject *
peony_icon_canvas_item_accessible_factory_create_accessible (GObject *for_object)
{
    AtkObject *accessible;
    PeonyIconCanvasItem *item;
    GString *item_text;

    item = PEONY_ICON_CANVAS_ITEM (for_object);
    g_assert (item != NULL);

    item_text = g_string_new (NULL);
    if (item->details->editable_text)
    {
        g_string_append (item_text, item->details->editable_text);
    }
    if (item->details->additional_text)
    {
        if (item_text->len > 0)
            g_string_append_c (item_text, ' ');
        g_string_append (item_text, item->details->additional_text);
    }
    item->details->text_util = gail_text_util_new ();
    gail_text_util_text_setup (item->details->text_util,
                               item_text->str);
    g_string_free (item_text, TRUE);

    accessible = g_object_new (peony_icon_canvas_item_accessible_get_type (), NULL);
    atk_object_initialize (accessible, for_object);
    return accessible;
}

static GType
peony_icon_canvas_item_accessible_factory_get_accessible_type (void)
{
    return peony_icon_canvas_item_accessible_get_type ();
}

static void
peony_icon_canvas_item_accessible_factory_init (PeonyIconCanvasItemAccessibleFactory *self)
{
}

static void
peony_icon_canvas_item_accessible_factory_class_init (PeonyIconCanvasItemAccessibleFactoryClass *klass)
{
	klass->create_accessible = peony_icon_canvas_item_accessible_factory_create_accessible;
	klass->get_accessible_type = peony_icon_canvas_item_accessible_factory_get_accessible_type;
}

BRASERO_SCSI_COMMAND_DEFINE (BraseroRdTrackInfoCDB,
				 READ_TRACK_INFORMATION,
				 BRASERO_SCSI_READ);

typedef enum {
BRASERO_FIELD_LBA			= 0x00,
BRASERO_FIELD_TRACK_NUM			= 0x01,
BRASERO_FIELD_SESSION_NUM		= 0x02,
	/* reserved */
} BraseroFieldType;

static void
brasero_sg_command_setup (struct sg_io_hdr *transport,
			  guchar *sense_data,
			  BraseroScsiCmd *cmd,
			  guchar *buffer,
			  int size)
{
	memset (sense_data, 0, BRASERO_SENSE_DATA_SIZE);
	memset (transport, 0, sizeof (struct sg_io_hdr));
	
	transport->interface_id = 'S';				/* mandatory */
//	transport->flags = SG_FLAG_LUN_INHIBIT|SG_FLAG_DIRECT_IO;
	transport->cmdp = cmd->cmd;
	transport->cmd_len = cmd->info->size;
	transport->dxferp = buffer;
	transport->dxfer_len = size;

	/* where to output the scsi sense buffer */
	transport->sbp = sense_data;
	transport->mx_sb_len = BRASERO_SENSE_DATA_SIZE;

	if (cmd->info->direction & BRASERO_SCSI_READ)
		transport->dxfer_direction = SG_DXFER_FROM_DEV;
	else if (cmd->info->direction & BRASERO_SCSI_WRITE)
		transport->dxfer_direction = SG_DXFER_TO_DEV;
}

BraseroScsiResult
brasero_scsi_command_issue_sync (gpointer command,
				 gpointer buffer,
				 int size)
{
	guchar sense_buffer [BRASERO_SENSE_DATA_SIZE];
	struct sg_io_hdr transport;
	BraseroScsiResult res;
	BraseroScsiCmd *cmd;

	g_return_val_if_fail (command != NULL, BRASERO_SCSI_FAILURE);

	cmd = command;
	brasero_sg_command_setup (&transport,
				  sense_buffer,
				  cmd,
				  buffer,
				  size);

	/* NOTE on SG_IO: only for TEST UNIT READY, REQUEST/MODE SENSE, INQUIRY,
	 * READ CAPACITY, READ BUFFER, READ and LOG SENSE are allowed with it */
	res = ioctl (cmd->handle->fd, SG_IO, &transport);
	/*if (res) {
		BRASERO_SCSI_SET_ERRCODE (error, BRASERO_SCSI_ERRNO);
		return BRASERO_SCSI_FAILURE;
	}*/

	if ((transport.info & SG_INFO_OK_MASK) == SG_INFO_OK)
		return BRASERO_SCSI_OK;

	/*if ((transport.masked_status & CHECK_CONDITION) && transport.sb_len_wr)
		return brasero_sense_data_process (sense_buffer, error);*/

	return BRASERO_SCSI_FAILURE;
}

gpointer
brasero_scsi_command_new (const BraseroScsiCmdInfo *info,
			  BraseroDeviceHandle *handle) 
{
	BraseroScsiCmd *cmd;

	g_return_val_if_fail (handle != NULL, NULL);

	/* make sure we can set the flags of the descriptor */

	/* allocate the command */
	cmd = g_new0 (BraseroScsiCmd, 1);
	cmd->info = info;
	cmd->handle = handle;

	BRASERO_SCSI_CMD_SET_OPCODE (cmd);
	return cmd;
}

BraseroScsiResult
brasero_scsi_command_free (gpointer cmd)
{
	g_free (cmd);
	return BRASERO_SCSI_OK;
}

/**
 * This is to open a device
 */

BraseroDeviceHandle *
brasero_device_handle_open (const gchar *path,
				gboolean exclusive)
{
	int fd;
	int flags = OPEN_FLAGS;
	BraseroDeviceHandle *handle;

	if (exclusive)
		flags |= O_EXCL;

	fd = open (path, flags);
	if (fd < 0) {
		return NULL;
	}

	handle = g_new (BraseroDeviceHandle, 1);
	handle->fd = fd;

	return handle;
}

void
brasero_device_handle_close (BraseroDeviceHandle *handle)
{
	close (handle->fd);
	g_free (handle);
}

static BraseroScsiResult
brasero_read_track_info (BraseroRdTrackInfoCDB *cdb,
			 BraseroScsiTrackInfo *info,
			 int *size)
{
	BraseroScsiTrackInfo hdr;
	BraseroScsiResult res;
	int datasize;

	if (!info || !size) {
		return BRASERO_SCSI_FAILURE;
	}

	/* first ask the drive how long should the data be and then ... */
	datasize = 4;
	memset (&hdr, 0, sizeof (info));
	BRASERO_SET_16 (cdb->alloc_len, datasize);
	res = brasero_scsi_command_issue_sync (cdb, &hdr, datasize);
	if (res)
		return res;

	/* ... check the size in case of a buggy firmware ... */
	if (BRASERO_GET_16 (hdr.len) + sizeof (hdr.len) >= datasize) {
		datasize = BRASERO_GET_16 (hdr.len) + sizeof (hdr.len);

		if (datasize > *size) {
			/* it must not be over sizeof (BraseroScsiTrackInfo) */
			if (datasize > sizeof (BraseroScsiTrackInfo)) {
				datasize = *size;
			}
			else
				*size = datasize;
		}
		else if (*size < datasize) {
			*size = datasize;
		}
	}
	else {
		datasize = *size;
	}

	/* ... and re-issue the command */
	memset (info, 0, sizeof (BraseroScsiTrackInfo));
	BRASERO_SET_16 (cdb->alloc_len, datasize);
	res = brasero_scsi_command_issue_sync (cdb, info, datasize);
	if (res == BRASERO_SCSI_OK) {
		if (datasize != BRASERO_GET_16 (info->len) + sizeof (info->len))

		*size = MIN (datasize, BRASERO_GET_16 (info->len) + sizeof (info->len));
	}

	return res;
}


BraseroScsiResult
brasero_mmc1_read_track_info (BraseroDeviceHandle *handle,
				  int track_num,
				  BraseroScsiTrackInfo *track_info,
				  int *size)
{
	BraseroRdTrackInfoCDB *cdb;
	BraseroScsiResult res;

	g_return_val_if_fail (handle != NULL, BRASERO_SCSI_FAILURE);

	cdb = brasero_scsi_command_new (&info, handle);
	cdb->addr_num_type = BRASERO_FIELD_TRACK_NUM;
	BRASERO_SET_32 (cdb->blk_addr_trk_ses_num, track_num);

	res = brasero_read_track_info (cdb, track_info, size);
	brasero_scsi_command_free (cdb);

	return res;
}

BraseroScsiResult
brasero_mmc1_read_disc_information_std (BraseroDeviceHandle *handle,
					BraseroScsiDiscInfoStd **info_return,
					int *size)
{
	BraseroScsiDiscInfoStd std_info;
	BraseroScsiDiscInfoStd *buffer;
	BraseroRdDiscInfoCDB *cdb;
	BraseroScsiResult res;
	int request_size;
	int buffer_size;
	   BRASERO_SCSI_COMMAND_DEFINE (BraseroRdDiscInfoCDB,
					 READ_DISC_INFORMATION,
					 BRASERO_SCSI_READ);

	g_return_val_if_fail (handle != NULL, BRASERO_SCSI_FAILURE);

	if (!info_return || !size) {
		return BRASERO_SCSI_FAILURE;
	}

	cdb = brasero_scsi_command_new (&info, handle);
	cdb->data_type = BRASERO_DISC_INFO_STD;
	BRASERO_SET_16 (cdb->alloc_len, sizeof (BraseroScsiDiscInfoStd));

	memset (&std_info, 0, sizeof (BraseroScsiDiscInfoStd));
	res = brasero_scsi_command_issue_sync (cdb,
						   &std_info,
						   sizeof (BraseroScsiDiscInfoStd));
	if (res)
		goto end;

	request_size = BRASERO_GET_16 (std_info.len) + 
			   sizeof (std_info.len);
	
	buffer = (BraseroScsiDiscInfoStd *) g_new0 (guchar, request_size);

	BRASERO_SET_16 (cdb->alloc_len, request_size);
	res = brasero_scsi_command_issue_sync (cdb, buffer, request_size);
	if (res) {
		g_free (buffer);
		goto end;
	}

	buffer_size = BRASERO_GET_16 (buffer->len) +
			  sizeof (buffer->len);

	*info_return = buffer;
	*size = MIN (request_size, buffer_size);

end:

	brasero_scsi_command_free (cdb);
	return res;
}
