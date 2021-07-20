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

#include <QDebug>
#include <QProcess>

using namespace Peony;

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

QModelIndex SideBarNetWorkItem::firstColumnIndex()
{
    return m_model->firstColumnIndex(this);
}

QModelIndex SideBarNetWorkItem::lastColumnIndex()
{
    return m_model->lastColumnIndex(this);
}

void SideBarNetWorkItem::clearChildren()
{
    SideBarAbstractItem::clearChildren();
}

void SideBarNetWorkItem::findChildrenAsync()
{
    findChildren();
}

void SideBarNetWorkItem::findChildren()
{
    //只有根节点才设置子节点
    if (m_parentItem == nullptr) {
        clearChildren();

        //获取共享文件夹很慢，所以使用单独的线程处理 - Obtaining shared folders is slow, so use a separate thread for processing
        SharedDirectoryInfoThread *thread = new SharedDirectoryInfoThread(m_children, m_model, this);

        connect(thread, &SharedDirectoryInfoThread::querySharedInfoFinish, this, [=]() {
            delete thread;
            findRemoteServers();
            m_model->insertRows(0, m_children->count(), firstColumnIndex());
        });

        thread->start();
    }
}

void SideBarNetWorkItem::findRemoteServers()
{
    if (m_parentItem == nullptr) {
        //获取连接过的服务器
        QStringList remoteServerList = GlobalSettings::getInstance()->getValue(REMOTE_SERVER_IP).toStringList();

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
