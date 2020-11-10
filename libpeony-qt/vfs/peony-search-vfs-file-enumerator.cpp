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

#include "peony-search-vfs-file-enumerator.h"
#include "peony-search-vfs-file.h"
#include "file-enumerator.h"
#include "search-vfs-manager.h"
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QTextStream>

//G_DEFINE_TYPE(PeonySearchVFSFileEnumerator, peony_search_vfs_file_enumerator, G_TYPE_FILE_ENUMERATOR)

G_DEFINE_TYPE_WITH_PRIVATE(PeonySearchVFSFileEnumerator,
                           peony_search_vfs_file_enumerator,
                           G_TYPE_FILE_ENUMERATOR)

static void peony_search_vfs_file_enumerator_parse_uri(PeonySearchVFSFileEnumerator *enumerator,
        const char *uri);

static gboolean peony_search_vfs_file_enumerator_is_file_match(PeonySearchVFSFileEnumerator *enumerator, const QString &uri);

static void peony_search_vfs_file_enumerator_add_directory_to_queue(PeonySearchVFSFileEnumerator *enumerator, const QString &directory_uri) {
    auto queue = enumerator->priv->enumerate_queue;

    GError *err = nullptr;
    GFile *top = g_file_new_for_uri(directory_uri.toUtf8().constData());
    GFileEnumerator *e = g_file_enumerate_children(top, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                   nullptr, &err);

    if (err) {
        QString errMsg = err->message;
        g_error_free(err);
    }
    g_object_unref(top);
    if (!e)
        return;

    auto child_info = g_file_enumerator_next_file(e, nullptr, nullptr);
    while (child_info) {
        auto child = g_file_enumerator_get_child(e, child_info);
        auto uri = g_file_get_uri(child);
        *queue<<uri;
        g_free(uri);
        g_object_unref(child);
        g_object_unref(child_info);
        child_info = g_file_enumerator_next_file(e, nullptr, nullptr);
    }

    g_file_enumerator_close(e, nullptr, nullptr);
    g_object_unref(e);
}

/* -- init -- */

static void peony_search_vfs_file_enumerator_init(PeonySearchVFSFileEnumerator *self)
{
    PeonySearchVFSFileEnumeratorPrivate *priv = (PeonySearchVFSFileEnumeratorPrivate*)peony_search_vfs_file_enumerator_get_instance_private(self);
    self->priv = priv;

    self->priv->search_vfs_directory_uri = new QString;
    self->priv->enumerate_queue = new QQueue<QString>;
    self->priv->name_regexp_extend_list = new QList<QRegExp*>;
    self->priv->recursive = false;
    self->priv->save_result = false;
    self->priv->search_hidden = true;
    self->priv->use_regexp = true;
    self->priv->case_sensitive = false;
    self->priv->match_name_or_content = true;
}

static void enumerator_dispose(GObject *object);

static GFileInfo *enumerate_next_file(GFileEnumerator *enumerator,
                                      GCancellable *cancellable,
                                      GError **error);

/// async method is modified from glib source file gfileenumerator.c
static void
enumerate_next_files_async (GFileEnumerator     *enumerator,
                            int                  num_files,
                            int                  io_priority,
                            GCancellable        *cancellable,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data);

static GList* //GFileInfo*
enumerate_next_files_finished(GFileEnumerator                *enumerator,
                              GAsyncResult                   *result,
                              GError                        **error);


static gboolean enumerator_close(GFileEnumerator *enumerator,
                                 GCancellable *cancellable,
                                 GError **error);

static void peony_search_vfs_file_enumerator_class_init (PeonySearchVFSFileEnumeratorClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GFileEnumeratorClass *enumerator_class = G_FILE_ENUMERATOR_CLASS(klass);

    gobject_class->dispose = enumerator_dispose;

    enumerator_class->next_file = enumerate_next_file;

    //async
    enumerator_class->next_files_async = enumerate_next_files_async;
    enumerator_class->next_files_finish = enumerate_next_files_finished;

    enumerator_class->close_fn = enumerator_close;
}

