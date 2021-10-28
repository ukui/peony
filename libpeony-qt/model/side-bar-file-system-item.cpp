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
//#include "volume-manager.h"
#include "volumeManager.h"
#include "side-bar-model.h"
#include "file-enumerator.h"
#include "gobject-template.h"
#include "linux-pwd-helper.h"
#include "side-bar-separator-item.h"
#include <udisks/udisks.h>
#include <sys/stat.h>

#include <QIcon>
#include <QThread>
#include <QMessageBox>
#include <QPushButton>
#include <QUrl>

using namespace Peony;

SideBarFileSystemItem::SideBarFileSystemItem(QString uri, const Experimental_Peony::Volume& volume,
                                             SideBarFileSystemItem *parentItem,
                                             SideBarModel *model,
                                             QObject *parent) :
    SideBarAbstractItem (model, parent),
    /*m_uri(uri),*/m_volume(volume),m_parent(parentItem)
{
    m_uri = uri;
    if(!volume.device().isEmpty()){
        initVolumeInfo(volume);
    }else{
        if(!parentItem){
            initComputerInfo();
            m_isRootChild = true;
        }else{
            initDirInfo(uri);
        }
    }
    m_children = new QVector<SideBarAbstractItem*>();
}

SideBarFileSystemItem::~SideBarFileSystemItem()
{
    if(m_enumerator)
    {
        delete m_enumerator;
        m_enumerator = nullptr;
    }
}

QString SideBarFileSystemItem::uri() {
    return FileUtils::urlEncode(m_uri);
}

QString SideBarFileSystemItem::displayName()
{
    return m_displayName;
}

QModelIndex SideBarFileSystemItem::firstColumnIndex()
{
    return m_model->firstColumnIndex(this);
}

QModelIndex SideBarFileSystemItem::lastColumnIndex()
{
    return m_model->lastColumnIndex(this);
}

bool SideBarFileSystemItem::filterShowRow()
{
    if (m_uri == "file:///data" && FileUtils::isFileExsit("file:///data/usershare")) {
        return false;
    }
    if (m_uri.startsWith("file:///home/")){
        return false;
    }
    if (m_uri != "file:///") {

        QString gvfsDisplayName = m_mountPoint;
        QString gvfsUnixDevice = m_device;


        if (!gvfsUnixDevice.isNull() && (gvfsDisplayName.contains("DVD")))
            return true;

        if(!gvfsUnixDevice.isNull() && !gvfsDisplayName.contains(":"))
            return false;//Filter some non-mountable drive items

        if (isMounted())
            return true;
        if (isRemoveable() && isUnmountable()) {
            return true;
        }
        if (!isRemoveable() && !isEjectable() && !isStopable())
            return true;
        return false;
    }

    return true;
}

void SideBarFileSystemItem::initDirInfo(const QString &uri)
{

   if(uri.isEmpty())
       return;

   m_children    = nullptr;
   m_watcher     = nullptr;
   m_iconName    = "gtk-directory";
   m_device      = m_mountPoint = "";

   m_uri         = uri;
   auto info     = FileInfo::fromUri(m_uri);
   m_displayName = info.get()->displayName();
   m_mounted = m_ejectable = m_stopable = m_removeable = m_mountable = false;
   m_unmountable = false;
}

void SideBarFileSystemItem::initComputerInfo()
{
    m_device     = "";
    m_mountPoint = "";
    m_mounted    = true;
    m_ejectable  = false;
    m_mountable  = false;
    m_removeable = false;
    m_unmountable= false;
    m_watcher    = nullptr;
    m_uri        = "computer:///";
    m_iconName   = "";
    m_displayName = QObject::tr("Computer");

}

