/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 * test-widget.c
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2001 - Mikael Hermansson <tyan@linux.se>
 * Copyright (C) 2003 - Gustavo Giráldez <gustavo.giraldez@gmx.net>
 * Copyright (C) 2014 - Sébastien Wilmet <swilmet@gnome.org>
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * GtkSourceView is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GtkSourceView is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Modified by: quankang <quankang@kylinos.cn>
 */

#ifndef TEST_WIDGET_H
#define TEST_WIDGET_H

#include <string.h>
#include <gtksourceview/gtksource.h>

#define TEST_TYPE_WIDGET             (test_widget_get_type ())
#define TEST_WIDGET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_TYPE_WIDGET, TestWidget))
#define TEST_WIDGET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_TYPE_WIDGET, TestWidgetClass))
#define TEST_IS_WIDGET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_TYPE_WIDGET))
#define TEST_IS_WIDGET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_TYPE_WIDGET))
#define TEST_WIDGET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_TYPE_WIDGET, TestWidgetClass))

typedef struct _TestWidget         TestWidget;
typedef struct _TestWidgetClass    TestWidgetClass;
typedef struct _TestWidgetPrivate  TestWidgetPrivate;

#endif

TestWidget *
//static TestWidget *
test_widget_new (void);

void widget_init(TestWidget *self);
void mode_init(TestWidget *self);

void open_file_cb(TestWidget *widget, char *filename);
/*
int
main (int argc, char *argv[])
{
	GtkWidget *window;
	TestWidget *test_widget;

	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 900, 600);

	g_signal_connect (window, "destroy", gtk_main_quit, NULL);

	test_widget = test_widget_new ();
	gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (test_widget));

	gtk_widget_show (window);

	gtk_main ();
	return 0;
}
*/
