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

using namespace Peony;

SideBarVFSItem::SideBarVFSItem(const QString& uri, SideBarModel *model, QObject *parent) :
    SideBarAbstractItem(model, parent)
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
    return m_plugin->icon().name();
}

void SideBarVFSItem::findChildren()
{
    clearChildren();
    /* 枚举操作 */
    if(!m_enumerator)
        m_enumerator= new FileEnumerator();
    m_enumerator->setEnumerateDirectory(m_uri);
    m_enumerator->setEnumerateWithInfoJob();

    connect(m_enumerator,&FileEnumerator::prepared,this,&SideBarVFSItem::slot_enumeratorPrepared);
    m_enumerator->prepare();

}

void SideBarVFSItem::findChildrenAsync()
{
    //TODO add async method.
    findChildren();
}

void SideBarVFSItem::slot_enumeratorPrepared(const std::shared_ptr<GErrorWrapper> &err, const QString &targetUri, bool critical)
{
    connect(m_enumerator,&FileEnumerator::enumerateFinished,this, &SideBarVFSItem::slot_enumeratorFinish);
    m_enumerator->enumerateAsync();
}

void SideBarVFSItem::slot_enumeratorFinish(bool successed)
{
    if(!successed)
        return;
    auto infos = m_enumerator->getChildren();
    bool isEmpty = true;
    int real_children_count = infos.count();
    if (infos.isEmpty()) {
        return;
    }
    for (auto info: infos) {
        if (!info->displayName().startsWith(".") && (info->isDir())) {
            isEmpty = false;
        }

        if (!(info->isDir())){
            real_children_count--;
            continue;
        }

        SideBarVFSItem *item = new SideBarVFSItem(info->uri(), m_model);

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
