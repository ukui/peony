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
#include "file-enumerator.h"
#include "audio-play-manager.h"

#include <QUrl>
#include <QFileInfo>

#include <QMessageBox>

using namespace Peony;

FileOperationUtils::FileOperationUtils()
{

}

FileOperation *FileOperationUtils::move(const QStringList &srcUris, const QString &destUri, bool addHistory, bool copyMove)
{
    FileOperation *op;
    QString destDir = nullptr;
    auto fileOpMgr = FileOperationManager::getInstance();
    if (destUri != "trash:///") {
        if (true == destUri.startsWith("computer:///")) {
            destDir = FileUtils::getTargetUri(destUri);
            if (nullptr == destDir){
                qWarning()<<"get target uri failed, from uri:"
                          <<destUri;
                destDir = destUri;
            }
        }
        else {
            destDir = destUri;
        }

        auto moveOp = new FileMoveOperation(srcUris, destDir);
        moveOp->setCopyMove(copyMove);
        op = moveOp;
        fileOpMgr->startOperation(moveOp, addHistory);
    } else {
        op = FileOperationUtils::trash(srcUris, true);
    }
    return op;
}

FileOperation *FileOperationUtils::copy(const QStringList &srcUris, const QString &destUri, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto copyOp = new FileCopyOperation(srcUris, destUri);
    fileOpMgr->startOperation(copyOp, addHistory);
    return copyOp;
}

FileOperation *FileOperationUtils::trash(const QStringList &uris, bool addHistory)
{
    FileOperation *op = nullptr;
    bool canNotTrash = false;
    for (auto uri : uris) {
        if (!uri.startsWith("file:/")) {
            canNotTrash = true;
        }
    }

    if (!canNotTrash) {
        for (auto uri : uris) {
            QUrl url(uri);
            QFile file(url.path());
            //fix iso symbolLink delete prompt can not trash issue
            //FIXME: replace BLOCKING api in ui thread. and no type.
            auto info = FileInfo::fromUri(uri);
            if (info->isSymbolLink())
                continue;
            if (file.size() > 1024*1024*1024) {
                canNotTrash = true;
                break;
            }
        }
    }

    if (!canNotTrash) {
        FileEnumerator e;
        e.setEnumerateDirectory("trash:///");
        e.enumerateSync();
        if (e.getChildrenUris().count() > 1000) {
            canNotTrash = true;
        }
    }

    if (canNotTrash) {
        Peony::AudioPlayManager::getInstance()->playWarningAudio();
        auto result = QMessageBox::question(nullptr, QObject::tr("Can not trash"), QObject::tr("Can not trash these files. "
                                            "You can delete them permanently. "
                                            "Are you sure doing that?"));

        if (result == QMessageBox::Yes) {
            op = FileOperationUtils::remove(uris);
        }
        return op;
    }

    auto fileOpMgr = FileOperationManager::getInstance();
    auto trashOp = new FileTrashOperation(uris);
    fileOpMgr->startOperation(trashOp, addHistory);

    return trashOp;
}

FileOperation *FileOperationUtils::rename(const QString &uri, const QString &newName, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto renameOp = new FileRenameOperation(uri, newName);
    fileOpMgr->startOperation(renameOp, addHistory);
    return renameOp;
}

FileOperation *FileOperationUtils::remove(const QStringList &uris)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto removeOp = new FileDeleteOperation(uris);
    fileOpMgr->startOperation(removeOp);
    return removeOp;
}

FileOperation *FileOperationUtils::link(const QString &srcUri, const QString &destUri, bool addHistory)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto linkOp = new FileLinkOperation(srcUri, destUri);
    fileOpMgr->startOperation(linkOp, addHistory);
    return linkOp;
}

std::shared_ptr<FileInfo> FileOperationUtils::queryFileInfo(const QString &uri)
{
    //FIXME: replace BLOCKING api in ui thread.
    auto info = FileInfo::fromUri(uri);
    auto job = new FileInfoJob(info);
    job->querySync();
    job->deleteLater();
    return info;
}

