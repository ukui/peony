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

#include <glib-2.0/glib.h>
#include <gio/gio.h>

char* get_mime_type_string_by_filename(char* filename);

gboolean is_text_type(char* filename);

gboolean is_pdf_type(char* filename);

gboolean is_image_type(char *filename);

gboolean is_office_file(char *filename);

gboolean is_excel_doc(char* filename);

gboolean is_html_file(char* filename);

//this function is used when officetransform ready cb. the tmp file sometimes can't get mimetype soon.
gboolean filename_has_suffix (char* filename, const char* suffix);


