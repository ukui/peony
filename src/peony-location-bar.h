/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
 * Copyright (C) 2018, Tianjin KYLIN Information Technology Co., Ltd.
 * 
 * Peony is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Maciej Stachowiak <mjs@eazel.com>
 *         Ettore Perazzoli <ettore@gnu.org>
 * Modified by: liupeng <liupeng@kylinos.cn>
 */

/* peony-location-bar.h - Location bar for Peony
 */

#ifndef PEONY_LOCATION_BAR_H
#define PEONY_LOCATION_BAR_H

#include "peony-navigation-window.h"
#include "peony-navigation-window-pane.h"
#include <libpeony-private/peony-entry.h>
#include <gtk/gtk.h>

#define PEONY_TYPE_LOCATION_BAR peony_location_bar_get_type()
#define PEONY_LOCATION_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_LOCATION_BAR, PeonyLocationBar))
#define PEONY_LOCATION_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_LOCATION_BAR, PeonyLocationBarClass))
#define PEONY_IS_LOCATION_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_LOCATION_BAR))
#define PEONY_IS_LOCATION_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_LOCATION_BAR))
#define PEONY_LOCATION_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_LOCATION_BAR, PeonyLocationBarClass))

typedef struct PeonyLocationBarDetails PeonyLocationBarDetails;

typedef struct PeonyLocationBar
{
    GtkHBox parent;
    PeonyLocationBarDetails *details;
} PeonyLocationBar;

typedef struct
{
    GtkHBoxClass parent_class;

    /* for GtkBindingSet */
    void         (* cancel)           (PeonyLocationBar *bar);
} PeonyLocationBarClass;

GType      peony_location_bar_get_type     	(void);
GtkWidget* peony_location_bar_new          	(PeonyNavigationWindowPane *pane);
void       peony_location_bar_set_active     (PeonyLocationBar *location_bar,
        gboolean is_active);
PeonyEntry * peony_location_bar_get_entry (PeonyLocationBar *location_bar);

void    peony_location_bar_activate         (PeonyLocationBar *bar);
void    peony_location_bar_set_location     (PeonyLocationBar *bar,
                                            const char      *location);
void peony_set_location_bar_emit_flag(GtkWidget *widget,gboolean bEmit);
#endif /* PEONY_LOCATION_BAR_H */
