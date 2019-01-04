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

#include "peony-kdisk-createformat.h"
typedef struct
{
        UDisksObject *object,*drive_object;
        UDisksBlock *block,*drive_block;
        UDisksClient *client;
        
	const gchar *format_type;
        const gchar *device_name;
        const gchar *erase_type;
        const gchar *filesystem_name;

        int *format_finish;
}CreateformatData;

static UDisksObject *
get_object_from_block_device 	(UDisksClient *client,
                              		const gchar *block_device);
static void
ensure_format_disk	(CreateformatData *data);

static void
createformatfree(CreateformatData *data)
{
	g_object_unref(data->object);
        g_object_unref(data->block);
	if(data->drive_object!=NULL)
	{
		g_object_unref(data->drive_object);
	}
	if(data->drive_block!=NULL)
        {
                g_object_unref(data->drive_block);
        }
	g_clear_object(&(data->client));

        g_free(data);

}

gboolean is_iso(const gchar *device_path)
{
	UDisksObject *object;
	UDisksClient *client;
	UDisksBlock *block;
	client = udisks_client_new_sync(NULL,NULL);
	object = get_object_from_block_device(client,device_path);
	block = udisks_object_get_block(object);
	
	if(g_strcmp0(udisks_block_get_id_type(block),"iso9660")==0)
	{	
		g_object_unref(object);
		g_object_unref(block);
		g_clear_object(&client);
		return TRUE;
	}
	g_object_unref(object);
        g_object_unref(block);
        g_clear_object(&client);

	return FALSE;
}

static UDisksObject *
get_object_from_block_device (UDisksClient *client,
                              const gchar *block_device)
{
    	struct stat statbuf;
    	const gchar *crypto_backing_device;
    	UDisksObject *object, *crypto_backing_object;
    	UDisksBlock *block;

    	object = NULL;

    	if (stat (block_device, &statbuf) != 0)
    	{
        	return object;
    	}

    	block = udisks_client_get_block_for_dev (client, statbuf.st_rdev);
    	if (block == NULL)
    	{
        	return object;
    	}

    	object = UDISKS_OBJECT (g_dbus_interface_dup_object (G_DBUS_INTERFACE (block)));
    	g_object_unref (block);

    	crypto_backing_device = udisks_block_get_crypto_backing_device ((udisks_object_peek_block (object)));
    	crypto_backing_object = udisks_client_get_object (client, crypto_backing_device);
    	if (crypto_backing_object != NULL)
    	{
        	g_object_unref (object);
        	object = crypto_backing_object;
    	}
    	return object;
}

gchar * get_device_label(const gchar * device_name)
{
    	UDisksObject *object ;
    	UDisksBlock *block;
    	UDisksClient *client =udisks_client_new_sync (NULL,NULL);
    	object = get_object_from_block_device(client,device_name);
    	block = udisks_object_get_block (object);
    	gchar * name = udisks_block_dup_id_label(block);
    	g_clear_object(&client);
    	g_object_unref(object);
    	g_object_unref(block);

    	if(strlen(name) == 0) return NULL;
	return name;


}

static void
format_cb (GObject *source_object, GAsyncResult *res ,gpointer user_data)
{
	CreateformatData *data = user_data;
   	if (!udisks_block_call_format_finish (UDISKS_BLOCK (source_object), res,NULL))
      		*(data->format_finish) =  -1; //格式化失败
   	else
      		*(data->format_finish) =  1; //格式化完成
	createformatfree(data);
}

static void
ensure_format_cb (CreateformatData *data)
{
    	GVariantBuilder options_builder;

    	g_variant_builder_init (&options_builder, G_VARIANT_TYPE_VARDICT);
    	if (g_strcmp0 (data->format_type, "empty") != 0){
        	g_variant_builder_add (&options_builder, "{sv}", "label",
                               g_variant_new_string (data->filesystem_name));
    	}
    	
	if (g_strcmp0 (data->format_type, "vfat") != 0 && g_strcmp0 (data->format_type, "ntfs") != 0 && g_strcmp0 (data->format_type, "exfat") != 0)
    	{
      		g_variant_builder_add (&options_builder, "{sv}", "take-ownership", g_variant_new_boolean (TRUE));
    	}

	if (data->erase_type != NULL){
		g_variant_builder_add (&options_builder, "{sv}", "erase", g_variant_new_string (data->erase_type));
	}
   		g_variant_builder_add (&options_builder, "{sv}", "update-partition-type", g_variant_new_boolean (TRUE));
    	udisks_block_call_format (data->block,
				data->format_type,
                                g_variant_builder_end (&options_builder),
                                NULL,
                                format_cb,
                                data);
}

static void
create_partition_table (GObject *source_object, GAsyncResult *res ,gpointer user_data)
{
        CreateformatData *data = user_data;
        if (udisks_partition_table_call_create_partition_finish (UDISKS_PARTITION_TABLE (source_object), NULL,res,NULL))
	{
	// 	*(data->format_finish) =  -1; //创建分区失败
	//	createformatfree(data);
		 ensure_format_cb(data);
		return;
	}
        else
		ensure_format_cb(data);	
}