FileOperation *FileOperationUtils::moveWithAction(const QStringList &srcUris, const QString &destUri, bool addHistory, Qt::DropAction action)
{
    FileOperation *op;
    QString destDir = nullptr;
    auto fileOpMgr = FileOperationManager::getInstance();
    if (destUri != "trash:///") {
        if (true == destUri.startsWith("computer:///")) {
            destDir = FileUtils::getTargetUri(destUri);
            if (nullptr == destDir){
                qWarning()<<"get target uri failed, from uri:"
                          <<destUri;
                destDir = destUri;
            }
        }
        else {
            destDir = destUri;
        }

        auto moveOp = new FileMoveOperation(srcUris, destDir);
        moveOp->setAction(action);

        op = moveOp;
        fileOpMgr->startOperation(moveOp, addHistory);
    } else {
        op = FileOperationUtils::trash(srcUris, true);
    }
    return op;
}

FileOperation *FileOperationUtils::restore(const QString &uriInTrash)
{
    QStringList uris;
    uris<<uriInTrash;
    auto fileOpMgr = FileOperationManager::getInstance();
    auto untrashOp = new FileUntrashOperation(uris);
    fileOpMgr->startOperation(untrashOp, true);
    return untrashOp;
}

FileOperation *FileOperationUtils::restore(const QStringList &urisInTrash)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto untrashOp = new FileUntrashOperation(urisInTrash);
    //FIXME: support undo?
    fileOpMgr->startOperation(untrashOp, false);
    return untrashOp;
}

FileOperation *FileOperationUtils::create(const QString &destDirUri, const QString &name, CreateTemplateOperation::Type type)
{
    auto fileOpMgr = FileOperationManager::getInstance();
    auto createOp = new CreateTemplateOperation(destDirUri, type, name);
    fileOpMgr->startOperation(createOp, true);
    return createOp;
}

void FileOperationUtils::executeRemoveActionWithDialog(const QStringList &uris)
{
    if (uris.isEmpty())
        return;

    Peony::AudioPlayManager::getInstance()->playWarningAudio();
    int result = 0;
    if (uris.count() == 1) {
        QUrl url = uris.first();
        result = QMessageBox::question(nullptr, QObject::tr("Delete Permanently"), QObject::tr("Are you sure that you want to delete these files? Once you start a deletion, the files deleting will never be restored again."));
    } else {
        result = QMessageBox::question(nullptr, QObject::tr("Delete Permanently"), QObject::tr("Are you sure that you want to delete these files? Once you start a deletion, the files deleting will never be restored again."));
    }

    if (result == QMessageBox::Yes) {
        FileOperationUtils::remove(uris);
    }
}


bool FileOperationUtils::leftNameIsDuplicatedFileOfRightName(const QString &left, const QString &right)
{
    auto tmpl = left;
    auto tmpr = right;
    tmpl.remove(QRegExp("\\(\\d+\\)"));
    tmpr.remove(QRegExp("\\(\\d+\\)"));
    return tmpl == tmpr;
}

static int getNumOfFileName(const QString &name)
{
    QRegExp regExp("\\(\\d+\\)");
    int num = 0;

    if (name.contains(regExp)) {
        int pos = 0;
        QString tmp;
        while ((pos = regExp.indexIn(name, pos)) != -1) {
            tmp = regExp.cap(0).toUtf8();
            pos += regExp.matchedLength();
            qDebug()<<"pos"<<pos;
        }
        tmp.remove(0,1);
        tmp.chop(1);
        num = tmp.toInt();
    }
    return num;
}

bool FileOperationUtils::leftNameLesserThanRightName(const QString &left, const QString &right)
{
    auto tmpl = left;
    auto tmpr = right;
    int numl = getNumOfFileName(tmpl);
    int numr = getNumOfFileName(tmpr);
    return numl == numr? left < right: numl < numr;
}
