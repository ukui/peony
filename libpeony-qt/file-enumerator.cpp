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

#include "file-enumerator.h"
#include "file-info.h"
#include "file-info-manager.h"

#include "mount-operation.h"

#include "gerror-wrapper.h"

#include "file-utils.h"
#include "peony-search-vfs-file.h"

//play audio lib head file
#include <canberra.h>

#include <QList>
#include <QMessageBox>

#include <QDebug>
#include <QTimer>

#include <QUrl>

#ifndef PEONY_FIND_NEXT_FILES_BATCH_SIZE
#define PEONY_FIND_NEXT_FILES_BATCH_SIZE 100
#endif

using namespace Peony;

FileEnumerator::FileEnumerator(QObject *parent) : QObject(parent)
{
    m_root_file = g_file_new_for_uri("file:///");
    m_cancellable = g_cancellable_new();

    m_children_uris = new QList<QString>();

    m_cache_uris = new QStringList();

    m_idle = new QTimer(this);
    m_idle->setSingleShot(false);

    connect(this, &FileEnumerator::enumerateFinished, this, [=]() {
        if (m_auto_delete) {
            this->deleteLater();
        }
    });

    connect(this, &FileEnumerator::enumerateFinished, this, [=](){
        *m_children_uris<<*m_cache_uris;
        childrenUpdated(*m_cache_uris);
        m_cache_uris->clear();
        m_idle->stop();
    });

    connect(m_idle, &QTimer::timeout, this, [=](){
        *m_children_uris<<*m_cache_uris;
        childrenUpdated(*m_cache_uris);
        m_cache_uris->clear();
    });
}

/*!
 * \brief FileEnumerator::~FileEnumerator
 * \note
 * When a file enumerator instance in deconstructor,
 * we need consider if we need free the data of info.
 * If only global hash hold the shared data, it should.
 * <br>
 * It's just an insurance measure, we usually manage children list in other classes.
 * So, usually there's no data being released here.
 * You can notice that the use count of info should be free is 3. because temporary list,
 * list element and global hash will all hold one shared_ptr here.
 * \see FileInfo::~FileInfo()
 * </br>
 */
FileEnumerator::~FileEnumerator()
{
    g_cancellable_cancel(m_cancellable);
    disconnect();
    //qDebug()<<"~FileEnumerator";
    g_object_unref(m_root_file);
    g_object_unref(m_cancellable);

    delete m_children_uris;

    delete m_cache_uris;
}

void FileEnumerator::setEnumerateDirectory(QString uri)
{
    m_uri = uri;

    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_object_unref(m_cancellable);
    }
    m_cancellable = g_cancellable_new();

    if (m_root_file) {
        g_object_unref(m_root_file);
    }

    // TODO: implement a check for vfs extensions.

    //handle search can not use before glib 2.50
#if GLIB_CHECK_VERSION(2, 50, 0)
    m_root_file = g_file_new_for_uri(uri.toUtf8());
#else
    if (uri.startsWith("search:///"))
    {
        m_root_file = peony_search_vfs_file_new_for_uri(uri.toUtf8());
    }
    else
        m_root_file = g_file_new_for_uri(uri.toUtf8());
#endif

}

void FileEnumerator::setEnumerateDirectory(GFile *file)
{
    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_object_unref(m_cancellable);
    }
    m_cancellable = g_cancellable_new();

    if (m_root_file) {
        g_object_unref(m_root_file);
    }
    m_root_file = g_file_dup(file);

    char *uri = g_file_get_uri(m_root_file);
    if (uri) {
        m_uri = uri;
        g_free(uri);
    }
}

QString FileEnumerator::getEnumerateUri()
{
    return m_uri;
}

const QList<std::shared_ptr<FileInfo>> FileEnumerator::getChildren(bool addToHash)
{
    //m_children_uris->removeDuplicates();

    //qDebug()<<"FileEnumerator::getChildren():";
    QList<std::shared_ptr<FileInfo>> children;
    for (auto uri : *m_children_uris) {
        auto file_info = FileInfo::fromUri(uri, addToHash, false);
        children<<file_info;
    }
    return children;
}

