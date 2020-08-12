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

#include "create-template-operation.h"

#include "file-operation-manager.h"
#include "file-operation-utils.h"
#include <gio/gio.h>

#include <utime.h>

#include <QMessageBox>

using namespace Peony;

#define TEMPLATE_DIR "file://" + QString(g_get_user_special_dir(G_USER_DIRECTORY_TEMPLATES)) + "/"

void CreateTemplateOperation::handleDuplicate(const QString &uri) {
    setHasError(true);
    QString name = uri.split("/").last();
    QRegExp regExp("\\(\\d+\\)");
    if (name.contains(regExp)) {
        int pos = 0;
        int num = 0;
        QString tmp;
        while ((pos = regExp.indexIn(name, pos)) != -1) {
            tmp = regExp.cap(0).toUtf8();
            pos += regExp.matchedLength();
            qDebug()<<"pos"<<pos;
        }
        tmp.remove(0,1);
        tmp.chop(1);
        num = tmp.toInt();

        num++;
        name = name.replace(regExp, QString("(%1)").arg(num));
        m_target_uri = m_dest_dir_uri + "/" + name;
    } else {
        if (name.contains(".")) {
            auto list = name.split(".");
            if (list.count() <= 1) {
                m_target_uri = m_dest_dir_uri + "/" + name + "(1)";
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
                m_target_uri = m_dest_dir_uri + "/" + name;
            }
        } else {
            name = name + "(1)";
            m_target_uri = m_dest_dir_uri + "/" + name;
        }
    }
}

CreateTemplateOperation::CreateTemplateOperation(const QString &destDirUri, Type type, const QString &templateName, QObject *parent) : FileOperation(parent)
{
    m_target_uri = destDirUri + "/" + templateName;
    QStringList srcUris;
    m_src_uri = TEMPLATE_DIR + templateName;
    srcUris << m_src_uri;
    m_dest_dir_uri = destDirUri;
    m_type = type;
    m_info = std::make_shared<FileOperationInfo>(srcUris, destDirUri, FileOperationInfo::Type::Copy);
}

void CreateTemplateOperation::run()
{
    Q_EMIT operationStarted();
    Q_EMIT operationPrepared();
    switch (m_type) {
    case EmptyFile: {
        m_target_uri = m_dest_dir_uri + "/" + tr("NewFile") + ".txt";
retry_create_empty_file:
        GError *err = nullptr;
        g_file_create(wrapGFile(g_file_new_for_uri(m_target_uri.toUtf8())).get()->get(),
                      G_FILE_CREATE_NONE, nullptr, &err);
        if (err) {
            // todo: Allow user naming
            if (err->code == G_IO_ERROR_EXISTS) {
                g_error_free(err);
                handleDuplicate(m_target_uri);
                goto retry_create_empty_file;
            } else {
#if HANDLE_ERR_NEW
                FileOperationError except;
                except.srcUri = m_src_uri;
                except.dlgType = ED_CONFLICT;
                except.destDirUri = m_dest_dir_uri;
                except.isCritical = true;
                except.title = tr("Create file");
                except.errorCode = err->code;
                except.errorType = ET_GIO;
                Q_EMIT errored(except);
#else
                Q_EMIT errored(m_src_uri, m_dest_dir_uri, GErrorWrapper::wrapFrom(err), true);
#endif
            }
        }
        break;
    }
    case EmptyFolder: {
        m_target_uri = m_dest_dir_uri + "/" + tr("NewFolder");
retry_create_empty_folder:
        GError *err = nullptr;
        g_file_make_directory(wrapGFile(g_file_new_for_uri(m_target_uri.toUtf8())).get()->get(),
                              nullptr,
                              &err);
        if (err) {
            if (err->code == G_IO_ERROR_EXISTS) {
                g_error_free(err);
                handleDuplicate(m_target_uri);
                goto retry_create_empty_folder;
            } else {

#if HANDLE_ERR_NEW
                FileOperationError except;
                except.srcUri = m_src_uri;
                except.dlgType = ED_CONFLICT;
                except.destDirUri = m_dest_dir_uri;
                except.isCritical = true;
                except.title = tr("Create file");
                except.errorCode = err->code;
                except.errorType = ET_GIO;
                Q_EMIT errored(except);
#else
                Q_EMIT errored(m_src_uri, m_dest_dir_uri, GErrorWrapper::wrapFrom(err), true);
#endif
            }
        }
        break;
    }
    case Template: {
retry_create_template:
        qDebug()<<"create tmp";
        GError *err = nullptr;
        g_file_copy(wrapGFile(g_file_new_for_uri(m_src_uri.toUtf8())).get()->get(),
                    wrapGFile(g_file_new_for_uri(m_target_uri.toUtf8())).get()->get(),
                    GFileCopyFlags(G_FILE_COPY_NOFOLLOW_SYMLINKS),
                    nullptr,
                    nullptr,
                    nullptr,
                    &err);
        if (err) {
            if (err->code == G_IO_ERROR_EXISTS) {
                g_error_free(err);
                handleDuplicate(m_target_uri);
                goto retry_create_template;
            } else {
#if HANDLE_ERR_NEW
                FileOperationError except;
                except.srcUri = m_src_uri;
                except.dlgType = ED_CONFLICT;
                except.destDirUri = m_dest_dir_uri;
                except.isCritical = true;
                except.title = tr("Create file");
                except.errorCode = err->code;
                except.errorType = ET_GIO;
                Q_EMIT errored(except);
#else
                Q_EMIT errored(m_src_uri, m_dest_dir_uri, GErrorWrapper::wrapFrom(err), true);
#endif
            }
        }
        // change file's modify time and access time after copy templete file;
        time_t now_time = time(NULL);
        struct utimbuf tm = {now_time, now_time};

        if (m_target_uri.startsWith("file://")) {
            utime (m_target_uri.toStdString().c_str() + 6, &tm);
        } else if (m_target_uri.startsWith("/")) {
            utime (m_target_uri.toStdString().c_str(), &tm);
        }

        break;
    }
    }
    Q_EMIT operationFinished();
    notifyFileWatcherOperationFinished();
}
