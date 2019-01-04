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
#include "mime-utils.h"
#include "office-utils.h"

gboolean filename_has_suffix (char* filename, const char* suffix){
	return strstr(filename,suffix) ? TRUE : FALSE;
}

char* get_mime_type_string_by_filename(char* filename){
	return g_file_info_get_content_type(
			g_file_query_info(
				g_file_new_for_path(filename),
				G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
				0,
				g_cancellable_get_current (),
				NULL)
			);
}

gboolean is_html_file(char* filename){
	char* mime_type = get_mime_type_string_by_filename(filename);
	gboolean is_html = FALSE;

	if(strcmp(mime_type,"text/html") == 0){
		is_html = TRUE;
	}
	
	g_free(mime_type);
	return is_html;
}

gboolean is_text_type(char* filename){
	char* mime_type = get_mime_type_string_by_filename(filename);
	gboolean is_text = FALSE;

	if(strstr(mime_type,"text/")){
		is_text = TRUE;
	} else if (strstr(mime_type,"script")) {
		is_text = TRUE;
	}
	
	g_free(mime_type);
	return is_text;
	//return strstr(mime_type,"text") ? TRUE : strstr(mime_type,"script") ? TRUE : FALSE;
}

gboolean is_image_type(char *filename){
	char* mime_type = get_mime_type_string_by_filename(filename);
	gboolean is_image = FALSE;

	if(strstr(mime_type,"image/")){
		is_image = TRUE;
	} 
	
	g_free(mime_type);
	return is_image;
}

gboolean is_pdf_type(char* filename){
	char* mime_type = get_mime_type_string_by_filename(filename);
	gboolean is_pdf = FALSE;

	if(strstr(mime_type,"pdf"))
		is_pdf = TRUE;

	return is_pdf;
}

gboolean is_office_file(char* filename){

	gchar *mime_type = get_mime_type_string_by_filename(filename);
	//printf("start finding: %s\n",office_mime_types[0]);

	//init_office_mime_types();
	gint idx;
	gboolean found = FALSE;

	for (idx = 0; office_mime_types[idx] != NULL; idx++){
		//printf("finding %s\n",office_mime_types[idx]);
		found = g_content_type_is_a(mime_type,office_mime_types[idx]);
		if (found){
			found = TRUE;
			break;
		}
	}

	g_free(mime_type);

	return found;
}

gboolean is_excel_doc(char* filename){
	gchar *mime_type = get_mime_type_string_by_filename(filename);
	//printf("start finding: %s\n",office_mime_types[0]);

	//init_office_mime_types();
	gint idx;
	gboolean found = FALSE;

	for (idx = 0; excel_mime_types[idx] != NULL; idx++){
		//printf("finding %s\n",office_mime_types[idx]);
		found = g_content_type_is_a(mime_type,excel_mime_types[idx]);
		if (found){
			found = TRUE;
			break;
		}
	}

	g_free(mime_type);

	return found;
}
