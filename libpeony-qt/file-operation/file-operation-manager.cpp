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

#include "file-operation-manager.h"
#include "file-operation.h"

#include "global-settings.h"

#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <QtConcurrent>

#include "file-copy-operation.h"
#include "file-delete-operation.h"
#include "file-link-operation.h"
#include "file-move-operation.h"
#include "file-rename-operation.h"
#include "file-trash-operation.h"
#include "file-untrash-operation.h"

#include "file-operation-error-dialog.h"
#include "file-operation-progress-wizard.h"

#include "file-watcher.h"

#include <QDebug>

using namespace Peony;

static FileOperationManager *global_instance = nullptr;

FileOperationManager::FileOperationManager(QObject *parent) : QObject(parent)
{
    m_allow_parallel = GlobalSettings::getInstance()->getValue(ALLOW_FILE_OP_PARALLEL).toBool();

    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr");
    qRegisterMetaType<Peony::GErrorWrapperPtr>("Peony::GErrorWrapperPtr&");
    m_thread_pool = new QThreadPool(this);
    m_progressbar = FileOperationProgressBar::getInstance();

    if (!m_allow_parallel) {
        //Imitating queue execution.
        m_thread_pool->setMaxThreadCount(1);
    }
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

void FileOperationManager::setAllowParallel(bool allow)
{
    m_allow_parallel = allow;
    if (allow) {
        m_thread_pool->setMaxThreadCount(9999);
    } else {
        m_thread_pool->setMaxThreadCount(1);
    }
    GlobalSettings::getInstance()->setValue(ALLOW_FILE_OP_PARALLEL, allow);
}

bool FileOperationManager::isAllowParallel()
{
    return m_allow_parallel;
}

void FileOperationManager::startOperation(FileOperation *operation, bool addToHistory)
{
    QApplication::setQuitOnLastWindowClosed(false);

    connect(operation, &FileOperation::operationFinished, this, [=]() {
        operation->notifyFileWatcherOperationFinished();
        auto settings = GlobalSettings::getInstance();
        bool runbackend = settings->getInstance()->getValue(RESIDENT_IN_BACKEND).toBool();
        QApplication::setQuitOnLastWindowClosed(!runbackend);

        QTimer::singleShot(1000, this, [=]() {
            int last_op_count = m_thread_pool->children().count();
            if (last_op_count == 0) {
                if (qApp->allWidgets().isEmpty()) {
                    if (!runbackend) {
                        qApp->quit();
                    }
                }
            }
        });
    }, Qt::BlockingQueuedConnection);

    auto operationInfo = operation->getOperationInfo();

    bool allowParallel = m_allow_parallel;

    auto opType = operationInfo->operationType();
    switch (opType) {
    case FileOperationInfo::Trash:
    case FileOperationInfo::Delete: {
        allowParallel = true;
        auto operationSrcs = operationInfo->sources();
        auto currentOps = m_thread_pool->children();
        QList<FileOperation *> ops;
        for (auto child : currentOps) {
            auto op = qobject_cast<FileOperation *>(child);
            auto opInfo = op->getOperationInfo();
            {
                for (auto src : operationSrcs) {
                    if (opInfo->sources().contains(src)) {
                        //do not allow operation.
                        QMessageBox::critical(nullptr,
                                              tr("Can't delete."),
                                              tr("You can't delete a file when"
                                                 "the file is doing another operation"));
                        return;
                    }
                }
            }
        }
        break;
    }
    default:
        break;
    }

    // progress bar
   FileOperationProgress* proc = m_progressbar->addFileOperation();
   if (nullptr == proc) {
       // FIXME:// add more process bar
       QMessageBox::warning(nullptr,
                            tr("File Operation is Busy^^^"),
                            tr("There have been one or more file"
                               "operation(s) executing before. Your"
                               "operation will wait for executing"
                               "until it/them done. If you really "
                               "want to execute file operations "
                               "parallelly anyway, you can change "
                               "the default option \"Allow Parallel\" "
                               "in option menu."));
       return;
   }

   // begin
   proc->connect(operation, &FileOperation::operationPreparedOne, proc, &FileOperationProgress::onElementFoundOne);
   proc->connect(operation, &FileOperation::operationPrepared, proc, &FileOperationProgress::onElementFoundAll);
   proc->connect(operation, &FileOperation::operationProgressedOne, proc, &FileOperationProgress::onFileOperationProgressedOne);
   proc->connect(operation, &FileOperation::FileProgressCallback, proc, &FileOperationProgress::updateProgress);
   proc->connect(operation, &FileOperation::operationProgressed, proc, &FileOperationProgress::onFileOperationProgressedAll);
   proc->connect(operation, &FileOperation::operationAfterProgressedOne, proc, &FileOperationProgress::onElementClearOne);
   proc->connect(operation, &FileOperation::operationAfterProgressed, proc, &FileOperationProgress::switchToRollbackPage);
   proc->connect(operation, &FileOperation::operationStartRollbacked, proc, &FileOperationProgress::switchToRollbackPage);
   proc->connect(operation, &FileOperation::operationRollbackedOne, proc, &FileOperationProgress::onFileRollbacked);
   proc->connect(operation, &FileOperation::operationStartSnyc, proc, &FileOperationProgress::onStartSync);
   proc->connect(operation, &FileOperation::operationFinished, proc, &FileOperationProgress::onFinished);
   proc->connect(proc, &FileOperationProgress::cancelled, operation, &Peony::FileOperation::cancel);

   operation->connect(operation, &FileOperation::errored, [=]() {
       operation->setHasError(true);
   });

   operation->connect(operation, &FileOperation::errored, this, &FileOperationManager::handleError, Qt::BlockingQueuedConnection);

   operation->connect(operation, &FileOperation::operationFinished, this, [=](){
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
   }, Qt::BlockingQueuedConnection);


   m_progressbar->showProcess(*proc);

//    FileOperationProgressWizard *wizard = new FileOperationProgressWizard;
//    wizard->setAttribute(Qt::WA_DeleteOnClose);
//    wizard->connect(operation, &FileOperation::operationRequestShowWizard, wizard, &FileOperationProgressWizard::delayShow);
//    wizard->connect(operation, &FileOperation::operationRequestShowWizard, wizard, &FileOperationProgressWizard::switchToPreparedPage);
//    wizard->connect(operation, &FileOperation::operationPreparedOne, wizard, &FileOperationProgressWizard::onElementFoundOne);
//    wizard->connect(operation, &FileOperation::operationPrepared, wizard, &FileOperationProgressWizard::onElementFoundAll);
//    wizard->connect(operation, &FileOperation::operationProgressedOne, wizard, &FileOperationProgressWizard::onFileOperationProgressedOne);
//    wizard->connect(operation, &FileOperation::FileProgressCallback, wizard, &FileOperationProgressWizard::updateProgress);
//    wizard->connect(operation, &FileOperation::operationProgressed, wizard, &FileOperationProgressWizard::onFileOperationProgressedAll);
//    wizard->connect(operation, &FileOperation::operationAfterProgressedOne, wizard, &FileOperationProgressWizard::onElementClearOne);
//    wizard->connect(operation, &FileOperation::operationAfterProgressed, wizard, &FileOperationProgressWizard::switchToRollbackPage);
//    wizard->connect(operation, &FileOperation::operationStartRollbacked, wizard, &FileOperationProgressWizard::switchToRollbackPage);
//    wizard->connect(operation, &FileOperation::operationRollbackedOne, wizard, &FileOperationProgressWizard::onFileRollbacked);
//    wizard->connect(operation, &FileOperation::operationStartSnyc, wizard, &FileOperationProgressWizard::onStartSync);
//    wizard->connect(operation, &FileOperation::operationFinished, wizard, &FileOperationProgressWizard::deleteLater);

//    connect(wizard, &Peony::FileOperationProgressWizard::cancelled,
//            operation, &Peony::FileOperation::cancel);

//    operation->connect(operation, &FileOperation::errored, [=]() {
//        operation->setHasError(true);
//    });

//    operation->connect(operation, &FileOperation::errored,
//                       this, &FileOperationManager::handleError,
//                       Qt::BlockingQueuedConnection);

//    operation->connect(operation, &FileOperation::operationFinished, this, [=](){
//        if (operation->hasError()) {
//            this->clearHistory();
//            return ;
//        }

//        if (addToHistory) {
//            auto info = operation->getOperationInfo();
//            if (!info)
//                return;
//            if (info->operationType() != FileOperationInfo::Delete) {
//                m_undo_stack.push(info);
//                m_redo_stack.clear();
//            } else {
//                this->clearHistory();
//            }
//        }
//    }, Qt::BlockingQueuedConnection);

//    if (!allowParallel) {
//        if (m_thread_pool->activeThreadCount() > 0) {
//            QMessageBox::warning(nullptr,
//                                 tr("File Operation is Busy"),
//                                 tr("There have been one or more file"
//                                    "operation(s) executing before. Your"
//                                    "operation will wait for executing"
//                                    "until it/them done. If you really "
//                                    "want to execute file operations "
//                                    "parallelly anyway, you can change "
//                                    "the default option \"Allow Parallel\" "
//                                    "in option menu."));
//        }
//        operation->setParent(m_thread_pool);
//        m_thread_pool->start(operation);
//    } else {
        operation->setParent(m_thread_pool);
        m_thread_pool->start(operation);
//    }
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
        if (info->m_node_map.isEmpty())
            op = new FileDeleteOperation(info->m_src_uris);
        else
            op = new FileDeleteOperation(info->m_node_map.keys());
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
        if (info->m_node_map.isEmpty()) {
            op = new FileRenameOperation(info->m_src_uris.isEmpty()? nullptr: info->m_src_uris.at(0),
                                         info->m_dest_dir_uri);
        } else {
            auto destUri = info->m_node_map.first();
            QUrl url = destUri;
            op = new FileRenameOperation(info->m_node_map.firstKey(), url.fileName());
        }
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
    if (srcUri.startsWith("trash://") && err.get()->code() == G_IO_ERROR_PERMISSION_DENIED) {
        return QVariant(FileOperation::IgnoreOne);
    }
    FileOperationErrorDialog dlg;

    return dlg.handleError(srcUri, destUri, err, critical);
}

void FileOperationManager::registerFileWatcher(FileWatcher *watcher)
{
    m_watchers<<watcher;
}

void FileOperationManager::unregisterFileWatcher(FileWatcher *watcher)
{
    m_watchers.removeOne(watcher);
}

void FileOperationManager::manuallyNotifyDirectoryChanged(FileOperationInfo *info)
{
    if (!info)
        return;

    // skip create template opeartion, it will be handled by operation itself.
    if (info->m_src_dir_uri == QStandardPaths::writableLocation(QStandardPaths::TempLocation))
        return;

    for (auto watcher : m_watchers) {
        if (!watcher->supportMonitor()) {
            auto srcDir = info->m_src_dir_uri;
            auto destDir = info->m_dest_dir_uri;
            if (info->operationType() == FileOperationInfo::Link || info->operationType() == FileOperationInfo::Rename) {
                srcDir = FileUtils::getParentUri(info->m_src_uris.first());
            }
            // check watcher directory
            if (watcher->currentUri() == srcDir || watcher->currentUri() == destDir) {
                // tell the view/model the directory should be updated
                watcher->requestUpdateDirectory();
            }
        }
    }
}
