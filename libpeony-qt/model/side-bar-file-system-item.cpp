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
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "side-bar-file-system-item.h"
#include "side-bar-model.h"
#include "file-utils.h"
#include "file-enumerator.h"
#include "file-info-job.h"
#include "file-info.h"
#include "gobject-template.h"
#include "file-watcher.h"

#include "side-bar-separator-item.h"

#include "linux-pwd-helper.h"

#include <QIcon>

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
        m_icon_name = "computer";

        //m_watcher->setMonitorChildrenChange();
        //connect(m_watcher.get(), &FileWatcher::fileChanged, [=]())
    } else {
        m_uri = uri;
        m_display_name = FileUtils::getFileDisplayName(uri);
        m_icon_name = FileUtils::getFileIconName(uri);
    }
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
    connect(e, &FileEnumerator::prepared, [=](const GErrorWrapperPtr &err){
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
            item->m_is_mounted = !targetUri.isEmpty() && (targetUri != "file:///");
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
        connect(m_watcher.get(), &FileWatcher::fileCreated, [=](const QString &uri){
            //qDebug()<<"created:"<<uri;
            SideBarFileSystemItem *item = new SideBarFileSystemItem(uri,
                                                                    this,
                                                                    m_model);
            m_children->append(item);
            m_model->insertRows(m_children->count() - 1, 1, firstColumnIndex());
            m_model->indexUpdated(this->firstColumnIndex());
        });

        connect(m_watcher.get(), &FileWatcher::fileDeleted, [=](const QString &uri){
            //qDebug()<<"deleted:"<<uri;
            for (auto child : *m_children) {
                if (child->uri() == uri) {
                    int index = m_children->indexOf(child);
                    m_children->removeOne(child);
                    child->deleteLater();
                    m_model->removeRows(index, 1, firstColumnIndex());
                    break;
                }
            }
            m_model->indexUpdated(this->firstColumnIndex());
        });

        connect(m_watcher.get(), &FileWatcher::fileChanged, [=](const QString &uri) {
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
                    m_model->dataChanged(changedItem->firstColumnIndex(), changedItem->firstColumnIndex());
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

    }
    return false;
}

bool SideBarFileSystemItem::isEjectable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {

    }
    return false;
}

bool SideBarFileSystemItem::isMountable()
{
    if (m_uri.contains("computer:///") && m_uri != "computer:///") {
        return true;
    }
    return false;
}

bool SideBarFileSystemItem::isMounted()
{
    return m_is_mounted;
}

void SideBarFileSystemItem::unmount()
{
    auto file = wrapGFile(g_file_new_for_uri(this->uri().toUtf8().constData()));
    g_file_unmount_mountable_with_operation(file.get()->get(),
                                            G_MOUNT_UNMOUNT_NONE,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            nullptr);
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
