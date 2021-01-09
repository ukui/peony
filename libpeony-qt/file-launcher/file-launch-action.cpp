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

#include "file-launch-action.h"
#include <gio/gdesktopappinfo.h>

#include "file-info.h"
#include "file-info-job.h"
#include "file-operation-utils.h"
#include "audio-play-manager.h"

#include <QMessageBox>
#include <QPushButton>

#include <QUrl>
#include <QProcess>
#include <recent-vfs-manager.h>

#include <QDebug>

using namespace Peony;

FileLaunchAction::FileLaunchAction(const QString &uri, GAppInfo *app_info, bool forceWithArg, QObject *parent) : QAction(parent)
{
    m_uri = uri;
    m_app_info = static_cast<GAppInfo*>(g_object_ref(app_info));
    m_force_with_arg = forceWithArg;

    if (!isValid())
        return;

    GThemedIcon *icon = G_THEMED_ICON(g_app_info_get_icon(m_app_info));
    const char * const * icon_names = g_themed_icon_get_names(icon);

    if (icon_names)
        m_icon = QIcon::fromTheme(*icon_names);
    setIcon(m_icon);
    m_info_name = g_app_info_get_name(m_app_info);
    setText(m_info_name);
    m_info_display_name = g_app_info_get_display_name(m_app_info);

    connect(this, &QAction::triggered, [=]() {
        this->lauchFileAsync(m_force_with_arg, true);
    });
}

FileLaunchAction::~FileLaunchAction()
{
    if (m_app_info)
        g_object_unref(m_app_info);
}

const QString FileLaunchAction::getUri()
{
    return m_uri;
}

bool FileLaunchAction::isDesktopFileAction()
{
    //FIXME: replace BLOCKING api in ui thread.
    auto info = FileInfo::fromUri(m_uri);
    if (info->isEmptyInfo()) {
        FileInfoJob j(info);
        j.querySync();
    }
    return info->isDesktopFile();
}

const QString FileLaunchAction::getAppInfoName()
{
    return m_info_name;
}

const QString FileLaunchAction::getAppInfoDisplayName()
{
    return m_info_display_name;
}

bool FileLaunchAction::isExcuteableFile(QString fileType)
{
    if (m_executable_type.contains(fileType))
        return true;

    return false;
}

void FileLaunchAction::lauchFileSync(bool forceWithArg, bool skipDialog)
{
    //FIXME: replace BLOCKING api in ui thread.
    auto fileInfo = FileInfo::fromUri(m_uri);
    if (fileInfo->isEmptyInfo()) {
        FileInfoJob j(fileInfo);
        j.querySync();
    }

    bool executable = fileInfo->canExecute();
    bool isAppImage = fileInfo->type() == "application/vnd.appimage";
    bool isExecutable = isExcuteableFile(fileInfo->type());
    if (isAppImage) {
        if (executable) {
            QUrl url = m_uri;
            auto path = url.path();

            QProcess p;
            p.setProgram(path);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            p.startDetached();
#else
            p.startDetached(path);
#endif
            return;
        }
    }

    if (executable && !isDesktopFileAction() && !skipDialog && isExecutable) {
        QMessageBox msg;
        auto defaultAction = msg.addButton("By Default App", QMessageBox::ButtonRole::ActionRole);
        auto exec = msg.addButton(tr("Execute Directly"), QMessageBox::ButtonRole::ActionRole);
        auto execTerm = msg.addButton(tr("Execute in Terminal"), QMessageBox::ButtonRole::ActionRole);
        msg.addButton(QMessageBox::Cancel);

        msg.setText(tr("Detected launching an executable file %1, you want?").arg(fileInfo->displayName()));
        msg.exec();
        auto button = msg.clickedButton();
        if (button == exec) {
            execFile();
            return;
        } else if (button == execTerm) {
            execFileInterm();
            return;
        } else if (button == defaultAction) {
            //skip
        } else {
            return;
        }
    }

    if (!isValid()) {
        Peony::AudioPlayManager::getInstance()->playWarningAudio();
        QMessageBox::critical(nullptr, tr("Open Failed"), tr("Can not open %1, file not exist, is it deleted?").arg(m_uri));
        return;
    }

    if (isDesktopFileAction() && !forceWithArg) {
        g_app_info_launch(m_app_info, nullptr, nullptr, nullptr);
    } else {
        GList *l = nullptr;
        char *uri = g_strdup(m_uri.toUtf8().constData());
        l = g_list_prepend(l, uri);
        g_app_info_launch_uris(m_app_info,
                               l,
                               nullptr,
                               nullptr);
        g_list_free_full(l, g_free);
    }

    return;
    if (isDesktopFileAction() && !forceWithArg) {
        auto desktop_info = G_DESKTOP_APP_INFO(m_app_info);
        g_desktop_app_info_launch_uris_as_manager (desktop_info,
                nullptr,
                nullptr,
                G_SPAWN_DEFAULT,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr);
    } else {
        g_app_info_launch_default_for_uri(m_uri.toUtf8().constData(),
                                          nullptr,
                                          nullptr);
    }
}

