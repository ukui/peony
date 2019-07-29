#include "file-enumerator.h"
#include "file-info.h"
#include "file-info-manager.h"

#include "mount-operation.h"

#include "gerror-wrapper.h"

#include <QList>

#include <QDebug>
#include <QTimer>

#ifndef PEONY_FIND_NEXT_FILES_BATCH_SIZE
#define PEONY_FIND_NEXT_FILES_BATCH_SIZE 100
#endif

using namespace Peony;

FileEnumerator::FileEnumerator(QObject *parent) : QObject(parent)
{
    m_root_file = g_file_new_for_uri("file:///");
    m_cancellable = g_cancellable_new();

    m_children = g_list_alloc();
    //m_errs = g_list_alloc();
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
    disconnect();
    //qDebug()<<"~FileEnumerator";
    g_object_unref(m_root_file);
    g_object_unref(m_cancellable);

    auto list = getChildren();
    for (auto child : list) {
        //qDebug()<<child->uri()<<child.use_count();
        if (child.use_count() <= 3)
            FileInfoManager::getInstance()->remove(child);
    }

    g_list_free_full(m_children, g_object_unref);
    //g_list_free_full(m_errs, GDestroyNotify(g_error_free));
}

void FileEnumerator::setEnumerateDirectory(QString uri)
{
    //This usually doesn't happen.
    auto list = getChildren();
    for (auto child : list) {
        //qDebug()<<child->uri()<<child.use_count();
        if (child.use_count() <= 3)
            FileInfoManager::getInstance()->remove(child);
    }

    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_object_unref(m_cancellable);
    }
    m_cancellable = g_cancellable_new();

    if (m_root_file) {
        g_object_unref(m_root_file);
    }
    m_root_file = g_file_new_for_uri(uri.toUtf8());
}

void FileEnumerator::setEnumerateDirectory(GFile *file)
{
    //This usually doesn't happen.
    auto list = getChildren();
    for (auto child : list) {
        //qDebug()<<child->uri()<<child.use_count();
        if (child.use_count() <= 3)
            FileInfoManager::getInstance()->remove(child);
    }

    if (m_cancellable) {
        g_cancellable_cancel(m_cancellable);
        g_object_unref(m_cancellable);
    }
    m_cancellable = g_cancellable_new();

    if (m_root_file) {
        g_object_unref(m_root_file);
    }
    m_root_file = g_file_dup(file);
}

QList<std::shared_ptr<FileInfo>> FileEnumerator::getChildren()
{
    //qDebug()<<"FileEnumerator::getChildren():";
    QList<std::shared_ptr<FileInfo>> children;
    GList *i = m_children;
    while (i->data) {
        GFile *file = static_cast<GFile*>(i->data);
        char *uri = g_file_get_uri(file);
        //qDebug()<<uri;
        auto file_info = FileInfo::fromUri(uri);
        //qDebug()<<file_info.use_count();
        g_free(uri);
        children<<file_info;
        i = i->next;
    }
    return children;
}

void FileEnumerator::cancel()
{
    g_cancellable_cancel(m_cancellable);
    g_object_unref(m_cancellable);
    m_cancellable = g_cancellable_new();
}

void FileEnumerator::prepare()
{
    GError *err = nullptr;
    GFileEnumerator *enumerator = g_file_enumerate_children(m_root_file,
                                                            G_FILE_ATTRIBUTE_STANDARD_NAME,
                                                            G_FILE_QUERY_INFO_NONE,
                                                            m_cancellable,
                                                            &err);

    if (err) {
        //do not send prepared(err) here, wait handle err finished.
        handleError(err);
        g_error_free(err);
    } else {
        //even though there is no err, we still need to wait a little while
        //for confirming other object will recieve this signal, you can aslo
        //connect prepared signal before prepared() method to confirm that.
        //Q_EMIT prepared(nullptr);
        QTimer::singleShot(100, this, [=](){
            Q_EMIT prepared(nullptr);
        });
    }

    g_object_unref(enumerator);
}