void FileEnumerator::cancel()
{
    g_cancellable_cancel(m_cancellable);
    g_object_unref(m_cancellable);
    m_cancellable = g_cancellable_new();

    m_children_uris->clear();

    Q_EMIT enumerateFinished(false);
}

void FileEnumerator::prepare()
{
    g_file_enumerate_children_async(m_root_file,
                                    G_FILE_ATTRIBUTE_STANDARD_NAME,
                                    G_FILE_QUERY_INFO_NONE,
                                    0,
                                    m_cancellable,
                                    (GAsyncReadyCallback)prepare_aready_callback,
                                    this);
}

GFile *FileEnumerator::enumerateTargetFile()
{
    //FIXME: replace BLOCKING api in ui thread.
    GFileInfo *info = g_file_query_info(m_root_file,
                                        G_FILE_ATTRIBUTE_STANDARD_TARGET_URI,
                                        G_FILE_QUERY_INFO_NONE,
                                        nullptr,
                                        nullptr);
    char *uri = nullptr;
    uri = g_file_info_get_attribute_as_string(info,
            G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
    g_object_unref(info);

    GFile *target = nullptr;
    if (uri) {
        //qDebug()<<"enumerateTargetFile"<<uri;
        target = g_file_new_for_uri(uri);
        g_free(uri);
    } else {
        target = g_file_dup(m_root_file);
    }
    return target;
}

GAsyncReadyCallback FileEnumerator::prepare_aready_callback(GFile *file, GAsyncResult *res, FileEnumerator *p_this)
{
    GError *err = nullptr;
    GFileEnumerator *enumerator = g_file_enumerate_children_finish(file, res, &err);

    if (err) {
        if (err->code == G_IO_ERROR_CANCELLED) {
            g_error_free(err);
            return nullptr;
        }

        //do not send prepared(err) here, wait handle err finished.
        p_this->handleError(err);
        g_error_free(err);
    } else {
        //even though there is no err, we still need to wait a little while
        //for confirming other object will recieve this signal, you can aslo
        //connect prepared signal before prepared() method to confirm that.
        //Q_EMIT prepared(nullptr);
        g_object_unref(enumerator);

        QTimer::singleShot(100, p_this, [=]() {
            Q_EMIT p_this->prepared(nullptr);
        });
    }
}

void FileEnumerator::enumerateSync()
{
    m_idle->start(1000);

    GFile *target = enumerateTargetFile();

    GFileEnumerator *enumerator = g_file_enumerate_children(target,
                                  G_FILE_ATTRIBUTE_STANDARD_NAME,
                                  G_FILE_QUERY_INFO_NONE,
                                  m_cancellable,
                                  nullptr);

    if (enumerator) {
        enumerateChildren(enumerator);

        g_file_enumerator_close_async(enumerator, 0, nullptr, nullptr, nullptr);
        g_object_unref(enumerator);
    } else {
        Q_EMIT this->enumerateFinished(false);
    }

    g_object_unref(target);
}

/*!
 * \brief FileEnumerator::handleError
 * \param err
 * \bug
 * Some special vfs, such as mtp, can not be handled correctly.
 * \todo
 * Handle mtp, ptp correctly.
 */
void FileEnumerator::handleError(GError *err)
{
    qDebug()<<"handleError"<<err->code<<err->message;
    ca_context *caContext;
    ca_context_create(&caContext);
    const gchar* eventId = "dialog-warning";
    //eventid 是/usr/share/sounds音频文件名,不带后缀
    switch (err->code) {
    case G_IO_ERROR_NOT_DIRECTORY: {
        auto uri = g_file_get_uri(m_root_file);
        //FIXME: replace BLOCKING api in ui thread.
        auto targetUri = FileUtils::getTargetUri(uri);
        if (uri) {
            g_free(uri);
        }
        if (!targetUri.isEmpty()) {
            prepared(nullptr, targetUri);
            return;
        }

        bool isMountable = false;
        //FIXME: replace BLOCKING api in ui thread.
        GFileInfo *file_mount_info = g_file_query_info(m_root_file, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_MOUNT,
                                     G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);

        if (file_mount_info) {
            isMountable = g_file_info_get_attribute_boolean(file_mount_info, G_FILE_ATTRIBUTE_MOUNTABLE_CAN_MOUNT);
            g_object_unref(file_mount_info);
        }

        if (isMountable) {
            g_file_mount_mountable(m_root_file,
                                   G_MOUNT_MOUNT_NONE,
                                   nullptr,
                                   m_cancellable,
                                   GAsyncReadyCallback(mount_mountable_callback),
                                   this);
        } else {
            g_file_mount_enclosing_volume(m_root_file,
                                          G_MOUNT_MOUNT_NONE,
                                          nullptr,
                                          m_cancellable,
                                          GAsyncReadyCallback(mount_enclosing_volume_callback),
                                          this);
        }
        break;
    }
    case G_IO_ERROR_NOT_MOUNTED:
        //we first trying mount volume without mount operation,
        //because we might have saved password of target server.
        g_file_mount_enclosing_volume(m_root_file,
                                      G_MOUNT_MOUNT_NONE,
                                      nullptr,
                                      m_cancellable,
                                      GAsyncReadyCallback(mount_enclosing_volume_callback),
                                      this);
        break;
    case G_IO_ERROR_NOT_SUPPORTED:
        ca_context_play (caContext, 0,
                         CA_PROP_EVENT_ID, eventId,
                         CA_PROP_EVENT_DESCRIPTION, tr("Delete file Warning"), NULL);

        QMessageBox::critical(nullptr, tr("Error"), err->message);
        break;
    case G_IO_ERROR_PERMISSION_DENIED:
        //emit error message to upper levels to process
        Q_EMIT prepared(GErrorWrapper::wrapFrom(g_error_new(G_IO_ERROR, G_IO_ERROR_PERMISSION_DENIED, "permission denied")));
        break;
    case G_IO_ERROR_NOT_FOUND:
        Q_EMIT prepared(GErrorWrapper::wrapFrom(g_error_new(G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "file not found")));
        break;
    default:
        Q_EMIT prepared(GErrorWrapper::wrapFrom(err), nullptr, true);
        break;
    }
}

void FileEnumerator::enumerateAsync()
{
    m_idle->start(1000);

    //auto uri = g_file_get_uri(m_root_file);
    //auto path = g_file_get_path(m_root_file);
    g_file_enumerate_children_async(m_root_file,
                                    G_FILE_ATTRIBUTE_STANDARD_NAME,
                                    G_FILE_QUERY_INFO_NONE,
                                    G_PRIORITY_DEFAULT,
                                    m_cancellable,
                                    GAsyncReadyCallback(find_children_async_ready_callback),
                                    this);

}

void FileEnumerator::enumerateChildren(GFileEnumerator *enumerator)
{
    GFileInfo *info = nullptr;
    GFile *child = nullptr;
    info = g_file_enumerator_next_file(enumerator, m_cancellable, nullptr);
    if (!info) {
        Q_EMIT enumerateFinished(false);
        return;
    }
    while (info) {
        child = g_file_enumerator_get_child(enumerator, info);
        char *uri = g_file_get_uri(child);
        char *path = g_file_get_path(child);
        g_object_unref(child);

        QUrl url = QUrl(QString(uri));
        //qDebug()<<uri;
        if (path && !url.isLocalFile()) {
            QString localUri = QString("file://%1").arg(path);
            *m_children_uris<<localUri;
            g_free(path);
        } else {
            if (path) {
                g_free(path);
            }
            *m_children_uris<<uri;
        }

        g_free(uri);
        g_object_unref(info);
        info = g_file_enumerator_next_file(enumerator, m_cancellable, nullptr);
    }
    Q_EMIT enumerateFinished(true);
}

GAsyncReadyCallback FileEnumerator::mount_mountable_callback(GFile *file,
        GAsyncResult *res,
        FileEnumerator *p_this)
{
    GError *err = nullptr;
    GFile *target = g_file_mount_mountable_finish(file, res, &err);
    if (err && err->code != 0) {
        auto message = err->message;
        if (err->code == G_IO_ERROR_CANCELLED) {
            g_error_free(err);
            return nullptr;
        }
        qDebug()<<err->code<<err->message;
        if (!qobject_cast<QObject *>(p_this)) {
            g_error_free(err);
            return nullptr;
        }
        auto err_data = GErrorWrapper::wrapFrom(err);
        Q_EMIT p_this->prepared(err_data);
    } else {
        Q_EMIT p_this->prepared(nullptr);
        if (err) {
            g_error_free(err);
        }
    }
    if (target) {
        g_object_unref(target);
    }

    return nullptr;
}

GAsyncReadyCallback FileEnumerator::mount_enclosing_volume_callback(GFile *file,
        GAsyncResult *res,
        FileEnumerator *p_this)
{
    //qDebug()<<"mount_enclosing_volume_callback";
    GError *err = nullptr;
    if (g_file_mount_enclosing_volume_finish (file, res, &err)) {
        if (err) {
            if (err->code == G_IO_ERROR_CANCELLED) {
                g_error_free (err);
                return nullptr;
            }
            qDebug()<<"mount successed, err:"<<err->code<<err->message;
            Q_EMIT p_this->prepared(GErrorWrapper::wrapFrom(err), nullptr, true);
        } else {
            Q_EMIT p_this->prepared();
        }
    } else {
        if (err) {
            if (err->code == G_IO_ERROR_ALREADY_MOUNTED) {
                Q_EMIT p_this->prepared(GErrorWrapper::wrapFrom(err));
                return nullptr;
            }
            qDebug()<<"mount failed, err:"<<err->code<<err->message;
            //show the connect dialog
            if (!p_this->m_root_file) {
                //critical.
                return nullptr;
            }

            char *uri = g_file_get_uri(file);
            MountOperation *op = new MountOperation(uri);
            op->setAutoDelete();
            g_free(uri);
            //op->setAutoDelete();
            p_this->connect(op, &MountOperation::cancelled, p_this, [p_this]() {
                Q_EMIT p_this->enumerateFinished(false);
            });
            p_this->connect(op, &MountOperation::finished, p_this, [=](const std::shared_ptr<GErrorWrapper> &finished_err) {
                if (finished_err) {
                    qDebug()<<"finished err:"<<finished_err->code()<<finished_err->message();
                    if (finished_err->code() == G_IO_ERROR_PERMISSION_DENIED) {
                        p_this->enumerateFinished(false);
                        ca_context *caContext;
                        ca_context_create(&caContext);
                        const gchar* eventId = "dialog-warning";
                        //eventid 是/usr/share/sounds音频文件名,不带后缀
                        ca_context_play (caContext, 0,
                                         CA_PROP_EVENT_ID, eventId,
                                         CA_PROP_EVENT_DESCRIPTION, tr("Delete file Warning"), NULL);

                        QMessageBox::critical(nullptr, tr("Error"), finished_err->message());
                        return;
                    }
                    Q_EMIT p_this->prepared(finished_err);
                } else {
                    Q_EMIT p_this->prepared(nullptr);
                }
            });
            op->start();
        }
    }
    return nullptr;
}

GAsyncReadyCallback FileEnumerator::find_children_async_ready_callback(GFile *file,
        GAsyncResult *res,
        FileEnumerator *p_this)
{
    GError *err = nullptr;
    GFileEnumerator *enumerator = g_file_enumerate_children_finish(file, res, &err);
    if (err) {
        if (err->code == G_IO_ERROR_CANCELLED) {
            g_error_free(err);
            return nullptr;
        }
        qDebug()<<"find children async err:"<<err->code<<err->message;
        //NOTE: if the enumerator file has target uri, but target uri is not mounted,
        //it should be handled.
        //This nearly won't happend in local, but in a network server it might.
        if (err->code == G_IO_ERROR_NOT_MOUNTED) {
            g_object_unref(p_this->m_root_file);
            p_this->m_root_file = g_file_dup(file);
            p_this->prepare();
            g_error_free(err);
            return nullptr;
        }
        g_error_free(err);
    }
    if (!enumerator) {
        if (qobject_cast<QObject *>(p_this))
            Q_EMIT p_this->enumerateFinished(false);
        return nullptr;
    }
    //
    g_file_enumerator_next_files_async(enumerator,
                                       PEONY_FIND_NEXT_FILES_BATCH_SIZE,
                                       G_PRIORITY_DEFAULT,
                                       p_this->m_cancellable,
                                       GAsyncReadyCallback(enumerator_next_files_async_ready_callback),
                                       p_this);

    g_object_unref(enumerator);
    return nullptr;
}

GAsyncReadyCallback FileEnumerator::enumerator_next_files_async_ready_callback(GFileEnumerator *enumerator,
        GAsyncResult *res,
        FileEnumerator *p_this)
{
    GError *err = nullptr;
    GList *files = g_file_enumerator_next_files_finish(enumerator,
                   res,
                   &err);

    if (err) {
        if (err->code == G_IO_ERROR_CANCELLED) {
            g_error_free(err);
            return nullptr;
        }
    }

    auto errPtr = GErrorWrapper::wrapFrom(err);
    if (!files && !err) {
        //if a directory children count is same with BATCH_SIZE,
        //just send finished signal.
        qDebug()<<"no more files"<<endl<<endl<<endl;
        if (qobject_cast<QObject *>(p_this))
            Q_EMIT p_this->enumerateFinished(true);
        return nullptr;
    }
    if (!files && err) {
        //critical
        return nullptr;
    }
    if (err) {
        qDebug()<<"next_files_async:"<<err->code<<err->message;
    }

    GList *l = files;
    QStringList uriList;
    int files_count = 0;
    while (l) {
        GFileInfo *info = static_cast<GFileInfo*>(l->data);
        GFile *file = g_file_enumerator_get_child(enumerator, info);
        char *uri = g_file_get_uri(file);
        char *path = g_file_get_path(file);
        g_object_unref(file);
        //qDebug()<<uri;

        QUrl url = QUrl(QString(uri));

        if (path && !url.isLocalFile()) {
            QString localUri = QString("file://%1").arg(path);
            uriList<<localUri;
            *(p_this->m_cache_uris)<<localUri;
            g_free(path);
        } else {
            uriList<<uri;
            *(p_this->m_cache_uris)<<uri;
        }

        g_free(uri);
        files_count++;
        l = l->next;
    }
    g_list_free_full(files, g_object_unref);
    //Q_EMIT p_this->childrenUpdated(uriList);

    if (files_count == PEONY_FIND_NEXT_FILES_BATCH_SIZE) {
        //have next files, countinue.
        g_file_enumerator_next_files_async(enumerator,
                                           PEONY_FIND_NEXT_FILES_BATCH_SIZE,
                                           G_PRIORITY_DEFAULT,
                                           p_this->m_cancellable,
                                           GAsyncReadyCallback(enumerator_next_files_async_ready_callback),
                                           p_this);
    } else {
        //no next files, emit finished.
        //qDebug()<<"async enumerateFinished";
        if (qobject_cast<QObject *>(p_this))
            Q_EMIT p_this->enumerateFinished(true);
    }
    return nullptr;
}
