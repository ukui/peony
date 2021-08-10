/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "side-bar-file-system-item.h"
#include "sync-thread.h"
#include "file-info.h"
#include "file-utils.h"
#include "file-watcher.h"
#include "file-info-job.h"
#include "volume-manager.h"
#include "side-bar-model.h"
#include "file-enumerator.h"
#include "gobject-template.h"
#include "linux-pwd-helper.h"
#include "side-bar-separator-item.h"

#include <QIcon>
#include <QThread>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <udisks/udisks.h>
#include <sys/stat.h>
#include <libnotify/notify.h>

using namespace Peony;

void notifyUser(QString notifyContent);

SideBarFileSystemItem::SideBarFileSystemItem(QString uri,
                                             SideBarFileSystemItem *parentItem,
                                             SideBarModel *model,
                                             QObject *parent) :
    SideBarAbstractItem (model, parent),
    m_uri(uri),m_parent(parentItem)
{
    if (parentItem == nullptr) {
        m_is_root_child = true;
        m_uri = "computer:///";
        m_display_name = tr("Computer");        
        m_icon_name = "";
    }
    else {
        m_info = FileInfo::fromUri(uri);
        auto infoJob = new FileInfoJob(m_info);
        infoJob->setAutoDelete();
        connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=](){
            m_display_name = FileUtils::getFileDisplayName(uri);
            if (m_info.get()->targetUri() == "file:///") {
                m_display_name = tr("File System");
                return;
            }
            m_icon_name = FileUtils::getFileIconName(uri,false);
            FileUtils::queryVolumeInfo(m_uri, m_volume_name, m_unix_device, m_display_name);
            Q_EMIT this->queryInfoFinished();
        });

        infoJob->queryAsync();
        //FIXME: replace BLOCKING api in ui thread.
        m_display_name = FileUtils::getFileDisplayName(uri);
        m_icon_name = FileUtils::getFileIconName(uri, false);
    }
}

QString SideBarFileSystemItem::displayName()
{
    QString displayName;
    if (!m_volume_name.isEmpty()) {
        displayName += m_volume_name;
    }
    //fix data block not localized issue, link to bug#63470
    if (displayName.toLower() == "data")
        displayName = tr("data");
    if (!m_unix_device.isEmpty() && !m_uri.contains("root.link")) {
        displayName += QString(" (%1)").arg(m_unix_device);
    }

    if (!displayName.isEmpty())
        return displayName;

    return m_display_name;
}

QModelIndex SideBarFileSystemItem::firstColumnIndex()
{
    return m_model->firstColumnIndex(this);
}

QModelIndex SideBarFileSystemItem::lastColumnIndex()
{
    return m_model->lastColumnIndex(this);
}

void SideBarFileSystemItem::clearChildren()
{
    stopWatcher();
    SideBarAbstractItem::clearChildren();
}

/*!
 * \brief SideBarFileSystemItem::findChildren
 * \bug root doesn't support gvfs, so computer:/// cannot be enumerated.
 * to avoid the bug, I forbided find filesystem item children in root.
 * I should use another way to display the devices/volumes.
 */
