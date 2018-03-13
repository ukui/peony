/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* Peony - Icon canvas item class for icon container.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
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

#ifndef PEONY_ICON_CANVAS_ITEM_H
#define PEONY_ICON_CANVAS_ITEM_H

#include <eel/eel-canvas.h>
#include <eel/eel-art-extensions.h>
#include <sys/stat.h>
#include <scsi/scsi.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int x;
	int y;
	int width;
	int height;
}RECT_PARAM_T;

#define LAYOUT_PAD_LEFT 4
#define LAYOUT_PAD_TOP 4

#define PEONY_TYPE_ICON_CANVAS_ITEM peony_icon_canvas_item_get_type()
#define PEONY_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_ICON_CANVAS_ITEM, PeonyIconCanvasItem))
#define PEONY_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_ICON_CANVAS_ITEM, PeonyIconCanvasItemClass))
#define PEONY_IS_ICON_CANVAS_ITEM(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_ICON_CANVAS_ITEM))
#define PEONY_IS_ICON_CANVAS_ITEM_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_ICON_CANVAS_ITEM))
#define PEONY_ICON_CANVAS_ITEM_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_ICON_CANVAS_ITEM, PeonyIconCanvasItemClass))
#define BRASERO_GET_16(data)		(((guchar)(data)[0]<<8)+(guchar)(data)[1])
#define BRASERO_GET_24(data)		(((guchar)(data)[0]<<16)+((guchar)(data)[1]<<8)+((guchar)(data)[2]))
#define BRASERO_GET_32(data)		(((guchar)(data)[0]<<24)+((guchar)(data)[1]<<16)+((guchar)(data)[2]<<8)+(guchar)(data)[3])
		
#define BRASERO_SET_16(data, num)	(data)[0]=(((num)>>8)&0xFF);(data)[1]=(guchar)((num)&0xFF)
#define BRASERO_SET_24(data, num)	(data)[0]=(guchar)(((num)>>16)&0xFF);(data)[1]=(guchar)(((num)>>8)&0xFF);(data)[2]=(guchar)((num)&0xFF);
#define BRASERO_SET_32(data, num)	(data)[0]=(guchar)(((num)>>24)&0xFF);(data)[1]=(guchar)(((num)>>16)&0xFF);(data)[2]=(guchar)(((num)>>8)&0xFF);(data)[3]=(guchar)((num)&0xFF)
#define BRASERO_FIRST_TRACK_IN_LAST_SESSION(info) (((info)->first_track_nb_lastses_high << 8) + (info)->first_track_nb_lastses_low)
	
#define BRASERO_READ_TRACK_INFORMATION_OPCODE	0x52
#define BRASERO_READ_DISC_INFORMATION_OPCODE		0x51
	