void enumerator_dispose(GObject *object)
{
    PeonySearchVFSFileEnumerator *self = PEONY_SEARCH_VFS_FILE_ENUMERATOR(object);

    if (self->priv->name_regexp)
        delete self->priv->name_regexp;
    if (self->priv->content_regexp)
        delete self->priv->content_regexp;
    delete self->priv->search_vfs_directory_uri;
    self->priv->enumerate_queue->clear();
    delete self->priv->enumerate_queue;
    for(int i=self->priv->name_regexp_extend_list->count()-1; i>=0; i--)
    {
        delete self->priv->name_regexp_extend_list->at(i);
    }
    delete self->priv->name_regexp_extend_list;
}

static GFileInfo *enumerate_next_file(GFileEnumerator *enumerator,
                                      GCancellable *cancellable,
                                      GError **error)
{
    auto manager = Peony::SearchVFSManager::getInstance();

    qDebug()<<"next file";
    if (cancellable) {
        if (g_cancellable_is_cancelled(cancellable)) {
            //FIXME: how to add translation here? do i have to use gettext?
            *error = g_error_new_literal(G_IO_ERROR, G_IO_ERROR_CANCELLED, "search is cancelled");
            return nullptr;
        }
    }
    auto search_enumerator = PEONY_SEARCH_VFS_FILE_ENUMERATOR(enumerator);
    auto enumerate_queue = search_enumerator->priv->enumerate_queue;

    if (manager->hasHistory(*search_enumerator->priv->search_vfs_directory_uri)) {
        while (!enumerate_queue->isEmpty()) {
            auto uri = enumerate_queue->dequeue();
            auto search_vfs_info = g_file_info_new();
            QString realUriSuffix = "real-uri:" + uri;
            g_file_info_set_name(search_vfs_info, realUriSuffix.toUtf8().constData());
            return search_vfs_info;
        }
        return nullptr;
    }

    while (!enumerate_queue->isEmpty()) {
        //BFS enumeration
        auto uri = enumerate_queue->dequeue();
        GFile *tmp = g_file_new_for_uri(uri.toUtf8().constData());
        GFileType type = g_file_query_file_type(tmp, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr);
        g_object_unref(tmp);
        bool isDir = type == G_FILE_TYPE_DIRECTORY;
        if (isDir && search_enumerator->priv->recursive) {
            if (!search_enumerator->priv->search_hidden) {
                if (!uri.contains("/.")) {
                    peony_search_vfs_file_enumerator_add_directory_to_queue(search_enumerator, uri);
                }
            } else {
                peony_search_vfs_file_enumerator_add_directory_to_queue(search_enumerator, uri);
            }
        }
        //match
        if (peony_search_vfs_file_enumerator_is_file_match(search_enumerator, uri)) {
            //return this info, and the enumerate get child will return the
            //file crosponding the real uri, due to it would be handled in
            //vfs looking up method callback in registed vfs.
            if (!search_enumerator->priv->search_hidden) {
                if (uri.contains("/.")) {
                    goto return_info;
                }
            } else {
return_info:
                auto search_vfs_info = g_file_info_new();
                QString realUriSuffix = "real-uri:" + uri;
                g_file_info_set_name(search_vfs_info, realUriSuffix.toUtf8().constData());

                if (search_enumerator->priv->save_result) {
                    auto historyResults = manager->getHistroyResults(*search_enumerator->priv->search_vfs_directory_uri);
                    //FIXME: add lock?
                    historyResults<<realUriSuffix;
                }
                return search_vfs_info;
            }
        }
    }

    return nullptr;
}

static void
next_async_op_free (GList *files)
{
    g_list_free_full (files, g_object_unref);
}

