/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

#include "side-bar-net-work-item.h"
#include "side-bar-model.h"
#include "global-settings.h"
#include "file-info.h"
#include "file-info-job.h"
#include "connect-to-server-dialog.h"
#include "sync-thread.h"
#include "volume-manager.h"
//#include "volumeManager.h"
#include "gobject-template.h"
#include "file-utils.h"
#include "libnotify/notification.h"
#include <sys/stat.h>
#include <libnotify/notify.h>
#include "file-watcher.h"

#include <QMessageBox>
#include <QDebug>
#include <QProcess>

using namespace Peony;

void notifyUser(QString notifyContent);

SideBarNetWorkItem::SideBarNetWorkItem(const QString &uri,
                                       const QString &iconName,
                                       const QString &displayName,
                                       SideBarAbstractItem *parentItem,
                                       SideBarModel *model,
                                       QObject *parent) :
        SideBarAbstractItem(model, parent),
        m_parentItem(parentItem)
{
    m_uri = uri;
    m_iconName = iconName;
    m_displayName = displayName;

    auto userShareManager = UserShareInfoManager::getInstance();
    connect(userShareManager, &UserShareInfoManager::signal_addSharedFolder, this, &SideBarNetWorkItem::slot_addSharedFolder);
    connect(userShareManager, &UserShareInfoManager::signal_deleteSharedFolder, this, &SideBarNetWorkItem::slot_deleteSharedFolder);
    connect(GlobalSettings::getInstance(), &GlobalSettings::signal_updateRemoteServer,this,&SideBarNetWorkItem::slot_updateRemoteServer);
    qRegisterMetaType<QHash<QString,QString> >("QHash<QString,QString>");
}

QString SideBarNetWorkItem::uri()
{
    return m_uri;
}

QString SideBarNetWorkItem::displayName()
{
    return m_displayName;
}

QString SideBarNetWorkItem::iconName()
{
    return m_iconName;
}

bool SideBarNetWorkItem::hasChildren()
{
    return (m_parentItem == nullptr);
}


bool SideBarNetWorkItem::isUnmountable()
{
    /* 远程服务器返回true */
    if (!m_uri.startsWith("file://")&& m_uri!="network:///"){
        return true;
    }
    return false;
}

bool SideBarNetWorkItem::isMounted()
{
    if(FileUtils::isMountPoint(m_uri))
        return true;
    return false;
}

QModelIndex SideBarNetWorkItem::firstColumnIndex()
{
    return m_model->firstColumnIndex(this);
}

QModelIndex SideBarNetWorkItem::lastColumnIndex()
{
    return m_model->lastColumnIndex(this);
}

static void unmount_finished(GMount *mount, GAsyncResult *result, QString *mountPath)
{
    GError *err = nullptr;   
    g_mount_unmount_with_operation_finish(mount, result, &err);
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
        /* 卸载完成信息提示 */
        Q_EMIT Peony::VolumeManager::getInstance()->fileUnmounted(*mountPath);
        QString unmountNotify = QObject::tr("Data synchronization is complete,the device has been unmount successfully!");
        SyncThread::notifyUser(unmountNotify);
    }
    if(mountPath){
        delete mountPath;
        mountPath = nullptr;
    }
}

void SideBarNetWorkItem::realUnmount()
{
    GFile *gFile= g_file_new_for_uri(this->uri().toUtf8().constData());
    auto mountPath = new QString;
    *mountPath = m_uri;
    GMount *gMount = g_file_find_enclosing_mount(gFile, nullptr, nullptr);
    g_mount_unmount_with_operation(gMount,
                    G_MOUNT_UNMOUNT_NONE,
                    nullptr,
                    nullptr,
                    GAsyncReadyCallback(unmount_finished),
                    mountPath);
    g_object_unref(gFile);
    g_object_unref(gMount);
}

void SideBarNetWorkItem::ejectOrUnmount()
{
    unmount();
}

void SideBarNetWorkItem::unmount()
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
    });
    currentThread->start();
}

void SideBarNetWorkItem::clearChildren()
{
    stopWatcher();
    SideBarAbstractItem::clearChildren();
}

void SideBarNetWorkItem::findChildrenAsync()
{
    findChildren();
}

#include "file-utils.h"
#include <QUrl>
void SideBarNetWorkItem::findChildren()
{
    //只有根节点才设置子节点
    if (m_parentItem == nullptr) {
        clearChildren();
        findRemoteServers();
        querySharedFolders();
    }

    /* 计算机视图卸载时，侧边栏item状态也要响应 */
    this->initWatcher();
    this->m_watcher->setMonitorChildrenChange();
    connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](const QString &uri) {
        for (auto item : *m_children) {
            if(QUrl(item->uri()).host()==QUrl(uri).host())
                m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
        }
    });

    this->startWatcher();
}

