#include "pdfviewer.h"

static void
ev_previewer_load_job_finished (EvJob           *job,
				EvDocumentModel *model)
{
	if (ev_job_is_failed (job)) {
		//printf("==============%s",job->error->message);
		g_warning ("%s", job->error->message);
		g_object_unref (job);

		return;
	}
	ev_document_model_set_document (model, job->document);
	//printf("=====================ev_previewer_load_job_finished\n");
	g_object_unref (job);
}

static void
ev_previewer_load_document (const gchar     *filename,
			    EvDocumentModel *model)
{
	EvJob *job;
	gchar *uri;
	GFile  *file;

	file = g_file_new_for_commandline_arg (filename);
	uri = g_file_get_uri (file);
	g_object_unref (file);

	job = ev_job_load_new (uri);
	g_signal_connect (job, "finished",
			  G_CALLBACK (ev_previewer_load_job_finished),
			  model);
	ev_job_scheduler_push_job (job, EV_JOB_PRIORITY_NONE);
	g_free (uri);
}

void pdf_viewer_init(){
    if(!ev_init ())
		printf("pdf_viewer_init failed!\n");
	else
		printf("pdf_viewer_init done!\n");
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

	ev_document_model_set_page_layout(model, EV_PAGE_LAYOUT_AUTOMATIC);
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
	ev_document_model_set_page_layout(model, EV_PAGE_LAYOUT_AUTOMATIC);
	ev_document_model_set_continuous(model, TRUE);
	//printf("ev_previewer_load_document: %s\n",filename);
	ev_previewer_load_document (filename, model);
	//printf("ev_view_set_model\n");
	ev_view_set_model(EV_VIEW(widget), model);
	g_object_unref (model);
	//printf("done\n");
	//show widget
}
