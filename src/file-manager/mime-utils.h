#include <glib-2.0/glib.h>
#include <gio/gio.h>

char* get_mime_type_string_by_filename(char* filename);

gboolean is_text_type(char* filename);

gboolean is_pdf_type(char* filename);

gboolean is_image_type(char *filename);

gboolean is_office_file(char *filename);

gboolean is_excel_doc(char* filename);

