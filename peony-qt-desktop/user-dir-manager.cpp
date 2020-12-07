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

    m_settings = new QSettings("org.ukui", "peony-qt-preferences", this);
    m_do_not_thumbnail = m_settings->value("do-not-thumbnail").toBool();
    m_user_dir_watcher = new QFileSystemWatcher(this);

    QString path0 = QString("/home/"+m_user_name+"/.config/user-dirs.dirs");
    QString path1 = QString("/home/"+m_user_name+"/.config/org.ukui/peony-qt-preferences.conf");

    m_user_dir_watcher->addPath(path0);
    m_user_dir_watcher->addPath(path1);

    getUserdir();

    connect(m_user_dir_watcher, &QFileSystemWatcher::fileChanged, [=](const QString &uri){
        //user-dirs.dirs
        if(uri == path0)
        {
            getUserdir();
            moveFile();
        }
        //peony-qt-preferences  only thumbnail setting for now.
        else if(uri == path1)
        {
            m_settings = new QSettings("org.ukui", "peony-qt-preferences", this);
            if(m_do_not_thumbnail != m_settings->value("do-not-thumbnail").toBool())
            {
                m_do_not_thumbnail = m_settings->value("do-not-thumbnail").toBool();
                Q_EMIT thumbnailSetingChange();
            }
        }
        m_user_dir_watcher->addPath(uri);

    });

}
// read user-dirs.dirs for all XDG standard path.
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

    //XDG_TEMPLATES_DIR will be used by right click menu.
    GlobalSettings::getInstance()->setValue(TEMPLATES_DIR,m_current_user_dir.value("XDG_TEMPLATES_DIR"));
}

//rename the old paths to overwrire new one.
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
            QDir *lastDir = new QDir(m_last_user_dir.value(i.key()));

            if(lastDir->dirName() == QString("/home/"+m_user_name)||lastDir->isEmpty()||!lastDir->exists())
                continue;

            auto operation = new Peony::FileRenameOperation("file://"+m_last_user_dir.value(i.key()),fileName);
            operation->setAutoOverwrite();
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
