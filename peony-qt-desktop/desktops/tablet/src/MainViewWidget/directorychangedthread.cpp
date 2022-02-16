/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "directorychangedthread.h"
#include <syslog.h>
#include <QIcon>
#include <QtDBus>
#include "src/UtilityFunction/fulllistview.h"
#include "src/UtilityFunction/fullcommonusewidget.h"
#include <QDebug>

DirectoryChangedThread::DirectoryChangedThread()
{
    m_ukuiMenuInterface=new UkuiMenuInterface;
    QString path=QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    setting=new QSettings(path,QSettings::IniFormat);
}
DirectoryChangedThread::~DirectoryChangedThread()
{
    if(m_ukuiMenuInterface)
        delete m_ukuiMenuInterface;
    if(setting)
        delete setting;
    m_ukuiMenuInterface=nullptr;
    setting=nullptr;
}


void DirectoryChangedThread::run()
{
    QStringList desktopfpList=m_ukuiMenuInterface->getDesktopFilePath();

    qDebug()<<"desktopfpList.count() :UkuiMenuInterface::desktopfpVector.count()"<<desktopfpList.count()<<":"<<UkuiMenuInterface::desktopfpVector.count();
    if(desktopfpList.count() > UkuiMenuInterface::desktopfpVector.count())//有新的应用安装
    {
        qDebug()<<"安装应用";
        QString m_desktopfp;
        for(int i=0;i<desktopfpList.count();i++)
        {

            if(!UkuiMenuInterface::desktopfpVector.contains(desktopfpList.at(i)))
            {
                m_desktopfp=desktopfpList.at(i);
                //获取当前时间戳
                QDateTime dt=QDateTime::currentDateTime();
                int datetime=dt.toTime_t();
                QString str = desktopfpList.at(i).section(' ', 0, 0);
                QStringList list = str.split('/');
                str = list[list.size()-1];
                QString desktopfn=str;
                setting->beginGroup("recentapp");
                setting->setValue(desktopfn,datetime);
                setting->sync();
                setting->endGroup();
                //wgx

                setting->beginGroup("application");
                QStringList applist=setting->allKeys();
                int appnum=setting->allKeys().count();
                int maxindex=0;
                for(int i=0;i<appnum;i++)
                {
                    if(setting->value(applist.at(i)).toInt() >maxindex)
                    {
                        maxindex=setting->value(applist.at(i)).toInt();
                    }
                }
                setting->setValue(desktopfn,maxindex+1);//setting->allKeys().count()
                setting->sync();
                setting->endGroup();
                //
                QString iconstr=m_ukuiMenuInterface->getAppIcon(desktopfpList.at(i));
                syslog(LOG_LOCAL0 | LOG_DEBUG ,"%s",iconstr.toLocal8Bit().data());
                syslog(LOG_LOCAL0 | LOG_DEBUG ,"软件安装desktop文件名：%s",desktopfn.toLocal8Bit().data());
                Q_FOREACH(QString path,QIcon::themeSearchPaths())
                    syslog(LOG_LOCAL0 | LOG_DEBUG ,"%s",path.toLocal8Bit().data());
                break;
            }
        }
        QFileInfo fileInfo(m_desktopfp);
        if(fileInfo.isFile() && fileInfo.exists())
        {
            UkuiMenuInterface::appInfoVector.clear();
            UkuiMenuInterface::appInfoVector=m_ukuiMenuInterface->createAppInfoVector();
            Q_EMIT requestUpdateSignal(m_desktopfp);
        }
    }
    if(desktopfpList.count() < UkuiMenuInterface::desktopfpVector.count())//软件卸载
    {
        qDebug()<<"卸载应用";
        for(int i=0;i<UkuiMenuInterface::desktopfpVector.count();i++)
        {
            if(!desktopfpList.contains(UkuiMenuInterface::desktopfpVector.at(i)))
            {
                QString desktopfp=UkuiMenuInterface::desktopfpVector.at(i);
                QFileInfo fileInfo(desktopfp);
                QString desktopfn=fileInfo.fileName();
                qDebug()<<"卸载"<<desktopfn;
                setting->beginGroup("lockapplication");
                setting->remove(desktopfn);
                setting->sync();
                setting->endGroup();
                setting->beginGroup("application");

                if(!setting->contains(desktopfn))
                {

                    setting->sync();
                    setting->endGroup();
                    break;
                }
                int val=setting->value(desktopfn).toInt();
                qDebug()<<"卸载val"<<val;
                setting->remove(desktopfn);
                QStringList desktopfnList=setting->allKeys();
                for(int i=0;i<desktopfnList.count();i++)
                {
                    if(setting->value(desktopfnList.at(i)).toInt()>val)
                    {
                        setting->setValue(desktopfnList.at(i),setting->value(desktopfnList.at(i)).toInt()-1);
                    }
                }


                setting->sync();
                setting->endGroup();

                setting->beginGroup("recentapp");
                setting->remove(desktopfn);
                //qDebug()<<" desktopfn remove"<<desktopfn;
                setting->sync();
                setting->endGroup();
                syslog(LOG_LOCAL0 | LOG_DEBUG ,"软件卸载desktop文件名：%s",desktopfn.toLocal8Bit().data());

                QDBusInterface iface("com.ukui.panel.desktop",
                                     "/",
                                     "com.ukui.panel.desktop",
                                     QDBusConnection::sessionBus());
                iface.call("RemoveFromTaskbar",desktopfp);
                break;
            }
        }
        UkuiMenuInterface::appInfoVector.clear();
        UkuiMenuInterface::appInfoVector=m_ukuiMenuInterface->createAppInfoVector();
        Q_EMIT deleteAppSignal();

    }
    if(desktopfpList.count() == UkuiMenuInterface::desktopfpVector.count())
    {
        //qDebug()<<"=====";
        Q_EMIT deleteAppSignal();
    }
    qDebug()<<"end";
}
void DirectoryChangedThread::recvDirectoryPath(QString arg)
{
    this->m_path.clear();
    this->m_path=arg;
}