void SideBarNetWorkItem::findRemoteServers()
{
    if (m_parentItem == nullptr) {
        //获取连接过的服务器
        QMap <QString, QVariant> remoteServer = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_REMOTE_IP).toMap ();

        for (const QString& remoteServer : remoteServer.keys ()) {
            if (!remoteServer.isEmpty()) {

                SideBarNetWorkItem *item = new SideBarNetWorkItem(remoteServer,
                                                                  "network-workgroup-symbolic",
                                                                  remoteServer,
                                                                  this,
                                                                  m_model, this);
                item->m_isVolume = true;
                m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
                m_children->append(item);
                m_model->endInsertRows();
            }
        }
    }
}

void SideBarNetWorkItem::querySharedFolders()
{
    if (m_parentItem != nullptr)
        return;

    //获取共享文件夹很慢，所以使用单独的线程处理 - Obtaining shared folders is slow, so use a separate thread for processing
    SharedDirectoryInfoThread *thread = new SharedDirectoryInfoThread();

    connect(thread, &SharedDirectoryInfoThread::querySharedInfoFinish, this, [=](QHash<QString,QString> sharedFolderInfoMap)
    {
        delete thread;
        QHash<QString, QString>::iterator iter;
        for(iter = sharedFolderInfoMap.begin();iter != sharedFolderInfoMap.end();iter++){
            QString sharePath=iter.value();
            QString shareName=iter.key();
            if (!sharePath.isEmpty()) {
                SideBarNetWorkItem *item = new SideBarNetWorkItem("file://" + sharePath,"folder",
                                                                  shareName,this,m_model);
                m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
                m_children->append(item);
                m_model->endInsertRows();
            }
        }
    });

    thread->start();
}

void SideBarNetWorkItem::slot_addSharedFolder(const ShareInfo &shareInfo, bool successed)
{
    if (!successed)
        return;

    if (!shareInfo.originalPath.isEmpty()) {
        SideBarNetWorkItem *item = new SideBarNetWorkItem("file://" + shareInfo.originalPath,
                                                          "folder",
                                                          shareInfo.name,
                                                          this,
                                                          m_model, this);
        m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
        m_children->append(item);
        m_model->endInsertRows();
    }
    return;
}

void SideBarNetWorkItem::slot_deleteSharedFolder(const QString& originalPath, bool successed)
{
    if(!successed)
        return;
    for (auto item : *m_children){
        if(item->uri()!="file://" + originalPath)
            continue;
        int index = m_children->indexOf(item);
        m_model->beginRemoveRows(firstColumnIndex(), index, index);
        m_children->removeOne(item);
        m_model->endRemoveRows();
        item->deleteLater();
        break;
    }
    return;
}

void SideBarNetWorkItem::slot_updateRemoteServer(const QString& server,bool add)
{
   if(add){
       /* 防止一个远程服务器多次添加 */
       for (auto item : *m_children) {
           if (item->uri() == server) {
               return;
           }
       }
       SideBarNetWorkItem *item = new SideBarNetWorkItem(server,
                                                         "network-workgroup-symbolic",
                                                         server,
                                                         this,
                                                         m_model, this);
       item->m_isVolume = true;
       m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
       m_children->append(item);
       m_model->endInsertRows();
    }
   else{
       for (auto item : *m_children){
           if(item->uri()!= server)
               continue;
           int index = m_children->indexOf(item);
           m_model->beginRemoveRows(firstColumnIndex(), index, index);
           m_children->removeOne(item);
           m_model->endRemoveRows();
           item->deleteLater();
           break;

   }
   }
}

void SideBarNetWorkItem::initWatcher()
{
    /* 监听计算机视图的卸载信号 */
    if (!m_watcher) {
        m_watcher = std::make_shared<FileWatcher>("computer:///");
    }
}

void SideBarNetWorkItem::startWatcher()
{initWatcher();
    m_watcher->startMonitor();

}

void SideBarNetWorkItem::stopWatcher()
{
    initWatcher();
    m_watcher->stopMonitor();
}

SharedDirectoryInfoThread::SharedDirectoryInfoThread()
{

}

void SharedDirectoryInfoThread::run()
{
    /**
     * \brief 获取全部共享文件夹的共享名称
     * 在编写代码时的输出格式为：
     * 888
     *
     * 如果输出发生改变，视情况修改
     */
    QStringList args;
    args.append("usershare");
    args.append("list");
    args.append("");
    bool ret = false;

    auto userShareManager = UserShareInfoManager::getInstance();
    QString shareNames=userShareManager->exectueCommand(args,&ret);

    QHash<QString,QString> sharedFolderInfoMap;/* key:shareName,value: sharePath */

    for (QString shareName : shareNames.split(QRegExp("\\s+"))) {
        if (!shareName.isEmpty()) {
            /**
             * \brief 根据共享名称获取详细信息
             * 在编写代码时的输出格式为：
             * [888]
             * path=/home/hxf/888
             * comment=Peony-Qt-Share-Extension
             * usershare_acl=Everyone:R,
             * guest_ok=n
             *
             * 如果输出发生改变，视情况修改
             */

            const Peony::ShareInfo* shareInfo = userShareManager->getShareInfo(shareName);
            QString sharePath = shareInfo->originalPath;
            if (!sharePath.isEmpty())
                sharedFolderInfoMap.insert(shareName,sharePath);
        }
    }

    Q_EMIT querySharedInfoFinish(sharedFolderInfoMap);
}
