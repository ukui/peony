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

#ifndef FILEOPERATIONUTILS_H
#define FILEOPERATIONUTILS_H

#include <QStringList>
#include <memory>
#include "create-template-operation.h"

namespace Peony {

class FileInfo;

/*!
 * \brief The FileOperationUtils class
 * This is a class only provide static method for file operation.
 */
class FileOperationUtils
{
public:
    static void move(const QStringList &srcUris, const QString &destUri, bool addHistory);
    static void copy(const QStringList &srcUris, const QString &destUri, bool addHistory);
    static void trash(const QStringList &uris, bool addHistory);
    static void remove(const QStringList &uris);
    static void rename(const QString &uri, const QString &newName, bool addHistory);
    static void link(const QString &srcUri, const QString &destUri, bool addHistory);
    static void restore(const QString &uriInTrash);
    static void restore(const QStringList &urisInTrash);
    static void create(const QString &destDirUri, const QString &name = nullptr, CreateTemplateOperation::Type type = CreateTemplateOperation::EmptyFile);

    static void executeRemoveActionWithDialog(const QStringList &uris);

    /*!
     * \brief queryFileInfo
     * \param uri
     * \return
     * \retval the file's latest info at current time.
     * \details
     * I provide a method for force querying a file's info.
     * \note
     * This info might be hold by other class instance yet,
     * that means other object share this info can recive the info's
     * updated signal when the file's info is updated.
     */
    static std::shared_ptr<FileInfo> queryFileInfo(const QString &uri);
private:
    FileOperationUtils();
};

}

#endif // FILEOPERATIONUTILS_H
