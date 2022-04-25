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
#include "audio-play-manager.h"

#include "properties-window.h"

#include <QVector4D>

#include <QDebug>
#include <unistd.h>

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

    //
    connect(m_progressbar, &FileOperationProgressBar::canceled, [=] () {
        m_progressbar->removeAllProgressbar();
    });

    // 休眠检测
    GDBusConnection* pconnection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, NULL);
    if (pconnection) {
        g_dbus_connection_signal_subscribe(pconnection, "org.freedesktop.login1", "org.freedesktop.login1.Manager", "PrepareForSleep", "/org/freedesktop/login1", NULL, G_DBUS_SIGNAL_FLAGS_NONE, systemSleep, this, NULL);
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

bool FileOperationManager::isFileOccupied(const QString &sourceUri)
{
    QProcess process;
    QString cmd = QString("lsof %1").arg(sourceUri); /* 例如：lsof /home/kylin/test.wps  */
    process.start(cmd);
    process.waitForFinished();
    QString infos = QString(process.readAll());
    if(infos.isEmpty())
        return false;

    qDebug()<<infos;
    QStringList infoList = infos.split("\n");
    for(auto& info : infoList){
        QString procName = QString(info).split(" ").at(0);
        if("wps" == procName || "et" == procName || "wpp" == procName){/* 表示被wps占用 */
            return true;
        }
    }

    return false;
}

void FileOperationManager::startOperation(FileOperation *operation, bool addToHistory)
{    
    auto operationInfo = operation->getOperationInfo();
    /* 文件被占用时处理流程 */
    QString occupiedFiles;
    QStringList uriList = operationInfo.get()->sources();
    for(auto& uri :uriList){
        std::shared_ptr<FileInfo> fileinfo = FileInfo::fromUri(uri);
        if(!uri.endsWith(".txt") && !(fileinfo && fileinfo->type().startsWith("application/wps-office")))
            continue;

        auto operationType = operationInfo->operationType();
        QString absolutePath = FileUtils::urlDecode(uri).remove("file://");/* 获取uri的绝对路径 */
        if ((operationType == FileOperationInfo::Trash
             || operationType == FileOperationInfo::Delete
             || operationType == FileOperationInfo::Move
             ||operationType == FileOperationInfo::Copy && operation->isCopyMove())/* 鼠标拖动文件情形 */
                &&isFileOccupied(absolutePath))
        {
            operationInfo.get()->m_src_uris.removeOne(uri);
            operation->m_src_uris.removeOne(uri);
            occupiedFiles.append(FileUtils::urlDecode(uri)).append(" ");
        }
        if(operationType == FileOperationInfo::Rename && isFileOccupied(absolutePath)){
            QMessageBox::warning(nullptr, tr("Warn"), tr("'%1' is occupied，you cannot operate!").arg(FileUtils::urlDecode(uri)));
            return;
        }
    }
    if(!occupiedFiles.isEmpty()){
        QMessageBox::warning(nullptr, tr("Warn"), tr("'%1' is occupied，you cannot operate!").arg(occupiedFiles));
    }
    if(!operationInfo.get()->sources().count())
        return;
    //end

    if (operationInfo.get()->operationType() == FileOperationInfo::Trash) {
        auto value = GlobalSettings::getInstance()->getValue("showTrashDialog");
        if (value.isValid()) {
            if (value.toBool() == false) {
                goto start;
            }
        }

        // check dialog
        QMessageBox questionBox;
        questionBox.addButton(QMessageBox::Yes);
        questionBox.addButton(QMessageBox::No);
        questionBox.addButton(tr("No, go to settings"), QMessageBox::ActionRole);
        questionBox.setText(tr("Do you want to put selected %1 item(s) into trash?").arg(operationInfo.get()->sources().count()));
        auto result = questionBox.exec();

        if (result != QMessageBox::Yes) {
            if (result != QMessageBox::No) {
                // settings
                QStringList uris;
                uris<<"trash:///";
                auto propertyWindow = new PropertiesWindow(uris);
                propertyWindow->show();
            }
            return;
        }
    }

    // do not add move operation between favorite:///
    // FIXME: due to some desgin issues, we can not
    // support undo/redo with favorite:/// yet. that
    // should be fixed in the future.

    // dnd use copy move mode here between different
    // file system, so we should better check operation class,
    // not operation info type.
    if (dynamic_cast<FileMoveOperation *>(operation)) {
        if (addToHistory) {
            for (QString uri : operationInfo.get()->sources()) {
                if (uri.startsWith("favorite:///")) {
                    addToHistory = false;
                    break;
                }
            }
            if (addToHistory) {
                for (QString uri : operationInfo.get()->dests()) {
                    if (uri.startsWith("favorite:///")) {
                        addToHistory = false;
                        break;
                    }
                }
            }
        }
    }

start:

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
                        Peony::AudioPlayManager::getInstance()->playWarningAudio();
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
   ProgressBar* proc = m_progressbar->addFileOperation();
   if (nullptr == proc) {
       qDebug() << "malloc error!";
       return;
   }

   // begin
   proc->connect(operation, &FileOperation::operationPreparedOne, proc, &ProgressBar::onElementFoundOne);
   proc->connect(operation, &FileOperation::operationPrepared, proc, &ProgressBar::onElementFoundAll);
   proc->connect(operation, &FileOperation::operationProgressedOne, proc, &ProgressBar::onFileOperationProgressedOne);
   proc->connect(operation, &FileOperation::FileProgressCallback, proc, &ProgressBar::updateProgress);
   proc->connect(operation, &FileOperation::operationProgressed, proc, &ProgressBar::onFileOperationProgressedAll);
   proc->connect(operation, &FileOperation::operationAfterProgressedOne, proc, &ProgressBar::onElementClearOne);
   proc->connect(operation, &FileOperation::operationAfterProgressed, proc, &ProgressBar::switchToRollbackPage);
   proc->connect(operation, &FileOperation::operationStartRollbacked, proc, &ProgressBar::switchToRollbackPage);
   proc->connect(operation, &FileOperation::operationRollbackedOne, proc, &ProgressBar::onFileRollbacked);
   proc->connect(operation, &FileOperation::operationStartSnyc, proc, &ProgressBar::onStartSync);
   proc->connect(operation, &FileOperation::operationFinished, proc, &ProgressBar::onFinished);
   proc->connect(proc, &ProgressBar::cancelled, operation, &FileOperation::cancel);
   proc->connect(proc, &ProgressBar::cancelled, operation, &FileOperation::operationCancel);
   proc->connect(proc, &ProgressBar::pause, operation, &FileOperation::operationPause);
   proc->connect(proc, &ProgressBar::resume, operation, &FileOperation::operationResume);

   operation->connect(operation, &FileOperation::errored, [=]() {
       operation->setHasError(true);
   });

   operation->connect(operation, &FileOperation::errored, this, &FileOperationManager::handleError, Qt::BlockingQueuedConnection);
   operation->connect(operation, &FileOperation::operationFinished, this, [=](){
       Q_EMIT this->operationFinished(operation->getOperationInfo(), !operation->hasError());
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

   // fix #92481
   if (auto trashOp = qobject_cast<FileTrashOperation *>(operation)) {
       connect(trashOp, &FileTrashOperation::deleteRequest, this, [=](const QStringList &srcUris){
           auto deleteOperation = new FileDeleteOperation(srcUris);
           // wait trash operation finished. otherwise manager will asume
           // the operation is invalid due to deletion checkment.
           QTimer::singleShot(1000, this, [=]{
               startOperation(deleteOperation, true);
           });
       }, Qt::BlockingQueuedConnection);
   }

    if (!allowParallel) {
        if (m_thread_pool->activeThreadCount() > 0) {
            QMessageBox::warning(nullptr,
                                 tr("File Operation is Busy"),
                                 tr("There have been one or more file"
                                    "operation(s) executing before. Your"
                                    "operation will wait for executing"
                                    "until it/them done. If you really "
                                    "want to execute file operations "
                                    "parallelly anyway, you can change "
                                    "the default option \"Allow Parallel\" "
                                    "in option menu."));
        }
        operation->setParent(m_thread_pool);
        m_thread_pool->start(operation);
    } else {
        operation->setParent(m_thread_pool);
        m_thread_pool->start(operation);
    }

    Q_EMIT this->operationStarted(operation->getOperationInfo());

    m_progressbar->showDelay();
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
        op = new FileLinkOperation(info->m_src_uris.at(0), info->m_dest_dir_uri);
        break;
    }
    case FileOperationInfo::Move: {
        op = new FileMoveOperation(info->m_src_uris, info->m_dest_dir_uri);
        auto moveOp = qobject_cast<FileMoveOperation *>(op);
        moveOp->setAction(info->m_drop_action);
        break;
    }
    case FileOperationInfo::Rename: {
        if (info->m_node_map.isEmpty()) {
            op = new FileRenameOperation(info->m_src_uris.isEmpty()? nullptr: info->m_src_uris.at(0),
                                         info->m_dest_dir_uri);
        } else {
            op = new FileRenameOperation(info->m_node_map.firstKey(), info.get()->m_newname);
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

// optimize: Gets Windows should be created conditionally and errors handled so that memory is allocated in the stack space
void FileOperationManager::handleError(FileOperationError &error)
{
    // Empty files in the recycle bin without reminding
    if (error.srcUri.startsWith("trash://")
        && FileOpDelete == error.op)
    {
        error.respCode = IgnoreAll;
        return;
    }

    // Handle errors according to the error type
    FileOperationErrorHandler* handle = FileOperationErrorDialogFactory::getDialog(error);
    if (nullptr != handle) {
        if (m_progressbar->isHidden()) {
            m_progressbar->m_error = true;
        }
#if 0
        auto dialog = dynamic_cast<QWidget *>(handle);
        if (dialog) {
            dialog->setProperty("useCustomShadow", true);
            dialog->setProperty("customShadowDarkness", 0.5);
            dialog->setProperty("customShadowWidth", 30);
            dialog->setProperty("customShadowRadius", QVector4D(1, 1, 1, 1));
            dialog->setProperty("customShadowMargins", QVector4D(30, 30, 30, 30));
        }
#endif
        handle->handle(error);
        delete handle;
        m_progressbar->m_error = false;
        m_progressbar->showDelay(300);
    }
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
    if (!info) {
        return;
    }

    // skip create template opeartion, it will be handled by operation itself.
    if (info->m_src_dir_uri == QStandardPaths::writableLocation(QStandardPaths::TempLocation)) {
        return;
    }

    for (auto watcher : m_watchers) {
        if (!watcher->supportMonitor()) {
            auto srcDir = info->m_src_dir_uri;
            auto destDir = info->m_dest_dir_uri;
            auto firstUri = info->m_src_uris.first();
            
            //'file:///run/user/1000/gvfs/smb-share:server=xxx,share=xxx/' converted to 'smb://xxx'
            GFile * file  = g_file_new_for_uri(destDir.toLatin1().data());
            char *uri = g_file_get_uri(file);	
            if (uri) {
                destDir = uri;
            }
            g_object_unref(file);
            g_free(uri);
            
            if (info->operationType() == FileOperationInfo::Link || info->operationType() == FileOperationInfo::Rename) {
                srcDir = FileUtils::getParentUri(firstUri);
            }

            if ("" == srcDir) {
                if (firstUri.endsWith("/")) {
                    firstUri.chop(1);
                }

                QStringList fileSplit = firstUri.split("/");
                fileSplit.pop_back();

                srcDir = fileSplit.join("/");
            }

            // check watcher directory
            // srcDir is null in samba filesystem, so that it not work
            // currentUri maybe is 'file:///run/user/1000/gvfs/smb-share:server=xxx,share=xxx/' or 'smb://xxx'
            if (watcher->currentUri() == srcDir || watcher->currentUri() == destDir) {
                // tell the view/model the directory should be updated
                watcher->requestUpdateDirectory();
            }

            qDebug() << "src:" << srcDir << " == dest:" << destDir << " == type:" << info->operationType();

            if (srcDir.startsWith("smb://")
                 || srcDir.startsWith("ftp://")
                 || srcDir.startsWith("sftp://")
                 || destDir.startsWith("smb://")
                 || destDir.startsWith("ftp://")
                 || destDir.startsWith("sftp://")) {
                watcher->requestUpdateDirectory();
            }
        }
    }
}

//FIXME: get opposite info correcty.
FileOperationInfo::FileOperationInfo(QStringList srcUris,
                                     QString destDirUri,
                                     Type type,
                                     QObject *parent): QObject(parent)
{
    m_src_uris = srcUris;
    m_dest_dir_uri = destDirUri;

    oppositeInfoConstruct(type);
}

//FIXME: get opposite info correcty.
FileOperationInfo::FileOperationInfo(QStringList srcUris,
                                     QStringList destDirUris,
                                     Type type,
                                     QObject *parent): QObject(parent)
{
    m_src_uris = srcUris;
    m_dest_dir_uris = destDirUris;

    oppositeInfoConstruct(type);
}

void FileOperationInfo::oppositeInfoConstruct(Type type)
{
    m_type = type;

    switch (type) {
        case Move: {
            m_opposite_type = Move;
            commonOppositeInfoConstruct();
            break;
        }
        case Trash: {
            m_opposite_type = Untrash;
            trashOppositeInfoConstruct();
            break;
        }
        case Untrash: {
            m_opposite_type = Trash;
            UntrashOppositeInfoConstruct();
            break;
        }
        case Delete: {
            m_opposite_type = Other;
            break;
        }
        case Copy: {
            m_opposite_type = Delete;
            commonOppositeInfoConstruct();
            break;
        }
        case Rename: {
            m_opposite_type = Rename;
            RenameOppositeInfoConstruct();
            break;
        }
        case Link: {
            m_opposite_type = Delete;
            LinkOppositeInfoConstruct();
            break;
        }
        case CreateTxt: {
            m_opposite_type = Delete;
            commonOppositeInfoConstruct();
            break;
        }
        case CreateFolder: {
            m_opposite_type = Delete;
            commonOppositeInfoConstruct();
            break;
        }
        case CreateTemplate: {
            m_opposite_type = Delete;
            commonOppositeInfoConstruct();
            break;
        }
        default: {
            m_opposite_type = Other;
        }
    }

    return;
}

void FileOperationInfo::commonOppositeInfoConstruct()
{
    for (auto srcUri : m_src_uris) {
        auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
        if (m_src_dir_uri.isNull()) {
            auto srcParent = FileUtils::getFileParent(srcFile);
            m_src_dir_uri = FileUtils::getFileUri(srcParent);
        }
        QString relativePath = FileUtils::getFileBaseName(srcFile);
        auto destDirFile = wrapGFile(g_file_new_for_uri(m_dest_dir_uri.toUtf8().constData()));
        auto destFile = FileUtils::resolveRelativePath(destDirFile, relativePath);
        QString destUri = FileUtils::getFileUri(destFile);
        m_dest_uris<<destUri;
    }
}
void FileOperationInfo::LinkOppositeInfoConstruct()
{
    QUrl url = m_src_uris.first();
    if (url.fileName().startsWith(".")) {
        m_dest_uris<<m_dest_dir_uri + "/" + url.fileName() + " - " + tr("Symbolic Link");
    } else {
        auto dest_uri = m_dest_dir_uri + "/" + tr("Symbolic Link") + " - " + url.fileName();
        m_dest_uris<<dest_uri;
    }
}
void FileOperationInfo::RenameOppositeInfoConstruct()
{
    //Rename also use the common args format.
    QString src = m_src_uris.at(0);
    m_dest_uris<<src;
    m_src_dir_uri = m_dest_dir_uri;
}
void FileOperationInfo::UntrashOppositeInfoConstruct()
{
    m_dest_uris = m_dest_dir_uris;
    m_src_dir_uri = "trash:///";
    return;
}

void FileOperationInfo::trashOppositeInfoConstruct()
{
    // note that this function is unreliable.
    // the info would be updated while FileTrashOperation::run()
    // again.
    commonOppositeInfoConstruct();
}

std::shared_ptr<FileOperationInfo> FileOperationInfo::getOppositeInfo(FileOperationInfo *info) {

    auto oppositeInfo = std::make_shared<FileOperationInfo>(info->m_dest_uris, info->m_src_dir_uri, m_opposite_type);
    if (info->m_drop_action == Qt::TargetMoveAction) {
        oppositeInfo->m_drop_action = Qt::TargetMoveAction;
        oppositeInfo->m_type = FileOperationInfo::Move;
    }
    QMap<QString, QString> oppsiteMap;
    for (auto key : m_node_map.keys()) {
        auto value = m_node_map.value(key);
        oppsiteMap.insert(value, key);
    }
    oppositeInfo->m_node_map = oppsiteMap;
    oppositeInfo->m_newname = this->m_oldname;
    oppositeInfo->m_oldname = this->m_newname;

    return oppositeInfo;
}

// S3/S4
void FileOperationManager::systemSleep (GDBusConnection *connection, const gchar *senderName, const gchar *objectPath, const gchar *interfaceName, const gchar *signalName, GVariant *parameters, gpointer udata)
{
    FileOperationProgressBar* pb = static_cast<FileOperationManager*>(udata)->m_progressbar;
    if (pb) {
        Q_EMIT pb->pause();
    }

    Q_UNUSED(connection)
    Q_UNUSED(senderName)
    Q_UNUSED(objectPath)
    Q_UNUSED(signalName)
    Q_UNUSED(parameters)
    Q_UNUSED(interfaceName)
}