#define BRASERO_SCSI_CMD_MAX_LEN	16
	
	static char *device_cdrom[]={"/dev/sr0","/dev/sr1","/dev/sr2",
													"/dev/sr3","/dev/sr4","/dev/sr5",
													"/dev/sr6","/dev/sr7","/dev/sr8",
													"/dev/sr9"};
	static gboolean find_device(char* device)
	{
		int i;
		for(i=0;i<10;i++)
		{
			if(strcmp(device,device_cdrom[i])==0)
				return TRUE;
		}
		return FALSE;
	}
	
	typedef enum {
	BRASERO_SCSI_DATA_MODE_1			= 0x01,
	BRASERO_SCSI_DATA_MODE_2_XA 		= 0x02,
	BRASERO_SCSI_DATA_BLOCK_TYPE			= 0x0F
	} BraseroScsiDataMode;
	
	struct _BraseroScsiTrackInfo {
		guchar len			[2];
	
		guchar track_num_low;
		guchar session_num_low;
	
		guchar reserved0;
	
		guchar track_mode		:4;
		guchar copy 		:1;
		guchar damage			:1;
		guchar layer_jmp_rcd_status :2;
	
		guchar data_mode			:4;
		/* the 4 next bits indicate the track status */
		guchar fixed_packet 	:1;
		guchar packet			:1;
		guchar blank			:1;
		guchar reserved_track		:1;
	
		guchar next_wrt_address_valid	:1;
		guchar last_recorded_blk_valid	:1;
		guchar reserved1			:6;
	
		guchar start_lba			[4];
		guchar next_wrt_address 	[4];
		guchar free_blocks		[4];
		guchar packet_size		[4];
		guchar track_size		[4];
		guchar last_recorded_blk		[4];
	
		guchar track_num_high;
		guchar session_num_high;
	
		guchar reserved2			[2];		/* 36 bytes MMC1 */
	
		guchar rd_compat_lba		[4];		/* 40 bytes */
		guchar next_layer_jmp		[4];
		guchar last_layer_jmp		[4];		/* 48 bytes */
	};
	
	struct _BraseroScsiOPCEntry {
		guchar speed			[2];
		guchar opc			[6];
	};
	typedef struct _BraseroScsiOPCEntry BraseroScsiOPCEntry;
	
	
	struct _BraseroScsiDiscInfoStd {
		guchar len			[2];
	
		guchar status			:2;
		guchar last_session_state	:2;
		guchar erasable 		:1;
		guchar info_type			:3;
	
		guchar first_track_num;
		guchar sessions_num_low;
		guchar first_track_nb_lastses_low;
		guchar last_track_nb_lastses_low;
	
		guchar bg_format_status 	:2;
		guchar dbit 		:1;
		guchar reserved0			:1;
		guchar disc_app_code_valid	:1;
		guchar unrestricted_use 	:1;
		guchar disc_barcode_valid	:1;
		guchar disc_id_valid		:1;
	
		guchar disc_type;
	
		guchar sessions_num_high;
		guchar first_track_nb_lastses_high;
		guchar last_track_nb_lastses_high;
	
		guchar disc_id			[4];
		guchar last_session_leadin	[4];
	
		guchar last_possible_leadout_res;
		guchar last_possible_leadout_mn;
		guchar last_possible_leadout_sec;
		guchar last_possible_leadout_frame;
	
		guchar disc_barcode 	[8];
	
		guchar reserved1;
	
		guchar OPC_table_num;
		BraseroScsiOPCEntry opc_entry	[0];
	};
	
	struct _BraseroRdDiscInfoCDB {
		guchar opcode;
	
		guchar data_type		:3;
		guchar reserved0		:5;
	
		guchar reserved1		[5];
		guchar alloc_len		[2];
	
		guchar ctl;
	};
	
	struct _BraseroRdTrackInfoCDB {
		guchar opcode;
	
		guchar addr_num_type		:2;
		guchar open 		:1; /* MMC5 field only */
		guchar reserved0			:5;
	
		guchar blk_addr_trk_ses_num [4];
	
		guchar reserved1;
	
		guchar alloc_len			[2];
		guchar ctl;
	};
	struct _BraseroDeviceHandle {
		int fd;
	};
	typedef struct _BraseroDeviceHandle BraseroDeviceHandle; 
	
	
	typedef enum {
		BRASERO_SCSI_OK,
		BRASERO_SCSI_FAILURE,
		BRASERO_SCSI_RECOVERABLE,
	} BraseroScsiResult;
	
			
	typedef enum {
			BRASERO_SCSI_WRITE	= 1,
			BRASERO_SCSI_READ	= 1 << 1
	} BraseroScsiDirection;
		
	struct _BraseroScsiCmdInfo {
			int size;
			guchar opcode;
			BraseroScsiDirection direction;
	};
	typedef struct _BraseroScsiCmdInfo BraseroScsiCmdInfo;
	typedef struct _BraseroScsiTrackInfo BraseroScsiTrackInfo;
	typedef struct _BraseroRdTrackInfoCDB BraseroRdTrackInfoCDB;
	typedef struct _BraseroScsiDiscInfoStd BraseroScsiDiscInfoStd;
	typedef struct _BraseroRdDiscInfoCDB BraseroRdDiscInfoCDB;
	
	
	struct _BraseroScsiCmd {
		guchar cmd [BRASERO_SCSI_CMD_MAX_LEN];
		BraseroDeviceHandle *handle;
	
		const BraseroScsiCmdInfo *info;
	};
	typedef struct _BraseroScsiCmd BraseroScsiCmd;
	
	typedef enum {
	BRASERO_DISC_INFO_STD		= 0x00,
	BRASERO_DISC_INFO_TRACK_RES = 0x01,
	BRASERO_DISC_INFO_POW_RES	= 0x02,
		/* reserved */
	} BraseroDiscInfoType;
	
	
#define BRASERO_SCSI_COMMAND_DEFINE(cdb, name, direction)			\
		static const BraseroScsiCmdInfo info =						\
		{	/* SCSI commands always end by 1 byte of ctl */ 			\
			G_STRUCT_OFFSET (cdb, ctl) + 1, 					\
			BRASERO_##name##_OPCODE,						\
			direction								\
		}
#define BRASERO_SCSI_CMD_OPCODE_OFF			0
#define BRASERO_SCSI_CMD_SET_OPCODE(command)		(command->cmd [BRASERO_SCSI_CMD_OPCODE_OFF] = command->info->opcode)
		
