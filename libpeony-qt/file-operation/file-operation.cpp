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

#include <QApplication>
#include <QProcess>
#include <file-info-job.h>
#include <file-info.h>

#include "file-operation.h"
#include "file-operation-manager.h"

using namespace Peony;


#define FAT_FORBIDDEN_CHARACTERS "/:*?\"<>\\|"

FileOperation::FileOperation(QObject *parent) : QObject (parent)
{
    m_cancellable_wrapper = wrapGCancellable(g_cancellable_new());
    setAutoDelete(true);
}

FileOperation::~FileOperation()
{

}

void FileOperation::run()
{

}

void FileOperation::cancel()
{
    g_cancellable_cancel(m_cancellable_wrapper.get()->get());
    m_is_cancelled = true;
}

bool FileOperation::makeFileNameValidForDestFS(QString &srcPath, QString &destPath, QString *newFileName)
{
    FileInfoJob fileInfoJob(destPath);
    fileInfoJob.querySync();

    QString srcFileName = srcPath.split("/").back();
    *newFileName = srcFileName;
    QString fsType = fileInfoJob.getInfo()->fileSystemType();

    if ("fat" == fsType || "vfat" == fsType || "fuse" == fsType || "ntfs" == fsType || "msdos" == fsType || "msdosfs" == fsType) {
        for (size_t i = 0; i < strlen(FAT_FORBIDDEN_CHARACTERS); ++i) {
            *newFileName = newFileName->replace(FAT_FORBIDDEN_CHARACTERS[i], "_");
        }
    }

    return *newFileName != srcFileName;
}

void FileOperation::fileSync(QString srcFile, QString destDir)
{
    if (srcFile.endsWith("/")) {
        srcFile.chop(1);
    }

    QString destFile = "";
    if (destDir.split("/").back() == srcFile.split("/").back()) {
        destFile = destDir;
    } else {
        destFile = destDir + "/" + srcFile.split("/").back();
    }

    if (!FileUtils::isFileExsit(destFile)) {
        qDebug() << "file:" << destFile << " is not existed!";
        return;
    }

    bool needSync = false;
    GFile* srcGfile = g_file_new_for_uri(srcFile.toUtf8().constData());
    GFile* destGfile = g_file_new_for_uri(destFile.toUtf8().constData());
    GMount* srcMount = g_file_find_enclosing_mount(srcGfile, nullptr, nullptr);
    GMount* destMount = g_file_find_enclosing_mount(destGfile, nullptr, nullptr);

    // xxxMount is null in root filesystem
    if ((srcMount != destMount) && (NULL != destMount)) {
        needSync = true;
    }

    if (needSync) {
        auto path = g_file_get_path(destGfile);
        qDebug() << "sync -- src: " << srcFile << "  ===  " << destDir << "  ===  " << destFile << "  path:" << path;
        if (path) {
            QProcess p;
            auto shellPath = g_shell_quote(path);
            qDebug() << "start execute: " << QString("sync -f %1").arg(shellPath);
            p.start(QString("sync -f %1").arg(shellPath));
            g_free(path);
            g_free(shellPath);
            p.waitForFinished(-1);
            qDebug() << "execute: " << QString("sync -f %1  ok!!!").arg(shellPath);
        }
    }

    if (nullptr != srcMount) {
        g_object_unref(srcMount);
    }

    if (nullptr != destMount) {
        g_object_unref(destMount);
    }

    if (nullptr != srcGfile) {
        g_object_unref(srcGfile);
    }

    if (nullptr != destGfile) {
        g_object_unref(destGfile);
    }
}

void FileOperation::notifyFileWatcherOperationFinished()
{
    if (!qApp->allWidgets().isEmpty()) {
        // notify operation for file watchers.
        auto info = this->getOperationInfo();
        qDebug()<<info->m_src_dir_uri;
        if (info)
            FileOperationManager::getInstance()->manuallyNotifyDirectoryChanged(info.get());
    }
}
