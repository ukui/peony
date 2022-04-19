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
#include <global-settings.h>
#include <QUrl>

#include <QProcess>

static QString set_desktop_name (QString file, QString& name, GError** error);

using namespace Peony;

static QString handleDuplicate(QString name)
{
    return FileUtils::handleDuplicateName(name);
}

FileRenameOperation::FileRenameOperation(QString uri, QString newName)
{
    m_uri = uri;
    m_new_name = FileUtils::urlDecode(newName);
    m_old_name = FileUtils::getFileDisplayName(uri);
    QStringList srcUris;
    srcUris<<uri;
    QString destUri = FileUtils::getParentUri(FileUtils::urlEncode(uri));
    if (destUri != nullptr) {
        destUri = FileUtils::urlEncode(FileUtils::urlDecode(destUri) + "/" + m_new_name);
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

    if (m_new_name == "/" || m_new_name == "." || !nameIsValid(m_new_name)) {
        FileOperationError except;
        except.srcUri = m_uri;
        except.errorType = ET_GIO;
        except.op = FileOpRename;
        except.dlgType = ED_WARNING;
        except.title = tr("File Rename error");
        except.errorStr = tr("Invalid file name %1%2%3 .").arg("\“").arg(m_new_name).arg("\”");

        Q_EMIT errored(except);

        Q_EMIT operationFinished();
        return;
    } else if (m_new_name.startsWith(".")) {
        auto showHidden = GlobalSettings::getInstance()->getValue(SHOW_HIDDEN_PREFERENCE).toBool();
        if (! showHidden)
        {
            FileOperationError except;
            except.srcUri = m_uri;
            except.errorType = ET_GIO;
            except.op = FileOpRenameToHideFile;
            except.dlgType = ED_WARNING;
            except.title = tr("File Rename warning");
            except.errorStr = tr("The file %1%2%3 will be hidden when you refresh or change directory!").arg("\“").arg(m_new_name).arg("\”");

            Q_EMIT errored(except);
        }
    }
    bool showFileExtension = Peony::GlobalSettings::getInstance()->isExist(SHOW_FILE_EXTENSION)?
                Peony::GlobalSettings::getInstance()->getValue(SHOW_FILE_EXTENSION).toBool():true;
    if(!showFileExtension){
        renameHandleWhenExtensionIsHidden();
    }

    auto file = wrapGFile(g_file_new_for_uri(FileUtils::urlEncode(m_uri).toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(), "*",
                              G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                              getCancellable().get()->get(),
                              nullptr));

    bool is_local_desktop_file = false;
    QUrl url = m_uri;
    QString oldDesktopName = "";
    //change the content of .desktop file;
    if (url.isLocalFile() && m_uri.endsWith(".desktop")) {
        GDesktopAppInfo *desktop_info = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
        if (G_IS_DESKTOP_APP_INFO(desktop_info)) {
            bool is_executable = g_file_test (url.path().toUtf8().constData(), G_FILE_TEST_IS_EXECUTABLE);
            is_local_desktop_file = is_executable;
            if (is_executable) {
                g_autoptr (GError) error = nullptr;
                oldDesktopName = set_desktop_name (url.path (), m_new_name, &error);
                if (error) {
                    qDebug() << "set_desktop_name error code: " << error->code << " msg:" << error->message;
                }

                //set attributes again.
                g_file_set_attributes_from_info(file.get()->get(), info.get()->get(), G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, &error);
                if (error) {
                    qDebug() << "g_file_set_attributes_from_info error code: " << error->code << " msg: " << error->message;
                }
            }
            g_object_unref(desktop_info);
        }
    }

    QString targetName = m_new_name;
    if (is_local_desktop_file) {
        targetName = m_new_name+".desktop";
    }

    auto parent = FileUtils::getFileParent(file);
    auto newFile = FileUtils::resolveRelativePath(parent, targetName);
    getOperationInfo().get()->m_dest_dir_uri = FileUtils::getFileUri(newFile);

    if (is_local_desktop_file) {
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
            except.dlgType = ED_WARNING;
            except.errorCode = err->code;
            except.errorStr = err->message;

            errored (except);

            // fallback
            g_autoptr (GError) error = nullptr;
            set_desktop_name (url.path (), oldDesktopName, &error);
            if (error) {
                qDebug() << "fallback error: " << error->code << "  msg: " << error->message;
            }

            g_file_set_attributes_from_info(file.get()->get(), info.get()->get(), G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, nullptr, &error);
            if (error) {
                qDebug() << "g_file_set_attributes_from_info error code: " << error->code << " msg: " << error->message;
            }

            cancel ();
        }
    } else {
retry:
        GError* err = nullptr;
        FileOperationError except;
        except.srcUri = m_uri;
        except.errorType = ET_GIO;
        except.op = FileOpRename;
        except.dlgType = ED_WARNING;
        except.title = tr("Rename file error");
        except.destDirUri = FileUtils::getFileUri(newFile);
        qDebug() << "rename: " << g_file_get_uri(newFile.get()->get());
        g_autofree char* newName = g_file_get_basename(newFile.get()->get());

        g_file_set_display_name(file.get()->get(), newName, nullptr, &err);

        if (err) {
            except.errorCode = err->code;
            except.errorStr = err->message;
            qDebug() << err->message;

            if (G_IO_ERROR_EXISTS == err->code) {
                ExceptionResponse resp = prehandle(err);
                if (Other == resp) {
                    except.dlgType = ED_CONFLICT;
                    except.errorCode = G_IO_ERROR_EXISTS;
                }
                Q_EMIT errored(except);
                resp = except.respCode;
                switch (resp) {
                case BackupAll:
                    setAutoBackup();
                case BackupOne:{
                    while (FileUtils::isFileExsit(g_file_get_uri(newFile.get()->get()))) {
                        QString fileUri = handleDuplicate(FileUtils::getFileUri(newFile));
                        newFile = FileUtils::resolveRelativePath(parent, FileUtils::getUriBaseName(fileUri));
                        getOperationInfo().get()->m_dest_dir_uri = FileUtils::getFileUri(newFile);
                    }
                    goto retry;
                }
                case OverWriteAll:
                    setAutoOverwrite();
                case OverWriteOne:
                    g_file_delete(newFile.get()->get(), nullptr, nullptr);
                    goto retry;
                case IgnoreAll:
                    setAutoIgnore();
                case IgnoreOne:
                    break;
                case Cancel:
                    cancel();
                    goto cancel;
                default:
                    break;
                }
            } else {
                Q_EMIT errored(except);
                switch (except.respCode) {
                case Retry:
                    goto retry;
                case Cancel:
                    cancel();
                    break;
                default:
                    break;
                }
            }

            g_error_free(err);
        }
    }

    getOperationInfo().get()->m_dest_dir_uri = FileUtils::getFileUri(newFile);
cancel:
    if (!isCancelled()) {
        auto string = g_file_get_uri(newFile.get()->get());
        destUri = string;
        if (string)
            g_free(string);
        m_info->m_node_map.insert(m_uri, destUri);
        m_info->m_newname = m_new_name;
        m_info->m_oldname = m_old_name;
    }

    fileSync(m_uri, destUri);

    Q_EMIT operationFinished();
    //notifyFileWatcherOperationFinished();
}

