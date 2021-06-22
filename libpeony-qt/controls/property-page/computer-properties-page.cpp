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

#include "computer-properties-page.h"

#include "linux-pwd-helper.h"

#include "volume-manager.h"

#include "file-utils.h"

#include <QFormLayout>
#include <QFile>

#include <QLabel>
#include <QFrame>

#include <QProgressBar>
#include <QPushButton>
#include <QProcess>
#include <QDBusConnection>
#include <QDBusInterface>

#include <glib.h>

using namespace Peony;

ComputerPropertiesPage::ComputerPropertiesPage(const QString &uri, QWidget *parent) : PropertiesWindowTabIface(parent)
{
    m_uri = uri;
    m_layout = new QFormLayout(this);
    m_layout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    m_layout->setFormAlignment(Qt::AlignLeft);
    m_layout->setLabelAlignment(Qt::AlignRight);
    setLayout(m_layout);

    if (uri == "computer:///") {

        //computer infos
        QFile cpuInfo("/proc/cpuinfo");
        cpuInfo.open(QFile::ReadOnly);
        QFile memInfo("/proc/meminfo");
        memInfo.open(QFile::ReadOnly);

        auto cpuInfos = cpuInfo.readAll().split('\n');
        auto memInfos = memInfo.readAll().split('\n');

        QString cpuName;
        QString cpuCoreCount;
        QString memSize;

        for (QString string : cpuInfos) {
            if (!cpuName.isEmpty() && !cpuCoreCount.isEmpty()) {
                break;
            }

            if (string.startsWith("model name")) {
                cpuName = string.split(":").last();
            }

            if (string.startsWith("cpu cores")) {
                cpuCoreCount = string.split(":").last();
            }
        }

        for (QString string : memInfos) {
            if (string.startsWith("MemTotal")) {
                memSize = string.split(":").last();
                break;
            }
        }

        memSize.remove(" ");

        m_layout->addRow(tr("CPU Name:"), new QLabel(cpuName, this));
        m_layout->addRow(tr("CPU Core:"), new QLabel(cpuCoreCount, this));
        m_layout->addRow(tr("Memory Size:"), new QLabel(" "+memSize, this));

        cpuInfo.close();
        memInfo.close();

        addSeparator();

        //user infos
        auto user = LinuxPWDHelper::getCurrentUser();
        QString userName = user.fullName();
        userName.replace(',', " ");
        QString desktopEnv = g_getenv("XDG_CURRENT_DESKTOP");
        m_layout->addRow(tr("User Name: "), new QLabel(userName, this));
        m_layout->addRow(tr("Desktop: "), new QLabel(desktopEnv, this));
    } else {
        //FIXME: get volume info correctly.

        //FIXME: replace BLOCKING api in ui thread.
        auto targetUri = FileUtils::getTargetUri(uri);
        if (targetUri.isNull()) {
            m_layout->addRow(new QLabel(tr("You should mount this volume first"), nullptr));
            return;
        }
        auto mount = VolumeManager::getMountFromUri(targetUri);
        if (targetUri == "file:///") {
            //NOTE: file:/// has not mount.
            GFile *file = g_file_new_for_uri(targetUri.toUtf8().constData());
            GFileInfo *info = g_file_query_filesystem_info(file, "*", nullptr, nullptr);
            quint64 total = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
            quint64 used = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_USED);
            quint64 available = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_FREE);
            // char *total_format = g_format_size(total);
            // char *used_format = g_format_size(used);
            // char *available_format = g_format_size(available);
            //Calculated by 1024 bytes
            char *total_format =g_format_size_full(total,G_FORMAT_SIZE_IEC_UNITS);
            char *used_format = g_format_size_full(used,G_FORMAT_SIZE_IEC_UNITS);
            char *available_format = g_format_size_full(available,G_FORMAT_SIZE_IEC_UNITS);

            QString total_format_string(total_format);
            QString used_format_string(used_format);
            QString available_format_string(available_format);
            //根据设计要求，按照1024字节对数据进行格式化（1GB = 1024MB），同时将GiB改为GB显示，以便于用户理解。参考windows显示样式。
            total_format_string.replace("iB", "B");
            used_format_string.replace("iB", "B");
            available_format_string.replace("iB", "B");

            char *fs_type = g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_FILESYSTEM_TYPE);
            QString type = getFileSystemType(uri);
            if (type.length() <=0)
                type = fs_type;

            m_layout->addRow(tr("Name: "), new QLabel(tr("File System"), this));
            m_layout->addRow(tr("Total Space: "), new QLabel(total_format_string, this));
            m_layout->addRow(tr("Used Space: "), new QLabel(used_format_string, this));
            m_layout->addRow(tr("Free Space: "), new QLabel(available_format_string, this));
            m_layout->addRow(tr("Type: "), new QLabel(type, this));
            g_free(total_format);
            g_free(used_format);
            g_free(available_format);
            g_free(fs_type);
            g_object_unref(info);
            g_object_unref(file);

            auto progressBar = new QProgressBar(this);
            auto value = double(used*1.0/total)*100;
            progressBar->setValue(int(value));
            m_layout->addRow(progressBar);
            m_layout->setAlignment(progressBar, Qt::AlignBottom);
            return;
        }
        if (mount) {
            auto volume = VolumeManager::getVolumeFromMount(mount);
            auto drive = VolumeManager::getDriveFromMount(mount);

            GFile *file = g_file_new_for_uri(targetUri.toUtf8().constData());
            GFileInfo *info = g_file_query_filesystem_info(file, "*", nullptr, nullptr);
            quint64 total = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_SIZE);
            quint64 used = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_USED);
            quint64 available = g_file_info_get_attribute_uint64(info, G_FILE_ATTRIBUTE_FILESYSTEM_FREE);
            //char *total_format = g_format_size(total);
            //char *used_format = g_format_size(used);
            //char *available_format = g_format_size(available);
            //Calculated by 1024 bytes
            char *total_format = g_format_size_full(total,G_FORMAT_SIZE_IEC_UNITS);
            char *used_format = g_format_size_full(used,G_FORMAT_SIZE_IEC_UNITS);
            char *available_format = g_format_size_full(available,G_FORMAT_SIZE_IEC_UNITS);

            QString total_format_string(total_format);
            QString used_format_string(used_format);
            QString available_format_string(available_format);
            //根据设计要求，按照1024字节对数据进行格式化（1GB = 1024MB），同时将GiB改为GB显示，以便于用户理解。参考windows显示样式。
            total_format_string.replace("iB", "B");
            used_format_string.replace("iB", "B");
            available_format_string.replace("iB", "B");
            //fix system Udisk calculate size wrong issue
            //comment to fix caculate volume capacity wrong issue,bug#51957
