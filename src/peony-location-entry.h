/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Peony
 *
 * Copyright (C) 2000 Eazel, Inc.
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
 */

#ifndef PEONY_LOCATION_ENTRY_H
#define PEONY_LOCATION_ENTRY_H

#include <libpeony-private/peony-entry.h>

#define PEONY_TYPE_LOCATION_ENTRY peony_location_entry_get_type()
#define PEONY_LOCATION_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_LOCATION_ENTRY, PeonyLocationEntry))
#define PEONY_LOCATION_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_LOCATION_ENTRY, PeonyLocationEntryClass))
#define PEONY_IS_LOCATION_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_LOCATION_ENTRY))
#define PEONY_IS_LOCATION_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_LOCATION_ENTRY))
#define PEONY_LOCATION_ENTRY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_LOCATION_ENTRY, PeonyLocationEntryClass))

typedef struct PeonyLocationEntryDetails PeonyLocationEntryDetails;

typedef struct PeonyLocationEntry
{
    PeonyEntry parent;
    PeonyLocationEntryDetails *details;
} PeonyLocationEntry;

typedef struct
{
    PeonyEntryClass parent_class;
} PeonyLocationEntryClass;

typedef enum
{
    PEONY_LOCATION_ENTRY_ACTION_GOTO,
    PEONY_LOCATION_ENTRY_ACTION_CLEAR
} PeonyLocationEntryAction;

GType      peony_location_entry_get_type     	(void);
GtkWidget* peony_location_entry_new          	(void);
void       peony_location_entry_set_special_text     (PeonyLocationEntry *entry,
        const char            *special_text);
void       peony_location_entry_set_secondary_action (PeonyLocationEntry *entry,
        PeonyLocationEntryAction secondary_action);
void       peony_location_entry_update_current_location (PeonyLocationEntry *entry,
        const char *path);

#endif /* PEONY_LOCATION_ENTRY_H */