void FileLaunchAction::lauchFileAsync(bool forceWithArg, bool skipDialog)
{
    //FIXME: replace BLOCKING api in ui thread.
    auto fileInfo = FileInfo::fromUri(m_uri);
    if (fileInfo->isEmptyInfo()) {
        FileInfoJob j(fileInfo);
        j.querySync();
    }

    bool executable = fileInfo->canExecute();
    bool isAppImage = fileInfo->type() == "application/vnd.appimage";
    bool isExecutable = isExcuteableFile(fileInfo->type());
    qDebug() <<"executable:" <<executable <<isAppImage <<fileInfo->type();
    if (isAppImage) {
        if (executable) {
            QUrl url = m_uri;
            auto path = url.path();

            QProcess p;
            p.setProgram(path);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            p.startDetached();
#else
            p.startDetached(path);
#endif
            return;
        }
    }

    if (executable && !isDesktopFileAction() && !skipDialog && isExecutable) {
        QMessageBox msg;
        auto defaultAction = msg.addButton(tr("By Default App"), QMessageBox::ButtonRole::ActionRole);
        auto exec = msg.addButton(tr("Execute Directly"), QMessageBox::ButtonRole::ActionRole);
        auto execTerm = msg.addButton(tr("Execute in Terminal"), QMessageBox::ButtonRole::ActionRole);
        msg.addButton(QMessageBox::Cancel);

        msg.setWindowTitle(tr("Launch Options"));
        msg.setText(tr("Detected launching an executable file %1, you want?").arg(fileInfo->displayName()));
        msg.exec();
        auto button = msg.clickedButton();
        if (button == exec) {
            execFile();
            return;
        } else if (button == execTerm) {
            execFileInterm();
            return;
        } else if (button == defaultAction) {
            //skip
        } else {
            return;
        }
    }

    if (!isValid()) {
        Peony::AudioPlayManager::getInstance()->playWarningAudio();
        bool isReadable = fileInfo->canRead();
        if (!isReadable)
        {
            if (fileInfo->isSymbolLink())
            {
                auto result = QMessageBox::question(nullptr, tr("Open Link failed"),
                                      tr("File not exist, do you want to delete the link file?"));
                if (result == QMessageBox::Yes) {
                    qDebug() << "Delete unused symbollink.";
                    QStringList selections;
                    selections.push_back(m_uri);
                    FileOperationUtils::trash(selections, true);
                }
            }
            else
                QMessageBox::critical(nullptr, tr("Open Failed"),
                                  tr("Can not open %1, Please confirm you have the right authority.").arg(m_uri));
        }
        else if (fileInfo->isDesktopFile())
        {
            auto result = QMessageBox::question(nullptr, tr("Open App failed"),
                                  tr("The linked app is changed or uninstalled, so it can not work correctly. \n"
                                     "Do you want to delete the link file?"));
            if (result == QMessageBox::Yes) {
                qDebug() << "Delete unused desktop file";
                QStringList selections;
                selections.push_back(m_uri);
                FileOperationUtils::trash(selections, true);
            }
        }
        else {
            auto result = QMessageBox::question(nullptr, tr("Error"), tr("Can not get a default application for opening %1, do you want open it with text format?").arg(m_uri));
            if (result == QMessageBox::Yes) {
                GAppInfo *text_info = g_app_info_get_default_for_type("text/plain", false);
                GList *l = nullptr;
                char *uri = g_strdup(m_uri.toUtf8().constData());
                l = g_list_prepend(l, uri);
#if GLIB_CHECK_VERSION(2, 60, 0)
                g_app_info_launch_uris_async(text_info, l,
                                             nullptr, nullptr,
                                             nullptr, nullptr);
#else
                g_app_info_launch_uris(text_info, l, nullptr, nullptr);
#endif
                g_list_free_full(l, g_free);
                g_object_unref(text_info);
            }
        }
        return;
    }

    if (isDesktopFileAction() && !forceWithArg) {
#if GLIB_CHECK_VERSION(2, 60, 0)
        g_app_info_launch_uris_async(m_app_info, nullptr,
                                     nullptr, nullptr,
                                     nullptr, nullptr);
#else
        g_app_info_launch_uris(m_app_info, nullptr, nullptr, nullptr);
#endif
    } else {
        GList *l = nullptr;
        char *uri = g_strdup(m_uri.toUtf8().constData());
        l = g_list_prepend(l, uri);
#if GLIB_CHECK_VERSION(2, 60, 0)
        g_app_info_launch_uris_async(m_app_info, l,
                                     nullptr, nullptr,
                                     nullptr, nullptr);

        RecentVFSManager::getInstance()->insert(fileInfo.get()->uri(), fileInfo.get()->mimeType(), fileInfo.get()->displayName(), g_app_info_get_name(m_app_info));
#else
        g_app_info_launch_uris(m_app_info, l, nullptr, nullptr);
#endif
        g_list_free_full(l, g_free);
    }

    return;
    if (isDesktopFileAction() && !forceWithArg) {
        auto desktop_info = G_DESKTOP_APP_INFO(m_app_info);
        g_desktop_app_info_launch_uris_as_manager (desktop_info,
                nullptr,
                nullptr,
                G_SPAWN_DEFAULT,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr);
    } else {
#if GLIB_CHECK_VERSION(2, 50, 0)
        g_app_info_launch_default_for_uri_async(m_uri.toUtf8().constData(),
                                                nullptr,
                                                nullptr,
                                                nullptr,
                                                nullptr);
#else
        g_app_info_launch_default_for_uri(m_uri.toUtf8().constData(),
                                          nullptr,
                                          nullptr);
#endif
    }
}

