/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 *  Peony
 *
 *  Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author : Mr Jamie McCracken (jamiemcc at blueyonder dot co dot uk)
 *
 */
#ifndef _PEONY_PLACES_SIDEBAR_H
#define _PEONY_PLACES_SIDEBAR_H

#include <libpeony-private/peony-view.h>
#include <libpeony-private/peony-window-info.h>
#include <gtk/gtk.h>

#define PEONY_PLACES_SIDEBAR_ID    "places"

#define PEONY_TYPE_PLACES_SIDEBAR peony_places_sidebar_get_type()
#define PEONY_PLACES_SIDEBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_PLACES_SIDEBAR, PeonyPlacesSidebar))
#define PEONY_PLACES_SIDEBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_PLACES_SIDEBAR, PeonyPlacesSidebarClass))
#define PEONY_IS_PLACES_SIDEBAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_PLACES_SIDEBAR))
#define PEONY_IS_PLACES_SIDEBAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_PLACES_SIDEBAR))
#define PEONY_PLACES_SIDEBAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_PLACES_SIDEBAR, PeonyPlacesSidebarClass))


GType peony_places_sidebar_get_type (void);
void peony_places_sidebar_register (void);

#endif