void SideBarFileSystemItem::findChildren()
{
    auto pwdItem = LinuxPWDHelper::getCurrentUser();
    if (pwdItem.userId() == 0) {
        return;
    }
    clearChildren();

    FileEnumerator *e = new FileEnumerator;
    e->setEnumerateDirectory(m_uri);
    e->setEnumerateWithInfoJob();
    connect(e, &FileEnumerator::prepared, this, [=](const GErrorWrapperPtr &err/*, const QString &targetUri*/) {
        // fix can not enumerate children in computer:///xxx, related to 0708ebd30a3ee3f397592f9c64edba263e900a37
        auto info = FileInfo::fromUri(m_uri);
        auto targetUri = info.get()->targetUri();
        if (targetUri != nullptr) {
            if (targetUri != this->uri()) {
                e->setEnumerateDirectory(targetUri);
            }
        }

        connect(e, &FileEnumerator::enumerateFinished, this, [=](){
            auto infos = e->getChildren();
            bool isEmpty = true;
            int real_children_count = infos.count();
            if (infos.isEmpty()) {
                auto separator = new SideBarSeparatorItem(SideBarSeparatorItem::EmptyFile, this, m_model);
                this->m_children->prepend(separator);
                m_model->insertRows(0, 1, this->firstColumnIndex());
                goto end;
            }

            for (auto info: infos) {
                if (!info->displayName().startsWith(".") && (info->isDir() || info->isVolume())) {
                    isEmpty = false;
                }

                auto targetUri = FileUtils::getTargetUri(info->uri());

                //skip the independent files and remote server
                bool bRemoteServer=false;
                if(targetUri.startsWith("ftp://")||targetUri.startsWith("sftp://")||targetUri.startsWith("samba://"))
                    bRemoteServer=true;

                if (!(info->isDir() || info->isVolume())||bRemoteServer) {
                    real_children_count--;
                    continue;
                }

                SideBarFileSystemItem *item = new SideBarFileSystemItem(info->uri(),
                                                                        this,
                                                                        m_model,
                                                                        this);
                //check is mounted.
                //FIXME: replace BLOCKING api in ui thread.

                bool isUmountable = FileUtils::isFileUnmountable(info->uri());
                item->m_is_mounted = (!targetUri.isEmpty() && (targetUri != "file:///")) || isUmountable;
                m_children->append(item);
                //qDebug()<<info->uri();
            }
            m_model->insertRows(0, real_children_count, firstColumnIndex());

            if (isEmpty) {
                auto separator = new SideBarSeparatorItem(SideBarSeparatorItem::EmptyFile, this, m_model);
                this->m_children->prepend(separator);
                m_model->insertRows(0, 1, this->firstColumnIndex());
            }
end:
            Q_EMIT this->findChildrenFinished();
            if (err != nullptr) {
                //qDebug()<<"prepared:"<<err.get()->message();
            }
            delete e;

            //NOTE: init watcher after prepared.
            this->initWatcher();
            this->m_watcher->setMonitorChildrenChange();
            /*
            if (this->uri() == "computer:///") {
                this->m_watcher->setMonitorChildrenChange();
            }
            */

            //start listening.
            connect(m_watcher.get(), &FileWatcher::fileCreated, this, [=](const QString &uri) {
                //qDebug()<<"created:"<<uri;
                for (auto item : *m_children) {
                    if (item->uri() == uri) {
                        return;
                    }
                }

                SideBarFileSystemItem *item = new SideBarFileSystemItem(uri,
                                                                        this,
                                                                        m_model);
                m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
                m_children->append(item);
                m_model->endInsertRows();
                m_model->indexUpdated(this->firstColumnIndex());
                m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
            });

            connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](const QString &uri) {
                //qDebug()<<"deleted:"<<uri;
                for (auto child : *m_children) {
                    if (child->uri() == uri) {
                        int index = m_children->indexOf(child);
                        m_model->beginRemoveRows(firstColumnIndex(), index, index);
                        m_children->removeOne(child);
                        m_model->endRemoveRows();
                        child->deleteLater();
                        break;
                    }
                }
                m_model->indexUpdated(this->firstColumnIndex());
            });

            connect(m_watcher.get(), &FileWatcher::fileChanged, this, [=](const QString &uri) {
                //FIXME: maybe i have to remove this changed item then add it again to avoid
                //qt's view expander cannot show correctly after the volume item unmounted.
                //qDebug()<<"side bar fs item changed:"<<uri;
                for (auto child : *m_children) {
                    if (child->uri() == uri) {
                        SideBarFileSystemItem *changedItem = static_cast<SideBarFileSystemItem*>(child);
                        updateFileInfo(changedItem);
                        //FIXME: replace BLOCKING api in ui thread.
                        if (FileUtils::getTargetUri(uri).isEmpty()) {
                            changedItem->m_is_mounted = false;
                            changedItem->clearChildren();
                        } else {
                            changedItem->m_is_mounted = true;
                        }

                        //why it would failed when send changed signal for newly mounted item?
                        //m_model->dataChanged(changedItem->firstColumnIndex(), changedItem->firstColumnIndex());
                        m_model->dataChanged(changedItem->firstColumnIndex(), changedItem->lastColumnIndex());
                        break;
                    }
                }
            });

            this->startWatcher();
            //m_model->setData(lastColumnIndex(), QVariant(QIcon::fromTheme("media-eject-symbolic")), Qt::DecorationRole);
        });

        e->enumerateAsync();
    });
    e->prepare();

    Q_EMIT findChildrenFinished();
}

