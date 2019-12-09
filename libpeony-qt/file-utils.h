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

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "peony-core_global.h"
#include "gobject-template.h"
#include "gerror-wrapper.h"

#include <QString>

namespace Peony {

class PEONYCORESHARED_EXPORT FileUtils
{
public:
    static QString getQStringFromCString(char *c_string, bool free = true);
    static QString getFileUri(const GFileWrapperPtr &file);
    static QString getFileBaseName(const GFileWrapperPtr &file);
    static QString getUriBaseName(const QString &uri);
    static GFileWrapperPtr getFileParent(const GFileWrapperPtr &file);
    static QString getRelativePath(const GFileWrapperPtr &dir, const GFileWrapperPtr &file);
    static GFileWrapperPtr resolveRelativePath(const GFileWrapperPtr &dir, const QString &relativePath);
    static bool getFileHasChildren(const GFileWrapperPtr &file);
    static bool getFileIsFolder(const GFileWrapperPtr &file);
    static bool getFileIsFolder(const QString &uri);

    static QString getNonSuffixedBaseNameFromUri(const QString &uri);
    static QString getFileDisplayName(const QString &uri);
    static QString getFileIconName(const QString &uri);

    static GErrorWrapperPtr getEnumerateError(const QString &uri);
    static QString getTargetUri(const QString &uri);

    static bool stringStartWithChinese(const QString &string);
    static bool stringLesserThan(const QString &left, const QString &right);

    static const QString getParentUri(const QString &uri);

    static bool isFileExsit(const QString &uri);

    static const QStringList toDisplayUris(const QStringList &args);

private:
    FileUtils();
};

}

#endif // FILEUTILS_H
