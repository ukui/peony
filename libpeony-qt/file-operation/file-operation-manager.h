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

#ifndef FILEOPERATIONMANAGER_H
#define FILEOPERATIONMANAGER_H

#include <QUrl>
#include <QMutex>
#include <QStack>
#include <QObject>
#include <QThreadPool>

#include "file-utils.h"
#include "file-operation.h"
#include "gerror-wrapper.h"
#include "gobject-template.h"
#include "peony-core_global.h"
#include "file-operation-progress-bar.h"
#include "file-operation-error-dialogs.h"


namespace Peony {

class FileOperationInfo;
class FileWatcher;

/*!
 * \brief The FileOperationManager class
 * \details
 * In peony, the undo/redo stack manager is bind with a directory view.
 * And in peony-qt, it is similar to peony. But there are higher level
 * api to manage these 'managers' in peony-qt.
 * Not only the undo/redo stacks' management. FileOperationManager
 * only allows up to one file operation instace to run at same times,
 * this means the operations will be queue executed.
 * FileOperationManager will provide the operation-ui and error-handler-ui
 * which are implement as defaut in peony-qt's operation frameworks.
 * \note
 * FileOperationManager is not a strong binding in peony-qt's file operation
 * framework. If not considerring the operation-safety, you can just use a
 * QThreadPool instance to start over one operations at same time.
 * In this case you also should re-implement the operation wizard interface and
 * error handler interface. For example, do nothing at wizard (just not create
 * a wizard and not connect the related signal) and let the error handler
 * always response error type as ignore.
 */
class PEONYCORESHARED_EXPORT FileOperationManager : public QObject
{
    Q_OBJECT
public:
    static FileOperationManager *getInstance();
    void close();

    void setAllowParallel(bool allow = true);
    bool isAllowParallel();

Q_SIGNALS:
    void closed();

public Q_SLOTS:
    void startOperation(FileOperation *operation, bool addToHistory = true);
    void startUndoOrRedo(std::shared_ptr<FileOperationInfo> info);
    bool canUndo();
    std::shared_ptr<FileOperationInfo> getUndoInfo();
    void undo();
    bool canRedo();
    std::shared_ptr<FileOperationInfo> getRedoInfo();
    void redo();

    void clearHistory();
    void onFilesDeleted(const QStringList &uris);

    void handleError(FileOperationError& error);

    /*!
     * \brief registerFileWatcher
     * \param watcher
     * \details
     * For some limitation of gvfs/gio, some directory doesn't
     * support monitor yet. So we have to handle the data changed
     * by ourselves.
     *
     * The main idea is, every view will hold a watcher, and watcher
     * will be register to operation manager instance.
     *
     * everytime the file operation finished, watcher which not support
     * monitor will recived a signal from operation manager. And the view
     * will response the signal as same as other directories which allow monitor
     * action.
     */
    void registerFileWatcher(FileWatcher *watcher);

    /*!
     * \brief unregisterFileWatcher
     * \param watcher
     * \details
     * when a file watcher deconstructed, it should be unregistered.
     * \see registerFileWatcher()
     */
    void unregisterFileWatcher(FileWatcher *watcher);

    /*!
     * \brief manuallyNotifyDirectoryChanged
     * \param info
     * \details
     * real action to notify directory changed for directory
     * not support monitoring.
     */
    void manuallyNotifyDirectoryChanged(FileOperationInfo *info);
private:
    explicit FileOperationManager(QObject *parent = nullptr);
    ~FileOperationManager();

private:
    QThreadPool *m_thread_pool;
    bool m_allow_parallel = false;
    QVector<FileWatcher *> m_watchers;
    bool m_is_current_operation_errored = false;
    FileOperationProgressBar *m_progressbar = nullptr;
    QStack<std::shared_ptr<FileOperationInfo>> m_undo_stack;
    QStack<std::shared_ptr<FileOperationInfo>> m_redo_stack;
};

class FileOperationInfo : public QObject
{
    Q_OBJECT
    friend class FileOperationManager;
    friend class FileOperation;
public:
    QMap<QString, QString> m_node_map;