void SideBarFileSystemItem::findChildrenAsync()
{
    //TODO add async method.
    findChildren();
}

bool SideBarFileSystemItem::isRemoveable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///root.link" && m_uri != "computer:///") {
        //FIXME: replace BLOCKING api in ui thread.
        auto info = FileInfo::fromUri(m_uri);
        if (info->displayName().isEmpty()) {
            FileInfoJob j(info);
            j.querySync();
        }
        bool removable = info->canEject() || info->canStop();
        if (!removable && !info.get()->unixDeviceFile().isEmpty()) {
            // check if drive is removable
            auto targetUri = info.get()->targetUri();
            auto targetFile = g_file_new_for_uri(targetUri.toUtf8().constData());
            auto mount = g_file_find_enclosing_mount(targetFile, nullptr, nullptr);
            g_object_unref(targetFile);
            if (mount) {
                auto drive = g_mount_get_drive(mount);
                if (drive) {
                    removable = g_drive_is_removable(drive);
                    g_object_unref(drive);
                }
                g_object_unref(mount);
            }
            return removable;
        } else {
            return true;
        }
    }
    return false;
}

bool SideBarFileSystemItem::isEjectable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///root.link" && m_uri != "computer:///") {
        //FIXME: replace BLOCKING api in ui thread.
        auto info = FileInfo::fromUri(m_uri);
        if (info->displayName().isEmpty()) {
            FileInfoJob j(info);
            j.querySync();
        }
        return isRemoveable();
    }
    return false;
}

bool SideBarFileSystemItem::isMountable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {
        //some mountable item can be unmounted but can't be mounted.
        //the most of them is remote servers and shared directories.
        //they should be seemed as mountable items.
        //FIXME: replace BLOCKING api in ui thread.
        auto info = FileInfo::fromUri(m_uri);
        if (info->displayName().isEmpty()) {
            FileInfoJob j(info);
            j.querySync();
        }
        return info->canMount() || info->canUnmount();
    }
    return false;
}

bool SideBarFileSystemItem::isMounted()
{
    if(!FileUtils::getTargetUri(m_uri).isEmpty())
        m_is_mounted = true;

    return m_is_mounted;
}

void SideBarFileSystemItem::unmount()
{
    SyncThread *syncThread = new SyncThread(m_uri);
    QThread* currentThread = new QThread();
    syncThread->moveToThread(currentThread);
    connect(currentThread,&QThread::started,syncThread,&SyncThread::parentStartedSlot);
    connect(syncThread,&SyncThread::syncFinished,this,[=](){
        realUnmount();
        syncThread->disconnect(this);
        syncThread->deleteLater();
        currentThread->disconnect(SIGNAL(started()));
        //currentThread->deleteLater();
    });
    currentThread->start();
}

void SideBarFileSystemItem::eject(GMountUnmountFlags ejectFlag)
{
    SyncThread *syncThread = new SyncThread(m_uri);
    QThread* currentThread = new QThread();
    syncThread->moveToThread(currentThread);
    connect(currentThread,&QThread::started,syncThread,&SyncThread::parentStartedSlot);
    connect(syncThread,&SyncThread::syncFinished,this,[=](){
        realEject(ejectFlag);
        QString unmountNotify = QObject::tr("Data synchronization is complete,the device has been unmount successfully!");
        notifyUser(unmountNotify);
        syncThread->disconnect(this);
        syncThread->deleteLater();
        currentThread->disconnect(SIGNAL(started()));
        //currentThread->deleteLater();
    });
    currentThread->start();
}