static void 
ensure_create_partition_table(CreateformatData *data)
{
	UDisksPartitionTable *table;
	GVariantBuilder options_builder;
        g_variant_builder_init(&options_builder,G_VARIANT_TYPE_VARDICT);
	table=udisks_object_get_partition_table(data->drive_object);
	g_variant_builder_add(&options_builder,"{sv}","partition-type",g_variant_new_string("primary"));
	udisks_partition_table_call_create_partition(table,
							0,
							udisks_block_get_size(data->drive_block),
							"",
							"",
							g_variant_builder_end(&options_builder),
							NULL,
							create_partition_table,
							data);
		
}

static void
format_disk (GObject *source_object, GAsyncResult *res ,gpointer user_data)
{
        CreateformatData *data = user_data;
        if (!udisks_block_call_format_finish (UDISKS_BLOCK (source_object), res,NULL))
	{
		*(data->format_finish) =  -1; //格式化失败
		createformatfree(data);	
		return ;
	}
	else{
		//格式化磁盘，接下来创建分区表
		ensure_create_partition_table(data);
	}
}

static void
ensure_format_disk(CreateformatData *data)
{
	UDisksClient	*client;
	char ch[10]={0};
	for(int i=0;i<=7;i++)	
		ch[i]=(data->device_name)[i];
	data->drive_object = get_object_from_block_device(data->client,ch); 
	data->drive_block = udisks_object_get_block(data->drive_object);
	GVariantBuilder options_builder;
	g_variant_builder_init(&options_builder,G_VARIANT_TYPE_VARDICT);
	udisks_block_call_format(data->drive_block,
					"dos",
					g_variant_builder_end(&options_builder),
					NULL,
					format_disk,
					data);	
}

static void
ensure_unused_cb(CreateformatData *data)
{

        if(is_iso(data->device_name)==FALSE)
                ensure_format_cb (data);
        else
                ensure_format_disk(data);
}
double
get_device_size(const gchar * device_name)
{
    	UDisksObject *object ;
    	UDisksBlock *block;
    	UDisksClient *client =udisks_client_new_sync (NULL,NULL);
    	object = get_object_from_block_device(client,device_name);
    	block = udisks_object_get_block (object);
    	guint64 size = udisks_block_get_size (block);
    	double volume_size =(double)size/1000/1000/1000;

    	g_clear_object(&client);
    	g_object_unref(object);
    	g_object_unref(block);
    	return volume_size;

}

void
cancel_format(const gchar * device_name)
{
    	UDisksObject *object ;
    	UDisksBlock *block;
    	UDisksClient *client =udisks_client_new_sync (NULL,NULL);
    	object = get_object_from_block_device(client,device_name);
    	block = udisks_object_get_block (object);
    	GList *jobs;

    	jobs = udisks_client_get_jobs_for_object (client,object);

    	if(jobs != NULL){
                    udisks_job_call_cancel_sync ((UDisksJob *)jobs->data,
                               g_variant_new ("a{sv}", NULL),
                               NULL,
                               NULL);
    	}
    	g_list_foreach (jobs, (GFunc) g_object_unref, NULL);
    	g_list_free (jobs);
    	g_clear_object(&client);
    	g_object_unref(object);
    	g_object_unref(block);
}

double get_format_bytes_done(const gchar * device_name)
{
    	UDisksObject *object ;
    	UDisksClient *client =udisks_client_new_sync (NULL,NULL);
    	object = get_object_from_block_device(client,device_name);
    	GList *jobs;
    	jobs = udisks_client_get_jobs_for_object (client,object);
    	g_clear_object(&client);
    	g_object_unref(object);
    	if(jobs!=NULL)
    	{
        	UDisksJob *job =(UDisksJob *)jobs->data;
        	if(udisks_job_get_progress_valid (job))
        	{
			double res = udisks_job_get_progress(job);
            		g_list_foreach (jobs, (GFunc) g_object_unref, NULL);
            		g_list_free (jobs);
            		return res;
        	}
	 	g_list_foreach (jobs, (GFunc) g_object_unref, NULL);
         	g_list_free (jobs);
	}
    	return 0;

}

int find_cb(const gchar *device_name)
{
    	UDisksObject *object ;
    	UDisksClient *client =udisks_client_new_sync (NULL,NULL);
    	object = get_object_from_block_device(client,device_name);
    	g_clear_object(&client);
    	g_object_unref(object);
    	if(object == NULL)return 0;
    	else return 1;
}

void
kdisk_format(const gchar * device_name,const gchar *format_type,const gchar * erase_type,const gchar * filesystem_name,int *format_finish)
{
	CreateformatData *data;
	data = g_new(CreateformatData,1);
    	data->format_finish = 0;
	data->device_name = device_name;
	data->format_type = format_type;
	data->erase_type = erase_type;
	data->filesystem_name = filesystem_name;
	data->format_finish = format_finish;
	data->drive_object = NULL;
	data->drive_block = NULL;
		
	data->client =udisks_client_new_sync (NULL,NULL);
        data->object = get_object_from_block_device(data->client,data->device_name);
        data->block = udisks_object_get_block (data->object);
	
	ensure_unused_cb(data);
}






