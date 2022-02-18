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

#ifndef FILEUTILS_H
#define FILEUTILS_H

#define NO_BLOCKING
#define BLOCKING

#include "peony-core_global.h"
#include "gobject-template.h"
#include "gerror-wrapper.h"

#include <QString>

namespace Peony {

class PEONYCORESHARED_EXPORT FileUtils
{
public:
    NO_BLOCKING static QString getQStringFromCString(char *c_string, bool free = true);
    NO_BLOCKING static QString getFileUri(const GFileWrapperPtr &file);
    NO_BLOCKING static QString getFileBaseName(const GFileWrapperPtr &file);
    NO_BLOCKING static QString getUriBaseName(const QString &uri);
    NO_BLOCKING static GFileWrapperPtr getFileParent(const GFileWrapperPtr &file);
    NO_BLOCKING static QString getRelativePath(const GFileWrapperPtr &dir, const GFileWrapperPtr &file);
    NO_BLOCKING static GFileWrapperPtr resolveRelativePath(const GFileWrapperPtr &dir, const QString &relativePath);
    NO_BLOCKING static QString urlEncode(const QString& url);
    NO_BLOCKING static QString urlDecode(const QString& url);


    /**
     * @return handled name
     */
    BLOCKING static QString handleDuplicateName (const QString& uri);
    BLOCKING static bool getFileHasChildren(const GFileWrapperPtr &file);
    BLOCKING static bool getFileIsFolder(const GFileWrapperPtr &file);
    BLOCKING static bool getFileIsFolder(const QString &uri);
    BLOCKING static bool getFileIsSymbolicLink(const QString &uri);
    BLOCKING static QStringList getChildrenUris(const QString &directoryUri);

    NO_BLOCKING static QString handleDesktopFileName(const QString& uri, const QString& displayName);
    NO_BLOCKING static QString getNonSuffixedBaseNameFromUri(const QString &uri);
    BLOCKING static QString getFileDisplayName(const QString &uri);
    BLOCKING static QString getFileIconName(const QString &uri, bool checkValid = true);

    BLOCKING static GErrorWrapperPtr getEnumerateError(const QString &uri);
    BLOCKING static QString getTargetUri(const QString &uri);
    BLOCKING static QString getEncodedUri(const QString &uri);
    BLOCKING static QString getSymbolicTarget(const QString &uri);
    BLOCKING static bool isMountPoint(const QString& uri);

    NO_BLOCKING static bool stringStartWithChinese(const QString &string);
    NO_BLOCKING static bool stringLesserThan(const QString &left, const QString &right);

    NO_BLOCKING static const QString getParentUri(const QString &uri);
    NO_BLOCKING static const QString getOriginalUri(const QString &uri);

    NO_BLOCKING static bool isStandardPath(const QString &uri);
    NO_BLOCKING static bool isMobileDeviceFile(const QString &uri);
    NO_BLOCKING static bool isSamePath(const QString &uri, const QString &targetUri);
    NO_BLOCKING static bool containsStandardPath(const QStringList &list);
    NO_BLOCKING static bool containsStandardPath(const QList<QUrl> &urls);

    BLOCKING static bool isFileExsit(const QString &uri);

    NO_BLOCKING static const QStringList toDisplayUris(const QStringList &args);

    BLOCKING static bool isMountRoot(const QString &uri);

    BLOCKING static bool queryVolumeInfo(const QString &volumeUri,
                                QString &volumeName,
                                QString &unixDeviceName,
                                const QString &volumeDisplayName = nullptr);

    BLOCKING static bool isReadonly (const QString&);
    BLOCKING static bool isFileDirectory(const QString &uri);

    BLOCKING static bool isFileUnmountable(const QString &uri);
    BLOCKING static double getDeviceSize(const gchar * device_name);
    BLOCKING static quint64 getFileTotalSize(const QString &uri);

    static void handleVolumeLabelForFat32(QString &volumeName,const QString &unixDevcieName);
    static QString getUnixDevice(const QString &uri);
    static quint64 getFileSystemSize(QString uri);
    static QString getFileSystemType(QString uri);

    static void   saveCreateTime (const QString& url);
    static gint64 getCreateTimeOfMicro (const QString& url);
private:
    FileUtils();
};

}

#endif // FILEUTILS_H
