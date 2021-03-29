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

#include "file-trash-operation.h"
#include "file-operation-manager.h"

#include <QProcess>
#include <file-info-job.h>
#include <file-info.h>

using namespace Peony;

FileTrashOperation::FileTrashOperation(QStringList srcUris, QObject *parent) : FileOperation (parent)
{
    m_src_uris = srcUris;
    m_info = std::make_shared<FileOperationInfo>(srcUris, "trash:///", FileOperationInfo::Trash);
}

void FileTrashOperation::run()
{
    Q_EMIT operationStarted();
    Peony::ExceptionResponse response = Invalid;
    for (auto src : m_src_uris) {
        if (isCancelled())
            break;
retry:
        GError *err = nullptr;
        auto srcFile = wrapGFile(g_file_new_for_uri(src.toUtf8().constData()));
        //
        FileInfoJob fileInfoJob (src);
        if (fileInfoJob.querySync()) {
            FileInfo* fileInfo = fileInfoJob.getInfo().get();
            if (!fileInfo->isDir() && (!fileInfo->canRead() || !fileInfo->canWrite())) {
                FileOperationError except;
                except.srcUri = src;
                except.destDirUri = tr("trash:///");
                except.isCritical = true;
                except.op = FileOpTrash;
                except.title = tr("Trash file error");
                except.errorCode = G_IO_ERROR_FAILED;
                except.errorStr = QString(tr("The user does not have read and write rights to the file '%1' and cannot delete it to the Recycle Bin.").arg(fileInfo->displayName()));
                except.errorType = ET_GIO;
                except.dlgType = ED_WARNING;
                Q_EMIT errored(except);
                if (Cancel == except.respCode) {
                    cancel();
                }
                continue;
            }
        }

        g_file_trash(srcFile.get()->get(), getCancellable().get()->get(), &err);
        if (err) {
            if (response == IgnoreAll) {
                g_error_free(err);
                continue;
            }

            // ref the error, released after error handling.
            auto refPtr = GErrorWrapper::wrapFrom(err);

            FileOperationError except;
            except.srcUri = src;
            except.destDirUri = tr("trash:///");
            except.isCritical = true;
            except.op = FileOpTrash;
            except.title = tr("Trash file error");
            except.errorCode = err->code;
            except.errorStr = err->message;
            except.errorType = ET_GIO;
            if (G_IO_ERROR_EXISTS == err->code) {
                except.dlgType = ED_CONFLICT;
                Q_EMIT errored(except);
                auto responseType = except.respCode;
                auto responseData = responseType;
                switch (responseData) {
                case Retry:
                    goto retry;
                case Cancel:
                    cancel();
                    break;
                case IgnoreAll:
                    response = IgnoreAll;
                    break;
                default:
                    break;
                }
            } else {
                if (err->code == G_IO_ERROR_NOT_SUPPORTED) {
                    except.dlgType = ED_NOT_SUPPORTED;
                    auto fileName = g_file_get_basename(srcFile.get()->get());
                    except.errorStr = tr("Can not trash this file, would you like to delete it permanently?");
                    if (fileName) {
                        g_free(fileName);
                    }
                } else if (err->code == G_IO_ERROR_FILENAME_TOO_LONG) {
                    char *orig_path = g_file_get_path(srcFile.get()->get());
                    char *basename = g_file_get_basename(srcFile.get()->get());
                    QString trashDir = QString(g_get_user_data_dir()) + "/Trash/files";
                    GFile *trash = g_file_new_for_path(trashDir.toUtf8().constData());
                    GFile *dest_file = g_file_resolve_relative_path(trash, basename);
                    g_object_unref(trash);
                    g_file_move(srcFile.get()->get(), dest_file, G_FILE_COPY_NOFOLLOW_SYMLINKS, nullptr, nullptr, nullptr, nullptr);
                    g_file_set_attribute_string(dest_file, "metadata::orig-path", orig_path, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, nullptr);
                    g_object_unref(dest_file);
                    if (orig_path) {
                        g_free(orig_path);
                    }
                    if (basename) {
                        g_free(basename);
                    }

                    response = IgnoreOne;
                } else {
                    except.dlgType = ED_WARNING;
                }
                if (response == Invalid)
                    Q_EMIT errored(except);
                auto responseType = except.respCode;
                auto responseData = responseType;
                if (response != Invalid)
                    responseData = response;
                switch (responseData) {
                case Retry:
                    goto retry;
                case OverWriteOne: {
                    GError *err1 = nullptr;
                    g_file_delete(srcFile.get()->get(), getCancellable().get()->get(), &err1);
                    if (err1) {
                        g_error_free(err1);
                    }
                    break;
                }
                case OverWriteAll: {
                    response = OverWriteOne;
                    goto retry;
                }
                case Cancel:
                    cancel();
                    break;
                case IgnoreAll:
                    response = IgnoreAll;
                    break;
                case Force:
                    forceDelete(src);
                    break;
                default:
                    break;
                }
            }
        }
    }

    // judge if the operation should sync.
    bool needSync = false;
    GFile *src_first_file = g_file_new_for_uri(m_src_uris.first().toUtf8().constData());
    GMount *src_first_mount = g_file_find_enclosing_mount(src_first_file, nullptr, nullptr);
    if (src_first_mount) {
        needSync = g_mount_can_unmount(src_first_mount);
        g_object_unref(src_first_mount);
    } else {
        // maybe a vfs file.
        needSync = true;
    }
    g_object_unref(src_first_file);

    if (needSync) {
        operationStartSnyc();
        QProcess p;
        p.start("sync");
        p.waitForFinished(-1);
    }

    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}

void FileTrashOperation::forceDelete(QString uri)
{
    auto deleteFile = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    g_file_delete(deleteFile.get()->get(), nullptr, nullptr);
}

void FileTrashOperation::setErrorMessage(GError** err)
{

}
