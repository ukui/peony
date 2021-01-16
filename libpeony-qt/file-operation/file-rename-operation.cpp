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

#include "file-rename-operation.h"
#include "file-operation-manager.h"
#include "file-utils.h"
#include <gio/gdesktopappinfo.h>
#include <glib/gprintf.h>
#include <QUrl>

#include <QProcess>

using namespace Peony;

static QString handleDuplicate(QString name) {
    QRegExp regExpNum("^\\(\\d+\\)");
    QRegExp regExp("\\(\\d+\\)(\\.[\\.0-9a-zA-Z]+|)$");
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
        return name;
    } else {
        if (name.contains(".")) {
            auto list = name.split(".");
            if (list.count() <= 1) {
                return name + "(1)";
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
                return name;
            }
        } else {
            name = name + "(1)";
            return name;
        }
    }
}

FileRenameOperation::FileRenameOperation(QString uri, QString newName)
{
    m_uri = uri;
    m_new_name = newName;
    m_auto_overwrite = false;
    QStringList srcUris;
    srcUris<<uri;
    QString destUri = FileUtils::getParentUri(uri);
    if (destUri != nullptr) {
        destUri = destUri + "/" + newName;
    }

    m_info = std::make_shared<FileOperationInfo>(srcUris, destUri, FileOperationInfo::Rename);
}

/*!
 * \brief FileRenameOperation::run
 * \bug
 * how to keep the permission?
 */