#include <QFileInfo>
void FileRenameOperation::renameHandleWhenExtensionIsHidden()
{
    /* 一些常见扩展名处理，特殊情况以后待完善 */
    QFileInfo qFileInfo(m_uri);
    QString suffix = qFileInfo.suffix();
    QString fileBaseName = qFileInfo.fileName().left(qFileInfo.fileName().length() - suffix.length() - 1);
    m_old_name = fileBaseName;
    if (fileBaseName.isEmpty()){
        suffix = QString();
        m_old_name = qFileInfo.fileName();
    }
    else if(fileBaseName.endsWith(".tar")){
        suffix = QString("tar").append(".").append(suffix);
        m_old_name = fileBaseName.remove(".tar");
    }
    else if(fileBaseName.endsWith(".7z")){
        suffix = QString("7z").append(".").append(suffix);
        m_old_name = fileBaseName.remove(".7z");
    }

    if(!suffix.isEmpty())
        m_new_name += QString(".").append(suffix);

    return;
}

ExceptionResponse FileRenameOperation::prehandle(GError *err)
{
    if (err && G_IO_ERROR_EXISTS == err->code) {
        return m_apply_all;
    }

    return Other;
}


/**
 * @brief
 *  If the desktop shortcut rename fails, you need to roll back to the original name.
 * @param file: .desktop file path
 * @param name: .desktop file's Name[Local]=name
 * @param error: out param
 *
 * @return oldName
 */
static QString set_desktop_name (QString file, QString& name, GError** error)
{
    QString oldName = "";
    GKeyFile *keyFile = g_key_file_new();
    g_key_file_load_from_file(keyFile, file.toUtf8().constData(), G_KEY_FILE_KEEP_COMMENTS, nullptr);
    QString locale_name = QLocale::system().name();
    QString local_name_key = QString("Name[%1]").arg(locale_name);
    if (g_key_file_has_key(keyFile, G_KEY_FILE_DESKTOP_GROUP, local_name_key.toUtf8().constData(), nullptr)) {
        oldName = g_key_file_get_string (keyFile, G_KEY_FILE_DESKTOP_GROUP, local_name_key.toUtf8().constData(), nullptr);
        g_key_file_set_value(keyFile, G_KEY_FILE_DESKTOP_GROUP, local_name_key.toUtf8().constData(), name.toUtf8().constData());
    } else {
        g_key_file_set_value(keyFile, G_KEY_FILE_DESKTOP_GROUP, G_KEY_FILE_DESKTOP_KEY_NAME, name.toUtf8().constData());
    }

    g_key_file_save_to_file(keyFile, file.toUtf8().constData(), error);

    if (keyFile)    g_key_file_free(keyFile);

    return oldName;
}
