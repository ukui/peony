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

#include "file-watcher.h"
#include "gerror-wrapper.h"

#include "file-label-model.h"

#include <QUrl>
#include "file-utils.h"

#include <QDebug>

using namespace Peony;

FileWatcher::FileWatcher(QString uri, QObject *parent) : QObject(parent)
{
    m_uri = uri;
    m_target_uri = uri;
    m_file = g_file_new_for_uri(uri.toUtf8().constData());
    m_cancellable = g_cancellable_new();

    connect(FileLabelModel::getGlobalModel(), &FileLabelModel::fileLabelChanged, this, [=](const QString &uri){
        auto parentUri = FileUtils::getParentUri(uri);
        if (parentUri == m_uri || parentUri == m_target_uri) {
            Q_EMIT fileChanged(uri);
            qDebug()<<"file label changed"<<uri;
        }
    });

    //monitor target file if existed.
    prepare();

    GError *err1 = nullptr;
    m_monitor = g_file_monitor_file(m_file,
                                    G_FILE_MONITOR_WATCH_MOVES,
                                    m_cancellable,
                                    &err1);
    if (err1) {
        qDebug()<<err1->code<<err1->message;
        g_error_free(err1);
    }

    GError *err2 = nullptr;
    m_dir_monitor = g_file_monitor_directory(m_file,
                                             G_FILE_MONITOR_NONE,
                                             m_cancellable,
                                             &err2);
    if (err2) {
        qDebug()<<err2->code<<err2->message;
        g_error_free(err2);
    }
}

FileWatcher::~FileWatcher()
{
    disconnect();
    //qDebug()<<"~FileWatcher"<<m_uri;
    stopMonitor();
    cancel();

    g_object_unref(m_cancellable);
    g_object_unref(m_dir_monitor);
    g_object_unref(m_monitor);
    g_object_unref(m_file);
}

/*!
 * \brief FileWatcher::prepare
 * <br>
 * If file handle has target uri, we need monitor file that target uri point to.
 * FileWatcher::prepare() just query if there is a target uri of current file handle,
 * and it asumed that everything is no problem. If you want to use
 * a file watcher instance, I recommend you call a file enumerator class instance
 * with FileEnumerator::prepare() and wait it finished first.
 * </br>
 * \see FileEnumerator::prepare().
 */
void FileWatcher::prepare()
{
    GFileInfo *info = g_file_query_info(m_file,
                                        G_FILE_ATTRIBUTE_STANDARD_TARGET_URI,
                                        G_FILE_QUERY_INFO_NONE,
                                        m_cancellable,
                                        nullptr);

    char *uri = g_file_info_get_attribute_as_string(info,
                                                    G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);

    if (uri) {
        g_object_unref(m_file);
        m_file = g_file_new_for_uri(uri);
        m_target_uri = uri;
        g_free(uri);
    }

    g_object_unref(info);
}

void FileWatcher::cancel()
{
    g_cancellable_cancel(m_cancellable);
    g_object_unref(m_cancellable);
    m_cancellable = g_cancellable_new();
}

void FileWatcher::startMonitor()
{
    //make sure only connect once in a watcher.
    stopMonitor();
    m_file_handle = g_signal_connect(m_monitor, "changed", G_CALLBACK(file_changed_callback), this);
    m_dir_handle = g_signal_connect(m_dir_monitor, "changed", G_CALLBACK(dir_changed_callback), this);
}

void FileWatcher::stopMonitor()
{
    if (m_file_handle > 0) {
        g_signal_handler_disconnect(m_monitor, m_file_handle);
        m_file_handle = 0;
    }
    if (m_dir_handle > 0) {
        g_signal_handler_disconnect(m_dir_monitor, m_dir_handle);
        m_dir_handle = 0;
    }
}

