/*
 *  Peony
 *
 *  Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 *  Peony is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Peony is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Modified by: quankang <quankang@kylinos.cn>
 */

#ifndef kDISK_CREATEFORMAT_H
#define KDISK_CREATEFORMAT_H

#include <sys/stat.h>
#include <udisks/udisks.h>
#include <glib/gi18n.h>
#include <errno.h>

void    kdisk_format(const gchar * device_name,const gchar * format_type,const gchar * is_erase,const gchar * filesystem_name,int *format_finish);
double  get_device_size(const gchar * device_name);
void cancel_format(const gchar* device_name);


int find_cb(const gchar *device_name);

double get_format_bytes_done(const gchar *device_name);

gchar * get_device_label(const gchar * device_name);
#endif // CREATEFORMAT_H#include <stdio.h>
