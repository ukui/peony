/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Peony
 *
 * Copyright (C) 2011 Red Hat, Inc.
 *               2012 Stefano Karapetsas
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
 * Authors: Cosimo Cecchi <cosimoc@redhat.com>
 *          Stefano Karapetsas <stefano@karapetsas.com>
 */

#ifndef __PEONY_DESKTOP_METADATA_H__
#define __PEONY_DESKTOP_METADATA_H__

#include <glib.h>

#include <libpeony-private/peony-file.h>

void peony_desktop_set_metadata_string (PeonyFile *file,
                                       const gchar *name,
                                       const gchar *key,
                                       const gchar *string);

void peony_desktop_set_metadata_stringv (PeonyFile *file,
                                        const char *name,
                                        const char *key,
                                        const char * const *stringv);

gboolean peony_desktop_update_metadata_from_keyfile (PeonyFile *file,
                                                    const gchar *name);

#endif /* __PEONY_DESKTOP_METADATA_H__ */
