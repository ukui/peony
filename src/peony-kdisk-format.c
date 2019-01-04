/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
  
/*
 *  Peony
 *
 *  Copyright (C) 1999, 2000 Red Hat, Inc.
 *  Copyright (C) 1999, 2000, 2001 Eazel, Inc.
 *  Copyright (C) 2018, Tianjin KYLIN Information Technology Co., Ltd.
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
 *  Authors: Elliot Lee <sopwith@redhat.com>
 *           John Sullivan <sullivan@eazel.com>
 *           Darin Adler <darin@bentspoon.com>
 *  Modified by: liupeng <liupeng@kylinos.cn>
 */

#include "peony-kdisk-format.h"
typedef struct
{
	bool is_format;
        double per;
	int timeout_id;
	int format_finish;
	GtkWidget *window;
        GtkWidget *label1,*label2,*label3,*label4,*label5;
        GtkWidget *fixed;
        GtkWidget *sizecombox,*typecombox;
	GtkWidget *entry_name;
	GtkWidget *check_button;
	GtkWidget *processbar;
	GtkWidget *ensure_button,*cancel_button;

	const gchar *block_device;
	gchar *format_type;
     	gchar *is_erase ;
        const gchar * filesystem_name;
}format_window;
static int FORMAT_WINDOW = 0;
static void 
delete_format_window(GtkWidget *window,gpointer user_data)
{
	format_window *data = user_data;
	if(data->is_format == true){
		g_source_remove (data->timeout_id);	
	}
	gtk_widget_hide(data->window);				
	gtk_widget_destroy(data->window);	
	g_free(data->format_type);
	g_free(data);
	FORMAT_WINDOW = 0;	
}

static gboolean 
ensure_delete_format_window(GtkWidget *window,GdkEvent *event,gpointer user_data)
{
	format_window *data = user_data;
	if(data->is_format == true){
		GtkWidget *dialog;

       		dialog = gtk_message_dialog_new(
        		NULL,
        		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        		GTK_MESSAGE_INFO,
       			GTK_BUTTONS_YES_NO,
			_("Formatting is in progress, do you want to continue?"));
		gtk_window_set_skip_taskbar_hint (GTK_WINDOW(dialog),FALSE);
		GtkIconTheme *icon_theme;
        	GdkPixbuf *pixbuf;
        	icon_theme = gtk_icon_theme_get_default ();
        	pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                   "disks", /* icon name */
                                   48, /* size */
                                   0,  /* flags */
                                   NULL);
        	if (!pixbuf)
        	{
                	g_warning ("Couldn't load icon ");
        	}
        	else
        	{
                  	gtk_window_set_icon(GTK_WINDOW(dialog),pixbuf);
                	/* Use the pixbuf */
                	g_object_unref (pixbuf);
        	}
		
		gtk_window_set_title(GTK_WINDOW(dialog),_("Warning"));
		int res = gtk_dialog_run (GTK_DIALOG (dialog));

        	if(res == GTK_RESPONSE_YES || res == GTK_RESPONSE_APPLY)
        	{
			if(data->is_erase != NULL)
			{
				cancel_format(data -> block_device);	
			}
			gtk_widget_destroy(dialog);
			return FALSE;
        	}
		else
		{
        		gtk_widget_destroy(dialog);
			return TRUE;
		}
	}
	return FALSE;

}

static void
set_sensitive_false(format_window *data)
{

        gtk_widget_set_sensitive(data->sizecombox,FALSE);
        gtk_widget_set_sensitive(data->typecombox,FALSE);
        gtk_widget_set_sensitive(data->entry_name,FALSE);
        gtk_widget_set_sensitive(data->check_button,FALSE);
        gtk_widget_set_sensitive(data->ensure_button,FALSE);
	gtk_widget_set_sensitive(data->cancel_button,FALSE);

}

