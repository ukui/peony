#include "mime-utils.h"
#include "office-utils.h"
#include <gdk/gdkx.h>
#include <glib/gstdio.h>

#include <libpeony-private/peony-signaller.h>

static PeonyWindowInfo *current_window;
char *current_filename, *pending_filename;

static GPid old_sleep_pid = -1;
static GPid old_pid = -1;

GPid get_current_sleep_child_pid(){
	return old_sleep_pid;
}

void set_current_sleep_child_pid(int pid){
	old_sleep_pid = pid;
}

char* get_html_tmp_name (char* filename){
	gchar *html_path, *tmp_name, *tmp_path, *quoted_path;
	GFile *file;
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

	//printf("pid: %d\n",getpid());

	file = g_file_new_for_path(filename);
	tmp_name = g_strdup_printf("%s.html",g_file_get_basename (file));

	//printf("tmp_name: %s\n",tmp_name);
	tmp_path = g_build_filename (g_get_user_cache_dir (), "peony", NULL);
	html_path = g_build_filename (tmp_path, tmp_name, NULL);

	return html_path;
}

char* get_pdf_tmp_name(char* filename){
	gchar *pdf_path, *tmp_name, *tmp_path, *quoted_path;
	GFile *file;
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

	//printf("pid: %d\n",getpid());

	file = g_file_new_for_path(filename);
	tmp_name = g_strdup_printf("%s.pdf",g_file_get_basename (file));

	//printf("tmp_name: %s\n",tmp_name);
	tmp_path = g_build_filename (g_get_user_cache_dir (), "peony", NULL);
	pdf_path = g_build_filename (tmp_path, tmp_name, NULL);

	return pdf_path;
}

char* get_pending_preview_filename (char* filename){
	if (is_excel_doc(filename)) {
		printf ("is excel\n");
		return get_html_tmp_name(filename);
	} else {
		printf ("is doc or ppt\n");
		return get_pdf_tmp_name(filename);
	}
}

sleep_child_watch_cb (GPid pid,
                        gint status,
                        gpointer user_data)
{
	printf("sleep_child_watch_cb pid: %d\n",pid);
	printf("current: %s\n", current_filename);
	printf("old_sleep_pid: %d\n",old_sleep_pid);
	g_spawn_close_pid (pid);
	if (pid != old_sleep_pid){
		printf ("this preview progress request by pid %d will not be shedueled\n", pid);
		return;
	}

	if (is_excel_doc(current_filename)) {
		excel2html_by_window_internal (current_window, pending_filename, current_filename);
	} else {
		office2pdf_by_window_internal (current_window, pending_filename, current_filename);
	}
}

GPid child_prog_sleep_and_preview_office (char* second, PeonyWindowInfo *window, char* filename, char* filename2){

	current_window = window;
	if(current_filename && pending_filename){
		g_free(current_filename);
		g_free(pending_filename);
	}
	current_filename = g_strdup(filename);
	pending_filename = g_strdup(filename2);

	printf("%s, %s\n",current_filename, pending_filename);

	gboolean res;
	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	char* cmd = g_strdup_printf("sleep %s", second);
	res = g_shell_parse_argv (cmd, &argc, &argv, &error);
	g_free (cmd);

	if (!res) {
		g_warning ("Error while parsing the sleep command line: %s",
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
		g_warning ("Error while spawning sleep: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	g_child_watch_add (pid, sleep_child_watch_cb, window);

	old_sleep_pid = pid;
	printf("sleep pid: %d\n",pid);
}

unoconv_child_watch_cb2 (GPid pid,
                        gint status,
                        gpointer user_data)
{
	//printf("unoconv_child_watch_cb pid: %d\n",pid);
	g_spawn_close_pid (pid);
	g_signal_emit_by_name (PEONY_WINDOW_INFO (user_data), "office_trans_ready", NULL);    //office ready cb , if pdf name = global preview file name, show it. else g_remove file.
}

void office2pdf_by_window_internal (PeonyWindowInfo *window, char *pdf_filename, char* office_filename){

    if (old_pid != -1){
        gchar *cmd = g_strdup_printf ("kill %d\n", old_pid);
        printf("cmd :%s", cmd);
        //system (cmd);
        kill (old_pid, SIGKILL); //kill old pid for office transform anyway
        old_pid = -1;
        g_free (cmd);
	}

	gboolean res;
	gchar *cmd;

	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	const gchar *unoconv_path;

	cmd = g_strdup_printf ("unoconv -f pdf -o %s '%s'", pdf_filename, office_filename);
	printf("cmd: %s\n",cmd);
	
	res = g_shell_parse_argv (cmd, &argc, &argv, &error);
	//g_free (cmd);

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

	g_child_watch_add (pid, unoconv_child_watch_cb2, window);

	//g_spawn_close_pid(pid);
	old_pid = pid;
}

void excel2html_by_window_internal (PeonyWindowInfo *window, char *html_filename, char *excel_filename){

    if (old_pid != -1){
        gchar *cmd = g_strdup_printf ("kill %d", old_pid);
        printf("cmd :%s\n", cmd);
        //system (cmd);
        kill (old_pid, SIGKILL); //kill old pid for office transform anyway
        old_pid = -1;
        g_free (cmd);
	}

	gboolean res;
	gchar *cmd;

	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	const gchar *unoconv_path;

	cmd = g_strdup_printf ("unoconv -f html -o %s '%s'", html_filename, excel_filename);
	printf("cmd: %s\n",cmd);	
	
	res = g_shell_parse_argv (cmd, &argc, &argv, &error);
	//g_free (cmd);

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

	g_child_watch_add (pid, unoconv_child_watch_cb2, window);

	old_pid = pid;
}