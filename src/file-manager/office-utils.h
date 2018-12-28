#include <glib-2.0/glib.h>
#include <glib/gstdio.h>
#include "navigation-window-interface.h"

char* get_pending_preview_filename (char* filename);
gboolean is_office_busy();

void office_utils_conncet_window_info (PeonyWindowInfo *window_info);
void office_utils_disconnect_window_info (PeonyWindowInfo *window_info);

void clean_cache_files_anyway (); 

static char* office_mime_types[] = {
    "application/wps-office.doc",
    "application/msword",
    "application/vnd.ms-word",
    "application/x-msword",
    "application/vnd.ms-word.document.macroenabled.12",
    "application/wps-office.dot",
    "application/msword-template",
    "application/vnd.ms-word.template.macroenabled.12",
    "application/wps-office.dotx",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.template",
    "application/wps-office.docx",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
    "application/wps-office.wps",
    "application/vnd.ms-works",
    "application/vnd.oasis.opendocument.text",

    "application/wps-office.wpt",
    "application/wps-office.ppt",
    "application/vnd.ms-powerpoint",
    "application/powerpoint",
    "application/mspowerpoint",
    "application/x-mspowerpoint",
    "application/vnd.ms-powerpoint.presentation.macroenabled.12",
    "application/wps-office.pot",
    "application/vnd.ms-powerpoint.template.macroenabled.12",
    "application/wps-office.potx",
    "application/vnd.openxmlformats-officedocument.presentationml.template",
    "application/wps-office.pptx",
    "application/vnd.openxmlformats-officedocument.presentationml.presentation",
    "application/vnd.openxmlformats-officedocument.presentationml.slideshow",
    "application/wps-office.dps",
    "application/wps-office.dpt",
    "application/vnd.oasis.opendocument.presentation",
    "application/vnd.openxmlformats-officedocument.presentationml.presentation",
    
    "application/wps-office.xlsx",
    "application/wps-office.xls",
    "application/vnd.ms-excel",
    "application/msexcel",
    "application/x-msexcel",
    "application/vnd.ms-excel.sheet.macroenabled.12",
    "application/vnd.ms-excel.template.macroenabled.12",
    "application/wps-office.xlt",
    "application/wps-office.xltx",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.template",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
    "application/vnd.oasis.opendocument.spreadsheet",
    "application/wps-office.et",
    "application/wps-office.ett"
};

static char* excel_mime_types[] = {
    "application/wps-office.xlsx",
    "application/wps-office.xls",
    "application/vnd.ms-excel",
    "application/msexcel",
    "application/x-msexcel",
    "application/vnd.ms-excel.sheet.macroenabled.12",
    "application/vnd.ms-excel.template.macroenabled.12",
    "application/wps-office.xlt",
    "application/wps-office.xltx",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.template",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
    "application/wps-office.et",
    "application/wps-office.ett",
    "application/vnd.oasis.opendocument.spreadsheet"
};
