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

#include "side-bar-vfs-item.h"
#include "vfs-plugin-iface.h"
#include "vfs-plugin-manager.h"
#include "file-utils.h"

#include "side-bar-model.h"
#include "file-enumerator.h"
#include "side-bar-separator-item.h"
#include "file-info.h"
#include "file-info-job.h"
#include "file-watcher.h"

using namespace Peony;

SideBarVFSItem::SideBarVFSItem(const QString& uri, SideBarVFSItem* parentItem, SideBarModel *model, QObject *parent) :
    SideBarAbstractItem(model, parent)
  ,m_parentItem(parentItem)
{
   m_uri = uri;
}

SideBarVFSItem::~SideBarVFSItem()
{
    if(m_enumerator)
    {
        delete m_enumerator;
        m_enumerator = nullptr;
    }
}

QString SideBarVFSItem::uri()
{
    return m_uri;
}

QString SideBarVFSItem::displayName()
{
    if (m_displayName.isEmpty()) {
        auto vfsFile = VFSPluginManager::getInstance()->newVFSFile(uri());
        auto info = g_file_query_info(vfsFile, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
        m_displayName = g_file_info_get_display_name(info);
        g_object_unref(info);
        g_object_unref(vfsFile);
    }
    return m_displayName;
}

QString SideBarVFSItem::iconName()
{
    if (m_iconName.isEmpty()) {
        m_iconName = FileUtils::getFileIconName(m_uri);
    }
    return m_iconName;
}

void SideBarVFSItem::findChildren()
{
    clearChildren();
    /* 枚举操作 */
    if(!m_enumerator)
        m_enumerator= new FileEnumerator();
    m_enumerator->setEnumerateDirectory(m_uri);

    connect(m_enumerator,&FileEnumerator::enumerateFinished,this, &SideBarVFSItem::slot_enumeratorFinish);
    m_enumerator->enumerateAsync();

    if(!m_watcher)
        m_watcher = std::make_shared<FileWatcher>(m_uri, nullptr, true);
    m_watcher->setMonitorChildrenChange();
    connect(m_watcher.get(), &FileWatcher::fileCreated, this, &SideBarVFSItem::slot_fileCreate);
    connect(m_watcher.get(), &FileWatcher::fileDeleted, this, &SideBarVFSItem::slot_fileDelete);    
    connect(m_watcher.get(), &FileWatcher::fileChanged, this, &SideBarVFSItem::slot_fileSafeUpdate);
    connect(m_watcher.get(), &FileWatcher::directoryUnmounted, this, &SideBarVFSItem::slot_fileSafeLocked);

    m_watcher->startMonitor();

}

void SideBarVFSItem::findChildrenAsync()
{
    //TODO add async method.
    findChildren();
}

void SideBarVFSItem::clearChildren()
{
    if(m_watcher)
        m_watcher->stopMonitor();
    SideBarAbstractItem::clearChildren();
}

void SideBarVFSItem::slot_enumeratorFinish(bool successed)
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
        if (!info->displayName().startsWith(".") && (info->isDir())) {
            isEmpty = false;
        }

        if (!(info->isDir())){/* 只显示文件夹，文件不显示 */
            real_children_count--;
            continue;
        }

        /* 更新fileinfo */
        FileInfoJob job(info);
        job.querySync();

        SideBarVFSItem *item = new SideBarVFSItem(info->uri(), this, m_model);

        bool isUmountable = FileUtils::isFileUnmountable(info->uri());
        auto targetUri = FileUtils::getTargetUri(info->uri());
        item->m_mounted = !targetUri.isEmpty()|| isUmountable;
        m_children->append(item);
    }
    m_model->insertRows(0, real_children_count, firstColumnIndex());

    if (isEmpty) {
        auto separator = new SideBarSeparatorItem(SideBarSeparatorItem::EmptyFile, this, m_model);
        this->m_children->prepend(separator);
        m_model->insertRows(0, 1, this->firstColumnIndex());
    }

}

void SideBarVFSItem::slot_fileCreate(const QString &uri)
{
    qDebug()<<"created:"<<uri;
    for (auto item : *m_children) {
        if (item->uri() == uri) {
            return;
        }
    }

    SideBarVFSItem *item = new SideBarVFSItem(uri, this, m_model);
    m_model->beginInsertRows(this->firstColumnIndex(), m_children->count(), m_children->count());
    m_children->append(item);
    m_model->endInsertRows();
    m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
}

void SideBarVFSItem::slot_fileDelete(const QString &uri)
{
    qDebug()<<"deleted:"<<uri;
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
}

void SideBarVFSItem::slot_fileSafeLocked(const QString &uri)
{
    /* 锁定后折叠子项 */
    auto index = this->firstColumnIndex();
    Q_EMIT m_model->signal_collapsedChildren(index);
    m_model->dataChanged(this->firstColumnIndex(), this->lastColumnIndex());
}

void SideBarVFSItem::slot_fileSafeUpdate(const QString &uri)
{
    /* 更新iconName */
    for (auto item : *m_children){
        if (item->uri() == uri){
            auto info = FileInfo::fromUri(item->m_uri);
            /* 更新file info */
            FileInfoJob j(info);
            j.querySync();

            item->m_iconName = info.get()->iconName();
            m_model->dataChanged(item->firstColumnIndex(), item->lastColumnIndex());
            break;
        }
    }
}