void SideBarFileSystemItem::initVolumeInfo(const Experimental_Peony::Volume &volumeItem)
{
    m_watcher     = nullptr;
    m_iconName    = volumeItem.icon();
    m_device      = volumeItem.device();
    m_ejectable   = volumeItem.canEject();
    m_stopable   = volumeItem.canStop();
    m_unmountable = volumeItem.canUnmount();//可卸载
    m_mountable   = volumeItem.canMount();  //可挂载
    m_removeable  = m_ejectable? true:false;
    m_uri = m_mountPoint = volumeItem.mountPoint();
    m_mounted   = !m_uri.isEmpty();
    m_displayName = volumeItem.name() + "(" + m_device + ")";
    m_isVolume = true;
    /* 手机和空光盘的m_uri需要额外设置 */
    if(m_uri.isEmpty()){
        if(m_device.startsWith("/dev/bus/usb")){
            m_uri = "computer:///" + volumeItem.name() + ".volume";/* 手机(mtp、gphoto2) */
            m_mounted = true;
        }
        else if(m_device.contains("/dev/sr") &&
                (m_displayName.contains("DVD") ||m_displayName.contains("CD")))//更好的方法区分是否是空光盘?
        {
            m_mounted=true;
            m_unmountable = m_mountable=false;
            m_uri = "burn://";
        }

    }else{
        m_uri = "file://" + m_uri;
    }
    /* 文件系统项特殊处理 */
    if("file:///"==m_uri){
        m_unmountable = m_mountable = m_ejectable = m_stopable = false;
        m_mounted=true;
        m_displayName = QObject::tr("File System");
        m_iconName="drive-harddisk-system-symbolic";
    }
}

void SideBarFileSystemItem::clearChildren()
{
    if(m_watcher)
        m_watcher->stopMonitor();
    SideBarAbstractItem::clearChildren();
}

void SideBarFileSystemItem::slot_volumeDeviceAdd(const Experimental_Peony::Volume &addItem)
{
    qDebug()<<"add device:"<<addItem.device();
    if(addItem.device().isEmpty())
      return;
    /* 防止一个设备多次添加 */
    for (auto item : *m_children) {
        if (item->m_device == addItem.device()) {
            return;
        }
    }

    SideBarFileSystemItem *item = new SideBarFileSystemItem(nullptr,
                                                          addItem,
                                                          this,
                                                          m_model);
    m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
    m_children->append(item);
    m_model->endInsertRows();
    m_model->indexUpdated(this->firstColumnIndex());
    m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
}

void SideBarFileSystemItem::slot_volumeDeviceRemove(const QString &removeDevice)
{
    for (auto child : *m_children) {
        if (child->m_device == removeDevice|| (removeDevice == child->m_mountPoint/*gparted*/)) {
            int index = m_children->indexOf(child);
            m_model->beginRemoveRows(firstColumnIndex(), index, index);
            m_children->removeOne(child);
            m_model->endRemoveRows();
            child->deleteLater();
            break;
        }
    }
    m_model->indexUpdated(this->firstColumnIndex());
}

void SideBarFileSystemItem::slot_volumeDeviceMount(const Experimental_Peony::Volume &volume)
{
    QString device = volume.device();
    QString mountPoint = volume.mountPoint();
    if(mountPoint.isEmpty())
        return;
    //更新model,元素信息更新
    for(auto item:*m_children){
        if(item->m_device == device){
            item->m_mounted = true;            /* 更新挂载状态 */
            item->m_mountPoint = mountPoint;     /* 设置挂载点，属性页会用到 */
            item->m_mountable =volume.canMount();
            item->m_unmountable = true;
            item->m_uri = "file://" + mountPoint;/* 更新uri,为了枚举操作 */
            /* 手机和空光盘的m_uri需要额外设置 */
            if(device.startsWith("/dev/bus/usb"))/* 手机设备(mtp、gphoto2)的uri */
                item->m_uri = "computer:///" + volume.name() + ".volume";
            else if(item->m_device.contains("/dev/sr") &&
                    (item->m_displayName.contains("DVD") ||item->m_displayName.contains("CD")))/* 空光盘 */
            {
                item->m_uri="burn:///";
            }
            m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
            break;
        }
    }
}

void SideBarFileSystemItem::slot_volumeDeviceUnmount(const QString &unmountDevice)
{
     qDebug()<<__func__<<__LINE__<<unmountDevice;
    if(unmountDevice.isEmpty()||m_children->isEmpty())
            return;

    //qDebug()<<__func__<<__LINE__<<unmountDevice;
    //更新model,元素信息更新
    for(auto& item:*m_children){
        if(item->m_mountPoint == unmountDevice){/* 依靠挂载点属性匹配 */
            item->m_uri = "";                   /* 分区卸载后不可以做枚举操作 */
            item->m_mountPoint = "";            /*挂载点置空，属性页会用到? */
            item->m_mounted = false;            /* 分区已卸载 */
            item->m_unmountable = false;
            item->m_mountable = true;
            m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
            break;
        }
    }
}

