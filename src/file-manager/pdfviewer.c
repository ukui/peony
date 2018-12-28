#include "pdfviewer.h"

static gboolean is_busy;

static void window_show_pdf_file (PeonyWindowInfo *window) {
	g_signal_emit_by_name (window, "show_pdf_file", NULL);
}

static void
ev_previewer_load_job_finished (EvJob           *job,
				EvDocumentModel *model)
{
	if (ev_job_is_failed (job)) {
		g_warning ("%s", job->error->message);
		g_object_unref (job);

		return;
	}

	//printf ("ev_document_model_set_document\n");
	if (EV_IS_DOCUMENT_MODEL(model) && EV_IS_DOCUMENT (job->document)){
		ev_document_model_set_document (model, job->document);
		//g_message ("ev_previewer_load_job_finished");
	}

	g_object_unref (job);
}

static void
ev_previewer_load_document (char     *filename,
			    EvDocumentModel *model)
{
	//g_message ("ev_previewer_load_document: %s", filename);
	EvJob *job;
	gchar *uri;
	GFile  *file;

	file = g_file_new_for_commandline_arg (filename);
	uri = g_file_get_uri (file);
	g_object_unref (file);

	//printf ("ev_job_load_new\n");
	job = ev_job_load_new (uri);
	g_signal_connect (job, "finished",
			  G_CALLBACK (ev_previewer_load_job_finished),
			  model);
	ev_job_scheduler_push_job (job, EV_JOB_PRIORITY_NONE);
	g_free (uri);
}

void pdf_viewer_init(){
    if(!ev_init ())
		return;
}

void pdf_viewer_shutdown(){
    ev_shutdown ();
}

gboolean is_pdf_file(char* name){
	/*
    char* suffix;
    suffix = strrchr (name + 1, '.');
    if (suffix == "pdf") {
        return TRUE;
    } else {
        return FALSE;
    }
    */
	return strstr(name,".pdf") ? TRUE : FALSE;
}

//NOTE: you must use this widget in a gtk_scrolled_window

GtkWidget* get_pdf_previewer_by_filename(char* filename){
	EvDocumentModel *model;
	GtkWidget *widget;
	model = ev_document_model_new ();

	//ev_document_model_set_page_layout(model, EV_PAGE_LAYOUT_AUTOMATIC);
	ev_document_model_set_continuous(model, TRUE);
	ev_previewer_load_document (filename, model);

	widget = ev_view_new();
	ev_view_set_model(EV_VIEW(widget), model);
	g_object_unref (model);

	return widget;
}

GtkWidget* pdf_preview_widget_new(){
	return ev_view_new();
}

void set_pdf_preview_widget_file_by_filename(GtkWidget *widget, char* filename){
	//printf("set_pdf_preview_widget_file_by_filename\n");
	EvDocumentModel *model;
	model = ev_document_model_new ();
	//ev_document_model_set_page_layout(model, EV_PAGE_LAYOUT_AUTOMATIC);
	ev_document_model_set_continuous(model, TRUE);
	//printf("ev_previewer_load_document: %s\n",filename);
	//char* current_filename = strdup (filename);
	ev_previewer_load_document (filename, model);
	//printf("ev_view_set_model\n");
	ev_view_set_model(EV_VIEW(widget), model);
	g_object_unref (model);
	//printf("done\n");
	//show widget
}

static void sleep_child_watch_cb (GPid pid,
                        gint status,
                        gpointer user_data)
{
	PeonyWindowInfo *window = PEONY_WINDOW_INFO (user_data);
	window_show_pdf_file (window);
	is_busy = FALSE;
	g_spawn_close_pid (pid);
}

void child_sleep_for_fews_times (PeonyWindowInfo *window) {
	gboolean res;
	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	char* cmd = g_strdup_printf("sleep 0.5");
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
}

void window_delay_set_pdf_preview_widget_file_by_filename (PeonyWindowInfo *window, char* filename) {
	peony_navigation_window_set_latest_pdf_preview_file_by_window_info (window, filename);
	if (!is_busy) {
		is_busy = TRUE;
		child_sleep_for_fews_times (window);
	}
}
