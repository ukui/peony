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
#include "gobject-template.h"
#include "file-utils.h"
#include "libnotify/notification.h"
#include <sys/stat.h>
#include <libnotify/notify.h>

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
        m_uri(uri),
        m_iconName(iconName),
        m_displayName(displayName),
        m_parentItem(parentItem)
{
    auto userShareManager = UserShareInfoManager::getInstance();
    connect(userShareManager, &UserShareInfoManager::signal_addSharedFolder, this, &SideBarNetWorkItem::slot_addSharedFolder);
    connect(userShareManager, &UserShareInfoManager::signal_deleteSharedFolder, this, &SideBarNetWorkItem::slot_deleteSharedFolder);
    connect(GlobalSettings::getInstance(), &GlobalSettings::signal_updateRemoteServer,this,&SideBarNetWorkItem::slot_updateRemoteServer);
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

bool SideBarNetWorkItem::isMountable()
{
    if (!m_uri.startsWith("file://")&& m_uri!="network:///"){
        auto info = FileInfo::fromUri(m_uri);
        if (info->displayName().isEmpty()) {
            FileInfoJob j(info);
            j.querySync();
        }
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
        SyncThread::notifyUser(unmountNotify);
    }
    delete targetUri;
}

void SideBarNetWorkItem::realUnmount()
{
    auto mount = VolumeManager::getMountFromUri(this->uri().toUtf8().constData());
    QString *targetUri = new QString;
    *targetUri = this->uri();
    g_mount_unmount(mount->getGMount(),
                    G_MOUNT_UNMOUNT_NONE,
                    nullptr,
                    GAsyncReadyCallback(unmount_finished),
                    targetUri);
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
    SideBarAbstractItem::clearChildren();
}

void SideBarNetWorkItem::findChildrenAsync()
{
    findChildren();
}
#include "file-enumerator.h"
#include "file-utils.h"
void SideBarNetWorkItem::findChildren()
{
    //只有根节点才设置子节点
    if (m_parentItem == nullptr) {
        clearChildren();
        //获取共享文件夹很慢，所以使用单独的线程处理 - Obtaining shared folders is slow, so use a separate thread for processing
        SharedDirectoryInfoThread *thread = new SharedDirectoryInfoThread(m_children, m_model, this);

        connect(thread, &SharedDirectoryInfoThread::querySharedInfoFinish, this, [=]() {
            delete thread;
            m_model->insertRows(0, m_children->count(), firstColumnIndex());
        });
        findRemoteServers();
        thread->start();
    }

}

void SideBarNetWorkItem::findRemoteServers()
{
    if (m_parentItem == nullptr) {
        //获取连接过的服务器
        QStringList remoteServerList = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_REMOTE_IP).toStringList();

        for (const QString& remoteServer : remoteServerList) {
            if (!remoteServer.isEmpty()) {
                SideBarNetWorkItem *item = new SideBarNetWorkItem(remoteServer,
                                                                  "network-workgroup-symbolic",
                                                                  remoteServer,
                                                                  this,
                                                                  m_model, this);

                m_children->append(item);
            }
        }
    }
}

void SideBarNetWorkItem::slot_addSharedFolder(const ShareInfo &shareInfo, bool successed)
{
    if (!successed)
        return;

    if (!shareInfo.originalPath.isEmpty()) {
        SideBarNetWorkItem *item = new SideBarNetWorkItem("file://" + shareInfo.originalPath,
                                                          "inode-directory",
                                                          shareInfo.name,
                                                          this,
                                                          m_model, this);

        m_children->append(item);
        m_model->insertRows(m_children->count() - 1, 1, this->firstColumnIndex());
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
        m_model->removeRow(m_children->indexOf(item), this->firstColumnIndex());
        m_children->removeOne(item);
    }
    return;
}

void SideBarNetWorkItem::slot_updateRemoteServer(const QString& server,bool add)
{
   if(add){
       SideBarNetWorkItem *item = new SideBarNetWorkItem(server,
                                                         "network-workgroup-symbolic",
                                                         server,
                                                         this,
                                                         m_model, this);

       m_children->append(item);
       m_model->insertRows(m_children->count() - 1, 1, this->firstColumnIndex());
    }
   else{
       for (auto item : *m_children){
           if(item->uri()!= server)
               continue;
           m_model->removeRow(m_children->indexOf(item), this->firstColumnIndex());
           m_children->removeOne(item);
   }
    }
}

SharedDirectoryInfoThread::SharedDirectoryInfoThread(QVector<SideBarAbstractItem *> *children, SideBarModel *model,
                                                   SideBarNetWorkItem *parent) :m_children(children), m_model(model), m_parent(parent)
{

}

QString SharedDirectoryInfoThread::getShareInfo(QString arg1, QString arg2, QString arg3)
{
    QProcess proc;
    proc.open();

    QStringList args;
    args.prepend(arg3);
    args.prepend(arg2);
    args.prepend(arg1);
    args.prepend("/usr/bin/peony-share.sh");
    args.prepend("pkexec");

    proc.start("bash");
    proc.waitForStarted();

    QString cmd = args.join(" ");
    proc.write(cmd.toUtf8() + "\n");
    proc.waitForFinished(500);

    QString result("");
    result = proc.readAllStandardOutput();

    proc.close();

    return result;
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
    QString shareNames = getShareInfo("usershare", "list" , "");

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
            QString shareInfo = getShareInfo("usershare", "info" , shareName);
            QString sharePath = shareInfo.split(QRegExp("\\s+")).at(1).split("=").at(1);

            if (!sharePath.isEmpty()) {
                SideBarNetWorkItem *item = new SideBarNetWorkItem("file://" + sharePath,
                                                                  "inode-directory",
                                                                  shareName,
                                                                  m_parent,
                                                                  m_model, m_parent);

                m_children->append(item);
            }
        }
    }

    Q_EMIT querySharedInfoFinish();
}