void FileRenameOperation::run()
{
    QString destUri;
    Q_EMIT operationStarted();

    if (m_new_name == "/" || m_new_name == ".") {
        Q_EMIT operationFinished();
        return;
    }

    auto file = wrapGFile(g_file_new_for_uri(m_uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(), "*",
                              G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                              getCancellable().get()->get(),
                              nullptr));

    bool is_local_desktop_file = false;
    QUrl url = m_uri;
    //change the content of .desktop file;
    if (url.isLocalFile()) {
        GDesktopAppInfo *desktop_info = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
        if (G_IS_DESKTOP_APP_INFO(desktop_info)) {
            bool is_executable = g_file_test (url.path().toUtf8().constData(), G_FILE_TEST_IS_EXECUTABLE);
            is_local_desktop_file = is_executable;
            if (is_executable) {
                //rename the generic name
                GKeyFile *key_file = g_key_file_new();
                g_key_file_load_from_file(key_file,
                                          url.path().toUtf8().constData(),
                                          G_KEY_FILE_KEEP_COMMENTS,
                                          nullptr);
                QString locale_name = QLocale::system().name();
                QString local_generic_name_key = QString("Name[%1]").arg(locale_name);
                if (g_key_file_has_key(key_file,
                                       G_KEY_FILE_DESKTOP_GROUP,
                                       local_generic_name_key.toUtf8().constData(),
                                       nullptr)) {
                    g_key_file_set_value(key_file,
                                         G_KEY_FILE_DESKTOP_GROUP,
                                         local_generic_name_key.toUtf8().constData(),
                                         m_new_name.toUtf8().constData());
                } else {
                    g_key_file_set_value(key_file,
                                         G_KEY_FILE_DESKTOP_GROUP,
                                         G_KEY_FILE_DESKTOP_KEY_NAME,
                                         m_new_name.toUtf8().constData());
                }

                g_key_file_save_to_file(key_file,
                                        url.path().toUtf8().constData(),
                                        nullptr);

                //set attributes again.
                GError *set_err = nullptr;
                g_file_set_attributes_from_info(file.get()->get(),
                                                info.get()->get(),
                                                G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                                nullptr,
                                                &set_err);
                if (set_err) {
                    qDebug()<<set_err->message;
                    g_error_free(set_err);
                }
                g_key_file_free(key_file);
            }
            g_object_unref(desktop_info);
        }
    }

    if (is_local_desktop_file) {
        m_new_name = m_new_name+".desktop";
    }

    //move the file. normally means 'rename'.
    auto parent = FileUtils::getFileParent(file);
    auto newFile = FileUtils::resolveRelativePath(parent, m_new_name);
    if (is_local_desktop_file) {
fallback_retry:
        GError *err = nullptr;
        g_file_move(file.get()->get(),
                    newFile.get()->get(),
                    G_FILE_COPY_ALL_METADATA,
                    nullptr,
                    nullptr,
                    nullptr,
                    &err);
        if (err) {
            qDebug()<<err->message;
            FileOperationError except;
            except.srcUri = m_uri;
            except.destDirUri = FileUtils::getFileUri(newFile);
            except.isCritical = true;
            except.op = FileOpRename;
            except.title = tr("Rename file error");
            except.errorType = ET_GIO;
            except.errorCode = err->code;
            except.errorStr = err->message;
            if (G_IO_ERROR_EXISTS == err->code) {
                except.dlgType = ED_CONFLICT;
                auto responseType = except.respCode;
                switch (responseType) {
                case Retry:
                    goto fallback_retry;
                case Cancel:
                    cancel();
                    break;
                default:
                    break;
                }
            } else {
                except.dlgType = ED_WARNING;
                auto responseType = except.respCode;
                switch (responseType) {
                case Retry:
                    goto fallback_retry;
                case Cancel:
                    cancel();
                    break;
                default:
                    break;
                }
            }

        }
    } else {
retry:
        FileOperationError except;
        except.srcUri = m_uri;
        except.destDirUri = FileUtils::getFileUri(newFile);
        except.isCritical = true;
        except.errorType = ET_GIO;
        except.op = FileOpRename;
        except.title = tr("Rename file error");
        GError *err = nullptr;
        if (FileUtils::isFileExsit(g_file_get_uri(newFile.get()->get()))&&!m_auto_overwrite) {
            except.dlgType = ED_CONFLICT;
            except.errorCode = G_IO_ERROR_EXISTS;
            Q_EMIT errored(except);
            switch (except.respCode) {
            case BackupOne: {
                // use custom name
                QString srcName = FileUtils::getFileBaseName(newFile);
                QString name = "";
                QString endStr = "";
                QStringList extendStr = srcName.split(".");
                if (extendStr.length() > 0) {
                    extendStr.removeAt(0);
                }
                endStr = extendStr.join(".");

                if (except.respValue.contains("name")) {
                    name = except.respValue["name"].toString();
                    if (endStr != "" && name.endsWith(endStr)) {
                        newFile = FileUtils::resolveRelativePath(parent, name);
                    } else if ("" != endStr && "" != name) {
                        newFile = FileUtils::resolveRelativePath(parent, name + "." + endStr);
                    } else if ("" == endStr) {
                        newFile = FileUtils::resolveRelativePath(parent, name);
                    }
                }
            }
                break;
            case BackupAll: {
                QString fileUri = handleDuplicate(FileUtils::getFileUri(newFile));
                newFile = FileUtils::resolveRelativePath(parent, FileUtils::getUriBaseName(fileUri));
                break;
            }
            case OverWriteOne:
            case OverWriteAll:
                g_file_delete(newFile.get()->get(), nullptr, nullptr);
                break;
            case Cancel:
                cancel();
                goto cancel;
            default:
                break;
            }
        }
        else
           g_file_delete(newFile.get()->get(), nullptr, nullptr);

        char* newName = g_file_get_basename(newFile.get()->get());
        g_file_set_display_name(file.get()->get(), newName, nullptr, &err);
        if (nullptr != newName) {
            g_free(newName);
        }
/*
        g_file_move(file.get()->get(),
                    newFile.get()->get(),
                    m_default_copy_flag,
                    getCancellable().get()->get(),
                    nullptr,
                    nullptr,
                    &err);
*/
        if (err) {
            except.errorType = ET_GIO;
            except.errorCode = err->code;
            except.errorStr = err->message;
            auto responseType = Invalid;
            if (G_IO_ERROR_EXISTS != err->code) {
                except.dlgType = ED_WARNING;
                Q_EMIT errored(except);
                responseType = except.respCode;
            }
            switch (responseType) {
            case Retry:
                goto retry;
            case Cancel:
                cancel();
                break;
            default:
                break;
            }
        }
    }

cancel:
    if (!isCancelled()) {
        setHasError(false);
        auto string = g_file_get_uri(newFile.get()->get());
        destUri = string;
        if (string)
            g_free(string);
        m_info->m_node_map.insert(m_uri, destUri);
    }

    // judge if the operation should sync.
    bool needSync = false;
    GFile *src_first_file = g_file_new_for_uri(m_uri.toUtf8().constData());
    GMount *src_first_mount = g_file_find_enclosing_mount(src_first_file, nullptr, nullptr);
    if (src_first_mount) {
        needSync = g_mount_can_unmount(src_first_mount);
        g_object_unref(src_first_mount);
    } else {
        // maybe a vfs file.
        needSync = true;
    }
    g_object_unref(src_first_file);

    GFile *dest_dir_file = g_file_new_for_uri(destUri.toUtf8().constData());
    GMount *dest_dir_mount = g_file_find_enclosing_mount(dest_dir_file, nullptr, nullptr);
    if (src_first_mount) {
        needSync = g_mount_can_unmount(dest_dir_mount);
        g_object_unref(dest_dir_mount);
    } else {
        needSync = true;
    }
    g_object_unref(dest_dir_file);

    //needSync = true;

    if (needSync) {
        char *path = g_file_get_path(dest_dir_file);
        if (path) {
            operationStartSnyc();
            QProcess p;
            auto shell_path = g_shell_quote(path);
            p.start(QString("sync -d %1").arg(shell_path));
            g_free(shell_path);
            g_free(path);
            p.waitForFinished(-1);
        } else {
            operationStartSnyc();
            QProcess p;
            p.start("sync");
            p.waitForFinished(-1);
        }
    }

    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}


