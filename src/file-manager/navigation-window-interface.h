#include "libpeony-private/peony-window-info.h"

void     peony_navigation_window_set_latest_pdf_preview_file_by_window_info (PeonyWindowInfo *window_info, char* filename);
char*    peony_navigation_window_get_latest_preview_file_by_window_info (PeonyWindowInfo *window_info);

void     peony_navigation_window_set_current_preview_file_by_window_info (PeonyWindowInfo *window_info, char* filename);
char*    peony_navigation_window_get_current_preview_file_by_window_info (PeonyWindowInfo *window_info);
void     peony_navigation_window_set_current_previewing_office_file_by_window_info (PeonyWindowInfo *window_info, char* filename);
char*    peony_navigation_window_get_current_previewing_office_file_by_window_info (PeonyWindowInfo *window_info);
void     peony_navigation_window_set_pending_preview_file_by_window_info (PeonyWindowInfo *window_info, char* filename);
char*    peony_navigation_window_get_pending_preview_file_by_window_info (PeonyWindowInfo *window_info);
void     peony_navigation_window_set_loading_office_file_by_window_info (PeonyWindowInfo *window_info, char* filename);
char*    peony_navigation_window_get_loading_office_file_by_window_info (PeonyWindowInfo *window_info);
