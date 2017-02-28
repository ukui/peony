/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/* peony-file-management-properties-main.c - Start the peony-file-management preference dialog.

   Copyright (C) 2002 Jan Arne Petersen
   Copyright (C) 2017, Tianjin KYLIN Information Technology Co., Ltd.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Authors: Jan Arne Petersen <jpetersen@uni-bonn.de>
            Zuxun Yang <yangzuxun@kylinos.cn>
*/

#include <config.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <libpeony-private/peony-global-preferences.h>
#include <libpeony-private/peony-module.h>

#include "peony-file-management-properties.h"

static void
peony_file_management_properties_main_close_callback (GtkDialog *dialog,
        int response_id)
{
    if (response_id == GTK_RESPONSE_CLOSE)
    {
        gtk_main_quit ();
    }
}

int
main (int argc, char *argv[])
{
    bindtextdomain (GETTEXT_PACKAGE, UKUILOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);

    gtk_init (&argc, &argv);

    peony_global_preferences_init ();

    peony_module_setup ();

    peony_file_management_properties_dialog_show (G_CALLBACK (peony_file_management_properties_main_close_callback), NULL);

    gtk_main ();

    return 0;
}
