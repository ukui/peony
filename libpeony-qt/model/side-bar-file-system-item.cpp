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
#include <QMessageBox>
#include <udisks/udisks.h>
#include <sys/stat.h>

using namespace Peony;

SideBarFileSystemItem::SideBarFileSystemItem(QString uri,
        SideBarFileSystemItem *parentItem,
        SideBarModel *model,
        QObject *parent) : SideBarAbstractItem (model, parent)
{
    m_parent = parentItem;

    if (parentItem == nullptr) {
        m_is_root_child = true;
        m_uri = "computer:///";
        m_display_name = tr("Computer");
        //m_icon_name = "computer";
        //top dir don't show icon
        m_icon_name = "";

        //m_watcher->setMonitorChildrenChange();
        //connect(m_watcher.get(), &FileWatcher::fileChanged, [=]())
    } else {
        m_uri = uri;
        m_display_name = FileUtils::getFileDisplayName(uri);
        m_icon_name = FileUtils::getFileIconName(uri);
        FileUtils::queryVolumeInfo(m_uri, m_volume_name, m_unix_device, m_display_name);
    }
}

QString SideBarFileSystemItem::displayName()
{
    QString displayName;
    if (!m_volume_name.isEmpty()) {
        displayName += m_volume_name;
    }
    if (!m_unix_device.isEmpty()) {
        displayName += QString(" (%1)").arg(m_unix_device);
    }
    if (!displayName.isEmpty())
        return displayName;

    return m_display_name;
}

QModelIndex SideBarFileSystemItem::firstColumnIndex()
{
    return m_model->firstCloumnIndex(this);
}

QModelIndex SideBarFileSystemItem::lastColumnIndex()
{
    return m_model->lastCloumnIndex(this);
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
    connect(e, &FileEnumerator::prepared, this, [=](const GErrorWrapperPtr &err, const QString &targetUri) {
        if (targetUri != nullptr) {
            if (targetUri != this->uri()) {
                e->setEnumerateDirectory(targetUri);
            }
        }

        e->enumerateSync();
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
            //skip the independent files
            if (!(info->isDir() || info->isVolume())) {
                real_children_count--;
                continue;
            }

            SideBarFileSystemItem *item = new SideBarFileSystemItem(info->uri(),
                    this,
                    m_model,
                    this);
            //check is mounted.
            auto targetUri = FileUtils::getTargetUri(info->uri());
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
                if (item->uri() == uri)
                    return;
            }

            SideBarFileSystemItem *item = new SideBarFileSystemItem(uri,
                    this,
                    m_model);
            m_children->append(item);
            m_model->insertRows(m_children->count() - 1, 1, firstColumnIndex());
            m_model->indexUpdated(this->firstColumnIndex());
        });

        connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](const QString &uri) {
            //qDebug()<<"deleted:"<<uri;
            for (auto child : *m_children) {
                if (child->uri() == uri) {
                    int index = m_children->indexOf(child);
                    m_model->removeRows(index, 1, firstColumnIndex());
                    m_children->removeOne(child);
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
        //m_model->setData(lastColumnIndex(), QVariant(QIcon::fromTheme("media-eject")), Qt::DecorationRole);
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
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {
        auto info = FileInfo::fromUri(m_uri);
        if (info->displayName().isEmpty()) {
            FileInfoJob j(info);
            j.querySync();
        }
        return info->canEject() || info->canStop();
    }
    return false;
}

bool SideBarFileSystemItem::isEjectable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {
        auto info = FileInfo::fromUri(m_uri);
        if (info->displayName().isEmpty()) {
            FileInfoJob j(info);
            j.querySync();
        }
        return info->canEject();
    }
    return false;
}

bool SideBarFileSystemItem::isMountable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {
        //some mountable item can be unmounted but can't be mounted.
        //the most of them is remote servers and shared directories.
        //they should be seemed as mountable items.
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
    return m_is_mounted;
}

static GAsyncReadyCallback eject_cb(GFile *file,
                                    GAsyncResult *res,
                                    SideBarFileSystemItem *p_this)
{
    GError *err = nullptr;
    bool successed = g_file_eject_mountable_with_operation_finish(file, res, &err);
    qDebug()<<successed;
    if (err) {
        qDebug()<<err->message;
        g_error_free(err);
    }
    return nullptr;
}

void SideBarFileSystemItem::eject()
{
    auto file = wrapGFile(g_file_new_for_uri(this->uri().toUtf8().constData()));
    auto target = FileUtils::getTargetUri(m_uri);
    auto drive = VolumeManager::getDriveFromUri(target);
    g_file_eject_mountable_with_operation(file.get()->get(),
                                          G_MOUNT_UNMOUNT_NONE,
                                          nullptr,
                                          nullptr,
                                          GAsyncReadyCallback(eject_cb),
                                          this);
}

static void unmount_finished(GObject*, GAsyncResult*, gpointer udata)
{
    SideBarFileSystemItem* th = (SideBarFileSystemItem*)udata;
    if (FileUtils::isMountPoint(FileUtils::getTargetUri(th->uri()))) {
        QMessageBox::warning(nullptr, QObject::tr("eject device failed"),
                             QObject::tr("Please check whether the device is occupied and then eject the device again"),
                             QMessageBox::Ok);
    }
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


void SideBarFileSystemItem::unmount()
{
    UDisksObject *object;
    UDisksClient *client;
    UDisksBlock *block;
    client = udisks_client_new_sync(NULL,NULL);

    object = get_object_from_block_device(client,m_unix_device.toUtf8().constData());
    block = udisks_object_get_block(object);

    // if device type is disc , Eject optical drive 
    if(g_strcmp0(udisks_block_get_id_type(block),"iso9660")==0 || strlen(udisks_block_get_id_type(block))==0){

        char cmd[1024] ={0} ;
        strcpy(cmd,"eject ");
        strcat(cmd,m_unix_device.toUtf8().constData());

        system(cmd);
        
        return;
    }

    auto file = wrapGFile(g_file_new_for_uri(this->uri().toUtf8().constData()));
    g_file_unmount_mountable_with_operation(file.get()->get(),
                                            G_MOUNT_UNMOUNT_NONE,
                                            nullptr,
                                            nullptr,
                                            unmount_finished,
                                            this);
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
