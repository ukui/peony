#include <gtk/gtk.h>
#include <atril-document.h>
#include <atril-view.h>
#include "navigation-window-interface.h"
//#include "libpeony-private/peony-window-info.h"

//when using pdf viewer, we must call pdf_viewer_init()
void pdf_viewer_init();
void pdf_viewer_shutdown();

void pdfviewer_connect_window (PeonyWindowInfo *window);
void pdfviewer_disconnect_window (PeonyWindowInfo *window);

GtkWidget* pdf_preview_widget_new();
void window_delay_set_pdf_preview_widget_file_by_filename (PeonyWindowInfo *window, char* filename);
void set_pdf_preview_widget_file_by_filename (GtkWidget *widget, char* filename);

//void     peony_navigation_window_set_latest_pdf_preview_file_by_window_info (PeonyWindowInfo *window_info, char* filename);
//char*    peony_navigation_window_get_latest_preview_file_by_window_info (PeonyWindowInfo *window_info);

