/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-desktop-link.h: Class that handles the links on the desktop

   Copyright (C) 2003 Red Hat, Inc.

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

   Author: Alexander Larsson <alexl@redhat.com>
*/

#ifndef PEONY_DESKTOP_LINK_H
#define PEONY_DESKTOP_LINK_H

#include <libpeony-private/peony-file.h>
#include <gio/gio.h>

#define PEONY_TYPE_DESKTOP_LINK peony_desktop_link_get_type()
#define PEONY_DESKTOP_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), PEONY_TYPE_DESKTOP_LINK, PeonyDesktopLink))
#define PEONY_DESKTOP_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), PEONY_TYPE_DESKTOP_LINK, PeonyDesktopLinkClass))
#define PEONY_IS_DESKTOP_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PEONY_TYPE_DESKTOP_LINK))
#define PEONY_IS_DESKTOP_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), PEONY_TYPE_DESKTOP_LINK))
#define PEONY_DESKTOP_LINK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), PEONY_TYPE_DESKTOP_LINK, PeonyDesktopLinkClass))

typedef struct PeonyDesktopLinkDetails PeonyDesktopLinkDetails;

typedef struct
{
    GObject parent_slot;
    PeonyDesktopLinkDetails *details;
} PeonyDesktopLink;

typedef struct
{
    GObjectClass parent_slot;
} PeonyDesktopLinkClass;

typedef enum
{
    PEONY_DESKTOP_LINK_HOME,
    PEONY_DESKTOP_LINK_COMPUTER,
    PEONY_DESKTOP_LINK_TRASH,
    PEONY_DESKTOP_LINK_MOUNT,
    PEONY_DESKTOP_LINK_NETWORK
} PeonyDesktopLinkType;

GType   peony_desktop_link_get_type (void);

PeonyDesktopLink *   peony_desktop_link_new                     (PeonyDesktopLinkType  type);
PeonyDesktopLink *   peony_desktop_link_new_from_mount          (GMount                 *mount);
PeonyDesktopLinkType peony_desktop_link_get_link_type           (PeonyDesktopLink     *link);
char *                  peony_desktop_link_get_file_name           (PeonyDesktopLink     *link);
char *                  peony_desktop_link_get_display_name        (PeonyDesktopLink     *link);
GIcon *                 peony_desktop_link_get_icon                (PeonyDesktopLink     *link);
GFile *                 peony_desktop_link_get_activation_location (PeonyDesktopLink     *link);
char *                  peony_desktop_link_get_activation_uri      (PeonyDesktopLink     *link);
gboolean                peony_desktop_link_get_date                (PeonyDesktopLink     *link,
        PeonyDateType         date_type,
        time_t                  *date);
GMount *                peony_desktop_link_get_mount               (PeonyDesktopLink     *link);
gboolean                peony_desktop_link_can_rename              (PeonyDesktopLink     *link);
gboolean                peony_desktop_link_rename                  (PeonyDesktopLink     *link,
        const char              *name);


#endif /* PEONY_DESKTOP_LINK_H */
