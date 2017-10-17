/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

/* 
 * Copyright (C) 2004 Roberto Majadas <roberto.majadas@openshine.com>
 * Copyright (C) 2012 Stefano Karapetsas <stefano@karapetsas.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more av.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301  USA.
 *
 * Authors:  Roberto Majadas <roberto.majadas@openshine.com>
 *           Stefano Karapetsas <stefano@karapetsas.com>
 */

#include "config.h"

#include <glib/gi18n-lib.h>
#include <string.h>
#include "caja-sendto-plugin.h"
#include <gio/gio.h>

typedef enum {
	MAILER_UNKNOWN,
	MAILER_EVO,
	MAILER_BALSA,
	MAILER_SYLPHEED,
	MAILER_THUNDERBIRD,
} MailerType;

static char *mail_cmd = NULL;
static MailerType type = MAILER_UNKNOWN;

static char *
get_evo_cmd (void)
{
	char *tmp = NULL;
	char *retval;
	char *cmds[] = {"evolution",
		"evolution-2.0",
		"evolution-2.2",
		"evolution-2.4",
		"evolution-2.6",
		"evolution-2.8", /* for the future */
		"evolution-3.0", /* but how far to go ? */
		NULL};
	guint i;

	
	for (i = 0; cmds[i] != NULL; i++) {
		tmp = g_find_program_in_path (cmds[i]);
		if (tmp != NULL)
			break;
	}

	if (tmp == NULL)
		return NULL;

	retval = g_strdup_printf ("%s --component=mail %%s", tmp);
	g_free (tmp);
	return retval;
}

static gboolean
init (NstPlugin *plugin)
{
	GAppInfo *app_info = NULL;

	g_print ("Init email client plugin\n");
	
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

	app_info = g_app_info_get_default_for_uri_scheme ("mailto");
	if (app_info) {
		mail_cmd = g_strdup(g_app_info_get_executable (app_info));
		g_object_unref (app_info);
	}

	if (mail_cmd == NULL || *mail_cmd == '\0') {
		g_free (mail_cmd);
		mail_cmd = get_evo_cmd ();
		type = MAILER_EVO;
	} else {
		/* Find what the default mailer is */
		if (strstr (mail_cmd, "balsa"))
			type = MAILER_BALSA;
		else if (strstr (mail_cmd, "thunder") || strstr (mail_cmd, "seamonkey")) {
			char **strv;

			type = MAILER_THUNDERBIRD;

			/* Thunderbird sucks, see
			 * https://bugzilla.gnome.org/show_bug.cgi?id=614222 */
			strv = g_strsplit (mail_cmd, " ", -1);
			g_free (mail_cmd);
			mail_cmd = g_strdup_printf ("%s %%s", strv[0]);
			g_strfreev (strv);
		} else if (strstr (mail_cmd, "sylpheed") || strstr (mail_cmd, "claws"))
			type = MAILER_SYLPHEED;
		else if (strstr (mail_cmd, "anjal"))
			type = MAILER_EVO;
	}

	if (mail_cmd == NULL)
		return FALSE;

	return TRUE;
}

static
GtkWidget* get_contacts_widget (NstPlugin *plugin)
{
	GtkWidget *entry;

	// TODO: add an email address format check
	entry = gtk_entry_new();

	return entry;
}

static void
get_evo_mailto (GtkWidget *contact_widget, GString *mailto, GList *file_list)
{
	GList *l;

	g_string_append (mailto, "mailto:");

		const char *text;

	text = gtk_entry_get_text (GTK_ENTRY (contact_widget));
	if (text != NULL && *text != '\0')
		g_string_append_printf (mailto, "\"%s\"", text);
	else
		g_string_append (mailto, "\"\"");

	g_string_append_printf (mailto,"?attach=\"%s\"", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next){
		g_string_append_printf (mailto,"&attach=\"%s\"", (char *)l->data);
	}
}

static void
get_balsa_mailto (GtkWidget *contact_widget, GString *mailto, GList *file_list)
{
	GList *l;

	if (strstr (mail_cmd, " -m ") == NULL && strstr (mail_cmd, " --compose=") == NULL)
		g_string_append (mailto, " --compose=");

	const char *text;

	text = gtk_entry_get_text (GTK_ENTRY (contact_widget));
	if (text != NULL && *text != '\0')
		g_string_append_printf (mailto, "\"%s\"", text);
	else
		g_string_append (mailto, "\"\"");

	g_string_append_printf (mailto," --attach=\"%s\"", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next){
		g_string_append_printf (mailto," --attach=\"%s\"", (char *)l->data);
	}
}

static void
get_thunderbird_mailto (GtkWidget *contact_widget, GString *mailto, GList *file_list)
{
	GList *l;

	g_string_append (mailto, "-compose \"");

	const char *text;

	text = gtk_entry_get_text (GTK_ENTRY (contact_widget));
	if (text != NULL && *text != '\0')
		g_string_append_printf (mailto, "to='%s',", text);

	g_string_append_printf (mailto,"attachment='%s", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next){
		g_string_append_printf (mailto,",%s", (char *)l->data);
	}
	g_string_append (mailto, "'\"");
}

static void
get_sylpheed_mailto (GtkWidget *contact_widget, GString *mailto, GList *file_list)
{
	GList *l;

	g_string_append (mailto, "--compose ");

	const char *text;

	text = gtk_entry_get_text (GTK_ENTRY (contact_widget));
	if (text != NULL && *text != '\0')
		g_string_append_printf (mailto, "\"%s\" ", text);
	else
		g_string_append (mailto, "\"\"");

	g_string_append_printf (mailto,"--attach \"%s\"", (char *)file_list->data);
	for (l = file_list->next ; l; l=l->next){
		g_string_append_printf (mailto," \"%s\"", (char *)l->data);
	}
}

static gboolean
send_files (NstPlugin *plugin,
	    GtkWidget *contact_widget,
	    GList *file_list)
{
	gchar *cmd;
	GString *mailto;

	mailto = g_string_new ("");
	switch (type) {
	case MAILER_BALSA:
		get_balsa_mailto (contact_widget, mailto, file_list);
		break;
	case MAILER_SYLPHEED:
		get_sylpheed_mailto (contact_widget, mailto, file_list);
		break;
	case MAILER_THUNDERBIRD:
		get_thunderbird_mailto (contact_widget, mailto, file_list);
		break;
	case MAILER_EVO:
	default:
		get_evo_mailto (contact_widget, mailto, file_list);
	}

	cmd = g_strdup_printf (mail_cmd, mailto->str);
	g_string_free (mailto, TRUE);

	g_message ("Mailer type: %d", type);
	g_message ("Command: %s", cmd);

	g_spawn_command_line_async (cmd, NULL);
	g_free (cmd);

	return TRUE;
}

static 
gboolean destroy (NstPlugin *plugin){
	g_free (mail_cmd);
	mail_cmd = NULL;
	return TRUE;
}

static 
NstPluginInfo plugin_info = {
	"emblem-mail",
	"emailclient",
	N_("Email"),
	NULL,
	CAJA_CAPS_NONE,
	init,
	get_contacts_widget,
	NULL,
	send_files,
	destroy
}; 

NST_INIT_PLUGIN (plugin_info)