void SideBarFileSystemItem::slot_volumeDeviceUpdate(const Experimental_Peony::Volume &updateDevice, QString property)
{
    qDebug()<<__func__<<__LINE__;
    QString device;
    if(property != "name")
        return;

    device = updateDevice.device();
    for(auto& item:*m_children){
        if(item->m_device == device){
            item->m_displayName = updateDevice.name() + "(" + device + ")";
            //model更新
             m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
            break;
        }
    }

}

void SideBarFileSystemItem::slot_fileCreate(const QString &uri)
{
    //qDebug()<<"created:"<<uri;
    for (auto item : *m_children) {
        if (item->uri() == uri) {
            return;
        }
    }

    SideBarFileSystemItem *item = new SideBarFileSystemItem(uri,
                                                            nullptr,
                                                            this,
                                                            m_model);

    m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
    m_children->append(item);
    m_model->endInsertRows();
    m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
}

void SideBarFileSystemItem::slot_fileDelete(const QString &uri)
{
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
}

void SideBarFileSystemItem::slot_fileRename(const QString &oldUri, const QString &newUri)
{
    qDebug()<<"rename,old uri:"<<oldUri<<" new uri:"<<newUri;
    for (auto item : *m_children){
        if (item->uri() == oldUri){
            item->m_uri = newUri;
            auto info = FileInfo::fromUri(item->m_uri);
            if (info->displayName().isEmpty()) {
                FileInfoJob j(info);
                j.querySync();
            }
            item->m_displayName = info.get()->displayName();
            m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
        }
    }

}

void SideBarFileSystemItem::slot_enumeratorPrepared(const std::shared_ptr<GErrorWrapper> &err, const QString &targetUri, bool critical)
{
    connect(m_enumerator,&FileEnumerator::enumerateFinished,this, &SideBarFileSystemItem::slot_enumeratorFinish);
    m_enumerator->enumerateAsync();
}

void SideBarFileSystemItem::slot_enumeratorFinish(bool successed)
{
    if(!successed)
        return;

    auto infos = m_enumerator->getChildren();
    bool isEmpty = true;
    int real_children_count = infos.count();
    if (infos.isEmpty()) {
        auto separator = new SideBarSeparatorItem(SideBarSeparatorItem::EmptyFile, this, m_model);
        this->m_children->prepend(separator);
        m_model->insertRows(0, 1, this->firstColumnIndex());
        return;
    }

    for (auto info: infos) {
        if (!info->displayName().startsWith(".") && (info->isDir() || info->isVolume())) {
            isEmpty = false;
        }

        auto targetUri = FileUtils::getTargetUri(info->uri());

        //skip the independent files and remote server
        bool bRemoteServer=false;
        if(targetUri.startsWith("ftp://")||targetUri.startsWith("sftp://")||targetUri.startsWith("smb://"))
            bRemoteServer=true;

        if (!(info->isDir() || info->isVolume())||bRemoteServer) {
            real_children_count--;
            continue;
        }

        SideBarFileSystemItem *item = new SideBarFileSystemItem(info->uri(),
                                                                nullptr,
                                                                this,
                                                                m_model);
        //check is mounted.
        //FIXME: replace BLOCKING api in ui thread.

        bool isUmountable = FileUtils::isFileUnmountable(info->uri());
        item->m_mounted = (!targetUri.isEmpty() && (targetUri != "file:///")) || isUmountable;
        m_children->append(item);
    }
    m_model->insertRows(0, real_children_count, firstColumnIndex());

    if (isEmpty) {
        auto separator = new SideBarSeparatorItem(SideBarSeparatorItem::EmptyFile, this, m_model);
        this->m_children->prepend(separator);
        m_model->insertRows(0, 1, this->firstColumnIndex());
    }
}

/*!
 * \brief SideBarFileSystemItem::findChildren
 * \bug root doesn't support gvfs, so computer:/// cannot be enumerated.
 * to avoid the bug, I forbided find filesystem item children in root.
 * I should use another way to display the devices/volumes.
 */
