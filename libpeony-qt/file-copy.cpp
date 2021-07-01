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
 * Authors: Ding Jing <dingjing@kylinos.cn>
 *
 */
#include "file-copy.h"
#include "file-utils.h"

#include <cstring>
#include <QString>
#include "file-info.h"
#include "file-info-job.h"

#define BUF_SIZE        1024000

using namespace Peony;

FileCopy::FileCopy (QString srcUri, QString destUri, GFileCopyFlags flags, GCancellable* cancel, GFileProgressCallback cb, gpointer pcd, GError** error, QObject* obj) : QObject (obj)
{
    mSrcUri = FileUtils::urlDecode(srcUri);
    mDestUri = FileUtils::urlDecode(destUri);
    QString destUrit = nullptr;

    mCopyFlags = flags;

    mCancel = cancel;
    mProgress = cb;
    mProgressData = pcd;
    mError = error;
}

FileCopy::~FileCopy()
{

}

void FileCopy::pause ()
{
    mStatus = PAUSE;
    mPause.lock();
}

void FileCopy::resume ()
{
    mStatus = RUNNING;
    mPause.unlock();
}

void FileCopy::cancel()
{
    if (mCancel) {
        g_cancellable_cancel (mCancel);
    }
    mPause.unlock();
}

void FileCopy::detailError (GError** error)
{
    if (nullptr == error || nullptr == *error || nullptr == mError) {
        return;
    }

    g_set_error(mError, (*error)->domain, (*error)->code, "%s", (*error)->message);
    g_error_free(*error);

    *error = nullptr;
}


void FileCopy::updateProgress () const
{
    if (nullptr == mProgress) {
        return;
    }

    mProgress(mOffset, mTotalSize, mProgressData);
}

