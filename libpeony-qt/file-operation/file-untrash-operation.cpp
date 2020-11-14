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

#include "file-utils.h"
#include "file-untrash-operation.h"
#include "file-operation-manager.h"

#include <QUrl>

using namespace Peony;

FileUntrashOperation::FileUntrashOperation(QStringList uris, QObject *parent) : FileOperation (parent)
{
    m_uris = uris;
    //FIXME: should i put this into prepare process?
    cacheOriginalUri();
    QStringList destUris;
    for (auto value : m_restore_hash) {
        destUris<<value;
    }
    m_info = std::make_shared<FileOperationInfo>(uris, destUris, FileOperationInfo::Untrash);
}

void FileUntrashOperation::cacheOriginalUri()
{
    for (auto uri : m_uris) {
        if (isCancelled())
            break;

        auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
        auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                                  G_FILE_ATTRIBUTE_TRASH_ORIG_PATH,
                                  G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                  getCancellable().get()->get(),
                                  nullptr));
        auto origin_path = g_file_info_get_attribute_byte_string(info.get()->get(), G_FILE_ATTRIBUTE_TRASH_ORIG_PATH);
        qDebug()<<"orig-path"<<origin_path;

        auto destFile = wrapGFile(g_file_new_for_path(origin_path));
        auto originUri = FileUtils::getFileUri(destFile);

        m_restore_hash.insert(uri, originUri);
    }
}

const QString FileUntrashOperation::handleDuplicate(const QString &uri)
{
    setHasError(true);

    QStringList l = uri.split("/");
    QString name = l.last();
    l.removeLast();

    QRegExp regExpNum("^\\(\\d+\\)");
    QRegExp regExp("\\(\\d+\\)(\\.[0-9a-zA-Z]+|)$");
    if (name.contains(regExp)) {
        int num = 0;
        QString numStr = "";

        QString ext = regExp.cap(0);
        if (ext.contains(regExpNum)) {
            numStr = regExpNum.cap(0);
        }

        numStr.remove(0, 1);
        numStr.chop(1);
        num = numStr.toInt();
        ++num;
        name = name.replace(regExp, ext.replace(regExpNum, QString("(%1)").arg(num)));
        l.append(name);
        auto newUri = l.join("/");
        return newUri;
    } else {
        if (name.contains(".")) {
            auto list = name.split(".");
            if (list.count() <= 1) {
                l.append(name + "(1)");
                auto newUri = l.join("/");
                return newUri;
            } else {
                int pos = list.count() - 1;
                if (list.last() == "gz" |
                        list.last() == "xz" |
                        list.last() == "Z" |
                        list.last() == "sit" |
                        list.last() == "bz" |
                        list.last() == "bz2") {
                    pos--;
                }
                if (pos < 0)
                    pos = 0;
                //list.insert(pos, "(1)");
                auto tmp = list;
                QStringList suffixList;
                for (int i = 0; i < list.count() - pos; i++) {
                    suffixList.prepend(tmp.takeLast());
                }
                auto suffix = suffixList.join(".");

                auto basename = tmp.join(".");
                name = basename + "(1)" + "." + suffix;
                if (name.endsWith("."))
                    name.chop(1);
                l.append(name);
                auto newUri = l.join("/");
                return newUri;
            }
        } else {
            return uri + "(1)";
        }
    }
}

ExceptionResponse FileUntrashOperation::prehandle(GError *err)
{
    setHasError(true);

    if (m_prehandle_hash.contains(err->code))
        return m_prehandle_hash.value(err->code);

    return Other;
}

void FileUntrashOperation::untrashFileErrDlg(
                                FileOperationError &except,
                                QString &srcUri,
                                QString &originUri,
                                GError *err)
{
    except.srcUri = srcUri;
    except.destDirUri = originUri;
    except.isCritical = false;
    except.op = FileOpUntrash;
    except.title = tr("Untrash file error");
    except.errorCode = err->code;
    except.errorType = ET_GIO;
    if (G_IO_ERROR_EXISTS == err->code) {
        except.dlgType = ED_CONFLICT;
    } else {
        except.dlgType = ED_WARNING;
    }

    Q_EMIT errored(except);
}

void FileUntrashOperation::getBackupName(
                    QString &originUri,
                    FileOperationError &except)
{
    QString name = "";
    QStringList extendStr;

    if (except.respValue.contains("name")) {
        name = except.respValue["name"].toString();
        if (name.isEmpty())
        {
            qDebug()<<"input file name is empty.";
            return;
        }

        int endIndex = originUri.lastIndexOf('/');

        extendStr = originUri.split(".");
        if (extendStr.length() > 0) {
            extendStr.removeAt(0);
        }

        QString endStr = extendStr.join(".");
        if ("" != endStr) {
            endStr = "." + endStr;
            if (!name.endsWith(endStr)){
                originUri = originUri.left(endIndex) + "/" + name + endStr;
            } else {
                originUri = originUri.left(endIndex) + "/" + name;
            }
        } else {
            originUri = originUri.left(endIndex) + "/" + name;
        }
    }

    return;
}

void FileUntrashOperation::run()
{
    /*!
      \bug
      can not restore the files in desktop.
      it caused by the parent uri string has chinese.
      */
    for (auto uri : m_uris) {
        //cacheOriginalUri();
        auto originUri = m_restore_hash.value(uri);

        auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
        auto destFile = wrapGFile(g_file_new_for_uri(originUri.toUtf8().constData()));

retry:
        if (isCancelled())
            break;

        GError *err = nullptr;
        if (FileUtils::isFileExsit(originUri)) {
            err = g_error_new(G_IO_ERROR, G_IO_ERROR_EXISTS, "");
        } else {
            g_file_move(file.get()->get(),
                        destFile.get()->get(),
                        GFileCopyFlags(m_default_copy_flag),
                        getCancellable().get()->get(),
                        nullptr,
                        nullptr,
                        &err);
        }

        if (err) {

            FileOperationError except;
            ExceptionResponse type = prehandle(err);
            if (Other == type)
            {
                untrashFileErrDlg(except, uri, originUri, err);
                type = except.respCode;
            }

            g_error_free(err);
            err = nullptr;

            switch (type) {
            case Retry:
                goto retry;
            case Cancel:
                cancel();
                break;
            case OverWriteOne:
                g_file_move(file.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            nullptr,
                            nullptr,
                            nullptr);
                break;
            case OverWriteAll:
                g_file_move(file.get()->get(),
                            destFile.get()->get(),
                            GFileCopyFlags(m_default_copy_flag|G_FILE_COPY_OVERWRITE),
                            getCancellable().get()->get(),
                            nullptr,
                            nullptr,
                            nullptr);
                m_prehandle_hash.insert(except.errorCode, OverWriteOne);
                break;
            case BackupOne: {
                // use custom name
                getBackupName(originUri, except);
                if (FileUtils::isFileExsit(originUri)) {
                    originUri = handleDuplicate(originUri);
                }
                destFile = wrapGFile(g_file_new_for_uri(originUri.toUtf8().constData()));
                goto retry;
            }
            case BackupAll: {
                originUri = handleDuplicate(originUri);
                destFile = wrapGFile(g_file_new_for_uri(originUri.toUtf8().constData()));
                m_prehandle_hash.insert(except.errorCode, BackupOne);
                goto retry;
            }
            case IgnoreOne: {
                break;
            }
            case IgnoreAll: {
                m_prehandle_hash.insert(except.errorCode, IgnoreOne);
                break;
            }
            default:
                break;
            }
        }
    }

    operationFinished();
}
