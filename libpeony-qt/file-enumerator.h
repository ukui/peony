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

#ifndef FILEENUMERATOR_H
#define FILEENUMERATOR_H

#include <QObject>
#include "peony-core_global.h"

#include <memory>
#include <gio/gio.h>

namespace Peony {

class FileInfo;
class GErrorWrapper;

/*!
 * \brief The FileEnumerator class
 * <br>
 * FileEnumerator is a file enumeration class with its own exception handling.
 * This class provides a unified interface for accessing various uri path subfiles.
 * It also supports pre-processing for some uri paths that require special processing
 * and provides interaction when needed.
 * The essence of this class is a wrapper of GFileEnumerator.
 * </br>
 */
class PEONYCORESHARED_EXPORT FileEnumerator : public QObject
{
    Q_OBJECT
public:
    explicit FileEnumerator(QObject *parent = nullptr);
    ~FileEnumerator();
    void setEnumerateDirectory(QString uri);
    void setEnumerateDirectory(GFile *file);
    /*!
     * \brief prepare
     * <br>
     * prepare
     * This is an async method that do some pre-handling before
     * we really start an enumerating. Ofcourse,
     * you can enumerate a file without any preparing, but
     * I recommend do something preparing before we enumerate a directory.
     * For some special uri, such as drive in computer or remote sftp, we
     * can not directly enumerate their children. It is best to wait async
     * of prepare done, then we can enumerate the file, or get something
     * error messages. we should connect prepared() signal for async.
     * </br>
     * \see prepared().
     */
    void prepare();
    /*!
     * \brief enumerateSync
     * <br>
     * Enumerate children of a path, blocking i/o.
     * </br>
     * \note It might not enum children successfully for some error.
     * if you want to
     * \see enumerateChildren().
     */
    void enumerateSync();

    /*!
     * \brief getChildren
     * \return
     */
    const QList<std::shared_ptr<FileInfo>> getChildren(bool addToHash = false);

    const QStringList getChildrenUris() {return *m_children_uris;}

    void setAutoDelete(bool autoDelete = true) {m_auto_delete = true;}

Q_SIGNALS:
    /*!
     * \brief prepared
     * \param err
     * <br>
     * We often start an enumerating after prepared signal sended.
     * This will reduce the 'risks' of errors.
     * </br>
     * \see prepare().
     */
    void prepared(const std::shared_ptr<Peony::GErrorWrapper> &err = nullptr);
    /*!
     * \brief childrenUpdated
     * \param uriList, uri list of newly enumerated files.
     * <br>
     * If we use enumerateAsync(), we might not get all
     * the children at once. This signal sends everytime
     * there are newly children found asynchronously.
     * connect this signal in you classes and update you data.
     * or connect finished signal which sends when all
     * children were found asynchronously.
     * </br>
     * \see enumerateAsync(), enumerator_next_files_async_ready_callback();
     */
    void childrenUpdated(const QStringList &uriList);
    /*!
     * \brief enumerateFinished
     * \param successed
     * \retval true, if enumerate children successed.
     * \retval false, if enumerate children failed.
     * \note For other class, they might only care whether
     * there are children found when enumerating.
     * They don't care how enumerator dealing with some error.
     * So just tell them the last result with this signal.
     */
    void enumerateFinished(bool successed = false);

public Q_SLOTS:
    void enumerateAsync();
    /*!
     * \brief cancel
     * <br>
     * Cancel all the work of this eumerator excuting now,
     * including mounting, enumerating, etc.
     * </br>
     */
    void cancel();

protected:
    /*!
     * \brief handleError
     * \param err
     *
     */
    void handleError(GError *err);
    /*!
     * \brief enumerateChildren, a sync method enumerate children and cached their GFile handle.
     * \param enumerator, handle of enum next file.
     */
    void enumerateChildren(GFileEnumerator *enumerator);
    /*!
     * \brief enumerateTargetFile
     * \return target uri which original uri point to.
     * \note for some special uri, such as volume in 'computer:///',
     * or server in 'network:///', etc, the could not enumerate directly.
     * gvfs supplied query their target uri, if they have mounted into local.
     * Use this uri rather than the raw one, otherwise we might not enumerate
     * their children.
     */
    GFile *enumerateTargetFile();

    /*!
     * \brief mount_mountable_callback
     * \param file
     * \param res
     * \param p_this
     * \return
     * \see handleError().
     */
    static GAsyncReadyCallback mount_mountable_callback(GFile *file,
                                                        GAsyncResult *res,
                                                        FileEnumerator *p_this);

    /*!
     * \brief mount_enclosing_volume_callback
     * \param file
     * \param res
     * \param p_this
     * \return
     * \see handleError().
     */
    static GAsyncReadyCallback mount_enclosing_volume_callback(GFile *file,
                                                               GAsyncResult *res,
                                                               FileEnumerator *p_this);

    /*!
     * \brief find_children_async_ready_callback
     * \param file
     * \param res
     * \param p_this
     * \return
     * \see enumerateAsync().
     */
    static GAsyncReadyCallback find_children_async_ready_callback(GFile *file,
                                                                  GAsyncResult *res,
                                                                  FileEnumerator *p_this);

    /*!
     * \brief enumerator_next_files_async_ready_callback
     * \param enumerator
     * \param res
     * \param p_this
     * \return
     * \see enumerateAsync().
     */
    static GAsyncReadyCallback enumerator_next_files_async_ready_callback(GFileEnumerator *enumerator,
                                                                          GAsyncResult *res,
                                                                          FileEnumerator *p_this);

private:
    GFile *m_root_file = nullptr;
    GCancellable *m_cancellable = nullptr;

    QList<QString> *m_children_uris = nullptr;

    bool m_auto_delete = false;
};

}

#endif // FILEENUMERATOR_H
