/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-empty-view.h - interface for empty view of directory.

   Copyright (C) 2006 Free Software Foundation, Inc.

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

   Authors: Christian Neumair <chris@mate-de.org>
*/

#ifndef FM_COMPUTER_VIEW_H
#define FM_COMPUTER_VIEW_H

#include "fm-directory-view.h"

typedef struct FMComputerViewClass FMComputerViewClass;


#define FM_TYPE_COMPUTER_VIEW fm_computer_view_get_type()
#define FM_COMPUTER_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), FM_TYPE_COMPUTER_VIEW, FMComputerView))
#define FM_COMPUTER_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), FM_TYPE_COMPUTER_VIEW, FMComputerViewClass))
#define FM_IS_COMPUTER_VIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FM_TYPE_COMPUTER_VIEW))
#define FM_IS_COMPUTER_VIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), FM_TYPE_COMPUTER_VIEW))
#define FM_COMPUTER_VIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), FM_TYPE_COMPUTER_VIEW, FMComputerViewClass))

#define FM_COMPUTER_VIEW_ID "OAFIID:Peony_File_Manager_Computer_View"

typedef struct FMComputerViewDetails FMComputerViewDetails;

typedef struct
{
    FMDirectoryView parent_instance;
    FMComputerViewDetails *details;
} FMComputerView;

struct FMComputerViewClass
{
	FMDirectoryViewClass parent_class;
	    /* Methods that can be overriden for settings you don't want to come from metadata.
     */

    /* Note: get_directory_sort_by must return a string that can/will be g_freed.
     */
    char *	 (* get_directory_sort_by)       (FMComputerView *computer_view,
            PeonyFile *file);
    void     (* set_directory_sort_by)       (FMComputerView *computer_view,
            PeonyFile *file,
            const char* sort_by);

    gboolean (* get_directory_sort_reversed) (FMComputerView *computer_view,
            PeonyFile *file);
    void     (* set_directory_sort_reversed) (FMComputerView *computer_view,
            PeonyFile *file,
            gboolean sort_reversed);

    gboolean (* get_directory_auto_layout)   (FMComputerView *computer_view,
            PeonyFile *file);
    void     (* set_directory_auto_layout)   (FMComputerView *computer_view,
            PeonyFile *file,
            gboolean auto_layout);

    gboolean (* get_directory_tighter_layout) (FMComputerView *computer_view,
            PeonyFile *file);
    void     (* set_directory_tighter_layout)   (FMComputerView *computer_view,
            PeonyFile *file,
            gboolean tighter_layout);

    /* Override "clean_up" if your subclass has its own notion of where icons should be positioned */
    void	 (* clean_up)			 (FMComputerView *computer_view);

    /* supports_auto_layout is a function pointer that subclasses may
     * override to control whether or not the automatic layout options
     * should be enabled. The default implementation returns TRUE.
     */
    gboolean (* supports_auto_layout)	 (FMComputerView *view);

    /* supports_manual_layout is a function pointer that subclasses may
     * override to control whether or not the manual layout options
     * should be enabled. The default implementation returns TRUE iff
     * not in compact mode.
     */
    gboolean (* supports_manual_layout)	 (FMComputerView *view);

    /* supports_scaling is a function pointer that subclasses may
     * override to control whether or not the manual layout supports
     * scaling. The default implementation returns FALSE
     */
    gboolean (* supports_scaling)	 (FMComputerView *view);

    /* supports_auto_layout is a function pointer that subclasses may
     * override to control whether snap-to-grid mode
     * should be enabled. The default implementation returns FALSE.
     */
    gboolean (* supports_keep_aligned)	 (FMComputerView *view);

    /* supports_auto_layout is a function pointer that subclasses may
     * override to control whether snap-to-grid mode
     * should be enabled. The default implementation returns FALSE.
     */
    gboolean (* supports_labels_beside_icons)	 (FMComputerView *view);
} ;

GType fm_computer_view_get_type (void);
void  fm_computer_view_register (void);
int     fm_computer_view_compare_files (FMComputerView   *computer_view,
                                    PeonyFile *a,
                                    PeonyFile *b);


#endif /* FM_COMPUTER_VIEW_H */