static  void
set_sensitive_true(format_window *data)
{
        gtk_widget_set_sensitive(data->sizecombox,TRUE);
        gtk_widget_set_sensitive(data->typecombox,TRUE);
        gtk_widget_set_sensitive(data->entry_name,TRUE);
        gtk_widget_set_sensitive(data->check_button,TRUE);
        gtk_widget_set_sensitive(data->ensure_button,TRUE);
	gtk_widget_set_sensitive(data->cancel_button,TRUE);

}

static void
get_volume_string_size(double volume_size,char *ch)
{
	 if (volume_size > 1){
		if(volume_size<10)
		{
			sprintf(ch,"%.1f",volume_size);
		}
		else
		{
			 sprintf(ch,"%.0f",volume_size);
		} 
		strcat(ch,"GB");
	}
        else if (volume_size > 0.001) {
                volume_size = volume_size*1000;
		if(volume_size<10)
                {
                        sprintf(ch,"%.1f",volume_size);
                }
                else
                {
                         sprintf(ch,"%.0f",volume_size);
                }
                strcat(ch,"MB");
	
	}
        else if (volume_size > 0.000001) {
                volume_size = volume_size*1000*1000;
		if(volume_size<10)
                {
                        sprintf(ch,"%.1f",volume_size);
                }
                else
                {
                         sprintf(ch,"%.0f",volume_size);
                }
                strcat(ch,"KB");
	}
        else if (volume_size > 0.000000001) {
                volume_size = volume_size*1000*1000*1000;
		sprintf(ch,"%.0f",volume_size);
		strcat(ch,"B");
	}
}


static void
create_format_window(format_window *data)
{
	int i=0;
	data->label1 = gtk_label_new(_("Capatity:"));
        data->label2 = gtk_label_new(_("File System:"));
        data->label3 = gtk_label_new(_("Volume Label:"));
        data->label4 = gtk_label_new(_("Formatting Options:"));

        data->sizecombox = gtk_combo_box_text_new();
        gtk_widget_set_size_request(data->sizecombox,276,30);

        data->typecombox = gtk_combo_box_text_new();
        gtk_widget_set_size_request(data->typecombox,276,30);
        char ch[5][10]={"Ext4","Ext3","Ext2","FAT32","NTFS"};
        for(i=0;i<5;i++){
                gtk_combo_box_text_append_text((GtkComboBoxText *)data->typecombox,ch[i]);
        }
        gtk_combo_box_set_active((GtkComboBox *)data->typecombox,0);

        data->entry_name = gtk_entry_new();
        gtk_widget_set_size_request(data->entry_name,276,30);

        data->check_button = gtk_check_button_new_with_label(_("Erase(Need more time)"));
	
	data->processbar = gtk_progress_bar_new();
        gtk_widget_set_size_request(data->processbar,276,20);
	gtk_progress_bar_set_show_text(data->processbar,TRUE);
	gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar," ");

        data->fixed = gtk_fixed_new();
        gtk_widget_set_size_request(data->fixed,296,450);

        data->ensure_button = gtk_button_new_with_label(_("Begin"));
        gtk_widget_set_size_request(data->ensure_button,60,35);

        data->cancel_button = gtk_button_new_with_label(_("Close"));
        gtk_widget_set_size_request(data->cancel_button,60,35);

	GtkWidget *box;
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);
	gtk_box_pack_end(box,data->cancel_button,0,FALSE,10);
	gtk_box_pack_end(box,data->ensure_button,0,FALSE,0);
	gtk_widget_set_size_request(box,296,35);

        gtk_fixed_put(GTK_FIXED(data->fixed),data->label1,10,15);
        gtk_fixed_put(GTK_FIXED(data->fixed),data->sizecombox,10,40);
        gtk_fixed_put(GTK_FIXED(data->fixed),data->label2,10,85);
        gtk_fixed_put(GTK_FIXED(data->fixed),data->typecombox,10,110);
        gtk_fixed_put(GTK_FIXED(data->fixed),data->label3,10,155);
        gtk_fixed_put(GTK_FIXED(data->fixed),data->entry_name,10,180);
        gtk_fixed_put(GTK_FIXED(data->fixed),data->label4,10,225);
        gtk_fixed_put(GTK_FIXED(data->fixed),data->check_button,10,250);
	gtk_fixed_put(GTK_FIXED(data->fixed),data->processbar,10,300);
