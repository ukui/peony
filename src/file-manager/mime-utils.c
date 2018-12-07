#include "mime-utils.h"

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

gboolean is_text_type(char* filename){
	return strstr(get_mime_type_string_by_filename(filename),
			"text") != NULL ? TRUE : FALSE;

}
