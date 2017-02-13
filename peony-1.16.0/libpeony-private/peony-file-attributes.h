/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*-

   peony-file-attributes.h: #defines and other file-attribute-related info

   Copyright (C) 2000 Eazel, Inc.

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

   Author: Darin Adler <darin@bentspoon.com>
*/

#ifndef PEONY_FILE_ATTRIBUTES_H
#define PEONY_FILE_ATTRIBUTES_H

/* Names for PeonyFile attributes. These are used when registering
 * interest in changes to the attributes or when waiting for them.
 */

typedef enum
{
    PEONY_FILE_ATTRIBUTE_INFO = 1 << 0, /* All standard info */
    PEONY_FILE_ATTRIBUTE_LINK_INFO = 1 << 1, /* info from desktop links */
    PEONY_FILE_ATTRIBUTE_DEEP_COUNTS = 1 << 2,
    PEONY_FILE_ATTRIBUTE_DIRECTORY_ITEM_COUNT = 1 << 3,
    PEONY_FILE_ATTRIBUTE_DIRECTORY_ITEM_MIME_TYPES = 1 << 4,
    PEONY_FILE_ATTRIBUTE_TOP_LEFT_TEXT = 1 << 5,
    PEONY_FILE_ATTRIBUTE_LARGE_TOP_LEFT_TEXT = 1 << 6,
    PEONY_FILE_ATTRIBUTE_EXTENSION_INFO = 1 << 7,
    PEONY_FILE_ATTRIBUTE_THUMBNAIL = 1 << 8,
    PEONY_FILE_ATTRIBUTE_MOUNT = 1 << 9,
    PEONY_FILE_ATTRIBUTE_FILESYSTEM_INFO = 1 << 10,
} PeonyFileAttributes;

#endif /* PEONY_FILE_ATTRIBUTES_H */
