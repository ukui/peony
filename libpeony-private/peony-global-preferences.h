/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-global-preferences.h - Peony specific preference keys and
                                   functions.

   Copyright (C) 1999, 2000, 2001 Eazel, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Ramiro Estrugo <ramiro@eazel.com>
*/

#ifndef PEONY_GLOBAL_PREFERENCES_H
#define PEONY_GLOBAL_PREFERENCES_H

#include <gio/gio.h>

G_BEGIN_DECLS

/* Whether exit when last window destroyed */
#define PEONY_PREFERENCES_EXIT_WITH_LAST_WINDOW		"exit-with-last-window"

/* Desktop Background options */
#define PEONY_PREFERENCES_BACKGROUND_SET			"background-set"
#define PEONY_PREFERENCES_BACKGROUND_COLOR		"background-color"
#define PEONY_PREFERENCES_BACKGROUND_URI			"background-uri"

/* Side Pane Background options */
#define PEONY_PREFERENCES_SIDE_PANE_BACKGROUND_SET	"side-pane-background-set"
#define PEONY_PREFERENCES_SIDE_PANE_BACKGROUND_COLOR	"side-pane-background-color"
#define PEONY_PREFERENCES_SIDE_PANE_BACKGROUND_URI	"side-pane-background-uri"

/* Automount options */
#define PEONY_PREFERENCES_MEDIA_AUTOMOUNT		"automount"
#define PEONY_PREFERENCES_MEDIA_AUTOMOUNT_OPEN		"automount-open"

/* Autorun options */
#define PEONY_PREFERENCES_MEDIA_AUTORUN_NEVER			"autorun-never"
#define PEONY_PREFERENCES_MEDIA_AUTORUN_X_CONTENT_START_APP	"autorun-x-content-start-app"
#define PEONY_PREFERENCES_MEDIA_AUTORUN_X_CONTENT_IGNORE		"autorun-x-content-ignore"
#define PEONY_PREFERENCES_MEDIA_AUTORUN_X_CONTENT_OPEN_FOLDER	"autorun-x-content-open-folder"

/* Trash options */
#define PEONY_PREFERENCES_CONFIRM_TRASH			"confirm-trash"
#define PEONY_PREFERENCES_ENABLE_DELETE			"enable-delete"

/* Desktop options */
#define PEONY_PREFERENCES_DESKTOP_IS_HOME_DIR		"desktop-is-home-dir"

/* Display  */
#define PEONY_PREFERENCES_SHOW_HIDDEN_FILES  		"show-hidden-files"
#define PEONY_PREFERENCES_SHOW_ADVANCED_PERMISSIONS	"show-advanced-permissions"
#define PEONY_PREFERENCES_DATE_FORMAT			"date-format"
#define PEONY_PREFERENCES_USE_IEC_UNITS			"use-iec-units"

/* Mouse */
#define PEONY_PREFERENCES_MOUSE_USE_EXTRA_BUTTONS 	"mouse-use-extra-buttons"
#define PEONY_PREFERENCES_MOUSE_FORWARD_BUTTON		"mouse-forward-button"
#define PEONY_PREFERENCES_MOUSE_BACK_BUTTON		"mouse-back-button"

typedef enum
{
    PEONY_DATE_FORMAT_LOCALE,
    PEONY_DATE_FORMAT_ISO,
    PEONY_DATE_FORMAT_INFORMAL
} PeonyDateFormat;

typedef enum
{
    PEONY_NEW_TAB_POSITION_AFTER_CURRENT_TAB,
    PEONY_NEW_TAB_POSITION_END,
} PeonyNewTabPosition;

/* Sidebar panels  */
#define PEONY_PREFERENCES_TREE_SHOW_ONLY_DIRECTORIES         "show-only-directories"

/* Single/Double click preference  */
#define PEONY_PREFERENCES_CLICK_POLICY			"click-policy"

/* Activating executable text files */
#define PEONY_PREFERENCES_EXECUTABLE_TEXT_ACTIVATION		"executable-text-activation"