//	gtk_fixed_put(GTK_FIXED(data->fixed),data->ensure_button,150,350);
  //      gtk_fixed_put(GTK_FIXED(data->fixed),data->cancel_button,220,350);
	gtk_fixed_put(GTK_FIXED(data->fixed),box,0,350);
}

static void create_format_init(format_window *data)
{
	
	if(data->block_device == NULL)
	{
		gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,_("The path is empty"));
		gtk_widget_set_sensitive(data->ensure_button,FALSE);
		return;
	}
	if(find_cb(data->block_device) == 0)
	{
		char ch[100]={0};
		sprintf(ch,_("can't find %s"),data->block_device);
		gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,ch);
	       	gtk_widget_set_sensitive(data->ensure_button,FALSE);
        	return;
	}
	
	data->is_erase = 0;
	data->format_type = gtk_combo_box_text_get_active_text((GtkComboBoxText *)(data->typecombox));
	data->is_format = false;
	data->filesystem_name  ="";
	data->is_erase = NULL;
	data->format_finish = 0;
	data->per = 0;
	
	double volume_size = get_device_size(data->block_device);
	char ch[10];
	get_volume_string_size(volume_size,&ch[0]);
	gchar * volume_label = get_device_label(data->block_device);		
	char window_title[100];	
	if(volume_label == NULL)
	{
		sprintf(window_title,_("format %s volume"),ch);
	}
	else 
	{
		sprintf(window_title,_("format %s"),volume_label);
	}
	
	 gtk_window_set_title((GtkWindow *)data->window,window_title);
	g_free(volume_label);

	gtk_combo_box_text_append_text((GtkComboBoxText *)data->sizecombox,ch);	
	gtk_combo_box_set_active ((GtkComboBox *)(data->sizecombox),0);
	gtk_progress_bar_set_fraction ((GtkProgressBar *)data->processbar,0);

	GtkIconTheme *icon_theme;
        icon_theme = gtk_icon_theme_get_default ();
        GdkPixbuf *pixbuf;
        pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                   "disks", /* icon name */
                                   48, /* size */
                                   0,  /* flags */
                                   NULL);
        if (!pixbuf)
        {
                g_warning ("Couldn't load icon ");
        }
        else
        {
                  gtk_window_set_icon(GTK_WINDOW(data->window),pixbuf);
                /* Use the pixbuf */
		g_object_unref(pixbuf);
	}
	
}

static gboolean
update_quick_format_processbar(gpointer user_data)
{
	
	format_window *data = user_data;
 	int flag = 0;
	if((flag = data->format_finish) != 0)
        {
                if(flag == 1)
                {
		        gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,_("format succeed!"));
                	gtk_progress_bar_set_fraction((GtkProgressBar *)data->processbar,1);
		}
		else
                {
			GdkColor color;
		        gdk_color_parse("red", &color);
        		gtk_widget_modify_fg(GTK_WIDGET(data->processbar), GTK_STATE_NORMAL, &color);

		        gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,_("format failed!"));
                	gtk_progress_bar_set_fraction((GtkProgressBar *)data->processbar,0);
		}
		data->format_finish = 0;
                data->is_format = false;
                set_sensitive_true(data);
                data->per = 0;
                gtk_button_set_label ((GtkButton *)(data->cancel_button),_("Close"));
		
		double volume_size = get_device_size(data->block_device);
        	char ch[10];
        	get_volume_string_size(volume_size,&ch[0]);
        	gchar * volume_label = get_device_label(data->block_device);

        	gchar window_title[100];
        	if(volume_label == NULL)
        	{
			sprintf(window_title,_("format %s volume"),ch);
        	}
        	else
        	{
        		sprintf(window_title,_("format %s"),volume_label);
		}
		
		g_free(volume_label);
        	gtk_window_set_title((GtkWindow *)data->window,window_title);
		gtk_combo_box_text_remove((GtkComboBoxText *)data->sizecombox,0);
       		gtk_combo_box_text_insert_text((GtkComboBoxText *)data->sizecombox,0,ch);
       		gtk_combo_box_set_active ((GtkComboBox *)data->sizecombox,0);

		return FALSE;
        }

	
	
	gtk_progress_bar_pulse ((GtkProgressBar *)data->processbar);
        return TRUE;

}

