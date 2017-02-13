/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* fm-icon-container.h - the container widget for file manager icons

   Copyright (C) 2002 Sun Microsystems, Inc.

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

   Author: Michael Meeks <michael@ximian.com>
*/

#ifndef FM_COMPUTER_CONTAINER_H
#define FM_COMPUTER_CONTAINER_H

#include <libpeony-private/peony-icon-container.h>
#include "fm-computer-view.h"

typedef struct FMComputerContainer FMComputerContainer;
typedef struct FMComputerContainerClass FMComputerContainerClass;

#define FM_TYPE_COMPUTER_CONTAINER fm_computer_container_get_type()
#define FM_COMPUTER_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), FM_TYPE_COMPUTER_CONTAINER, FMComputerContainer))
#define FM_COMPUTER_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), FM_TYPE_COMPUTER_CONTAINER, FMComputerContainerClass))
#define FM_IS_COMPUTER_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FM_TYPE_COMPUTER_CONTAINER))
#define FM_IS_COMPUTER_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), FM_TYPE_COMPUTER_CONTAINER))
#define FM_COMPUTER_CONTAINER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), FM_TYPE_COMPUTER_CONTAINER, FMComputerContainerClass))

typedef struct FMComputerContainerDetails FMComputerContainerDetails;

struct FMComputerContainer
{
    PeonyIconContainer parent;

    FMComputerView *view;
    gboolean    sort_for_desktop;
};

struct FMComputerContainerClass
{
    PeonyIconContainerClass parent_class;
};

GType                  fm_computer_container_get_type         (void);
PeonyIconContainer *fm_computer_container_construct        (FMComputerContainer *icon_container,
        FMComputerView      *view);
PeonyIconContainer  *fm_computer_container_new              (FMComputerView      *view);
void                   fm_computer_container_set_sort_desktop (FMComputerContainer *container,
        gboolean         desktop);

#endif /* FM_COMPUTER_CONTAINER_H */