/* Installing new packages when unknown mime type activated */
#define PEONY_PREFERENCES_INSTALL_MIME_ACTIVATION		"install-mime-activation"

/* Spatial or browser mode */
#define PEONY_PREFERENCES_ALWAYS_USE_BROWSER       		"always-use-browser"
#define PEONY_PREFERENCES_NEW_TAB_POSITION       		"tabs-open-position"
#define PEONY_PREFERENCES_ALWAYS_USE_LOCATION_ENTRY			"always-use-location-entry"

/* Which views should be displayed for new windows */
#define PEONY_WINDOW_STATE_START_WITH_LOCATION_BAR			"start-with-location-bar"
#define PEONY_WINDOW_STATE_START_WITH_STATUS_BAR				"start-with-status-bar"
#define PEONY_WINDOW_STATE_START_WITH_SIDEBAR		 		"start-with-sidebar"
#define PEONY_WINDOW_STATE_START_WITH_TOOLBAR				"start-with-toolbar"
#define PEONY_WINDOW_STATE_SIDE_PANE_VIEW                    "side-pane-view"
#define PEONY_WINDOW_STATE_GEOMETRY 	"geometry"
#define PEONY_WINDOW_STATE_MAXIMIZED        "maximized"
#define PEONY_WINDOW_STATE_SIDEBAR_WIDTH  					"sidebar-width"

/* Sorting order */
#define PEONY_PREFERENCES_SORT_DIRECTORIES_FIRST		        "sort-directories-first"
#define PEONY_PREFERENCES_DEFAULT_SORT_ORDER			        "default-sort-order"
#define PEONY_PREFERENCES_DEFAULT_SORT_IN_REVERSE_ORDER	    "default-sort-in-reverse-order"

/* The default folder viewer - one of the two enums below */
#define PEONY_PREFERENCES_DEFAULT_FOLDER_VIEWER		"default-folder-viewer"

    enum
    {
        PEONY_DEFAULT_FOLDER_VIEWER_ICON_VIEW,
        PEONY_DEFAULT_FOLDER_VIEWER_COMPACT_VIEW,
        PEONY_DEFAULT_FOLDER_VIEWER_LIST_VIEW,
        PEONY_DEFAULT_FOLDER_VIEWER_OTHER
    };

    /* These IIDs are used by the preferences code and in peony-application.c */
#define PEONY_ICON_VIEW_IID		"OAFIID:Peony_File_Manager_Icon_View"
#define PEONY_COMPACT_VIEW_IID	"OAFIID:Peony_File_Manager_Compact_View"
#define PEONY_LIST_VIEW_IID		"OAFIID:Peony_File_Manager_List_View"


    /* Icon View */
#define PEONY_PREFERENCES_ICON_VIEW_DEFAULT_USE_TIGHTER_LAYOUT	    "default-use-tighter-layout"
#define PEONY_PREFERENCES_ICON_VIEW_DEFAULT_ZOOM_LEVEL		        "default-zoom-level"

#define PEONY_PREFERENCES_ICON_VIEW_LABELS_BESIDE_ICONS      	"labels-beside-icons"



    /* Which text attributes appear beneath icon names */
#define PEONY_PREFERENCES_ICON_VIEW_CAPTIONS				"captions"

    /* The default size for thumbnail icons */
#define PEONY_PREFERENCES_ICON_VIEW_THUMBNAIL_SIZE			"thumbnail-size"

    /* ellipsization preferences */
#define PEONY_PREFERENCES_ICON_VIEW_TEXT_ELLIPSIS_LIMIT		"text-ellipsis-limit"
#define PEONY_PREFERENCES_DESKTOP_TEXT_ELLIPSIS_LIMIT		"text-ellipsis-limit"

    /* Compact View */
