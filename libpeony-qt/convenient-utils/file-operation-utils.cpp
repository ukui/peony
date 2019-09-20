#include "file-operation-utils.h"
#include "file-operation-manager.h"

#include "file-move-operation.h"
#include "file-copy-operation.h"
#include "file-trash-operation.h"
#include "file-rename-operation.h"
#include "file-delete-operation.h"
#include "file-link-operation.h"

#include "file-info-job.h"
#include "file-info.h"

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
