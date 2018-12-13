#include "mime-utils.h"
#include "office-utils.h"
#include <gdk/gdkx.h>
#include <glib/gstdio.h>

#include <libpeony-private/peony-signaller.h>

static void
unoconv_child_watch_cb (GPid pid,
                        gint status,
                        gpointer user_data)
{
	printf("unoconv_child_watch_cb pid: %d, filepath: %s\n", pid, (char*) user_data);
	g_spawn_close_pid (pid);
	g_signal_emit_by_name (peony_signaller_get_current (), "office2pdf_ready", user_data);    //office ready cb , if pdf name = global preview file name, show it. else g_remove file.
	//kill (pid, SIGKILL);
	//exit (1);
}

static GPid old_pid = -1;
static char* old_preview_file_pdf_path;
static char* old_preview_file_html_path;

char* office2pdf(char* filename){

	if(old_pid != -1){
		printf("kill old progress and remove old preview file\n");
		kill (old_pid, SIGKILL);
		g_remove (old_preview_file_pdf_path);
		old_pid = -1;
	}

	gchar *doc_path, *pdf_path, *tmp_name, *tmp_path, *quoted_path;
	//GFile *file;
	gboolean res;
	gchar *cmd;

	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	const gchar *unoconv_path;

	unoconv_path = g_find_program_in_path ("unoconv");
	if (unoconv_path == NULL) {
		printf("unoconv is not found\n");
		//openoffice_missing_unoconv (self);
		return NULL;
	}

	printf("pid: %d\n",getpid());

	tmp_name = g_strdup_printf ("test.pdf");//,g_strcanon(filename,"/",'_'));//, getpid ());
	
	printf("tmp_name: %s\n",tmp_name);
	tmp_path = g_build_filename (g_get_user_cache_dir (), "peony", NULL);
	pdf_path = g_build_filename (tmp_path, tmp_name, NULL);
	g_mkdir_with_parents (tmp_path, 0700);

	cmd = g_strdup_printf ("unoconv -f pdf -o %s '%s'", pdf_path, filename);
	printf("cmd: %s\n",cmd);
	g_free (tmp_name);
	g_free (tmp_path);

/*
	gchar **standard_output;
	gchar **standard_error;
	gint *exit_status;
	g_spawn_command_line_sync(cmd,standard_output,standard_error,exit_status,&error);
 if (!res) {
    g_warning ("Error while g_spawn_command_line_async: %s",
               error->message);
    g_error_free (error);

    return NULL;
  }
*/
 	

	
	res = g_shell_parse_argv (cmd, &argc, &argv, &error);
	g_free (cmd);

	if (!res) {
		g_warning ("Error while parsing the unoconv command line: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	res = g_spawn_async (NULL, argv, NULL,
			G_SPAWN_DO_NOT_REAP_CHILD |
			G_SPAWN_SEARCH_PATH,
			NULL, NULL,
			&pid, &error);

	g_strfreev (argv);

	if (!res) {
		g_warning ("Error while spawning unoconv: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	g_child_watch_add (pid, unoconv_child_watch_cb, pdf_path);

	//g_spawn_close_pid(pid);
	old_pid = pid;
	old_preview_file_pdf_path = pdf_path;

	return pdf_path; //it will be saved in global preview filename, and wait for office ready cb if selection not changed.

}

char *excel2html (char *filename){

	if(old_pid != -1){
		printf("kill old progress and remove old preview file\n");
		kill (old_pid, SIGKILL);
		g_remove (old_preview_file_html_path);
		old_pid = -1;
	}

	gchar *doc_path, *html_path, *tmp_name, *tmp_path, *quoted_path;
	//GFile *file;
	gboolean res;
	gchar *cmd;

	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	const gchar *unoconv_path;

	unoconv_path = g_find_program_in_path ("unoconv");
	if (unoconv_path == NULL) {
		printf("unoconv is not found\n");
		//openoffice_missing_unoconv (self);
		return NULL;
	}

	printf("pid: %d\n",getpid());

	tmp_name = g_strdup_printf ("test.html");//,g_strcanon(filename,"/",'_'));//, getpid ());
	
	printf("tmp_name: %s\n",tmp_name);
	tmp_path = g_build_filename (g_get_user_cache_dir (), "peony", NULL);
	html_path = g_build_filename (tmp_path, tmp_name, NULL);
	g_mkdir_with_parents (tmp_path, 0700);

	cmd = g_strdup_printf ("unoconv -f html -o %s '%s'", html_path, filename);
	printf("cmd: %s\n",cmd);
	g_free (tmp_name);
	g_free (tmp_path);

/*
	gchar **standard_output;
	gchar **standard_error;
	gint *exit_status;
	g_spawn_command_line_sync(cmd,standard_output,standard_error,exit_status,&error);
 if (!res) {
    g_warning ("Error while g_spawn_command_line_async: %s",
               error->message);
    g_error_free (error);

    return NULL;
  }
*/
 	

	
	res = g_shell_parse_argv (cmd, &argc, &argv, &error);
	g_free (cmd);

	if (!res) {
		g_warning ("Error while parsing the unoconv command line: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	res = g_spawn_async (NULL, argv, NULL,
			G_SPAWN_DO_NOT_REAP_CHILD |
			G_SPAWN_SEARCH_PATH,
			NULL, NULL,
			&pid, &error);

	g_strfreev (argv);

	if (!res) {
		g_warning ("Error while spawning unoconv: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	g_child_watch_add (pid, unoconv_child_watch_cb, html_path);

	//g_spawn_close_pid(pid);
	old_pid = pid;
	old_preview_file_html_path = html_path;

	return html_path; //it will be saved in global preview filename, and wait for office ready cb if selection not changed.

}

/*
void init_office_mime_types(){
	
	if (office_mime_types[0] == NULL){
		printf("set init_office_mime_types\n");
		//word
		office_mime_types[0] = "application/wps-office.doc";
		office_mime_types[1] = "application/msword";
		office_mime_types[2] = "application/vnd.ms-word";
		office_mime_types[3] = "application/x-msword";
		office_mime_types[4] = "application/vnd.ms-word.document.macroenabled.12";
		office_mime_types[5] = "application/wps-office.dot";
		office_mime_types[6] = "application/msword-template";
		office_mime_types[7] = "application/vnd.ms-word.template.macroenabled.12";
		office_mime_types[8] = "application/wps-office.dotx";
		office_mime_types[9] = "application/vnd.openxmlformats-officedocument.wordprocessingml.template";
		office_mime_types[10] = "application/wps-office.docx";
		office_mime_types[11] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
		office_mime_types[12] = "application/wps-office.wps";
		office_mime_types[13] = "application/vnd.ms-works";
		//ppt
		office_mime_types[14] = "application/wps-office.wpt";
		office_mime_types[15] = "application/wps-office.ppt";
		office_mime_types[16] = "application/vnd.ms-powerpoint";
		office_mime_types[17] = "application/powerpoint";
		office_mime_types[18] = "application/mspowerpoint";
		office_mime_types[19] = "application/x-mspowerpoint";
		office_mime_types[20] = "application/vnd.ms-powerpoint.presentation.macroenabled.12";
		office_mime_types[21] = "application/wps-office.pot";
		office_mime_types[22] = "application/vnd.ms-powerpoint.template.macroenabled.12";
		office_mime_types[23] = "application/wps-office.potx";
		office_mime_types[25] = "application/vnd.openxmlformats-officedocument.presentationml.template";
		office_mime_types[26] = "application/wps-office.pptx";
		office_mime_types[27] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
		office_mime_types[28] = "application/vnd.openxmlformats-officedocument.presentationml.slideshow";
		office_mime_types[29] = "application/wps-office.dps";
		office_mime_types[30] = "application/wps-office.dpt";
		//excel
		office_mime_types[31] = "application/wps-office.xls";
		office_mime_types[32] = "application/vnd.ms-excel";
		office_mime_types[33] = "application/msexcel";
		office_mime_types[34] = "application/x-msexcel";
		office_mime_types[35] = "application/vnd.ms-excel.sheet.macroenabled.12";
		office_mime_types[36] = "application/vnd.ms-excel.template.macroenabled.12";
		office_mime_types[37] = "application/wps-office.xlt";
		office_mime_types[38] = "application/wps-office.xltx";
		office_mime_types[39] = "application/vnd.openxmlformats-officedocument.spreadsheetml.template";
		office_mime_types[40] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
		office_mime_types[41] = "application/wps-office.et";
		office_mime_types[42] = "application/wps-office.ett";
	}
	
}
*/