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

