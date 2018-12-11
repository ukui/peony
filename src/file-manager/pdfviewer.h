#include <gtk/gtk.h>
#include <atril-document.h>
#include <atril-view.h>

//when using pdf viewer, we must call pdf_viewer_init()
void pdf_viewer_init();
void pdf_viewer_shutdown();

GtkWidget* pdf_preview_widget_new();
void set_pdf_preview_widget_file_by_filename(GtkWidget *widget, char* filename);

