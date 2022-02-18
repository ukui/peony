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
#include "file-utils.h"
#include "file-operation-utils.h"
#include "audio-play-manager.h"

#include <QMessageBox>
#include <QPushButton>
#include <QDir>
#include <QUrl>
#include <QFile>
#include <QProcess>
#include <QtDBus/QtDBus>
#include <recent-vfs-manager.h>
#include <QApplication>

#include <ukuisdk/kylin-com4cxx.h>
#include <QDebug>
#include <QtX11Extras/QX11Info>
#include <kstartupinfo.h>

using namespace Peony;

#define USE_STARTUP_INFO true

FileLaunchAction::FileLaunchAction(const QString &uri, GAppInfo *app_info, bool forceWithArg, QObject *parent) : QAction(parent)
{
    if(uri.startsWith("recent:///"))
        m_uri = FileUtils::getTargetUri(uri);
    else
        m_uri = uri;

    m_app_info = static_cast<GAppInfo*>(g_object_ref(app_info));
    m_force_with_arg = forceWithArg;

    if (!isValid())
        return;

    GIcon *icon = g_app_info_get_icon(m_app_info);
    const char * const * icon_names = g_themed_icon_get_names(G_THEMED_ICON (icon));

    if (icon_names) {
        m_icon = QIcon::fromTheme(*icon_names);
    } else {
        // fix #68592
        g_autofree gchar *icon_path = g_icon_to_string(icon);
        m_icon.addFile(icon_path);
    }
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

    bool readable = fileInfo->canRead();
    if (!readable) {
        QMessageBox::critical(0, tr("No Permission"), tr("File is not readable. Please check if file has read permisson."));
        return;
    }

    if(fileInfo->isExecDisable())return;

    if (fileInfo->type() == "application/x-desktop") {
        //! \note Sometimes garbled characters appear when QSettings reads Chinese,
        //! even though QSettings::setIniCodec("UTF8") is used
        GKeyFileFlags flags=G_KEY_FILE_NONE;
        GKeyFile* keyfile=g_key_file_new ();
        QByteArray fpbyte=fileInfo->filePath().toLocal8Bit();
        const char* filepath=fpbyte.constData();
        g_key_file_load_from_file(keyfile, filepath, flags, nullptr);
        char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Exec", nullptr, nullptr);
        QString exe=QString::fromLocal8Bit(name);
        g_key_file_free(keyfile);
        g_free(name);

        if (exe.isEmpty()) {
            qDebug() << "Get desktop file Exec value error";
            return;
        }

        QStringList parameters;

        // 首先把exec整个截取成 path+parameter形式
        if (exe.contains(" ")) {
//            parameters = exe.split(" ");
            //排除参数之间多个空格分隔的情况
            parameters = exe.split(QRegExp("\\s+"));
            exe = parameters[0];
            parameters.removeAt(0);
        }

        // 优先判断path里有没有带%U等，如果存在的话，删除%和后面紧跟的字符
        if (exe.contains("%")) {
            exe = exe.left(exe.indexOf("%"));
        }

        for (auto begin = parameters.begin(); begin != parameters.end(); ++begin) {
            if (begin->contains("%")) {
                // 命令行最多可包含一个％f，％u，％F或％U字段代码
                if (begin->count() == 2)
                    parameters.removeOne(*begin);
                else {
                    begin->remove(begin->indexOf("%"), 2);
                }
                break;
            }
        }

        QDBusInterface session("org.gnome.SessionManager", "/com/ukui/app", "com.ukui.app");
        session.call("app_open", exe, parameters);
        return;
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

#if USE_STARTUP_INFO
void pid_callback(GDesktopAppInfo *appinfo, GPid pid, gpointer user_data) {
    KStartupInfoId* startInfoId = static_cast<KStartupInfoId*>(user_data);
    if (!startInfoId)
        return;

    KStartupInfoData data;
    data.addPid(pid);
    data.setIconGeometry(QRect(0, 0, 1, 1));  // ugly

    KStartupInfo::sendChange(*startInfoId, data);
    KStartupInfo::resetStartupEnv();
    delete startInfoId;
}
#endif

void FileLaunchAction::lauchFileAsync(bool forceWithArg, bool skipDialog)
{
    //FIXME: replace BLOCKING api in ui thread.
    auto fileInfo = FileInfo::fromUri(m_uri);
    if (fileInfo->isEmptyInfo()) {
        FileInfoJob j(fileInfo);
        j.querySync();
    }

    bool readable = fileInfo->canRead();
    if (!readable) {
        QMessageBox::critical(0, tr("No Permission"), tr("File is not readable. Please check if file has read permisson."));
        return;
    }

    //TODO 修改为通用接口
    bool intel = (QString::compare("V10SP1-edu", QString::fromStdString(KDKGetPrjCodeName()), Qt::CaseInsensitive) == 0);
    if (intel) {
        //intel应用禁用
        if (fileInfo->isExecDisable()) return;

        if (isDesktopFileAction()) {

            //! \note Sometimes garbled characters appear when QSettings reads Chinese,
            //! even though QSettings::setIniCodec("UTF8") is used
            GKeyFileFlags flags=G_KEY_FILE_NONE;
            GKeyFile* keyfile=g_key_file_new ();
            QByteArray fpbyte=fileInfo->filePath().toLocal8Bit();
            const char* filepath=fpbyte.constData();
            g_key_file_load_from_file(keyfile, filepath, flags, nullptr);
            char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Exec", nullptr, nullptr);
            QString exe=QString::fromLocal8Bit(name);
            g_key_file_free(keyfile);
            g_free(name);

            if (exe.isEmpty()) {
                qDebug() << "Get desktop file Exec value error";
                return;
            }

            QStringList parameters;
            // 首先把exec整个截取成 path+parameter形式
            if (exe.contains(" ")) {
//            parameters = exe.split(" ");
                //排除参数之间多个空格分隔的情况
                parameters = exe.split(QRegExp("\\s+"));
                exe = parameters[0];
                parameters.removeAt(0);
            }

            // 优先判断path里有没有带%U等，如果存在的话，删除%和后面紧跟的字符
            if (exe.contains("%")) {
                exe = exe.left(exe.indexOf("%"));
            }

            for (auto begin = parameters.begin(); begin != parameters.end(); ++begin) {
                if (begin->contains("%")) {
                    // 命令行最多可包含一个％f，％u，％F或％U字段代码
                    if (begin->count() == 2)
                        parameters.removeOne(*begin);
                    else {
                        begin->remove(begin->indexOf("%"), 2);
                    }
                    break;
                }
            }

            QDBusInterface session("org.gnome.SessionManager", "/com/ukui/app", "com.ukui.app");
            session.call("app_open", exe, parameters);
            return;
        }
    }

    bool executable = fileInfo->canExecute();
    bool isAppImage = fileInfo->type() == "application/vnd.appimage";
    bool isExecutable = isExcuteableFile(fileInfo->type());
    qDebug() <<"executable:" <<executable <<isAppImage <<fileInfo->type();

    QUrl url = m_uri;
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
                                  tr("Can not open %1, Please confirm you have the right authority.").arg(url.toDisplayString()));
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
        } else {
            QUrl url = m_uri;
            if(!QFile(url.path()).exists())
            {
                QMessageBox::warning(nullptr,
                                     tr("Error"),
                                     tr("File original path not exist, are you deleted or moved it?"));
                return;
            }

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

#if USE_STARTUP_INFO
    // send startup info to kwindowsystem
    bool needCleanStartInfoId = true;
    quint32 timeStamp = QX11Info::isPlatformX11() ? QX11Info::appUserTime() : 0;
    KStartupInfoId* startInfoId = new KStartupInfoId();
    startInfoId->initId(KStartupInfo::createNewStartupIdForTimestamp(timeStamp));
    startInfoId->setupStartupEnv();
    KStartupInfoData data;
    data.setHostname();
    float scale = qApp->devicePixelRatio();
    QRect rect = fileInfo.get()->property("iconGeometry").toRect();
    rect.moveTo(rect.x() * scale, rect.y() * scale);
    if (rect.isValid())
        data.setIconGeometry(rect);
    data.setLaunchedBy(getpid());

    KStartupInfo::sendStartup(*startInfoId, data);
#endif

    if (isDesktopFileAction() && !forceWithArg) {
#if USE_STARTUP_INFO
        needCleanStartInfoId = !g_desktop_app_info_launch_uris_as_manager(G_DESKTOP_APP_INFO(m_app_info), nullptr, nullptr,
                                                  GSpawnFlags::G_SPAWN_DEFAULT, nullptr, nullptr,
                                                  pid_callback, (gpointer)startInfoId, nullptr);
#elif GLIB_CHECK_VERSION(2, 60, 0)
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
#if USE_STARTUP_INFO
        needCleanStartInfoId = !g_desktop_app_info_launch_uris_as_manager(G_DESKTOP_APP_INFO(m_app_info), l, nullptr, 
                                                  GSpawnFlags::G_SPAWN_DEFAULT, nullptr, nullptr, 
                                                  pid_callback, (gpointer)startInfoId, nullptr);
        RecentVFSManager::getInstance()->insert(fileInfo.get()->uri(), fileInfo.get()->mimeType(), fileInfo.get()->displayName(), g_app_info_get_name(m_app_info));
#elif GLIB_CHECK_VERSION(2, 60, 0)
        g_app_info_launch_uris_async(m_app_info, l,
                                     nullptr, nullptr,
                                     nullptr, nullptr);

        RecentVFSManager::getInstance()->insert(fileInfo.get()->uri(), fileInfo.get()->mimeType(), fileInfo.get()->displayName(), g_app_info_get_name(m_app_info));
#else
        g_app_info_launch_uris(m_app_info, l, nullptr, nullptr);
#endif
        g_list_free_full(l, g_free);
#if USE_STARTUP_INFO
        if (needCleanStartInfoId && startInfoId)
            delete startInfoId;
#endif
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
    QString newDir = m_uri.section('/',0,m_uri.count('/')-1);
    GAppInfo *app_info = g_app_info_create_from_commandline(quote, nullptr, G_APP_INFO_CREATE_NONE, nullptr);
    QDir::setCurrent(QUrl(newDir).path());
    g_app_info_launch(app_info, nullptr, nullptr, nullptr);
    QDir::setCurrent(QDir::homePath());
    g_object_unref(app_info);
    g_free(quote);
}

void FileLaunchAction::execFileInterm()
{
    QUrl url = m_uri;
    char *quote = g_shell_quote(url.path().toUtf8());
    QString newDir = m_uri.section('/',0,m_uri.count('/')-1);
    GAppInfo *app_info = g_app_info_create_from_commandline(quote, nullptr, G_APP_INFO_CREATE_NEEDS_TERMINAL, nullptr);
    QDir::setCurrent(QUrl(newDir).path());
    g_app_info_launch(app_info, nullptr, nullptr, nullptr);
    QDir::setCurrent(QDir::homePath());
    g_object_unref(app_info);
    g_free(quote);
}