static void
next_files_thread (GTask        *task,
                   gpointer      source_object,
                   gpointer      task_data,
                   GCancellable *cancellable)
{
    auto enumerator = G_FILE_ENUMERATOR(source_object);
    int num_files = GPOINTER_TO_INT (task_data);
    GFileEnumeratorClass *c;
    GList *files = NULL;
    GError *error = NULL;
    GFileInfo *info;
    int i;

    c = G_FILE_ENUMERATOR_GET_CLASS (enumerator);
    for (i = 0; i < num_files; i++)
    {
        if (g_cancellable_set_error_if_cancelled (cancellable, &error))
            info = NULL;
        else
            info = c->next_file (enumerator, cancellable, &error);

        if (info == NULL)
        {
            break;
        }
        else
            files = g_list_prepend (files, info);
    }

    if (error)
        g_task_return_error (task, error);
    else
        g_task_return_pointer (task, files, (GDestroyNotify)next_async_op_free);
}

static void
enumerate_next_files_async (GFileEnumerator     *enumerator,
                            int                  num_files,
                            int                  io_priority,
                            GCancellable        *cancellable,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data)
{
    GTask *task;
    task = g_task_new (enumerator, cancellable, callback, user_data);
    g_task_set_source_tag (task, (gpointer)enumerate_next_files_async);
    g_task_set_task_data (task, GINT_TO_POINTER (num_files), NULL);
    g_task_set_priority (task, io_priority);

    g_task_run_in_thread (task, next_files_thread);
    g_object_unref (task);
}

static GList* //GFileInfo*
enumerate_next_files_finished(GFileEnumerator                *enumerator,
                              GAsyncResult                   *result,
                              GError                        **error)
{
    g_return_val_if_fail (g_task_is_valid (result, enumerator), NULL);

    return (GList*)g_task_propagate_pointer (G_TASK (result), error);
}

static gboolean enumerator_close(GFileEnumerator *enumerator,
                                 GCancellable *cancellable,
                                 GError **error)
{
    PeonySearchVFSFileEnumerator *self = PEONY_SEARCH_VFS_FILE_ENUMERATOR(enumerator);

    return true;
}

gboolean peony_search_vfs_file_enumerator_is_file_match(PeonySearchVFSFileEnumerator *enumerator, const QString &uri)
{
    PeonySearchVFSFileEnumeratorPrivate *details = enumerator->priv;
    if (!details->name_regexp && !details->content_regexp
            && details->name_regexp_extend_list->count() == 0)
        return false;

    GFile *file = g_file_new_for_uri(uri.toUtf8().constData());
    GFileInfo *info = g_file_query_info(file,
                                        G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                                        G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                        nullptr,
                                        nullptr);
    g_object_unref(file);

    char *file_display_name = g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME);
    g_object_unref(info);
    QString displayName = file_display_name;
    g_free(file_display_name);
    if (details->name_regexp) {
        if (details->use_regexp && details->match_name_or_content
                && displayName.contains(*enumerator->priv->name_regexp))
        {
            return true;
        }
        else if (displayName == details->name_regexp->pattern())
        {
            //this is most used for querying files which might be duplicate.
            return true;
        }
    }

    //extend list name match
    if (details->name_regexp_extend_list->count() >0)
    {
        for(int i=0; i<details->name_regexp_extend_list->count(); i++)
        {
            auto curRegexp = details->name_regexp_extend_list->at(i);
            //qDebug() <<"curRegexp:" <<*curRegexp<<i;
            if (! curRegexp)
                continue;
            if (details->use_regexp && details->match_name_or_content
                    && displayName.contains(*curRegexp))
            {
                return true;
            }
            else if (displayName == curRegexp->pattern())
            {
                //this is most used for querying files which might be duplicate.
                return true;
            }
        }
    }

    if (details->content_regexp) {
        //read file stream
        QUrl url = uri;
        QFile file(url.path());
        file.open(QIODevice::Text | QIODevice::ReadOnly);
        QTextStream stream(&file);
        bool content_matched = false;
        QString line;
        line = stream.readLine();
        while (!line.isNull()) {
            if (line.contains(*enumerator->priv->content_regexp)) {
                content_matched = true;
                break;
            }
            line = stream.readLine();
        }
        file.close();

        if (content_matched) {
            if (enumerator->priv->match_name_or_content) {
                return true;
            }
        }
    }

    //this may never happend.
    return false;
}
