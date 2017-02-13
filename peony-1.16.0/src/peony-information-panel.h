/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 1999, 2000 Eazel, Inc.
 *
 * Peony is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Peony is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Andy Hertzfeld <andy@eazel.com>
 *
 *  This is the header file for the index panel widget, which displays overview information
 *  in a vertical panel and hosts the meta-views.
 */

#ifndef PEONY_INFORMATION_PANEL_H
#define PEONY_INFORMATION_PANEL_H

#include <eel/eel-background-box.h>

#define PEONY_TYPE_INFORMATION_PANEL peony_information_panel_get_type()
#define PEONY_INFORMATION_PANEL(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_INFORMATION_PANEL, PeonyInformationPanel))
#define PEONY_INFORMATION_PANEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_INFORMATION_PANEL, PeonyInformationPanelClass))
#define PEONY_IS_INFORMATION_PANEL(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_INFORMATION_PANEL))
#define PEONY_IS_INFORMATION_PANEL_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_INFORMATION_PANEL))
#define PEONY_INFORMATION_PANEL_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_INFORMATION_PANEL, PeonyInformationPanelClass))

typedef struct PeonyInformationPanelDetails PeonyInformationPanelDetails;

#define PEONY_INFORMATION_PANEL_ID "information"

typedef struct
{
    EelBackgroundBox parent_slot;
    PeonyInformationPanelDetails *details;
} PeonyInformationPanel;

typedef struct
{
    EelBackgroundBoxClass parent_slot;

    void (*location_changed) (PeonyInformationPanel *information_panel,
                              const char *location);
} PeonyInformationPanelClass;

GType            peony_information_panel_get_type     (void);
void             peony_information_panel_register     (void);

#endif /* PEONY_INFORMATION_PANEL_H */
