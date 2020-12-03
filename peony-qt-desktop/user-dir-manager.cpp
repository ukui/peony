#include "user-dir-manager.h"
#include "file-operation-manager.h"
#include "file-rename-operation.h"
#include "global-settings.h"
#include "file-info.h"
#include "file-item.h"
#include <stdio.h>
#include <pwd.h>
#include <unistd.h>
#include <gio/gio.h>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>
#include <QSettings>
#include <QTextCodec>

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
    auto settings = new QSettings("/home/"+m_user_name+"/.config/user-dirs.dirs",QSettings::IniFormat);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));

    m_current_user_dir.insert("XDG_DESKTOP_DIR",settings->value(QString("XDG_DESKTOP_DIR")).toString().replace("$HOME","/home/"+m_user_name) + "/");
    m_current_user_dir.insert("XDG_DOWNLOAD_DIR",settings->value(QString("XDG_DOWNLOAD_DIR")).toString().replace("$HOME","/home/"+m_user_name) + "/");
    m_current_user_dir.insert("XDG_TEMPLATES_DIR",settings->value(QString("XDG_TEMPLATES_DIR")).toString().replace("$HOME","/home/"+m_user_name) + "/");
    m_current_user_dir.insert("XDG_PUBLICSHARE_DIR",settings->value(QString("XDG_PUBLICSHARE_DIR")).toString().replace("$HOME","/home/"+m_user_name) + "/");
    m_current_user_dir.insert("XDG_DOCUMENTS_DIR",settings->value(QString("XDG_DOCUMENTS_DIR")).toString().replace("$HOME","/home/"+m_user_name) + "/");
    m_current_user_dir.insert("XDG_MUSIC_DIR",settings->value(QString("XDG_MUSIC_DIR")).toString().replace("$HOME","/home/"+m_user_name)+"/");
    m_current_user_dir.insert("XDG_PICTURES_DIR",settings->value(QString("XDG_PICTURES_DIR")).toString().replace("$HOME","/home/"+m_user_name) + "/");
    m_current_user_dir.insert("XDG_VIDEOS_DIR",settings->value(QString("XDG_VIDEOS_DIR")).toString().replace("$HOME","/home/"+m_user_name) + "/");

    GlobalSettings::getInstance()->setValue(TEMPLATES_DIR,m_current_user_dir.value("XDG_TEMPLATES_DIR"));
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