#define PEONY_PREFERENCES_COMPACT_VIEW_DEFAULT_ZOOM_LEVEL		"default-zoom-level"
#define PEONY_PREFERENCES_COMPACT_VIEW_ALL_COLUMNS_SAME_WIDTH	"all-columns-have-same-width"

    /* List View */
#define PEONY_PREFERENCES_LIST_VIEW_DEFAULT_ZOOM_LEVEL		"default-zoom-level"
#define PEONY_PREFERENCES_LIST_VIEW_DEFAULT_VISIBLE_COLUMNS	"default-visible-columns"
#define PEONY_PREFERENCES_LIST_VIEW_DEFAULT_COLUMN_ORDER		"default-column-order"

    enum
    {
        PEONY_CLICK_POLICY_SINGLE,
        PEONY_CLICK_POLICY_DOUBLE
    };

    enum
    {
        PEONY_EXECUTABLE_TEXT_LAUNCH,
        PEONY_EXECUTABLE_TEXT_DISPLAY,
        PEONY_EXECUTABLE_TEXT_ASK
    };

    typedef enum
    {
        PEONY_SPEED_TRADEOFF_ALWAYS,
        PEONY_SPEED_TRADEOFF_LOCAL_ONLY,
        PEONY_SPEED_TRADEOFF_NEVER
    } PeonySpeedTradeoffValue;

#define PEONY_PREFERENCES_SHOW_TEXT_IN_ICONS		    "show-icon-text"
#define PEONY_PREFERENCES_SHOW_DIRECTORY_ITEM_COUNTS "show-directory-item-counts"
#define PEONY_PREFERENCES_SHOW_IMAGE_FILE_THUMBNAILS	"show-image-thumbnails"
#define PEONY_PREFERENCES_IMAGE_FILE_THUMBNAIL_LIMIT	"thumbnail-limit"
#define PEONY_PREFERENCES_PREVIEW_SOUND		        "preview-sound"

    typedef enum
    {
        PEONY_COMPLEX_SEARCH_BAR,
        PEONY_SIMPLE_SEARCH_BAR
    } PeonySearchBarMode;

#define PEONY_PREFERENCES_DESKTOP_FONT                  "font"
#define PEONY_PREFERENCES_DESKTOP_HOME_VISIBLE          "home-icon-visible"
#define PEONY_PREFERENCES_DESKTOP_HOME_NAME             "home-icon-name"
#define PEONY_PREFERENCES_DESKTOP_COMPUTER_VISIBLE      "computer-icon-visible"
#define PEONY_PREFERENCES_DESKTOP_COMPUTER_NAME         "computer-icon-name"
#define PEONY_PREFERENCES_DESKTOP_TRASH_VISIBLE         "trash-icon-visible"
#define PEONY_PREFERENCES_DESKTOP_TRASH_NAME            "trash-icon-name"
#define PEONY_PREFERENCES_DESKTOP_VOLUMES_VISIBLE       "volumes-visible"
#define PEONY_PREFERENCES_DESKTOP_NETWORK_VISIBLE       "network-icon-visible"
#define PEONY_PREFERENCES_DESKTOP_NETWORK_NAME          "network-icon-name"
#define PEONY_PREFERENCES_LOCKDOWN_COMMAND_LINE         "disable-command-line"
#define PEONY_PREFERENCES_DISABLED_EXTENSIONS           "disabled-extensions"

void peony_global_preferences_init                      (void);
char *peony_global_preferences_get_default_folder_viewer_preference_as_iid (void);

GSettings *peony_preferences;
GSettings *peony_media_preferences;
GSettings *peony_window_state;
GSettings *peony_icon_view_preferences;
GSettings *peony_desktop_preferences;
GSettings *peony_tree_sidebar_preferences;
GSettings *peony_compact_view_preferences;
GSettings *peony_list_view_preferences;
GSettings *peony_computer_view_preferences;
GSettings *peony_extension_preferences;

GSettings *ukui_background_preferences;
GSettings *ukui_lockdown_preferences;

G_END_DECLS

#endif /* PEONY_GLOBAL_PREFERENCES_H */
