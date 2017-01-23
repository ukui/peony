/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-metadata.h: #defines and other metadata-related info

   Copyright (C) 2000 Eazel, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Author: John Sullivan <sullivan@eazel.com>
*/

#ifndef PEONY_METADATA_H
#define PEONY_METADATA_H

/* Keys for getting/setting Peony metadata. All metadata used in Peony
 * should define its key here, so we can keep track of the whole set easily.
 * Any updates here needs to be added in peony-metadata.c too.
 */

#include <glib.h>

/* Per-file */

#define PEONY_METADATA_KEY_DEFAULT_VIEW		 	"peony-default-view"

#define PEONY_METADATA_KEY_LOCATION_BACKGROUND_COLOR 	"folder-background-color"
#define PEONY_METADATA_KEY_LOCATION_BACKGROUND_IMAGE 	"folder-background-image"

#define PEONY_METADATA_KEY_ICON_VIEW_ZOOM_LEVEL       	"peony-icon-view-zoom-level"
#define PEONY_METADATA_KEY_ICON_VIEW_AUTO_LAYOUT      	"peony-icon-view-auto-layout"
#define PEONY_METADATA_KEY_ICON_VIEW_TIGHTER_LAYOUT      	"peony-icon-view-tighter-layout"
#define PEONY_METADATA_KEY_ICON_VIEW_SORT_BY          	"peony-icon-view-sort-by"
#define PEONY_METADATA_KEY_ICON_VIEW_SORT_REVERSED    	"peony-icon-view-sort-reversed"
#define PEONY_METADATA_KEY_ICON_VIEW_KEEP_ALIGNED            "peony-icon-view-keep-aligned"
#define PEONY_METADATA_KEY_ICON_VIEW_LAYOUT_TIMESTAMP	"peony-icon-view-layout-timestamp"

#define PEONY_METADATA_KEY_LIST_VIEW_ZOOM_LEVEL       	"peony-list-view-zoom-level"
#define PEONY_METADATA_KEY_LIST_VIEW_SORT_COLUMN      	"peony-list-view-sort-column"
#define PEONY_METADATA_KEY_LIST_VIEW_SORT_REVERSED    	"peony-list-view-sort-reversed"
#define PEONY_METADATA_KEY_LIST_VIEW_VISIBLE_COLUMNS    	"peony-list-view-visible-columns"
#define PEONY_METADATA_KEY_LIST_VIEW_COLUMN_ORDER    	"peony-list-view-column-order"

#define PEONY_METADATA_KEY_COMPACT_VIEW_ZOOM_LEVEL		"peony-compact-view-zoom-level"

#define PEONY_METADATA_KEY_WINDOW_GEOMETRY			"peony-window-geometry"
#define PEONY_METADATA_KEY_WINDOW_SCROLL_POSITION		"peony-window-scroll-position"
#define PEONY_METADATA_KEY_WINDOW_SHOW_HIDDEN_FILES		"peony-window-show-hidden-files"
#define PEONY_METADATA_KEY_WINDOW_MAXIMIZED			"peony-window-maximized"
#define PEONY_METADATA_KEY_WINDOW_STICKY			"peony-window-sticky"
#define PEONY_METADATA_KEY_WINDOW_KEEP_ABOVE			"peony-window-keep-above"

#define PEONY_METADATA_KEY_SIDEBAR_BACKGROUND_COLOR   	"peony-sidebar-background-color"
#define PEONY_METADATA_KEY_SIDEBAR_BACKGROUND_IMAGE   	"peony-sidebar-background-image"
#define PEONY_METADATA_KEY_SIDEBAR_BUTTONS			"peony-sidebar-buttons"

#define PEONY_METADATA_KEY_ICON_POSITION              	"peony-icon-position"
#define PEONY_METADATA_KEY_ICON_POSITION_TIMESTAMP		"peony-icon-position-timestamp"
#define PEONY_METADATA_KEY_ANNOTATION                 	"annotation"
#define PEONY_METADATA_KEY_ICON_SCALE                 	"icon-scale"
#define PEONY_METADATA_KEY_CUSTOM_ICON                	"custom-icon"
#define PEONY_METADATA_KEY_SCREEN				"screen"
#define PEONY_METADATA_KEY_EMBLEMS				"emblems"

guint peony_metadata_get_id (const char *metadata);

#endif /* PEONY_METADATA_H */
