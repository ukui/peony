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

#include "file-operation-utils.h"
#include "file-operation-manager.h"

#include "file-move-operation.h"
#include "file-copy-operation.h"
#include "file-trash-operation.h"
#include "file-rename-operation.h"
#include "file-delete-operation.h"
#include "file-link-operation.h"

#include "file-untrash-operation.h"

#include "file-info-job.h"
#include "file-info.h"

#include <QMessageBox>

using namespace Peony;

FileOperationUtils::FileOperationUtils()
{

}

void FileOperationUtils::move(const QStringList &srcUris, const QString &destUri, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto moveOp = new FileMoveOperation(srcUris, destUri);
    fileOpMgr->startOperation(moveOp, addHistory);
}

void FileOperationUtils::copy(const QStringList &srcUris, const QString &destUri, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto copyOp = new FileCopyOperation(srcUris, destUri);
    fileOpMgr->startOperation(copyOp, addHistory);
}

void FileOperationUtils::trash(const QStringList &uris, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto trashOp = new FileTrashOperation(uris);
    fileOpMgr->startOperation(trashOp, addHistory);
}

void FileOperationUtils::rename(const QString &uri, const QString &newName, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto renameOp = new FileRenameOperation(uri, newName);
    fileOpMgr->startOperation(renameOp, addHistory);
}

void FileOperationUtils::remove(const QStringList &uris)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto removeOp = new FileDeleteOperation(uris);
    fileOpMgr->startOperation(removeOp);
}

void FileOperationUtils::link(const QString &srcUri, const QString &destUri, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto linkOp = new FileLinkOperation(srcUri, destUri);
    fileOpMgr->startOperation(linkOp, addHistory);
}

std::shared_ptr<FileInfo> FileOperationUtils::queryFileInfo(const QString &uri)
{
    auto info = FileInfo::fromUri(uri);
    auto job = new FileInfoJob(info);
    job->querySync();
    job->deleteLater();
    return info;
}

void FileOperationUtils::restore(const QString &uriInTrash)
{
    QStringList uris;
    uris<<uriInTrash;
    auto fileOpMgr = FileOperationManager::getInstance();
    auto untrashOp = new FileUntrashOperation(uris);
    fileOpMgr->startOperation(untrashOp, true);
}

void FileOperationUtils::restore(const QStringList &urisInTrash)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto untrashOp = new FileUntrashOperation(urisInTrash);
    //FIXME: support undo?
    fileOpMgr->startOperation(untrashOp, false);
}

void FileOperationUtils::create(const QString &destDirUri, const QString &name, CreateTemplateOperation::Type type)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto createOp = new CreateTemplateOperation(destDirUri, type, name);
    fileOpMgr->startOperation(createOp, true);
}

void FileOperationUtils::executeRemoveActionWithDialog(const QStringList &uris)
{
    if (uris.isEmpty())
        return;

    int result = 0;
    if (uris.count() == 1) {
        result = QMessageBox::question(nullptr, QObject::tr("Delete Permanently"), QObject::tr("Are you sure that you want to delete %1? "
                                                                                               "Once you start a deletion, the files deleting will never be "
                                                                                               "restored again.").arg(uris.first().split("/").last()));
    } else {
        result = QMessageBox::question(nullptr, QObject::tr("Delete Permanently"), QObject::tr("Are you sure that you want to delete these %1 files? "
                                                                                               "Once you start a deletion, the files deleting will never be "
                                                                                               "restored again.").arg(uris.count()));
    }

    if (result == QMessageBox::Yes) {
        FileOperationUtils::remove(uris);
    }
}
