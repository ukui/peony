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

#include "file-operation-manager.h"
#include "file-operation.h"
#include <QMessageBox>

#include "file-copy-operation.h"
#include "file-delete-operation.h"
#include "file-link-operation.h"
#include "file-move-operation.h"
#include "file-rename-operation.h"
#include "file-trash-operation.h"
#include "file-untrash-operation.h"

#include "file-operation-error-dialog.h"
#include "file-operation-progress-wizard.h"

using namespace Peony;

static FileOperationManager *global_instance = nullptr;

FileOperationManager::FileOperationManager(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr");
    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr&");
    m_thread_pool = new QThreadPool(this);
    //Imitating queue execution.
    m_thread_pool->setMaxThreadCount(1);
}

FileOperationManager::~FileOperationManager()
{

}

FileOperationManager *FileOperationManager::getInstance()
{
    if (global_instance == nullptr) {
        global_instance = new FileOperationManager;
    }
    return global_instance;
}

void FileOperationManager::close()
{
    disconnect();
    deleteLater();
    global_instance = nullptr;
    Q_EMIT closed();
}

void FileOperationManager::startOperation(FileOperation *operation, bool addToHistory)
{
    if (m_thread_pool->activeThreadCount() > 0) {
        QMessageBox::warning(nullptr,
                             tr("File Operation is Busy"),
                             tr("There have been one or more file"
                                "operation(s) executing before. Your"
                                "operation will wait for executing"
                                "until it/them done."));
    }

    FileOperationProgressWizard *wizard = new FileOperationProgressWizard;
    wizard->setAttribute(Qt::WA_DeleteOnClose);
    wizard->connect(operation, &FileOperation::operationRequestShowWizard, wizard, &FileOperationProgressWizard::show);
    wizard->connect(operation, &FileOperation::operationRequestShowWizard, wizard, &FileOperationProgressWizard::switchToPreparedPage);
    wizard->connect(operation, &FileOperation::operationPreparedOne, wizard, &FileOperationProgressWizard::onElementFoundOne);
    wizard->connect(operation, &FileOperation::operationPrepared, wizard, &FileOperationProgressWizard::onElementFoundAll);
    wizard->connect(operation, &FileOperation::operationProgressedOne, wizard, &FileOperationProgressWizard::onFileOperationProgressedOne);
    wizard->connect(operation, &FileOperation::operationProgressed, wizard, &FileOperationProgressWizard::onFileOperationProgressedAll);
    wizard->connect(operation, &FileOperation::operationAfterProgressedOne, wizard, &FileOperationProgressWizard::onElementClearOne);
    wizard->connect(operation, &FileOperation::operationAfterProgressed, wizard, &FileOperationProgressWizard::switchToRollbackPage);
    wizard->connect(operation, &FileOperation::operationStartRollbacked, wizard, &FileOperationProgressWizard::switchToRollbackPage);
    wizard->connect(operation, &FileOperation::operationRollbackedOne, wizard, &FileOperationProgressWizard::onFileRollbacked);
    wizard->connect(operation, &FileOperation::operationFinished, wizard, &FileOperationProgressWizard::deleteLater);

    connect(wizard, &Peony::FileOperationProgressWizard::cancelled,
            operation, &Peony::FileOperation::cancel);

    operation->connect(operation, &FileOperation::errored, [=](){
        operation->setHasError(true);
    });

    operation->connect(operation, &FileOperation::errored,
                       this, &FileOperationManager::handleError,
                       Qt::BlockingQueuedConnection);

    operation->connect(operation, &FileOperation::operationFinished, [=](){
        if (operation->hasError()) {
            this->clearHistory();
            return ;
        }

        if (addToHistory) {
            auto info = operation->getOperationInfo();
            if (!info)
                return;
            if (info->operationType() != FileOperationInfo::Delete) {
                m_undo_stack.push(info);
                m_redo_stack.clear();
            } else {
                this->clearHistory();
            }
        }
    });

    m_thread_pool->start(operation);
}

void FileOperationManager::startUndoOrRedo(std::shared_ptr<FileOperationInfo> info)
{
    FileOperation *op = nullptr;
    switch (info->m_type) {
    case FileOperationInfo::Copy: {
        op = new FileCopyOperation(info->m_src_uris, info->m_dest_dir_uri);
        break;
    }
    case FileOperationInfo::Delete: {
        op = new FileDeleteOperation(info->m_src_uris);
        break;
    }
    case FileOperationInfo::Link: {
        op = new FileDeleteOperation(info->m_src_uris);
        break;
    }
    case FileOperationInfo::Move: {
        op = new FileMoveOperation(info->m_src_uris, info->m_dest_dir_uri);
        break;
    }
    case FileOperationInfo::Rename: {
        op = new FileRenameOperation(info->m_src_uris.isEmpty()? nullptr: info->m_src_uris.at(0),
                                     info->m_dest_dir_uri);
        break;
    }
    case FileOperationInfo::Trash: {
        op = new FileTrashOperation(info->m_src_uris);
        break;
    }
    case FileOperationInfo::Untrash: {
        op = new FileUntrashOperation(info->m_src_uris);
        break;
    }
    default:
        break;
    }
    //do not record the undo/redo operation to history again.
    //this had been handled at undo() and redo() yet.
    //FIXME: if an undo/redo work went error (usually won't),
    //should i remove the operation info from stack?
    if (op) {
        startOperation(op, false);
    }
}

bool FileOperationManager::canUndo()
{
    return !m_undo_stack.isEmpty();
}

bool FileOperationManager::canRedo()
{
    return !m_redo_stack.isEmpty();
}

std::shared_ptr<FileOperationInfo> FileOperationManager::getUndoInfo()
{
    return m_undo_stack.top();
}

std::shared_ptr<FileOperationInfo> FileOperationManager::getRedoInfo()
{
    return m_redo_stack.top();
}

void FileOperationManager::undo()
{
    if(!canUndo())
        return;

    auto undoInfo = m_undo_stack.pop();
    m_redo_stack.push(undoInfo);

    auto oppositeInfo = undoInfo->getOppositeInfo(undoInfo.get());
    startUndoOrRedo(oppositeInfo);
}

void FileOperationManager::redo()
{
    if (!canRedo())
        return;

    auto redoInfo = m_redo_stack.pop();
    m_undo_stack.push(redoInfo);

    startUndoOrRedo(redoInfo);
}

void FileOperationManager::clearHistory()
{
    m_undo_stack.clear();
    m_redo_stack.clear();
}

void FileOperationManager::onFilesDeleted(const QStringList &uris)
{
    qDebug()<<uris;
    //FIXME: improve the handling here of file deleted event.
    clearHistory();
}

QVariant FileOperationManager::handleError(const QString &srcUri,
                                                              const QString &destUri,
                                                              const GErrorWrapperPtr &err,
                                                              bool critical)
{
    FileOperationErrorDialog dlg;
    return dlg.handleError(srcUri, destUri, err, critical);
}