//            QString m_volume_name, m_unix_device, m_display_name, sizeInfo;
//            FileUtils::queryVolumeInfo(uri, m_volume_name, m_unix_device, m_display_name);
//            bool bMobileDevice = false;
//            //U disk or other mobile device
//            if (! m_unix_device.isEmpty() && ! uri.startsWith("computer:///WDC"))
//            {
//               char dev_name[256] ={0};
//               strncpy(dev_name, m_unix_device.toUtf8().constData(),sizeof(m_unix_device.toUtf8().constData()-1));
//               auto size = FileUtils::getDeviceSize(dev_name);
//               if (size > 0)
//               {
//                   sizeInfo = QString::number(size, 'f', 1);
//                   qDebug() << "size:" <<size;
//                   sizeInfo += "G";
//                   bMobileDevice = true;
//               }
//            }

            char *fs_type = g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_FILESYSTEM_TYPE);
            //use dubs to get file system type, fix ntfs show as fuse issue
            QString type = getFileSystemType(uri);
            if (type.length() <=0)
                type = fs_type;

            m_layout->addRow(tr("Name: "), new QLabel(mount->name(), this));
/*            if (bMobileDevice)
                m_layout->addRow(tr("Total Space: "), new QLabel(sizeInfo, this));
            else */
            if (total != 0) {
                m_layout->addRow(tr("Total Space: "), new QLabel(total_format_string, this));
            }
            if (used != 0) {
                m_layout->addRow(tr("Used Space: "), new QLabel(used_format_string, this));
            }
            if (available != 0) {
                m_layout->addRow(tr("Free Space: "), new QLabel(available_format_string, this));
            }
            m_layout->addRow(tr("Type: "), new QLabel(type, this));

            auto progressBar = new QProgressBar(this);
            auto value = double(used*1.0/total)*100;
            progressBar->setValue(int(value));
            m_layout->addRow(progressBar);
            m_layout->setAlignment(progressBar, Qt::AlignBottom);

            if (QString(fs_type) == "isofs" && QFile::exists("/usr/bin/kylin-burner")) {
                auto pushbutton = new QPushButton(tr("Kylin Burner"));
                connect(pushbutton, &QPushButton::clicked, pushbutton, [=](){
                    QProcess p;
                    p.startDetached("kylin-burner");
                    p.waitForStarted();
                });
                m_layout->addRow(new QLabel(tr("Open with: \t")), pushbutton);
            }

            g_free(total_format);
            g_free(used_format);
            g_free(available_format);
            g_free(fs_type);
            g_object_unref(info);
            g_object_unref(file);
        } else {
            m_layout->addRow(new QLabel(tr("Unknown"), nullptr));
        }
    }
}

void ComputerPropertiesPage::addSeparator()
{
    auto separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    m_layout->addRow(separator);
}

void ComputerPropertiesPage::saveAllChange()
{

}

QString ComputerPropertiesPage::getFileSystemType(QString uri)
{
    QString unixDevice,dbusPath;
    QString fsType = "";

    unixDevice = FileUtils::getUnixDevice(uri);

    if (unixDevice.isEmpty()) {
        return fsType;
    }
    dbusPath = "/org/freedesktop/UDisks2/block_devices/" + unixDevice.split("/").last();
    if (! QDBusConnection::systemBus().isConnected())
        return fsType;
    QDBusInterface blockInterface("org.freedesktop.UDisks2",
                                  dbusPath,
                                  "org.freedesktop.UDisks2.Block",
                                  QDBusConnection::systemBus());

    if(blockInterface.isValid())
        fsType = blockInterface.property("IdType").toString();

    //if need diff FAT16 and FAT32, should use IdVersion
//    if(fsType == "" && blockInterface.isValid())
//        fsType = blockInterface.property("IdVersion").toString();

    return fsType;
}
