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

#include "file-info-manager.h"
#include "thumbnail-manager.h"
#include <QDebug>

using namespace Peony;

static FileInfoManager* global_file_info_manager = nullptr;
static QHash<QString, std::shared_ptr<FileInfo>> *global_info_list = nullptr;

FileInfoManager::FileInfoManager()
{
    global_info_list = new QHash<QString, std::shared_ptr<FileInfo>>();
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

std::shared_ptr<FileInfo> FileInfoManager::findFileInfoByUri(QString uri)
{
    Q_ASSERT(global_info_list);
    return global_info_list->value(uri);//.lock();
}

std::shared_ptr<FileInfo> FileInfoManager::insertFileInfo(std::shared_ptr<FileInfo> info)
{
    Q_ASSERT(global_info_list);

    if (global_info_list->value(info->uri())) {
        //qDebug()<<"has info yet"<<info->uri();
        info = global_info_list->value(info->uri());
    } else {
        global_info_list->insert(info->uri(), info);
    }

    return info;
}

void FileInfoManager::removeFileInfobyUri(QString uri)
{
    Q_ASSERT(global_info_list);
    global_info_list->remove(uri);
}

void FileInfoManager::clear()
{
    Q_ASSERT(global_info_list);
    global_info_list->clear();
}

void FileInfoManager::remove(QString uri)
{
    ThumbnailManager::getInstance()->releaseThumbnail(uri);
    Q_ASSERT(global_info_list);
    global_info_list->remove(uri);
}

void FileInfoManager::remove(std::shared_ptr<FileInfo> info)
{
    Q_ASSERT(global_info_list);
    this->remove(info->uri());
}

void FileInfoManager::showState()
{
    qDebug()<<global_info_list->keys().count()<<global_info_list->values().count();
}
