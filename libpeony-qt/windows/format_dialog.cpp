/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: ZJAY998 <zhangjun@kylinos.cn>
 *
 */

#include "format_dialog.h"
#include "ui_format_dialog.h"

#include <QMessageBox>
#include <QObject>

using namespace  Peony;

Format_Dialog::Format_Dialog(const QString &m_uris,SideBarAbstractItem *m_item,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Format_Dialog)
{
       ui->setupUi(this);

       fm_uris = m_uris;
       fm_item = m_item;


       //from uris get the rom size
       //FIXME: replace BLOCKING api in ui thread.
       auto targetUri = FileUtils::getTargetUri(fm_uris);
       GFile *fm_file = g_file_new_for_uri(targetUri .toUtf8().constData());

       GFileInfo *fm_info = g_file_query_filesystem_info(fm_file, "*", nullptr, nullptr);
       quint64 total = g_file_info_get_attribute_uint64(fm_info, G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

       //get the rom size
       char *total_format = g_format_size(total);

       //add the rom size value into  rom_size combox
       ui->comboBox_rom_size->addItem(total_format);


       auto mount = VolumeManager::getMountFromUri(targetUri);
       ui->lineEdit_device_name->setText(mount->name());

       ui->progressBar_process->setValue(0);


       connect(ui->pushButton_ok, SIGNAL(clicked(bool)), this, SLOT(acceptFormat(bool)));

       connect(ui->pushButton_close, SIGNAL(clicked(bool)), this, SLOT(colseFormat(bool)));
}

void Format_Dialog::colseFormat(bool)
{

    char dev_name[256] ={0};
    //get device name
    QString volname, devName, voldisplayname;

    //FIXME: replace BLOCKING api in ui thread.
    FileUtils::queryVolumeInfo(fm_uris, volname, devName, voldisplayname);
    strncpy(dev_name,devName.toUtf8().constData(),sizeof(devName.toUtf8().constData()-1));

    //cancel format function
    cancel_format(dev_name);
}

void Format_Dialog::acceptFormat(bool)
{

    ui->pushButton_ok->setDisabled(TRUE);
    ui->pushButton_close->setDisabled(TRUE);
    ui->lineEdit_device_name->setDisabled(TRUE);
    ui->checkBox_clean_or_not->setDisabled(TRUE);

    //init the value
    char rom_size[1024] ={0},rom_type[1024]={0},rom_name[1024]={0},dev_name[1024]={0};
    int quick_clean = 0;


    //get values from ui
    strncpy(rom_size,ui->comboBox_rom_size->itemText(0).toUtf8().constData(),sizeof(ui->comboBox_rom_size->itemText(0).toUtf8().constData())-1);
    strncpy(rom_type,ui->comboBox_system->currentText().toUtf8().constData(),sizeof(ui->comboBox_system->currentText().toUtf8().constData())-1);
    strcpy(rom_name,ui->lineEdit_device_name->text().toUtf8().constData());

    //disable name and rom size list
    ui->comboBox_rom_size->setDisabled(true);
    ui->comboBox_system->setDisabled(true);

    quick_clean = ui->checkBox_clean_or_not->isChecked();

    // umount device
    fm_item ->unmount();

    //get device name
    QString volname, devName, voldisplayname ,devtype;

    //FIXME: replace BLOCKING api in ui thread.
    FileUtils::queryVolumeInfo(fm_uris, volname, devName, voldisplayname);
    strcpy(dev_name,devName.toUtf8().constData());

    devtype = rom_type;

    //do format
    //enter kdisk_format function

    //init format_finish value
    int format_value = 0;

    //begin format
    kdisk_format(dev_name,devtype.toLower().toUtf8().constData(),quick_clean?"zero":NULL,rom_name,&format_value);

    //begin start my_timer, processbar
    my_time  = new QTimer(this);

    m_cost_seconds = 0;
    if(quick_clean){
        my_time->setInterval(500);
        m_total_predict = 1200;
    }else{
        my_time->setInterval(1000);
        m_total_predict = 3600;
    }

    //begin loop
    connect(my_time,SIGNAL(timeout()),this,SLOT(formatloop()));

    my_time->start();

}

double Format_Dialog::get_format_bytes_done(const gchar * device_name)
{
    UDisksObject *object ;
    UDisksClient *client =udisks_client_new_sync (NULL,NULL);
    object = get_object_from_block_device(client,device_name);
    GList *jobs;
    jobs = udisks_client_get_jobs_for_object(client,object);
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


void Format_Dialog::formatloop(){

    QString volname, devName, voldisplayname;
    static char name_dev[256] ={0};
    char prestr[10] = {0};

    //cost time count
    m_cost_seconds++;

    //FIXME: replace BLOCKING api in ui thread.
    FileUtils::queryVolumeInfo(fm_uris, volname, devName, voldisplayname);

    if(nullptr != devName)
    strcpy(name_dev,devName.toUtf8().constData());

    double pre = (get_format_bytes_done(name_dev) * 100);
    double cost = m_cost_seconds * 100/m_total_predict;

    if ((pre < 1 && cost <= 99) || pre < cost)
        pre = cost;

    sprintf(prestr,"%.1f",pre);
    strcat(prestr,"%");

    if(pre>=100){

        ui->progressBar_process->setValue(100);

        ui->label_process->setText("100%");

        my_time->stop();

    }else{

          ui->progressBar_process->setValue(pre);
          ui->label_process->setText(prestr);
    }

}


void Format_Dialog::cancel_format(const gchar* device_name){

      this->close();

//    UDisksObject *object ;
//    UDisksBlock *block;
//    UDisksClient *client =udisks_client_new_sync (NULL,NULL);
//    object = get_object_from_block_device(client,device_name);
//    block = udisks_object_get_block (object);
//    GList *jobs;

//    jobs = udisks_client_get_jobs_for_object (client,object);

//    if(jobs != NULL){
//                udisks_job_call_cancel_sync ((UDisksJob *)jobs->data,
//                           g_variant_new ("a{sv}", NULL),
//                           NULL,
//                           NULL);
//    }

//    g_list_foreach (jobs, (GFunc) g_object_unref, NULL);
//    g_list_free (jobs);
//    g_clear_object(&client);
//    g_object_unref(object);
//    g_object_unref(block);


};


/*  is_iso  function to  check is iso or not
 *
 *  args : const gchar *device_path
 *
 *  device_path: device path
 */
gboolean Format_Dialog::is_iso(const gchar *device_path){
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

};


/*
 * ensure_unused_cb   ensure to unused this
 *
 * args:
 *
 * CreateformatData *data: the data info device
 *
 * if is not iso , do ensure_format_cb.
 *
 * if is do format_disk
 *
 */
void Format_Dialog::ensure_unused_cb(CreateformatData *data)
{

    if(is_iso(data->device_name)==FALSE) {     
        ensure_format_cb (data);
    }
    else {
        ensure_format_disk(data);
    }
}

static void createformatfree(CreateformatData *data)
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


// format
static void format_cb (GObject *source_object, GAsyncResult *res ,gpointer user_data)
{
    static int end_flag = -1;

    CreateformatData *data = (CreateformatData *)user_data;
    GError *error = NULL;
    if (!udisks_block_call_format_finish (UDISKS_BLOCK (source_object), res,&error))
    {
        char *p = NULL;
        if(NULL!=error&&NULL!=(p=strstr(error->message, "wipefs:"))){
            notify_init("Peony");
            NotifyNotification *notify;
            QString title = QObject::tr("File Manager");
            notify = notify_notification_new (title.toUtf8().constData(),
                                              p,
                                              "drive-removable-media-usb"
                                              );
            notify_notification_show(notify,NULL);
            g_object_unref(G_OBJECT(notify));
            notify_uninit();
            g_clear_error(&error);
        }
        end_flag = -1;
        *(data->format_finish) =  -1; //format success

    }
    else
        end_flag = 1;
        *(data->format_finish) =  1; //format error


    if(end_flag == 1){

        data->dl->ui->progressBar_process->setValue(100);
        data->dl->ui->label_process->setText("100%");

        data->dl->format_ok_dialog();

    }else{

        data->dl->format_err_dialog();  
    }

    data->dl->my_time->stop();
    data->dl->close();

    createformatfree(data);

};


void Format_Dialog::format_ok_dialog(){

    QMessageBox::about(parentWidget(),tr("qmesg_notify"),tr("format_success"));
};


void Format_Dialog::format_err_dialog(){

      QMessageBox::warning(parentWidget(),tr("qmesg_notify"),tr("format_err"));
};


/* ensure_format_cb ,function ensure to do format
 *
 * args: CreateformatData *data
 *
 */
void Format_Dialog::ensure_format_cb (CreateformatData *data){


    GVariantBuilder options_builder;

    g_variant_builder_init(&options_builder,G_VARIANT_TYPE_VARDICT);

    if (g_strcmp0 (data->format_type, "empty") != 0){
        g_variant_builder_add (&options_builder, "{sv}", "label",
                               g_variant_new_string (data->filesystem_name));
    };



    if (g_strcmp0 (data->format_type, "vfat") != 0 &&
            g_strcmp0 (data->format_type, "ntfs") != 0 &&
            g_strcmp0 (data->format_type, "exfat") != 0){
        g_variant_builder_add (&options_builder, "{sv}", "take-ownership",
                               g_variant_new_boolean (TRUE));
    }


    if (data->erase_type != NULL){
        g_variant_builder_add (&options_builder, "{sv}", "erase",
                               g_variant_new_string (data->erase_type));
    }

    g_variant_builder_add (&options_builder, "{sv}", "update-partition-type",
                           g_variant_new_boolean (TRUE));


    udisks_block_call_format (data->block,
                              data->format_type,
                              g_variant_builder_end (&options_builder),
                              NULL,
                              format_cb,
                              data);


};


/*
 * ensure format iso disk
 *
 *
*/
void Format_Dialog::ensure_format_disk(CreateformatData *data){

        char ch[10]={0};
        for(int i=0;i<=7;i++)
            ch[i]=(data->device_name)[i];
        data->drive_object = get_object_from_block_device(data->client,ch);
        data->drive_block = udisks_object_get_block(data->drive_object);


        GVariantBuilder options_builder;
        g_variant_builder_init(&options_builder,G_VARIANT_TYPE_VARDICT);


        if (g_strcmp0 (data->format_type, "empty") != 0){
            g_variant_builder_add (&options_builder, "{sv}", "label",
                                   g_variant_new_string (data->filesystem_name));
        };


        if (g_strcmp0 (data->format_type, "vfat") != 0 &&
                g_strcmp0 (data->format_type, "ntfs") != 0 &&
                g_strcmp0 (data->format_type, "exfat") != 0){
            g_variant_builder_add (&options_builder, "{sv}", "take-ownership",
                                   g_variant_new_boolean (TRUE));
        }


        if (data->erase_type != NULL){
            g_variant_builder_add (&options_builder, "{sv}", "erase",
                                   g_variant_new_string (data->erase_type));
        }

        g_variant_builder_add (&options_builder, "{sv}", "update-partition-type",
                               g_variant_new_boolean (TRUE));


        udisks_block_call_format(data->drive_block,
                        data->format_type,
                        g_variant_builder_end(&options_builder),
                        NULL,
                        format_cb,
                        data);

}





UDisksObject *Format_Dialog::get_object_from_block_device (UDisksClient *client,const gchar *block_device)
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

/*  kdisk_format  function to format device
 *
 *  args:
 *  kdisk_format format the disk and  get_object_from_device
 *  device_name: device name
 *  format_type: device type to format
 *  erase_type:  clean it quick or not ,if zero not NULL
 *  filesystem_name: the device name display in filesystem
 *  format_finish: format finish is 1 ,not 0
 */
void Format_Dialog::kdisk_format(const gchar * device_name,const gchar *format_type,const gchar * erase_type,
                                const gchar * filesystem_name,int *format_finish){

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
    data->dl = this;

    data->client = udisks_client_new_sync(NULL,NULL);

    data->object = get_object_from_block_device(data->client,data->device_name);
    data->block = udisks_object_get_block(data->object);

    ensure_unused_cb(data);
}


Format_Dialog::~Format_Dialog()
{
    delete ui;
}