void FileCopy::run ()
{
    GError*                 error = nullptr;
    gssize                  readSize = 0;
    gssize                  writeSize = 0;
    char                    buf[BUF_SIZE] = {0};
    GFileInputStream*       readIO = nullptr;
    GFileOutputStream*      writeIO = nullptr;
    GFile*                  srcFile = nullptr;
    GFile*                  destFile = nullptr;
    GFileInfo*              srcFileInfo = nullptr;
    GFileInfo*              destFileInfo = nullptr;
    GFileType               srcFileType = G_FILE_TYPE_UNKNOWN;
    GFileType               destFileType = G_FILE_TYPE_UNKNOWN;

    srcFile = g_file_new_for_uri(FileUtils::urlEncode(mSrcUri).toUtf8());
    destFile = g_file_new_for_uri(FileUtils::urlEncode(mDestUri).toUtf8());

    // it's impossible
    if (nullptr == srcFile || nullptr == destFile) {
        error = g_error_new (1, G_IO_ERROR_INVALID_ARGUMENT,"%s", tr("Error in source or destination file path!").toUtf8().constData());
        detailError(&error);
        goto out;
    }

    // impossible
    srcFileType = g_file_query_file_type(srcFile, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr);
    if (G_FILE_TYPE_UNKNOWN == srcFileType || G_FILE_TYPE_DIRECTORY == srcFileType) {
        error = g_error_new (1, G_IO_ERROR_INVALID_ARGUMENT,"%s", tr("Error in source or destination file path!").toUtf8().constData());
        detailError(&error);
        goto out;
    }

    destFileType = g_file_query_file_type(srcFile, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr);
    if (G_FILE_TYPE_DIRECTORY == destFileType) {
        mDestUri = mDestUri + "/" + mSrcUri.split("/").last();
        g_object_unref(destFile);
        destFile = g_file_new_for_uri(FileUtils::urlEncode(mDestUri).toUtf8());
        if (nullptr == destFile) {
            error = g_error_new (1, G_IO_ERROR_INVALID_ARGUMENT,"%s", tr("Error in source or destination file path!").toUtf8().constData());
            detailError(&error);
            goto out;
        }
    }

    // check file status
    if (FileUtils::isFileExsit(mDestUri)) {
        if (mCopyFlags & G_FILE_COPY_OVERWRITE) {
            g_file_delete(destFile,  nullptr, &error);
            if (nullptr != error) {
                detailError(&error);
                goto out;
            }
        } else if (mCopyFlags & G_FILE_COPY_BACKUP) {
            do {
                QStringList newUrl = mDestUri.split("/");
                newUrl.pop_back();
                newUrl.append(FileUtils::handleDuplicateName(FileUtils::urlDecode(mDestUri)));
                mDestUri = newUrl.join("/");
            } while (FileUtils::isFileExsit(mDestUri));
            if (nullptr != destFile) {
                g_object_unref(destFile);
            }
            destFile = g_file_new_for_uri(FileUtils::urlEncode(mDestUri).toUtf8());
        } else {
            error = g_error_new (1, G_IO_ERROR_EXISTS, "%s", QString(tr("The dest file \"%1\" has existed!")).arg(mDestUri).toUtf8().constData());
            detailError(&error);
            goto out;
        }
    }

    srcFileInfo = g_file_query_info(srcFile, "standard::*,time::*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, mCancel ? mCancel : nullptr, &error);
    if (nullptr != error) {
        mTotalSize = 0;
        qDebug() << error->message;
        detailError(&error);
    } else {
        mTotalSize = g_file_info_get_size(srcFileInfo);
    }

    qDebug() << "copy - src: " << mSrcUri << "  to: " << mDestUri;

    // read io stream
    readIO = g_file_read(srcFile, mCancel ? mCancel : nullptr, &error);
    if (nullptr != error) {
        detailError(&error);
        qDebug() << "read source file error!";
        goto out;
    }

    // write io stream
    writeIO = g_file_create(destFile, G_FILE_CREATE_REPLACE_DESTINATION, mCancel ? mCancel : nullptr, &error);
    if (nullptr != error) {
        detailError(&error);
        qDebug() << "create dest file error!" << mDestUri << " == " << g_file_get_uri(destFile);
        goto out;
    }

    // copy file attribute
    // It is possible that some file systems do not support file attributes
    g_file_copy_attributes(srcFile, destFile, G_FILE_COPY_ALL_METADATA, nullptr, &error);
    if (nullptr != error) {
        qWarning() << "copy attribute error:" << error->code << "  ---  " << error->message;
        g_error_free(error);
        error = nullptr;
    }

    if (!readIO || !writeIO) {
        error = g_error_new (1, G_IO_ERROR_FAILED,"%s", tr("Error opening source or destination file!").toUtf8().constData());
        detailError(&error);
        goto out;
    }

    while (true) {
        if (RUNNING == mStatus) {
            if (nullptr != mCancel && g_cancellable_is_cancelled(mCancel)) {
                mStatus = CANCEL;
                continue;
            }

            memset(buf, 0, sizeof(buf));
            mPause.lock();
            // read data
            readSize = g_input_stream_read(G_INPUT_STREAM(readIO), buf, sizeof(buf) - 1, mCancel ? mCancel : nullptr, &error);
            if (0 == readSize && nullptr == error) {
                mStatus = FINISHED;
                mPause.unlock();
                continue;
            } else if (nullptr != error) {
                detailError(&error);
                mStatus = ERROR;
                mPause.unlock();
                continue;
            }
            mPause.unlock();

            // write data
            writeSize = g_output_stream_write(G_OUTPUT_STREAM(writeIO), buf, readSize, mCancel ? mCancel : nullptr, &error);
            if (nullptr != error) {
                detailError(&error);
                mStatus = ERROR;
                continue;
            }

            if (readSize != writeSize) {
                // it's impossible
                error = g_error_new (1, G_IO_ERROR_FAILED,"%s", tr("Reading and Writing files are inconsistent!").toUtf8().constData());
                detailError(&error);
                mStatus = ERROR;
                continue;
            }

            if (mOffset <= mTotalSize) {
                mOffset += writeSize;
            }

            updateProgress ();

        } else if (CANCEL == mStatus) {
            error = g_error_new(1, G_IO_ERROR_CANCELLED, "%s", tr("operation cancel").toUtf8().constData());
            detailError(&error);
            g_file_delete (destFile, nullptr, nullptr);
            break;
        } else if (ERROR == mStatus) {
            g_file_delete (destFile, nullptr, nullptr);
            break;
        } else if (FINISHED == mStatus) {
            qDebug() << "copy file finish!";
            break;
        } else {
            mStatus = RUNNING;
        }
    }

    // finally set some metaData, not used!
//    if (mCopyFlags & G_FILE_COPY_ALL_METADATA && FINISHED == mStatus && FileUtils::isFileExsit(mDestUri)) {
//        destFileInfo = g_file_query_info(destFile, "time::*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, &error);
//        if (nullptr != error) {
//            detailError(&error);
//            goto finish;
//        }

//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_MODIFIED, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_MODIFIED));
//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_MODIFIED_USEC, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_MODIFIED_USEC));
//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_ACCESS, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_ACCESS));
//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_ACCESS_USEC, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_ACCESS_USEC));
//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_CHANGED, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_ACCESS));
//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_CHANGED_USEC, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_ACCESS_USEC));
//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_CREATED, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_CREATED));
//        g_file_info_set_attribute_uint64(destFileInfo, G_FILE_ATTRIBUTE_TIME_CREATED_USEC, g_file_info_get_attribute_uint64(srcFileInfo, G_FILE_ATTRIBUTE_TIME_CREATED_USEC));
//    }

//finish:
    // if copy sucessed, flush all data
    if (FINISHED == mStatus) {
        g_output_stream_flush(G_OUTPUT_STREAM(writeIO), nullptr, &error);
        detailError(&error);
    }


out:
    if (nullptr != readIO) {
        g_input_stream_close (G_INPUT_STREAM(readIO), nullptr, nullptr);
        g_object_unref(readIO);
    }

    if (nullptr != writeIO) {
        g_output_stream_close (G_OUTPUT_STREAM(writeIO), nullptr, nullptr);
        g_object_unref(writeIO);
    }

    if (nullptr != error) {
        g_error_free(error);
    }

    if (nullptr != srcFile) {
        g_object_unref(srcFile);
    }

    if (nullptr != destFile) {
        g_object_unref(destFile);
    }

    if (nullptr != srcFileInfo) {
        g_object_unref(srcFile);
    }

    if (nullptr != destFileInfo) {
        g_object_unref(destFile);
    }
}