static gboolean 
update_format_processbar(gpointer user_data)
{
        format_window *data = user_data;
        int flag = 0;
		
        if((flag = data->format_finish) != 0)
        {
                if(flag == 1)
                {        
			gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,_("format succeed!"));
                	gtk_progress_bar_set_fraction((GtkProgressBar *)data->processbar,1);
		}
		else
                {       
			GdkColor color;
		        gdk_color_parse("red", &color);
        		gtk_widget_modify_fg(GTK_WIDGET(data->processbar), GTK_STATE_NORMAL, &color);
 
			gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,_("format failed!"));
                	gtk_progress_bar_set_fraction((GtkProgressBar *)data->processbar,0);
		}
		data->format_finish = 0;
                data->is_format = FALSE;
		set_sensitive_true(data);
		data->per = 0;
                gtk_button_set_label ((GtkButton *)(data->cancel_button),_("Close"));
		double volume_size = get_device_size(data->block_device);
        	char ch[10];
        	get_volume_string_size(volume_size,&ch[0]);
        	gchar * volume_label = get_device_label(data->block_device);
		
		char window_title[100];
        	if(volume_label == NULL)
        	{
			sprintf(window_title,_("format %s volume"),ch);
		}
        	else
        	{
        		sprintf(window_title,_("format %s"),volume_label);
		}
		gtk_window_set_title((GtkWindow *)data->window,window_title);
		gtk_combo_box_text_remove((GtkComboBoxText *)data->sizecombox,0);
        	gtk_combo_box_text_insert_text((GtkComboBoxText *)data->sizecombox,0,ch);
        	gtk_combo_box_set_active ((GtkComboBox *)data->sizecombox,0);

		g_free(volume_label);
		volume_label = NULL;
		return FALSE;
        }
   	
       	gtk_widget_set_sensitive(data->cancel_button,TRUE);
        data->per = get_format_bytes_done(data->block_device);
        if(data->per == 0)return TRUE;
	gtk_progress_bar_set_fraction((GtkProgressBar *)data->processbar,data->per);
	char ch[10]={0};
	sprintf(ch,"%.2f",data -> per *100);
        char bar_text[100];
	sprintf(bar_text,_("Current progress is %s%s"),ch,"%");
	gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,bar_text);
	

	return TRUE;
}

static void do_format(format_window *data)
{
        kdisk_format(data->block_device,data->format_type,data->is_erase,data->filesystem_name,&(data->format_finish));
        data->is_format = true;
        set_sensitive_false(data);
        if(data->is_erase != NULL){
                data->timeout_id = g_timeout_add(1000,update_format_processbar,data);
        }
        else {
                data->timeout_id = g_timeout_add(500,update_quick_format_processbar,data);
        }
        gtk_button_set_label ((GtkButton *)(data->cancel_button),_("Cancel"));
	
}

void unmount_finish(GObject *source_object,
                    GAsyncResult *res,
                    gpointer user_data)
{
        format_window *data = user_data; 
	GError *error = NULL;
        if(g_mount_unmount_with_operation_finish(G_MOUNT(source_object),res,&error)==TRUE)
        {
		do_format(data);
        }
        else
        {
		char ch[10];
        	double volume_size = get_device_size(data->block_device);
		get_volume_string_size(volume_size,&ch[0]);
		gchar * volume_label = get_device_label(data->block_device);
                char message[100]={0};
		if(volume_label == NULL)
                {
                        sprintf(message,_("Unable to unmount %s volume"),ch);	
		}
                else
                {
                        sprintf(message,_("Unable to unmount %s"),volume_label);
                }

                g_free(volume_label);

		gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,message);
	}
        if(error!=NULL)
        {
                g_error_free(error);
        }
        g_object_unref(source_object);
}

