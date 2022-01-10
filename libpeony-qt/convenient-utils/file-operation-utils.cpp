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
#include "file-count-operation.h"

#include "file-info-job.h"
#include "file-info.h"
#include "file-enumerator.h"
#include "audio-play-manager.h"

#include <QUrl>
#include <QFileInfo>
#include <gio/gio.h>

#include <QMessageBox>

#include <QStandardPaths>

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
    bool isBigFile = false;

    QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QString newBoxPath;
    QString oldBoxPath;

    //1.0保护箱路径为“/box/用户名/保护箱名称”,2.1保护箱路径为“/home/用户名/.box/保护箱名称”，只要包含这些路径就是保护箱下文件，是不可以删除到回收站的；
    //root下没有home；
    if(userPath.startsWith("/home")){
        newBoxPath = userPath+"/.box";

        QString user = userPath.remove("/home/");

        oldBoxPath = "/box/"+user;

    } else {
        newBoxPath = userPath+"/.box";
        oldBoxPath = "/box/root";
    }

    for (auto uri : uris) {
        if (!uri.startsWith("file:/") || (uri.contains(newBoxPath)) || (uri.contains(oldBoxPath))) {
            canNotTrash = true;
        }
    }

    if (!canNotTrash) {
        quint64 total_size = 0;
        const quint64 ONE_GIB_SIZE = 1024*1024*1024;
        for (auto uri : uris) {
            QUrl url(uri);
            QFile file(url.path());
            //fix iso symbolLink delete prompt can not trash issue
            //FIXME: replace BLOCKING api in ui thread. and no type.
            auto info = FileInfo::fromUri(uri);
            if (info->isSymbolLink())
                continue;

            //folder need recursive calculate file size
            //屏蔽本部分代码，文件和文件夹走一样的异常处理流程，关联bug#92483
//            if(! info->isDir()){
//                total_size += file.size();
//            }

//            //file total size more than 10G, not trash but delete, task#56444,  bug#88871, bug#88894
//            if (total_size/10 > ONE_GIB_SIZE) {
//                canNotTrash = true;
//                isBigFile = true;
//                break;
//            }
//            qDebug() <<"total_size:" <<total_size<<ONE_GIB_SIZE<<canNotTrash<<isBigFile;

            //file total size more than 10G, not trash but delete, task#56444
            //FIXME 判断是否是移动设备文件，可能不准确, 目前暂未找到好的判断方法
            bool isMobileDeviece = FileUtils::isMobileDeviceFile(uri);
            if(isMobileDeviece){
                canNotTrash = true;
                break;
            }
        }
    }

    // comment for fixing #82054
//    if (!canNotTrash) {
//        FileEnumerator e;
//        e.setEnumerateDirectory("trash:///");
//        e.enumerateSync();
//        if (e.getChildrenUris().count() > 1000) {
//            canNotTrash = true;
//        }
//    }

    if (canNotTrash) {
        Peony::AudioPlayManager::getInstance()->playWarningAudio();
        QString message = QObject::tr("Can not trash these files. "
                                      "You can delete them permanently. "
                                      "Are you sure doing that?");
        if (isBigFile)
           message = QObject::tr("Can not trash files more than 10GB, would you like to delete it permanently?");

        auto result = QMessageBox::question(nullptr, QObject::tr("Can not trash"), message);
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

        if (action == Qt::CopyAction) {
            // fix 71411
            op = new FileCopyOperation(srcUris, destDir);
        } else {
            auto moveOp = new FileMoveOperation(srcUris, destDir);
            moveOp->setAction(action);
            op = moveOp;
        }

        fileOpMgr->startOperation(op, addHistory);
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

//not accurate of process, name has tr("duplicate") not processed
//do not use this function before you fixed it
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
    //fix bug#97408,change indicator meanings
    return numl == numr? left > right: numl > numr;
}