GFile *FileEnumerator::enumerateTargetFile()
{
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
        qDebug()<<"enumerateTargetFile"<<uri;
        target = g_file_new_for_uri(uri);
        g_free(uri);
    } else {
        target = g_file_dup(m_root_file);
    }
    return target;
}

void FileEnumerator::enumerateSync()
{
    GFile *target = enumerateTargetFile();

    GFileEnumerator *enumerator = g_file_enumerate_children(target,
                                                            G_FILE_ATTRIBUTE_STANDARD_NAME,
                                                            G_FILE_QUERY_INFO_NONE,
                                                            m_cancellable,
                                                            nullptr);

    enumerateChildren(enumerator);

    g_object_unref(enumerator);
    g_object_unref(target);
}

void FileEnumerator::handleError(GError *err)
{
    qDebug()<<err->code<<err->message;
    switch (err->code) {
    case G_IO_ERROR_NOT_DIRECTORY:
        if (g_file_has_uri_scheme(m_root_file, "computer")) {
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
        g_file_mount_enclosing_volume(m_root_file,
                                      G_MOUNT_MOUNT_NONE,
                                      nullptr,
                                      m_cancellable,
                                      GAsyncReadyCallback(mount_enclosing_volume_callback),
                                      this);
        break;
    case G_IO_ERROR_PERMISSION_DENIED:
        //FIXME: do i need add an auth function for this kind of errors?
        qDebug()<<"need permisson";
        break;
    default:
        break;
    }
}

void FileEnumerator::enumerateAsync()
{
    GFile *target = enumerateTargetFile();

    g_file_enumerate_children_async(target,
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
        m_children = g_list_prepend(m_children, child);
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
        qDebug()<<err->code<<err->message;
        auto err_data = GErrorWrapper::wrapFrom(err);
        Q_EMIT p_this->prepared(err_data);
    } else {
        Q_EMIT p_this->prepared(nullptr);
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
            qDebug()<<"mount successed, err:"<<err->code<<err->message;
            Q_EMIT p_this->prepared(GErrorWrapper::wrapFrom(err));
        } else {
            Q_EMIT p_this->prepared();
        }
    } else {
        if (err) {
            qDebug()<<"mount failed, err:"<<err->code<<err->message;
            //show the connect dialog
            char *uri = g_file_get_uri(file);
            MountOperation *op = new MountOperation(uri);
            g_free(uri);
            //op->setAutoDelete();
            connect(op, &MountOperation::finished, [=](GError *finished_err){
                if (finished_err) {
                    qDebug()<<"finished err:"<<finished_err->code<<finished_err->message;
                }
                Q_EMIT p_this->prepared(GErrorWrapper::wrapFrom(finished_err));
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
        qDebug()<<"find children async err:"<<err->code<<err->message;
        g_error_free(err);
    }
    //
    g_file_enumerator_next_files_async(enumerator,
                                       PEONY_FIND_NEXT_FILES_BATCH_SIZE,
                                       G_PRIORITY_DEFAULT,
                                       p_this->m_cancellable,
                                       GAsyncReadyCallback(enumerator_next_files_async_ready_callback),
                                       p_this);
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
    if (!files && !err) {
        //if a directory children count is same with BATCH_SIZE,
        //just send finished signal.
        qDebug()<<"no more files"<<endl<<endl<<endl;
        Q_EMIT p_this->enumerateFinished(true);
        return nullptr;
    }
    if (!files && err) {
        //critical
        return nullptr;
    }
    if (err) {
        qDebug()<<"next_files_async:"<<err->code<<err->message;
        g_error_free(err);
    }

    GList *l = files;
    QStringList uriList;
    int files_count = 0;
    while (l) {
        GFileInfo *info = static_cast<GFileInfo*>(l->data);
        GFile *file = g_file_enumerator_get_child(enumerator, info);
        p_this->m_children = g_list_prepend(p_this->m_children, file);
        char *uri = g_file_get_uri(file);
        //qDebug()<<uri;
        uriList<<uri;
        g_free(uri);
        files_count++;
        l = l->next;
    }
    g_list_free_full(files, g_object_unref);
    Q_EMIT p_this->childrenUpdated(uriList);
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
        Q_EMIT p_this->enumerateFinished(true);
    }
    return nullptr;
}