#define OPEN_FLAGS			O_RDWR /*|O_EXCL */|O_NONBLOCK
#define BRASERO_SENSE_DATA_SIZE		19

    typedef struct PeonyIconCanvasItem PeonyIconCanvasItem;
    typedef struct PeonyIconCanvasItemClass PeonyIconCanvasItemClass;
    typedef struct PeonyIconCanvasItemDetails PeonyIconCanvasItemDetails;

    struct PeonyIconCanvasItem
    {
        EelCanvasItem item;
        PeonyIconCanvasItemDetails *details;
        gpointer user_data;
    };

    struct PeonyIconCanvasItemClass
    {
        EelCanvasItemClass parent_class;
    };

    /* not namespaced due to their length */
    typedef enum
    {
        BOUNDS_USAGE_FOR_LAYOUT,
        BOUNDS_USAGE_FOR_ENTIRE_ITEM,
        BOUNDS_USAGE_FOR_DISPLAY
    } PeonyIconCanvasItemBoundsUsage;

    /* GObject */
    GType       peony_icon_canvas_item_get_type                 (void);

    /* attributes */
    void        peony_icon_canvas_item_set_image                (PeonyIconCanvasItem       *item,
            GdkPixbuf                    *image);

    cairo_surface_t* peony_icon_canvas_item_get_drag_surface    (PeonyIconCanvasItem       *item);

    void        peony_icon_canvas_item_set_emblems              (PeonyIconCanvasItem       *item,
            GList                        *emblem_pixbufs);
    void        peony_icon_canvas_item_set_show_stretch_handles (PeonyIconCanvasItem       *item,
            gboolean                      show_stretch_handles);
    void        peony_icon_canvas_item_set_attach_points        (PeonyIconCanvasItem       *item,
            GdkPoint                     *attach_points,
            int                           n_attach_points);
    void        peony_icon_canvas_item_set_embedded_text_rect   (PeonyIconCanvasItem       *item,
            const GdkRectangle           *text_rect);
    void        peony_icon_canvas_item_set_embedded_text        (PeonyIconCanvasItem       *item,
            const char                   *text);
    double      peony_icon_canvas_item_get_max_text_width       (PeonyIconCanvasItem       *item);
    const char *peony_icon_canvas_item_get_editable_text        (PeonyIconCanvasItem       *icon_item);
    void        peony_icon_canvas_item_set_renaming             (PeonyIconCanvasItem       *icon_item,
            gboolean                      state);

    /* geometry and hit testing */
    gboolean    peony_icon_canvas_item_hit_test_rectangle       (PeonyIconCanvasItem       *item,
            EelIRect                      canvas_rect);
    gboolean    peony_icon_canvas_item_hit_test_stretch_handles (PeonyIconCanvasItem       *item,
            EelDPoint                     world_point,
            GtkCornerType                *corner);
    void        peony_icon_canvas_item_invalidate_label         (PeonyIconCanvasItem       *item);
    void        peony_icon_canvas_item_invalidate_label_size    (PeonyIconCanvasItem       *item);
    EelDRect    peony_icon_canvas_item_get_icon_rectangle       (const PeonyIconCanvasItem *item);
    EelDRect    peony_icon_canvas_item_get_text_rectangle       (PeonyIconCanvasItem       *item,
            gboolean                      for_layout);
    void        peony_icon_canvas_item_get_bounds_for_layout    (PeonyIconCanvasItem       *item,
            double *x1, double *y1, double *x2, double *y2);
    void        peony_icon_canvas_item_get_bounds_for_entire_item (PeonyIconCanvasItem       *item,
            double *x1, double *y1, double *x2, double *y2);
    void        peony_icon_canvas_item_update_bounds            (PeonyIconCanvasItem       *item,
            double i2w_dx, double i2w_dy);
    void        peony_icon_canvas_item_set_is_visible           (PeonyIconCanvasItem       *item,
            gboolean                      visible);
    /* whether the entire label text must be visible at all times */
    void        peony_icon_canvas_item_set_entire_text          (PeonyIconCanvasItem       *icon_item,
            gboolean                      entire_text);
    void    brasero_device_handle_close (BraseroDeviceHandle *handle);
    BraseroScsiResult   brasero_mmc1_read_track_info (BraseroDeviceHandle *handle,
			      int track_num,
			      BraseroScsiTrackInfo *track_info,
			      int *size);
    BraseroDeviceHandle *   brasero_device_handle_open (const gchar *path,
                    gboolean exclusive);
    BraseroScsiResult     brasero_scsi_command_free (gpointer cmd);
    BraseroScsiResult    brasero_scsi_command_issue_sync (gpointer command,
                     gpointer buffer,
                     int size);
    BraseroScsiResult brasero_mmc1_read_disc_information_std (BraseroDeviceHandle *handle,
					BraseroScsiDiscInfoStd **info_return,
					int *size);


    guint64 interface_get_disk_free_full(GFile * file);

    guint64 interface_get_disk_total_full(GFile * file);
	EelIRect get_compute_text_rectangle (const PeonyIconCanvasItem *item,
							gboolean canvas_coords,
							PeonyIconCanvasItemBoundsUsage usage);
	
	double get_pixels_per_unit (const PeonyIconCanvasItem *item);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_ICON_CANVAS_ITEM_H */