    enum Type {
        Invalid,
        Move,//move back if no error in original moving
        Copy,//delete if no error in original copying
        Link,//delete...
        Rename,//rename
        Trash,//untrash
        Untrash,//trash
        Delete,//nothing to do
        CreateTxt,//delete
        CreateFolder,//delete
        CreateTemplate,//delete
        Other//nothing to do
    };

    //FIXME: get opposite info correcty.
    explicit FileOperationInfo(QStringList srcUris, QString destDirUri, Type type, QObject *parent = nullptr): QObject(parent) {
        m_src_uris = srcUris;
        m_dest_dir_uri = destDirUri;
        m_type = type;

        //compute opposite.
        if (type != Rename && type != Link) {
            for (auto srcUri : srcUris) {
                auto srcFile = wrapGFile(g_file_new_for_uri(srcUri.toUtf8().constData()));
                if (m_src_dir_uri.isNull()) {
                    auto srcParent = FileUtils::getFileParent(srcFile);
                    m_src_dir_uri = FileUtils::getFileUri(srcParent);
                }
                QString relativePath = FileUtils::getFileBaseName(srcFile);
                auto destDirFile = wrapGFile(g_file_new_for_uri(destDirUri.toUtf8().constData()));
                auto destFile = FileUtils::resolveRelativePath(destDirFile, relativePath);
                QString destUri = FileUtils::getFileUri(destFile);
                m_dest_uris<<destUri;
            }
        } else {
            if (type == Link) {
                QUrl url = srcUris.first();
                if (!url.fileName().contains(".")) {
                    m_dest_uris<<destDirUri + "/" + url.fileName() + tr(" - Symbolic Link");
                } else {
                    auto dest_uri = destDirUri + "/" + url.fileName();
                    dest_uri = dest_uri.insert(dest_uri.lastIndexOf('.'), tr(" - Symbolic Link"));
                    m_dest_uris<<dest_uri;
                }
            } else {
                //Rename also use the common args format.
                QString src = srcUris.at(0);
                QString dest = destDirUri;
                m_dest_uris<<src;
                m_src_dir_uri = dest;
            }
        }

        switch (type) {
        case Move: {
            m_opposite_type = Move;
            break;
        }
        case Trash: {
            m_opposite_type = Untrash;
            break;
        }
        case Untrash: {
            m_opposite_type = Trash;
            break;
        }
        case Delete: {
            m_opposite_type = Other;
            break;
        }
        case Copy: {
            m_opposite_type = Delete;
            break;
        }
        case Rename: {
            m_opposite_type = Rename;
            break;
        }
        case Link: {
            m_opposite_type = Delete;
            break;
        }
        case CreateTxt: {
            m_opposite_type = Delete;
            break;
        }
        case CreateFolder: {
            m_opposite_type = Delete;
            break;
        }
        case CreateTemplate: {
            m_opposite_type = Delete;
            break;
        }
        default: {
            m_opposite_type = Other;
        }
        }
    }

    std::shared_ptr<FileOperationInfo> getOppositeInfo(FileOperationInfo *info) {
        auto oppositeInfo = std::make_shared<FileOperationInfo>(info->m_dest_uris, info->m_src_dir_uri, m_opposite_type);
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

    Type operationType() {
        return m_type;
    }
    QStringList sources() {
        return m_src_uris;
    }
    QString target() {
        return m_dest_dir_uri;
    }

private:
    QStringList m_src_uris;
    QString m_dest_dir_uri;

    //FIXME: if files in different src dir, how to deal with it?
    QStringList m_dest_uris;
    QString m_src_dir_uri;
    //QMutex m_mutex;

    Type m_type;
    Type m_opposite_type;

    bool m_enable = true;

    //Rename
    QString m_oldname = nullptr;
    QString m_newname = nullptr;
};

}

#endif // FILEOPERATIONMANAGER_H
