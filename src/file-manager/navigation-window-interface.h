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