void FileLaunchAction::lauchFilesAsync(const QStringList files, bool forceWithArg, bool skipDialog)
{
    if(files.isEmpty())
        return;

    //FIXME: replace BLOCKING api in ui thread.
    auto fileInfo = FileInfo::fromUri(m_uri);
    if (fileInfo->isEmptyInfo()) {
        FileInfoJob j(fileInfo);
        j.querySync();
    }

    bool executable = fileInfo->canExecute();
    bool isAppImage = fileInfo->type() == "application/vnd.appimage";
    bool isExecutable = isExcuteableFile(fileInfo->type());
    if (isAppImage) {
        if (executable) {
            QProcess p;
            for (auto uri:files) {
                auto path = ((QUrl) uri).path();
                QProcess p;
                p.setProgram(path);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                p.startDetached();
#else
                p.startDetached(path);
#endif
            }
            return;
        }
    }

    if (executable && !isDesktopFileAction() && !skipDialog && isExecutable) {
        QMessageBox msg;
        auto defaultAction = msg.addButton(tr("By Default App"), QMessageBox::ButtonRole::ActionRole);
        auto exec = msg.addButton(tr("Execute Directly"), QMessageBox::ButtonRole::ActionRole);
        auto execTerm = msg.addButton(tr("Execute in Terminal"), QMessageBox::ButtonRole::ActionRole);
        msg.addButton(QMessageBox::Cancel);

        msg.setWindowTitle(tr("Launch Options"));
        msg.setText(tr("Detected launching an executable file %1, you want?").arg(fileInfo->displayName()));
        msg.exec();
        auto button = msg.clickedButton();
        if (button == exec) {
            execFile();
            return;
        } else if (button == execTerm) {
            execFileInterm();
            return;
        } else if (button == defaultAction) {
            //skip
        } else {
            return;
        }
    }

    if (!isValid()) {
        Peony::AudioPlayManager::getInstance()->playWarningAudio();
        bool isReadable = fileInfo->canRead();
        if (!isReadable)
        {
            if (fileInfo->isSymbolLink())
            {
                auto result = QMessageBox::question(nullptr, tr("Open Link failed"),
                                      tr("File not exist, do you want to delete the link file?"));
                if (result == QMessageBox::Yes) {
                    qDebug() << "Delete unused symbollink.";
                    QStringList selections;
                    selections.push_back(m_uri);
                    FileOperationUtils::trash(selections, true);
                }
            }
            else
                QMessageBox::critical(nullptr, tr("Open Failed"),
                                  tr("Can not open %1, Please confirm you have the right authority.").arg(m_uri));
        }
        else if (fileInfo->isDesktopFile())
        {
            auto result = QMessageBox::question(nullptr, tr("Open App failed"),
                                  tr("The linked app is changed or uninstalled, so it can not work correctly. \n"
                                     "Do you want to delete the link file?"));
            if (result == QMessageBox::Yes) {
                qDebug() << "Delete unused desktop file";
                QStringList selections;
                selections.push_back(m_uri);
                FileOperationUtils::trash(selections, true);
            }
        }
        else {
            auto result = QMessageBox::question(nullptr, tr("Error"), tr("Can not get a default application for opening %1, do you want open it with text format?").arg(m_uri));
            if (result == QMessageBox::Yes) {
                GAppInfo *text_info = g_app_info_get_default_for_type("text/plain", false);
                GList *l = nullptr;
                for (auto uri : files) {
                    l = g_list_prepend(l, g_strdup(uri.toUtf8().constData()));
                }
#if GLIB_CHECK_VERSION(2, 60, 0)
                g_app_info_launch_uris_async(text_info, l,
                                             nullptr, nullptr,
                                             nullptr, nullptr);
#else
                g_app_info_launch_uris(text_info, l, nullptr, nullptr);
#endif
                g_list_free_full(l, g_free);
                g_object_unref(text_info);
            }
        }
        return;
    }

    if (isDesktopFileAction() && !forceWithArg) {
#if GLIB_CHECK_VERSION(2, 60, 0)
        g_app_info_launch_uris_async(m_app_info, nullptr,
                                     nullptr, nullptr,
                                     nullptr, nullptr);
#else
        g_app_info_launch_uris(m_app_info, nullptr, nullptr, nullptr);
#endif
    } else {
        GList *l = nullptr;
        for (auto uri : files) {
            l = g_list_prepend(l, g_strdup(uri.toUtf8().constData()));
        }
#if GLIB_CHECK_VERSION(2, 60, 0)
        g_app_info_launch_uris_async(m_app_info, l,
                                     nullptr, nullptr,
                                     nullptr, nullptr);
#else
        g_app_info_launch_uris(m_app_info, l, nullptr, nullptr);
#endif
        g_list_free_full(l, g_free);
    }

    return;
    if (isDesktopFileAction() && !forceWithArg) {
        auto desktop_info = G_DESKTOP_APP_INFO(m_app_info);
        g_desktop_app_info_launch_uris_as_manager (desktop_info,
                nullptr,
                nullptr,
                G_SPAWN_DEFAULT,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr);
    } else {
#if GLIB_CHECK_VERSION(2, 50, 0)
        g_app_info_launch_default_for_uri_async(m_uri.toUtf8().constData(),
                                                nullptr,
                                                nullptr,
                                                nullptr,
                                                nullptr);
#else
        g_app_info_launch_default_for_uri(m_uri.toUtf8().constData(),
                                          nullptr,
                                          nullptr);
#endif
    }
}

bool FileLaunchAction::isValid()
{
    return G_IS_APP_INFO(m_app_info);
}

void FileLaunchAction::execFile()
{
    QUrl url = m_uri;
    char *quote = g_shell_quote(url.path().toUtf8());
    GAppInfo *app_info = g_app_info_create_from_commandline(quote, nullptr, G_APP_INFO_CREATE_NONE, nullptr);
    g_app_info_launch(app_info, nullptr, nullptr, nullptr);
    g_object_unref(app_info);
    g_free(quote);
}

void FileLaunchAction::execFileInterm()
{
    QUrl url = m_uri;
    char *quote = g_shell_quote(url.path().toUtf8());
    GAppInfo *app_info = g_app_info_create_from_commandline(quote, nullptr, G_APP_INFO_CREATE_NEEDS_TERMINAL, nullptr);
    g_app_info_launch(app_info, nullptr, nullptr, nullptr);
    g_object_unref(app_info);
    g_free(quote);
}