void SideBarFileSystemItem::realEject(GMountUnmountFlags ejectFlag)
{
    //FIXME: replace BLOCKING api in ui thread.
    VolumeManager *volumeManager;
    std::shared_ptr<Drive> drive;
    GDrive *gdrive;

    auto file = wrapGFile(g_file_new_for_uri(this->uri().toUtf8().constData()));
    auto target = FileUtils::getTargetUri(m_uri);

    volumeManager = VolumeManager::getInstance();
    drive = volumeManager->getDriveFromUri(target);
    if(!drive)
        drive = volumeManager->getDriveFromSystemByPath(m_unix_device);

    if(!drive)
        return;

    gdrive = drive->getGDrive();

    if(g_drive_can_eject(gdrive)){//for udisk or DVD.
        g_file_eject_mountable_with_operation(file.get()->get(),
                                              ejectFlag,
                                              nullptr,
                                              nullptr,
                                              GAsyncReadyCallback(eject_cb),
                                              this);
    }else if(g_drive_can_stop(gdrive) || g_drive_is_removable(gdrive)){//for mobile harddisk.
        g_drive_stop(gdrive,ejectFlag,NULL,NULL,
                     GAsyncReadyCallback(ejectDevicebyDrive),
                     this);
    }
}

static void unmount_force_cb(GFile* file, GAsyncResult* result, gpointer udata) {
    auto targetUri = static_cast<QString *>(udata);
    QString unmountNotify;
    GError *err = nullptr;
    g_file_unmount_mountable_with_operation_finish (file, result, &err);
    if (err) {
        QMessageBox::warning(nullptr, QObject::tr("Force unmount failed"), QObject::tr("Error: %1\n").arg(err->message));
        g_error_free(err);
    } else {
        VolumeManager::getInstance()->fileUnmounted(*targetUri);
        unmountNotify = QObject::tr("Data synchronization is complete,the device has been unmount successfully!");
        notifyUser(unmountNotify);
    }
    delete targetUri;
}

static void unmount_finished(GFile* file, GAsyncResult* result, gpointer udata)
{
    auto targetUri = static_cast<QString *>(udata);
    GError *err = nullptr;
    QString unmountNotify;
    g_file_unmount_mountable_with_operation_finish (file, result, &err);
    if (err) {
        if(!strcmp(err->message,"Not authorized to perform operation")){//umount /data need permissions.
            g_error_free(err);
            return;
        }
        if(strstr(err->message,"umount: ")){
            QMessageBox::warning(nullptr,QObject::tr("Unmount failed"),QObject::tr("Unable to unmount it, you may need to close some programs, such as: GParted etc."),QMessageBox::Yes);
            g_error_free(err);
            return;
        }

        QMessageBox::warning(nullptr, QObject::tr("Unmount failed"), QObject::tr("Error: %1\n").arg(err->message), QMessageBox::Yes);
        g_error_free(err);
    } else {
        VolumeManager::getInstance()->fileUnmounted(*targetUri);
        unmountNotify = QObject::tr("Data synchronization is complete,the device has been unmount successfully!");
        notifyUser(unmountNotify);
    }
    delete targetUri;

    /*!
      \note

      the user data repesent current sidebar item in callback function here is unsafe.
      because once the item unmounted, the item itself will be deleted automaticly, too.
      so, never try using it, especially when unmount operation successed.

      */
    // dangerous data, it might be delete while unmounting, do not use it.

    //SideBarFileSystemItem* th = (SideBarFileSystemItem*)udata;

    //    if (FileUtils::isMountPoint(FileUtils::getTargetUri(th->uri()))) {
    //        QMessageBox::warning(nullptr, QObject::tr("eject device failed"),
    //                             QObject::tr("Please check whether the device is occupied and then eject the device again"),
    //                             QMessageBox::Ok);
    //    }
}

static UDisksObject *get_object_from_block_device (UDisksClient *client,const gchar *block_device)
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


void SideBarFileSystemItem::realUnmount()
{
    /*!
      \note

      unmount operation of gio will handle both native and remote mount point.
      so we must not use udisk's api directly, because it will lead to crash while
      unmounting a remote directory.

      */

    //    UDisksObject *object;
    //    UDisksClient *client;
    //    UDisksBlock *block;
    //    client = udisks_client_new_sync(NULL,NULL);

    //    object = get_object_from_block_device(client,m_unix_device.toUtf8().constData());
    //    block = udisks_object_get_block(object);

    //    // if device type is disc , Eject optical drive
    //    if(g_strcmp0(udisks_block_get_id_type(block),"iso9660")==0 || strlen(udisks_block_get_id_type(block))==0){

    //        // if can eject ,eject it
    //        if(isEjectable()){
    //            this->eject();
    //        }

    //        return;
    //    }

    auto file = wrapGFile(g_file_new_for_uri(this->uri().toUtf8().constData()));
    QString *targetUri = new QString;
    *targetUri = this->m_info.get()->targetUri();
    g_file_unmount_mountable_with_operation(file.get()->get(),
                                            G_MOUNT_UNMOUNT_NONE,
                                            nullptr,
                                            nullptr,
                                            GAsyncReadyCallback(unmount_finished),
                                            targetUri);
}