static void 
start_button_clicked(GtkWidget *button,gpointer user_data)
{
        format_window *data = user_data;
       
	if(gtk_toggle_button_get_active ((GtkToggleButton *)(data->check_button))==TRUE)
        {
                data->is_erase = "zero";
        }
        else
        {
                data->is_erase = NULL;
        }

	GtkWidget *dialog;
      
	if(data->is_erase != NULL){ 
        	dialog = gtk_message_dialog_new(
                	NULL,
                	GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                	GTK_MESSAGE_INFO,
               		GTK_BUTTONS_YES_NO,
                	_("Erase the device will takes a long time,do you want to continue?"));
        }
	else 
	{
		dialog = gtk_message_dialog_new(
                        NULL,
                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                       	GTK_BUTTONS_YES_NO,
                        _("Formatting will delete all data,are you sure you want to format the device?"));
	}
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW(dialog),FALSE);
	GtkIconTheme *icon_theme;
        GdkPixbuf *pixbuf;
        icon_theme = gtk_icon_theme_get_default ();
        pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                   "disks", /* icon name */
                                   48, /* size */
                                   0,  /* flags */
                                   NULL);
        if (!pixbuf)
        {
                g_warning ("Couldn't load icon ");
        }
        else
        {
                  gtk_window_set_icon(GTK_WINDOW(dialog),pixbuf);
                /* Use the pixbuf */
                g_object_unref (pixbuf);
       	}
	gtk_window_set_title(GTK_WINDOW(dialog),_("Warning"));

	int res = gtk_dialog_run (GTK_DIALOG (dialog));	
	
	if(res == GTK_RESPONSE_YES || res == GTK_RESPONSE_APPLY)
	{
		gtk_widget_destroy(dialog); 
	}
	else 
	{
		gtk_widget_destroy(dialog);
		return ;
	}
	
	if( find_cb(data->block_device)==0)
    	{
        	gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,_("The path is empty"));
		return ;
    	}
	
	data->format_type = gtk_combo_box_text_get_active_text((GtkComboBoxText *)(data->typecombox));	
	
	if(g_strcmp0(data->format_type,"exFAT")==0)
	       	strcpy(data->format_type,"exfat");
	else if(g_strcmp0(data->format_type,"NTFS")==0)
		 strcpy(data->format_type,"ntfs");
	else if(g_strcmp0(data->format_type,"FAT32")==0)
             	 strcpy(data->format_type,"vfat");
	else if(g_strcmp0(data->format_type,"Ext2")==0)
                 strcpy(data->format_type,"ext2");
	else if(g_strcmp0(data->format_type,"Ext3")==0)
                 strcpy(data->format_type,"ext3");
	else 
		strcpy(data->format_type,"ext4");
	
	GdkColor color;
        gdk_color_parse("black", &color);
	
        gtk_widget_modify_fg(GTK_WIDGET(data->processbar), GTK_STATE_NORMAL,&color);

	gtk_progress_bar_set_fraction ((GtkProgressBar *)data->processbar,0);
	gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,"");	
	data->filesystem_name = gtk_entry_get_text ((GtkEntry *)(data->entry_name));
	
	/*卸载*/
	 GVolumeMonitor *monitor = g_volume_monitor_get ();
         GList *volumes,*l;
         GVolume *volume;
         GMount *mount = NULL;
         int find_volume = 0;
         volumes = g_volume_monitor_get_volumes(monitor);
         g_object_unref(monitor);
         for (l = volumes; l != NULL; l = l->next)
         {
                volume = l->data;
                char *ch=g_volume_get_identifier(volume,G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE);
                if(g_strcmp0(ch,data->block_device)==0)
                {
                        find_volume = 1;
                        g_free(ch);
                        break;
                }
                g_free(ch);
         }
         if(find_volume == 1)
         {
                mount = g_volume_get_mount(volume);
         }
         g_list_foreach (volumes, (GFunc) g_object_unref, NULL);
         g_list_free (volumes);

         if(mount == NULL)
         {
		do_format(data);                
                return ;
         }
         if(g_mount_can_unmount(mount)==FALSE)
         {
		char ch[10];
                double volume_size = get_device_size(data->block_device);
                get_volume_string_size(volume_size,&ch[0]);
                gchar * volume_label = get_device_label(data->block_device);
                char message[100]={0};
                if(volume_label == NULL)
                {
                        sprintf(message,_("Unable to unmount %s volume"),ch);
                }
                else
                {
                        sprintf(message,_("Unable to unmount %s"),volume_label);
                }

                g_free(volume_label);

                gtk_progress_bar_set_text ((GtkProgressBar *)data->processbar,message);

		g_object_unref(mount);
                return ;
         }

         g_mount_unmount_with_operation(mount,G_MOUNT_UNMOUNT_NONE,NULL,NULL,unmount_finish,data);

}


