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

#include "file-info-manager.h"
#include "thumbnail-manager.h"
#include <QMap>
#include <QDebug>

using namespace Peony;

static FileInfoManager* global_file_info_manager = nullptr;
static QMap<QString, std::weak_ptr<FileInfo>> *global_info_list = nullptr;

static QMutex m_op_lock;

FileInfoManager::FileInfoManager()
{
    global_info_list = new QMap<QString, std::weak_ptr<FileInfo>>();
}

FileInfoManager::~FileInfoManager()
{
    delete global_info_list;
}

FileInfoManager *FileInfoManager::getInstance()
{
    if (!global_file_info_manager)
        global_file_info_manager = new FileInfoManager;
    return global_file_info_manager;
}

std::shared_ptr<FileInfo> FileInfoManager::findFileInfoByUri(const QString &uri)
{
    Q_ASSERT(global_info_list);
    m_op_lock.lock();
    auto info = global_info_list->value(uri).lock();//.lock();
    m_op_lock.unlock();
    return info;
}

std::shared_ptr<FileInfo> FileInfoManager::insertFileInfo(std::shared_ptr<FileInfo> info)
{
    Q_ASSERT(global_info_list);

    m_op_lock.lock();

    if (global_info_list->value(info->uri()).lock()) {
        //qDebug()<<"has info yet"<<info->uri();
        info = global_info_list->value(info->uri()).lock();
    } else {
        global_info_list->insert(info->uri(), info);
    }

    m_op_lock.unlock();

    return info;
}

void FileInfoManager::showState()
{
    qDebug()<<global_info_list->keys().count()<<global_info_list->values().count();
}