void SideBarFileSystemItem::findChildren()
{
    clearChildren();

    if (m_parent == nullptr) {
        int volumeCount = 0;
        QList<Experimental_Peony::Volume>* volumeList;
        volumeList = Experimental_Peony::VolumeManager::getInstance()->allVaildVolumes();
        volumeCount = volumeList->count();

        for(int i=0; i<volumeCount; ++i){
            auto volume = volumeList->at(i);

            m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
            SideBarFileSystemItem* item = new SideBarFileSystemItem(volume.name(), volume, this, m_model);
            m_children->append(item);
            m_model->endInsertRows();
            m_model->indexUpdated(this->lastColumnIndex());
            m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
        }

    }else{
        //对挂载点进行已存在文件的枚举操作
        QString enumdir = m_uri;
        if(m_uri.startsWith("computer:///")){//GFileEnumerator不识别computer:///，只识别file:///
            auto info = FileInfo::fromUri(m_uri);
            FileInfoJob job(info);
            job.querySync();
            enumdir = info.get()->targetUri();
        }

        if(!m_enumerator)
            m_enumerator= new FileEnumerator();
        m_enumerator->setEnumerateDirectory(enumdir);
        m_enumerator->setEnumerateWithInfoJob();

        connect(m_enumerator,&FileEnumerator::prepared,this,&SideBarFileSystemItem::slot_enumeratorPrepared);
        m_enumerator->prepare();

    }
    /* 设备动态增减处理 */
    if("computer:///" == m_uri){
        auto volumeManager = Experimental_Peony::VolumeManager::getInstance();
        connect(volumeManager,&Experimental_Peony::VolumeManager::volumeAdd,this,&SideBarFileSystemItem::slot_volumeDeviceAdd);
        connect(volumeManager,&Experimental_Peony::VolumeManager::volumeRemove,this,&SideBarFileSystemItem::slot_volumeDeviceRemove);
        connect(volumeManager,&Experimental_Peony::VolumeManager::mountAdd,this,&SideBarFileSystemItem::slot_volumeDeviceMount);
        connect(volumeManager,&Experimental_Peony::VolumeManager::mountRemove,this,&SideBarFileSystemItem::slot_volumeDeviceUnmount);
        connect(volumeManager,&Experimental_Peony::VolumeManager::volumeUpdate,this,&SideBarFileSystemItem::slot_volumeDeviceUpdate);

    }else{
        /* 对挂载目录监听 */
        if(!m_watcher)
            m_watcher = std::make_shared<FileWatcher>(m_uri, nullptr, true);
        m_watcher->setMonitorChildrenChange();
        connect(m_watcher.get(),&FileWatcher::fileCreated,this,&SideBarFileSystemItem::slot_fileCreate);
        connect(m_watcher.get(),&FileWatcher::fileDeleted,this,&SideBarFileSystemItem::slot_fileDelete);
        connect(m_watcher.get(),&FileWatcher::fileRenamed,this,&SideBarFileSystemItem::slot_fileRename);
        m_watcher->startMonitor();
    }

}

void SideBarFileSystemItem::findChildrenAsync()
{
    //TODO add async method.
    findChildren();
}

bool SideBarFileSystemItem::isRemoveable()
{
   return m_removeable;
}

bool SideBarFileSystemItem::isEjectable()
{
    return m_ejectable;
}

bool SideBarFileSystemItem::isMountable()
{
    return m_mountable;
}

bool SideBarFileSystemItem::isUnmountable()
{
    return m_unmountable;
}
bool SideBarFileSystemItem::isMounted()
{
    return m_mounted;
}

void SideBarFileSystemItem::unmount()
{
    SyncThread *syncThread = new SyncThread(m_uri);
    QThread* currentThread = new QThread();
    syncThread->moveToThread(currentThread);
    connect(currentThread,&QThread::started,syncThread,&SyncThread::parentStartedSlot);
    connect(syncThread,&SyncThread::syncFinished,this,[=](){
        m_volume.unmount();
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
        m_volume.eject(ejectFlag);
        syncThread->disconnect(this);
        syncThread->deleteLater();
        currentThread->disconnect(SIGNAL(started()));
        //currentThread->deleteLater();
    });
    currentThread->start();
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

void SideBarFileSystemItem::ejectOrUnmount()
{
    if (isRemoveable())
        eject(G_MOUNT_UNMOUNT_NONE);

    else if (isUnmountable())
        unmount();
}

void SideBarFileSystemItem::mount()
{
    if(isMountable())
        m_volume.mount();
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
    pThis->m_iconName = FileUtils::getFileIconName(pThis->m_uri,false);
}