static void
cancel_button_clicked(GtkWidget *button,gpointer user_data)
{
	format_window *data = user_data;
	if(data->is_format == true)
	{
		GtkWidget *dialog;

        	dialog = gtk_message_dialog_new(
                	NULL,
                	GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                	GTK_MESSAGE_INFO,
                	GTK_BUTTONS_YES_NO,
                	_("Formatting is in progress, is it canceled?"));
        	
		GtkIconTheme *icon_theme;
        	GdkPixbuf *pixbuf;
        	icon_theme = gtk_icon_theme_get_default ();
        	pixbuf = gtk_icon_theme_load_icon (icon_theme,
                	"disks", /* icon name */
               		48, /* size */
               		0,  /* flags */
                	NULL);
        	if (!pixbuf)
        	{
                	g_warning ("Couldn't load icon ");
        	}
        	else
        	{
                  	gtk_window_set_icon(GTK_WINDOW(dialog),pixbuf);
                	g_object_unref (pixbuf);
        	}
		gtk_window_set_skip_taskbar_hint (GTK_WINDOW(dialog),FALSE);
		gtk_window_set_title(GTK_WINDOW(dialog),_("Warning"));

		int res = gtk_dialog_run (GTK_DIALOG (dialog));

        	if(res == GTK_RESPONSE_YES || res == GTK_RESPONSE_APPLY)
        	{
			gtk_widget_destroy(dialog);
			cancel_format(data->block_device);
			data->is_format = 0;
		}
        	else
        	{
			gtk_widget_destroy(dialog);
                	return ;
        	}
	}
	else 
	{
		gtk_widget_destroy(data->window);
	}

}

int kdiskformat(const gchar * path)
{
	if(FORMAT_WINDOW != 0)return 0;	
	FORMAT_WINDOW +=1;
	format_window *data;

	data=g_new0(format_window,1);
	
	data->block_device = path;
	data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title(GTK_WINDOW(data->window),_("Format Volume"));
	gtk_window_set_position(GTK_WINDOW(data->window),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(data->window),296,440);
	gtk_window_set_resizable(GTK_WINDOW(data->window),FALSE);	

	create_format_window(data);
	create_format_init(data);

	g_signal_connect(GTK_WIDGET(data->window),"delete_event",G_CALLBACK(ensure_delete_format_window),data);
	g_signal_connect(GTK_WIDGET(data->window),"destroy",G_CALLBACK(delete_format_window),data);
 	g_signal_connect(GTK_BUTTON(data->ensure_button),"clicked",G_CALLBACK(start_button_clicked),data);
	g_signal_connect(GTK_BUTTON(data->cancel_button),"clicked",G_CALLBACK(cancel_button_clicked),data); 	
	
	gtk_container_add(GTK_CONTAINER(data->window),data->fixed);	
	
	gtk_widget_show_all(data->window);
	return 0;
}
