#include "user-dir-manager.h"
#include "file-operation-manager.h"
#include "file-rename-operation.h"
#include "file-info.h"
#include "file-item.h"
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <gio/gio.h>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>

using namespace Peony;
UserdirManager::UserdirManager(QObject *parent) : QObject(parent)
{
    struct passwd *pwd;
    pwd=getpwuid(getuid());
    auto userName = pwd->pw_name;
    m_user_name = QString(userName);
    m_user_dir_watcher = new QFileSystemWatcher(this);

    QString path = QString("/home/"+m_user_name+"/.config/user-dirs.dirs");
    m_user_dir_watcher->addPath(path);

    getUserdir();

    connect(m_user_dir_watcher, &QFileSystemWatcher::fileChanged, [=](const QString &uri){
        getUserdir();
        moveFile();
        m_user_dir_watcher->addPath(path);

    });

}

void UserdirManager::getUserdir()
{
    if(!m_current_user_dir.isEmpty())
    {
        m_last_user_dir.clear();
        m_last_user_dir = m_current_user_dir;
        m_current_user_dir.clear();
    }
    m_current_user_dir.insert("XDG_DESKTOP_DIR",QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/");
    m_current_user_dir.insert("XDG_DOWNLOAD_DIR",QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/");
//    m_current_user_dir.insert("XDG_TEMPLATES_DIR","file://" + QString(g_get_user_special_dir(G_USER_DIRECTORY_TEMPLATES)) + "/");
//    m_current_user_dir.insert("XDG_PUBLICSHARE_DIR","file://" + QString(g_get_user_special_dir(G_USER_DIRECTORY_PUBLIC_SHARE)) + "/");
    m_current_user_dir.insert("XDG_DOCUMENTS_DIR",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/");
    m_current_user_dir.insert("XDG_MUSIC_DIR",QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+"/");
    m_current_user_dir.insert("XDG_PICTURES_DIR",QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/");
    m_current_user_dir.insert("XDG_VIDEOS_DIR",QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/");

}

void UserdirManager::moveFile()
{
    QMap<QString, QString>::const_iterator i;
    auto fileOpMgr = FileOperationManager::getInstance();
    for(i=m_current_user_dir.constBegin();i!=m_current_user_dir.constEnd();++i)
    {
        if(i.value()!=m_last_user_dir.value(i.key()))
        {
            //rename the old dir
            QDir *dir = new QDir(i.value());
            auto fileName = dir->dirName();

            if(!QDir(m_last_user_dir.value(i.key())).exists())
                continue;

            auto operation = new Peony::FileRenameOperation("file://"+m_last_user_dir.value(i.key()),fileName);
            operation->setAutoDelete();
            fileOpMgr->startOperation(operation,false);
            if(i.key() == "XDG_DESKTOP_DIR")
            {
                QTimer::singleShot(500,[=](){
                    //refresh desktop
                    Q_EMIT desktopDirChanged();
                });
            }
        }
    }
}
