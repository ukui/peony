/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* PeonyEntry: one-line text editing widget. This consists of bug fixes
 * and other improvements to GtkEntry, and all the changes could be rolled
 * into GtkEntry some day.
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Author: John Sullivan <sullivan@eazel.com>
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

#ifndef PEONY_ENTRY_H
#define PEONY_ENTRY_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PEONY_TYPE_ENTRY peony_entry_get_type()
#define PEONY_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_ENTRY, PeonyEntry))
#define PEONY_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_ENTRY, PeonyEntryClass))
#define PEONY_IS_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_ENTRY))
#define PEONY_IS_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_ENTRY))
#define PEONY_ENTRY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_ENTRY, PeonyEntryClass))

    typedef struct PeonyEntryDetails PeonyEntryDetails;

    typedef struct
    {
        GtkEntry parent;
        PeonyEntryDetails *details;
    } PeonyEntry;

    typedef struct
    {
        GtkEntryClass parent_class;

        void (*user_changed)      (PeonyEntry *entry);
        void (*selection_changed) (PeonyEntry *entry);
    } PeonyEntryClass;

    GType       peony_entry_get_type                 (void);
    GtkWidget  *peony_entry_new                      (void);
    void        peony_entry_set_text                 (PeonyEntry *entry,
            const char    *text);
    void        peony_entry_select_all               (PeonyEntry *entry);
    void        peony_entry_select_all_at_idle       (PeonyEntry *entry);
    void        peony_entry_set_special_tab_handling (PeonyEntry *entry,
            gboolean       special_tab_handling);

#ifdef __cplusplus
}
#endif

#endif /* PEONY_ENTRY_H */
