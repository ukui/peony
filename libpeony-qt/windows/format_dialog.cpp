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

#include "file-utils.h"
#include "format_dialog.h"
#include "side-bar-menu.h"
#include "volume-manager.h"
#include "ui_format_dialog.h"
#include "side-bar-abstract-item.h"
#include "linux-pwd-helper.h"
#include "volumeManager.h"
#include "file-info.h"
#include "file-info-job.h"
#include <QObject>
#include <QMessageBox>
#include <KWindowSystem>
#include <QCloseEvent>

using namespace  Peony;
static bool b_finished = false;
static bool b_failed = false;
static bool b_canClose = true;

Format_Dialog::Format_Dialog(const QString &m_uris,SideBarAbstractItem *m_item,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Format_Dialog)
{
       ui->setupUi(this);
       setFixedSize(this->width(), this->height());

       fm_uris = m_uris;
       fm_item = m_item;
       m_parent = parent;
       b_canClose = true;

       /*!
         如果没有uri，尝试从computer:///获取uri，实际上未必能获取到computer:///的uri，比如#90081这种情况
         */
       if (fm_uris.isEmpty() && !fm_item->getDevice().isEmpty()) {
           auto itemUris = FileUtils::getChildrenUris("computer:///");
           for (auto itemUri : itemUris) {
               auto unixDevice = FileUtils::getUnixDevice(itemUri);
               if (unixDevice == fm_item->getDevice()) {
                   fm_uris = itemUri;
                   break;
               }
           }
       }

       //from uris get the rom size
       //FIXME: replace BLOCKING api in ui thread.
       auto targetUri = FileUtils::getTargetUri(fm_uris);
       if (targetUri.isEmpty()) {
           targetUri = fm_uris;
       }
       g_autoptr(GFile) fm_file = g_file_new_for_uri(targetUri .toUtf8().constData());

       g_autoptr(GFileInfo) fm_info = g_file_query_filesystem_info(fm_file, "*", nullptr, nullptr);
       quint64 total = g_file_info_get_attribute_uint64(fm_info, G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);

       //add the rom size value into  rom_size combox
       //fix system Udisk calculate size wrong issue
       QString m_volume_name, m_unix_device, m_display_name,m_fs_type;
       FileUtils::queryVolumeInfo(m_uris, m_volume_name, m_unix_device, m_display_name);
       bool hasSetRomSize = false;
       m_fs_type = FileUtils::getFileSystemType(m_uris);
       //U disk or other mobile device, only use in iso system install device
       if (! m_unix_device.isEmpty() && m_fs_type.startsWith("iso")
           && ! m_uris.startsWith("computer:///WDC"))
       {
          char dev_name[256] ={0};
          strncpy(dev_name, m_unix_device.toUtf8().constData(),sizeof(m_unix_device.toUtf8().constData()-1));
          auto size = FileUtils::getDeviceSize(dev_name);
          if (size > 0)
          {
              QString sizeInfo = QString::number(size, 'f', 1);
              qDebug() << "size:" <<size;
              sizeInfo += "G";
              ui->label_rom_size_text->setText(sizeInfo);
              hasSetRomSize = true;
          }
       }

       if (! hasSetRomSize)
       {
           //Calculated by 1024 bytes
           char *total_format = strtok(g_format_size_full(total,G_FORMAT_SIZE_IEC_UNITS),"iB");
           ui->label_rom_size_text->setText(total_format);
       }

       auto mount = VolumeManager::getMountFromUri(targetUri);
       //fix name not show complete in bottom issue, bug#36887
       ui->lineEdit_device_name->setFixedHeight(40);
       if (mount.get()) {
            ui->lineEdit_device_name->setText(mount->name());
       } else {
            ui->lineEdit_device_name->setText(LinuxPWDHelper::getCurrentUser().fullName());
       }

       ui->progressBar_process->setValue(0);

       // vfat/fat32 格式存在卷标相关问题，目前无法解决只能规避，参考#83826
       // 如果是可移动设备，格式化时默认使用ntfs格式，可以支持长卷标名，不可移动设备默认按Ext4格式格式化
       g_autoptr(GFile) computer_file = g_file_new_for_uri(m_uris.toUtf8().constData());
       g_autoptr(GFileInfo) gfileinfo = g_file_query_info(computer_file, "*", G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
       bool isRemoveable = g_file_info_get_attribute_boolean(gfileinfo, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_EJECT) || g_file_info_get_attribute_boolean(gfileinfo, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_STOP);
       bool mountable = g_file_info_get_attribute_boolean(gfileinfo, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_MOUNT) || g_file_info_get_attribute_boolean(gfileinfo, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_UNMOUNT);
       if (isRemoveable && mountable) {
           ui->comboBox_system->setCurrentText("NTFS");
       } else {
           ui->comboBox_system->setCurrentText("Ext4");
       }

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


static void unmount_finished(GFile* file, GAsyncResult* result, gpointer udata)
{
    
    int flags = 0;
    GError *err = nullptr;
    Format_Dialog *pthis = (Format_Dialog *)udata;

    if (g_file_unmount_mountable_with_operation_finish (file, result, &err) == TRUE){
        flags = 1;
        char *uri = g_file_get_uri(file);
        VolumeManager::getInstance()->fileUnmounted(uri);
        if (uri)
            g_free(uri);
    }

    //add QMessageBox error ,let user know the error messages
    
    if (err) {
        flags = 0;
        b_failed = true;
        
        QMessageBox message_error;
          
        message_error.setText(QObject::tr("Error: %1\n").arg(err->message));

        message_error.setWindowTitle(QObject::tr("Format failed"));

        QPushButton *okButton = message_error.addButton(QObject::tr("YES"),QMessageBox::YesRole);

        message_error.exec();
          
        if(message_error.clickedButton() == okButton)
        {    
            pthis->close();
        }    
        
        g_error_free(err);
    }

    
    if(flags){
        Q_EMIT pthis->ensure_format(true);
    }
}

void Format_Dialog::acceptFormat(bool)
{
    ui->pushButton_ok->setDisabled(true);
    ui->pushButton_close->setDisabled(true);

    //check format or not 
    if(!format_makesure_dialog()){
        ui->pushButton_ok->setDisabled(false);
        ui->pushButton_close->setDisabled(false);
        return;
    };

    //keep this window above, fix bug #41227
    KWindowSystem::setState(this->winId(), KWindowSystem::KeepAbove);

    //init the value
    char rom_size[1024] ={0},rom_type[1024]={0},rom_name[1024]={0},dev_name[1024]={0};
    int full_clean = 0;

    QString romType = ui->comboBox_system->currentText();
    if (QString("vfat/fat32") == romType) {
        romType = "vfat";
    }

    //get values from ui
    //strncpy(rom_size,ui->comboBox_rom_size->itemText(0).toUtf8().constData(),sizeof(ui->comboBox_rom_size->itemText(0).toUtf8().constData())-1);
    strncpy(rom_type, romType.toUtf8().constData(), strlen(romType.toUtf8().constData()));
    strcpy(rom_name,ui->lineEdit_device_name->text().trimmed ().toUtf8().constData());

    //disable name and rom size list
    //ui->comboBox_rom_size->setDisabled(true);
    ui->comboBox_system->setDisabled(true);

    full_clean = ui->checkBox_clean_or_not->isChecked();

    QString volname, devName, voldisplayname ,devtype;
    bool bEnsureFormat = false;
    auto info = FileInfo::fromUri(fm_uris);
    if (info.get()->isEmptyInfo()) {
        FileInfoJob j(info);
        j.querySync();
    }
    auto targetUri = FileUtils::getTargetUri(fm_uris);
    auto mount = VolumeManager::getInstance()->getMountFromUri(targetUri);
    if(mount){
       /* unmount */
       auto files = wrapGFile(g_file_new_for_uri(this->fm_uris.toUtf8().constData()));
       g_file_unmount_mountable_with_operation(files.get()->get(),
                                               G_MOUNT_UNMOUNT_NONE,
                                               nullptr,
                                               nullptr,
                                               GAsyncReadyCallback(unmount_finished),
                                               this);
    }else{
        bEnsureFormat = true;
    }
       //get device name
       //FIXME: replace BLOCKING api in ui thread.
    FileUtils::queryVolumeInfo(fm_uris, volname, devName, voldisplayname);


    strcpy(dev_name,devName.toUtf8().constData());
    devtype = rom_type;

    //do format
    //enter kdisk_format function

    //init format_finish value
    //int format_value = 0;

    //begin format
    //kdisk_format(dev_name,devtype.toLower().toUtf8().constData(),full_clean?"zero":NULL,rom_name,&format_value);

    //begin start my_timer, processbar
    my_time  = new QTimer(this);

    m_cost_seconds = 0;
    m_simulate_progress = 0;
    b_finished = false;
    b_failed = false;
    if(full_clean){
        my_time->setInterval(1000);
        m_total_predict = 1800;
    }else{
        my_time->setInterval(500);
        m_total_predict = 150;
    }

    //begin loop
    connect(my_time, SIGNAL(timeout()), this, SLOT(formatloop()));

    //while get ensure emit , do format 
    connect(this,&Format_Dialog::ensure_format,[=](bool){
        // time begin loop
        my_time->start();

        // set ui button disable
        ui->pushButton_ok->setDisabled(TRUE);
        ui->pushButton_close->setDisabled(TRUE);
        //ui->lineEdit_device_name->setDisabled(TRUE);
        //use set readonly property, fix exit issue link to task#33686
        ui->lineEdit_device_name->setReadOnly(true);
        ui->checkBox_clean_or_not->setDisabled(TRUE);

        int format_value = 0;
        //do format
        kdisk_format(dev_name, devtype.toLower().toUtf8().constData(),
                     full_clean?"zero":NULL, rom_name,&format_value);
    });
    if(bEnsureFormat){
        Q_EMIT ensure_format(true);
        bEnsureFormat = false;
    }
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

    if (b_finished)
    {
        ui->progressBar_process->setValue(100);
        ui->label_process->setText("100%");
        my_time->stop();
        return;
    }
    else if (b_failed)
    {
        my_time->stop();
        return;
    }

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
    if (cost > 100)
        cost = 100;

    if (m_simulate_progress >= pre){
        //fix waiting in 100% issue
        if (m_simulate_progress < 99)
           m_simulate_progress += (cost - pre)/100;
    }
    else{
        m_simulate_progress = pre;
    }

    qDebug() << "formatloop predict and cost:" <<pre <<cost
             <<m_simulate_progress <<m_cost_seconds <<b_finished;

    sprintf(prestr,"%.1f",m_simulate_progress);
    strcat(prestr,"%");

    if(m_simulate_progress>=100){
        b_finished = true;
        ui->progressBar_process->setValue(100);
        ui->label_process->setText("100%");
        my_time->stop();
    }
    else{
          ui->progressBar_process->setValue(m_simulate_progress);
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
    b_canClose = false;

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

UDisksObject* getObjectFromBlockDevice(UDisksClient* client, const gchar* bdevice)
{
    struct stat statbuf;
    UDisksBlock* block = NULL;
    UDisksObject* object = NULL;
    UDisksObject* cryptoBackingObject = NULL;
    g_autofree const gchar* cryptoBackingDevice = NULL;

    g_return_val_if_fail(stat(bdevice, &statbuf) == 0, object);

    block = udisks_client_get_block_for_dev (client, statbuf.st_rdev);
    g_return_val_if_fail(block != NULL, object);

    object = UDISKS_OBJECT (g_dbus_interface_dup_object (G_DBUS_INTERFACE (block)));

    cryptoBackingDevice = udisks_block_get_crypto_backing_device ((udisks_object_peek_block (object)));
    cryptoBackingObject = udisks_client_get_object (client, cryptoBackingDevice);
    if (cryptoBackingObject != NULL) {
        g_object_unref (object);
        object = cryptoBackingObject;
    }

    g_object_unref (block);

    return object;
}

// format
static void format_cb (GObject *source_object, GAsyncResult *res ,gpointer user_data)
{
    static int end_flag = -1;

    CreateformatData *data = (CreateformatData *)user_data;
    GError *error = NULL;
    QString curName = "";
    if (!udisks_block_call_format_finish (UDISKS_BLOCK (source_object), res,&error))
    {
        char *p = NULL;
        if(NULL != error && NULL != (p=strstr(error->message, "wipefs:"))){
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
        *(data->format_finish) =  -1; //format error
    }
    else
    {
        UDisksClient* client = udisks_client_new_sync(NULL, NULL);
        if (client) {
            UDisksObject* udiskObj = getObjectFromBlockDevice(client, data->device_name);
            if (udiskObj) {
                UDisksBlock* diskBlock = udisks_object_get_block (udiskObj);
                if (diskBlock) {
                    curName = udisks_block_get_id_label (diskBlock);
                    qDebug () << data->device_name << "  --  " << data->filesystem_name << "  --  " << curName;
                }
            }
        }

        // rename fail
        if (data->dl->ui->lineEdit_device_name->text ().trimmed () != curName) {
            data->dl->renameOK = false;
        }

        end_flag = 1;
        *(data->format_finish) =  1; //format success
    }

    if(end_flag == 1){
        b_finished = true;
        data->dl->ui->progressBar_process->setValue(100);
        data->dl->ui->label_process->setText("100%");
        data->dl->format_ok_dialog();
    }
    else{
        b_failed = true;
        data->dl->format_err_dialog();  
    }

    b_canClose = true;

    data->dl->my_time->stop();
    data->dl->close();

    createformatfree(data);
};


void Format_Dialog::format_ok_dialog()
{
    if (renameOK) {
        QMessageBox::about(m_parent,QObject::tr("format"),QObject::tr("Format operation has been finished successfully."));
    } else {
        QMessageBox::about(m_parent,QObject::tr("format"),QObject::tr("Formatting successful! But failed to set the device name."));
    }

    ui->pushButton_close->setEnabled(true);
}


void Format_Dialog::format_err_dialog()
{
      QMessageBox::warning(m_parent,QObject::tr("qmesg_notify"),QObject::tr("Sorry, the format operation is failed!"));
      ui->pushButton_close->setEnabled(true);
}

bool Format_Dialog::format_makesure_dialog(){

    QMessageBox message_format;

    message_format.setText(QObject::tr("Formatting this volume will erase all data on it. Please backup all retained data before formatting. Do you want to continue ?"));

    message_format.setWindowTitle(QObject::tr("format"));

    QPushButton *okButton = message_format.addButton(QObject::tr("begin format"),QMessageBox::YesRole);

    QPushButton *cancelButton = message_format.addButton(QObject::tr("close"),QMessageBox::NoRole);

    message_format.exec();

    if(message_format.clickedButton() == cancelButton)
    {
        ui->pushButton_ok->setDisabled(false);
        ui->pushButton_close->setDisabled(false);
        return false;
    }else if(message_format.clickedButton() == okButton){
        return true;
    }

    //close window, return false, link to bug#92330
    return false;
}

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
            g_strcmp0 (data->format_type, "exfat") != 0) {
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
                g_strcmp0 (data->format_type, "exfat") != 0) {
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
    b_canClose = true;
}

void Format_Dialog::closeEvent(QCloseEvent *e)
{
    if (!b_canClose) {

        QMessageBox::warning(nullptr, tr("Formatting. Do not close this window"), tr("Formatting. Do not close this window"), QMessageBox::Ok);

        e->ignore();
        return;
    }
}
