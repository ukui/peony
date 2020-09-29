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

#include "file-info.h"

#include "file-info-manager.h"
#include "file-info-job.h"
#include "file-meta-info.h"

#include "thumbnail-manager.h"

#include <QUrl>
#include <QtDBus/QDBusConnection>
#include <QStandardPaths>
#include <QDebug>

using namespace Peony;

FileInfo::FileInfo(QObject *parent) : QObject (parent)
{
    m_cancellable = g_cancellable_new();
//    bool a = QDBusConnection::systemBus().connect(QString(), QString("/com/ukui/desktop/software"),
//                                         "com.ukui.desktop.software", "send_to_client", this,
//                                         SLOT(getEnableSig(QString, bool)));
}

FileInfo::FileInfo(const QString &uri, QObject *parent) : QObject (parent)
{
    m_cancellable = g_cancellable_new();
    /*!
     * \note
     * In qt program we alwas handle file's uri format as unicode,
     * but in glib/gio it might be not.
     * I want to keep the uri strings format in peony-qt,
     * this would help me avoid some problem, such as the uri path completion
     * bug in PathBarModel enumeration.
     */
    m_uri = uri;
    m_file = g_file_new_for_uri(uri.toUtf8().data());
    m_parent = g_file_get_parent(m_file);
    m_is_remote = !g_file_is_native(m_file);
    GFileType type = g_file_query_file_type(m_file, G_FILE_QUERY_INFO_NONE, nullptr);
    switch (type) {
    case G_FILE_TYPE_DIRECTORY:
        //qDebug()<<"dir";
        m_is_dir = true;
        break;
    case G_FILE_TYPE_MOUNTABLE:
        //qDebug()<<"mountable";
        m_is_volume = true;
        break;
    default:
        break;
    }
//    QDBusConnection::systemBus().connect(QString(), QString("/com/ukui/desktop/software"),
//                                         "com.ukui.desktop.software", "send_to_client", this,
//                                         SLOT(getEnableSig(QString, bool)));

}

FileInfo::~FileInfo()
{
    //qDebug()<<"~FileInfo"<<m_uri;
    disconnect();

    g_object_unref(m_cancellable);
    g_object_unref(m_file);

    if (m_target_file)
        g_object_unref(m_target_file);
    if (m_parent)
        g_object_unref(m_parent);

    m_uri = nullptr;
}

std::shared_ptr<FileInfo> FileInfo::fromUri(QString uri, bool addToHash)
{
    FileInfoManager *info_manager = FileInfoManager::getInstance();
    info_manager->lock();
    std::shared_ptr<FileInfo> info = info_manager->findFileInfoByUri(uri);
    if (info != nullptr) {
        info_manager->unlock();
        return info;
    } else {
        std::shared_ptr<FileInfo> newly_info = std::make_shared<FileInfo>();

        newly_info->m_uri = uri;
        newly_info->m_file = g_file_new_for_uri(uri.toUtf8().data());

        newly_info->m_parent = g_file_get_parent(newly_info->m_file);
        newly_info->m_is_remote = !g_file_is_native(newly_info->m_file);
        GFileType type = g_file_query_file_type(newly_info->m_file, G_FILE_QUERY_INFO_NONE, nullptr);
        switch (type) {
        case G_FILE_TYPE_DIRECTORY:
            //qDebug()<<"dir";
            newly_info->m_is_dir = true;
            break;
        case G_FILE_TYPE_MOUNTABLE:
            //qDebug()<<"mountable";
            newly_info->m_is_volume = true;
            break;
        default:
            break;
        }
        if (addToHash) {
            newly_info = info_manager->insertFileInfo(newly_info);
        }
        info_manager->unlock();
        return newly_info;
    }
}

std::shared_ptr<FileInfo> FileInfo::fromPath(QString path, bool addToHash)
{
    QString uri = "file://"+path;
    return fromUri(uri, addToHash);
}

std::shared_ptr<FileInfo> FileInfo::fromGFile(GFile *file, bool addToHash)
{
    char *uri_str = g_file_get_uri(file);
    QString uri = uri_str;
    g_free(uri_str);
    return fromUri(uri, addToHash);
}

/*******
函数功能：判断文件是否是视频文件
一般的视频文件都是 video/*,但是有些视频文件比较特殊
比如：
asf :   application/vnd.ms-asf
rmvb:   application/vnd.rn-realmedia
swf :   application/vnd.adobe.flash.movie
ts  :   text/vnd.trolltech.linguist
h264:   application/octet-stream
目前上面这些是已经测试到的视频文件类型，如果有没有覆盖到的情况后面再补充
**/
bool FileInfo::isVideoFile()
{
    if (nullptr != m_mime_type_string)
    {
        if (m_mime_type_string.startsWith("video")
            || m_mime_type_string.endsWith("vnd.trolltech.linguist")
            || m_mime_type_string.endsWith("vnd.adobe.flash.movie")
            || m_mime_type_string.endsWith("vnd.rn-realmedia")
            || m_mime_type_string.endsWith("vnd.ms-asf")
            || m_mime_type_string.endsWith("octet-stream"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


bool FileInfo::isOfficeFile()
{
    int idx = 0;
    QString mtype = nullptr;

    for (idx = 0; office_mime_types[idx] != "end"; idx++)
    {
        mtype = office_mime_types[idx];
        if (m_mime_type_string.contains(mtype))
        {
            return true;
        }
    }

    return false;
}
bool FileInfo::isExecDisable()
{
     int nRet = m_meta_info->getMetaInfoInt("exec_disable");
     if(1==nRet) return true;
     return false;
}

//void FileInfo::getEnableSig(QString appid, bool execenable)
//{
//    QString uri = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/桌面/" + appid + ".desktop";
//    if (m_uri == nullptr || m_uri != uri)
//        return;
//    if (execenable)
//        m_meta_info->setMetaInfoString("exec_disable","1");
//    else
//        m_meta_info->setMetaInfoString("exec_disable","0");
//    return;
//}