void SideBarFileSystemItem::ejectOrUnmount()
{
    if (isRemoveable())
        eject(G_MOUNT_UNMOUNT_NONE);

    else if (isMountable())
        unmount();
}

void SideBarFileSystemItem::initWatcher()
{
    if (!m_watcher) {
        m_watcher = std::make_shared<FileWatcher>(m_uri);
    }
}

void SideBarFileSystemItem::startWatcher()
{
    initWatcher();
    m_watcher->startMonitor();
}

void SideBarFileSystemItem::stopWatcher()
{
    initWatcher();
    m_watcher->stopMonitor();
}

GAsyncReadyCallback SideBarFileSystemItem::eject_cb(GFile *file, GAsyncResult *res, SideBarFileSystemItem *p_this)
{
    GError *err = nullptr;
    QString ejectNotify;
    bool successed = g_file_eject_mountable_with_operation_finish(file, res, &err);
    qDebug()<<successed;
    if (err) {
        qDebug()<<err->message;
        QMessageBox warningBox(QMessageBox::Warning,QObject::tr("Eject failed"),QString(err->message), QMessageBox::Ok);
        warningBox.exec();

        g_error_free(err);
    } else {
        char *uri = g_file_get_uri(file);
        VolumeManager::getInstance()->fileUnmounted(uri);
        if (uri)
            g_free(uri);

        ejectNotify = QObject::tr("Data synchronization is complete and the device can be safely unplugged!");
        notifyUser(ejectNotify);
        // remove item anyway
        /*int index = p_this->parent()->m_children->indexOf(p_this);
        p_this->m_model->beginRemoveRows(p_this->parent()->firstColumnIndex(), index, index);
        p_this->parent()->m_children->removeOne(p_this);
        p_this->m_model->endRemoveRows();
        p_this->deleteLater();*/
    }
    return nullptr;
}

//update udisk file info
void SideBarFileSystemItem::updateFileInfo(SideBarFileSystemItem *pThis){
    //FIXME: replace BLOCKING api in ui thread.
    auto fileInfo = FileInfo::fromUri(pThis->m_uri);
    FileInfoJob fileJob(fileInfo);
    fileJob.querySync();

    QString tmpName = FileUtils::getFileDisplayName(pThis->m_uri);

    //old's drive name -> now's volume name. fix #17968
    FileUtils::queryVolumeInfo(pThis->m_uri,pThis->m_volume_name,pThis->m_unix_device,tmpName);
    //icon name.
    pThis->m_icon_name = FileUtils::getFileIconName(pThis->m_uri,false);
}

/* Eject some device by stop it's drive. Such as: mobile harddisk.
 */
void SideBarFileSystemItem::ejectDevicebyDrive(GObject* object,GAsyncResult* res,SideBarFileSystemItem *pThis)
{
    GError *error;
    QString errorMsg;

    error = NULL;
    if(!g_drive_poll_for_media_finish(G_DRIVE(object),res,&error)){
        if((NULL != error) && (G_IO_ERROR_FAILED_HANDLED != error->code)){
            errorMsg = QObject::tr("Unable to eject %1").arg(pThis->m_display_name);

            QMessageBox warningBox(QMessageBox::Warning, QObject::tr("Eject failed"), errorMsg, QMessageBox::Ok);
            warningBox.exec();

            g_error_free(error);
        }
    }
}

void notifyUser(QString notifyContent)
{
    NotifyNotification* notify;

    notify_init(QObject::tr("PeonyNotify").toUtf8().constData());
    notify  = notify_notification_new(QObject::tr("File Manager").toUtf8().constData(),
                                      notifyContent.toUtf8().constData(),
                                      "system-file-manager");
    notify_notification_show(notify,nullptr);

    notify_uninit();
    g_object_unref(G_OBJECT(notify));
}