void FileWatcher::changeMonitorUri(QString uri)
{
    QString oldUri = m_uri;

    stopMonitor();
    cancel();

    m_uri = uri;
    m_target_uri = uri;
    g_object_unref(m_file);
    g_object_unref(m_monitor);
    g_object_unref(m_dir_monitor);

    m_file = g_file_new_for_uri(uri.toUtf8().constData());

    prepare();

    GError *err1 = nullptr;
    m_monitor = g_file_monitor_file(m_file,
                                    G_FILE_MONITOR_WATCH_MOVES,
                                    m_cancellable,
                                    &err1);
    if (err1) {
        m_supprot_monitor = true;
        qDebug()<<err1->code<<err1->message;
        g_error_free(err1);
    }

    GError *err2 = nullptr;
    m_dir_monitor = g_file_monitor_directory(m_file,
                                             G_FILE_MONITOR_NONE,
                                             m_cancellable,
                                             &err2);
    if (err2) {
        m_supprot_monitor = true;
        qDebug()<<err2->code<<err2->message;
        g_error_free(err2);
    }

    startMonitor();

    Q_EMIT locationChanged(oldUri, m_uri);
}

void FileWatcher::file_changed_callback(GFileMonitor *monitor,
                                        GFile *file,
                                        GFile *other_file,
                                        GFileMonitorEvent event_type,
                                        FileWatcher *p_this)
{
    //qDebug()<<"file_changed_callback"<<event_type;
    //FIXME: when a volume unmounted, the delete signal
    //will be sent, but the volume may not be deleted (in computer:///).
    //I need deal with this case.
    Q_UNUSED(monitor);
    Q_UNUSED(file);
    switch (event_type) {
    case G_FILE_MONITOR_EVENT_MOVED_IN:
    case G_FILE_MONITOR_EVENT_MOVED_OUT:
    case G_FILE_MONITOR_EVENT_RENAMED: {
        char *new_uri = g_file_get_uri(other_file);
        QString uri = new_uri;
        QUrl url =  uri;
        uri = url.toDisplayString();
        g_free(new_uri);
        p_this->changeMonitorUri(uri);
        break;
    }
    case G_FILE_MONITOR_EVENT_DELETED: {
        p_this->stopMonitor();
        p_this->cancel();
        //qDebug()<<p_this->m_target_uri;
        Q_EMIT p_this->directoryDeleted(p_this->m_target_uri);
        break;
    }
    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED: {
        char *uri = g_file_get_uri(file);
        qDebug()<<uri;
        Q_EMIT p_this->fileChanged(uri);
        g_free(uri);
        break;
    }
    default:
        break;
    }
}

void FileWatcher::dir_changed_callback(GFileMonitor *monitor,
                                       GFile *file,
                                       GFile *other_file,
                                       GFileMonitorEvent event_type,
                                       FileWatcher *p_this)
{
    //qDebug()<<"dir_changed_callback";
    Q_UNUSED(monitor);
    Q_UNUSED(other_file);
    switch (event_type) {
    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
    case G_FILE_MONITOR_EVENT_CHANGED: {
        if (p_this->m_montor_children_change) {
            char *uri = g_file_get_uri(file);
            QString changedFileUri = uri;
            //qDebug()<<uri;
            QUrl url = changedFileUri;
            changedFileUri = url.toDisplayString();
            g_free(uri);
            Q_EMIT p_this->fileChanged(changedFileUri);
        }
        break;
    }
    case G_FILE_MONITOR_EVENT_CREATED: {
        char *uri = g_file_get_uri(file);
        QString createdFileUri = uri;
        QUrl url = createdFileUri;
        createdFileUri = url.toDisplayString();
        g_free(uri);
        Q_EMIT p_this->fileCreated(createdFileUri);
        break;
    }
    case G_FILE_MONITOR_EVENT_DELETED: {
        char *uri = g_file_get_uri(file);
        QString deletedFileUri = uri;
        QUrl url = deletedFileUri;
        deletedFileUri = url.toDisplayString();
        g_free(uri);
        Q_EMIT p_this->fileDeleted(deletedFileUri);
        break;
    }
    case G_FILE_MONITOR_EVENT_UNMOUNTED: {
        char *uri = g_file_get_uri(file);
        QString deletedFileUri = uri;
        QUrl url = deletedFileUri;
        deletedFileUri = url.toDisplayString();
        g_free(uri);
        Q_EMIT p_this->directoryUnmounted(deletedFileUri);
        break;
    }
    default:
        break;
    }
}
