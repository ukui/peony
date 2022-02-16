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

#include "ukuimenuinterface.h"
#include <glib.h>
#include <QDir>
#include <QDebug>
#include <QCollator>
#include <QLocale>
#include <QStringList>
#include "ukuichineseletter.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
//#include <QSqlTableMode>

struct installedapp
{
    QString applist;
    int retnum;
};
UkuiMenuInterface::UkuiMenuInterface()
{
    QString path = QDir::homePath()+"/.config/ukui/ukui-menu.ini";
    setting = new QSettings(path,QSettings::IniFormat);

    QString syspath = QDir::homePath()+"/.config/ukui/menusysapplist.ini";
    syssetting = new QSettings(syspath,QSettings::IniFormat);
}

QVector<QStringList> UkuiMenuInterface::appInfoVector=QVector<QStringList>();
QVector<QString> UkuiMenuInterface::desktopfpVector=QVector<QString>();
QVector<QStringList> UkuiMenuInterface::alphabeticVector=QVector<QStringList>();
QVector<QStringList> UkuiMenuInterface::functionalVector=QVector<QStringList>();
QVector<QString> UkuiMenuInterface::allAppVector=QVector<QString>();
QStringList UkuiMenuInterface::androidDesktopfnList=QStringList();

UkuiMenuInterface::~UkuiMenuInterface()
{
    if(setting)
        delete setting;
    if(syssetting)
        delete syssetting;
    setting=nullptr;
    syssetting=nullptr;
}

//文件递归查询
void UkuiMenuInterface::recursiveSearchFile(const QString& _filePath)
{
    GError** error = nullptr;
    GKeyFileFlags flags = G_KEY_FILE_NONE;
    GKeyFile* keyfile = g_key_file_new();

    QDir dir(_filePath);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    list.removeAll(QFileInfo("/usr/share/applications/screensavers"));
    if(list.size() < 1 ) {
        return;
    }
    int i = 0;

    //递归算法的核心部分
    do{
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool isDir = fileInfo.isDir();
        if(isDir) {
            recursiveSearchFile(fileInfo.filePath());
        }
        else{
            //过滤后缀不是.desktop的文件
            QString filePathStr=fileInfo.filePath();
            if(!filePathStr.endsWith(".desktop"))
            {
                i++;
                continue;
            }
            QByteArray fpbyte=filePathStr.toLocal8Bit();
            char* filepath=fpbyte.data();
            g_key_file_load_from_file(keyfile,filepath,flags,error);
            char* ret_1=g_key_file_get_locale_string(keyfile,"Desktop Entry","NoDisplay", nullptr, nullptr);
            if(ret_1!=nullptr)
            {
                QString str=QString::fromLocal8Bit(ret_1);
                if(str.contains("true"))
                {
                    i++;
                    continue;
                }
            }
            char* ret_2=g_key_file_get_locale_string(keyfile,"Desktop Entry","NotShowIn", nullptr, nullptr);
            if(ret_2!=nullptr)
            {
                QString str=QString::fromLocal8Bit(ret_2);
                if(str.contains("UKUI"))
                {
                    i++;
                    continue;
                }
            }

            //过滤LXQt、KDE
            char* ret=g_key_file_get_locale_string(keyfile,"Desktop Entry","OnlyShowIn", nullptr, nullptr);
            if(ret!=nullptr)
            {
                QString str=QString::fromLocal8Bit(ret);
                if(str.contains("LXQt") || str.contains("KDE"))
                {
                    i++;
                    continue;
                }
            }
            //过滤中英文名为空的情况
            QLocale cn;
            QString language=cn.languageToString(cn.language());
            if(QString::compare(language,"Chinese")==0)
            {
                char* nameCh=g_key_file_get_string(keyfile,"Desktop Entry","Name[zh_CN]", nullptr);
                char* nameEn=g_key_file_get_string(keyfile,"Desktop Entry","Name", nullptr);
                if(QString::fromLocal8Bit(nameCh).isEmpty() && QString::fromLocal8Bit(nameEn).isEmpty())
                {
                    i++;
                    continue;
                }
            }
            else {
                char* name=g_key_file_get_string(keyfile,"Desktop Entry","Name", nullptr);
                if(QString::fromLocal8Bit(name).isEmpty())
                {
                    i++;
                    continue;
                }
            }

            filePathList.append(filePathStr);
        }
        i++;

    } while(i < list.size());

    g_key_file_free(keyfile);

}

//获取系统desktop文件路径
QStringList UkuiMenuInterface::getDesktopFilePath()
{
    filePathList.clear();
    //getAndroidApp();
    recursiveSearchFile("/usr/share/applications/");
    //安卓app path
    getAndroidApp();
    filePathList.removeAll("/usr/share/applications/software-properties-livepatch.desktop");
    filePathList.removeAll("/usr/share/applications/mate-color-select.desktop");
    filePathList.removeAll("/usr/share/applications/blueman-adapters.desktop");
    filePathList.removeAll("/usr/share/applications/blueman-manager.desktop");
    filePathList.removeAll("/usr/share/applications/mate-user-guide.desktop");
    filePathList.removeAll("/usr/share/applications/nm-connection-editor.desktop");
    filePathList.removeAll("/usr/share/applications/debian-uxterm.desktop");
    filePathList.removeAll("/usr/share/applications/debian-xterm.desktop");
    filePathList.removeAll("/usr/share/applications/im-config.desktop");
    filePathList.removeAll("/usr/share/applications/fcitx.desktop");
    filePathList.removeAll("/usr/share/applications/fcitx-configtool.desktop");
    filePathList.removeAll("/usr/share/applications/onboard-settings.desktop");
    filePathList.removeAll("/usr/share/applications/info.desktop");
    filePathList.removeAll("/usr/share/applications/ukui-power-preferences.desktop");
    filePathList.removeAll("/usr/share/applications/ukui-power-statistics.desktop");
    filePathList.removeAll("/usr/share/applications/software-properties-drivers.desktop");
    filePathList.removeAll("/usr/share/applications/software-properties-gtk.desktop");
    filePathList.removeAll("/usr/share/applications/gnome-session-properties.desktop");
    filePathList.removeAll("/usr/share/applications/org.gnome.font-viewer.desktop");
    filePathList.removeAll("/usr/share/applications/xdiagnose.desktop");
    filePathList.removeAll("/usr/share/applications/gnome-language-selector.desktop");
    filePathList.removeAll("/usr/share/applications/mate-notification-properties.desktop");
    filePathList.removeAll("/usr/share/applications/transmission-gtk.desktop");
    filePathList.removeAll("/usr/share/applications/mpv.desktop");
    filePathList.removeAll("/usr/share/applications/system-config-printer.desktop");
    filePathList.removeAll("/usr/share/applications/org.gnome.DejaDup.desktop");
    filePathList.removeAll("/usr/share/applications/yelp.desktop");
    filePathList.removeAll("/usr/share/applications/peony-computer.desktop");
    filePathList.removeAll("/usr/share/applications/peony-home.desktop");
    filePathList.removeAll("/usr/share/applications/peony-trash.desktop");
    //filePathList.removeAll("/usr/share/applications/peony.desktop");
    //*过滤*//
    filePathList.removeAll("/usr/share/applications/recoll-searchgui.desktop");
    filePathList.removeAll("/usr/share/applications/ukui-about.desktop");
    filePathList.removeAll("/usr/share/applications/org.gnome.dfeet.desktop");
    filePathList.removeAll("/usr/share/applications/ukui-feedback.desktop");
    filePathList.removeAll("/usr/share/applications/users.desktop");
//    filePathList.removeAll("/usr/share/applications/mate-terminal.desktop");
    filePathList.removeAll("/usr/share/applications/vim.desktop");
    filePathList.removeAll("/usr/share/applications/mpv.desktop");

    filePathList.removeAll("/usr/share/applications/engrampa.desktop");
    filePathList.removeAll("/usr/share/applications/hp-document.desktop");
//    filePathList.removeAll("/usr/share/applications/kylin-user-guide.desktop");

    filePathList.removeAll("/usr/share/applications/wps-office-prometheus.desktop");
//    filePathList.removeAll("/usr/share/applications/indicator-china-weather.desktop");    //禁用麒麟天气

    //v10
    filePathList.removeAll("/usr/share/applications/mate-about.desktop");
    filePathList.removeAll("/usr/share/applications/time.desktop");
    filePathList.removeAll("/usr/share/applications/network.desktop");
    filePathList.removeAll("/usr/share/applications/shares.desktop");
    filePathList.removeAll("/usr/share/applications/mate-power-statistics.desktop");
    filePathList.removeAll("/usr/share/applications/display-im6.desktop");
    filePathList.removeAll("/usr/share/applications/display-im6.q16.desktop");
    filePathList.removeAll("/usr/share/applications/openjdk-8-policytool.desktop");
    filePathList.removeAll("/usr/share/applications/kylin-io-monitor.desktop");
    filePathList.removeAll("/usr/share/applications/wps-office-uninstall.desktop");
    filePathList.removeAll("/usr/share/applications/wps-office-misc.desktop");
    /*加入的*/
    filePathList.append("/usr/share/applications/mate-calc.desktop");

    filePathList=getInstalledAppList();
    return filePathList;
}
QStringList UkuiMenuInterface::getInstalledAppList()//获取已安装应用列表
{
    /*显示的应用列表*/
    //1、系统默认应用
    //2、腾讯应用与安装的系统应用
    //3、考虑用户隔离
    //应用列表的所有应用由：系统应用+应用商店安装了的应用

//    QDBusInterface desktopfpListiface("cn.kylinos.SSOBackend",
//                             "/cn/kylinos/SSOBackend",
//                             "cn.kylinos.SSOBackend.applications",
//                             QDBusConnection::systemBus());
//    QString username=getUserName();
//    QDBusReply<QString> reply = desktopfpListiface.call("GetDesktopAppList",username);




    //1、获取系统应用列表
    //filePathList;
    QStringList ifFileDesktopList;
    /*新的应用列表*/
    qDebug() << "sysapplistnum初始化默认应用列表1" << filePathList.count();
    for(int i=0;i<filePathList.count();i++)//过滤 得到真实存在的应用
    {
        QString tmp=filePathList.at(i);
        QFileInfo fileInfo(tmp);
        if(!fileInfo.isFile())//判断是否存在
        {
            //qDebug()<<tmp;
            continue;
        }

        ifFileDesktopList.append(tmp);
    }
//ifFileDesktopList  所有当前存在的应用

    /*得到系统默认应用*/
    //判断当前是否已经得到了默认应用
    syssetting->beginGroup("ukui-menu-sysapplist");
    int sysapplistnum=syssetting->allKeys().count();
    syssetting->sync();
    syssetting->endGroup();


    //qDebug()<<"sysapplistnum初始化默认应用列表3"<<ifFileDesktopList.count();
    int num = ifFileDesktopList.count();
    for(int i = 0;i<num;i++)
    {
        QString tmp = ifFileDesktopList.at(i);
        qDebug() << "filePathList.at(i)"<< ifFileDesktopList.at(i) << tmp.indexOf("tencent");
        QString str = ifFileDesktopList.at(i);
        QStringList list = str.split('/');
        str = list[list.size()-1];

        if(tmp.indexOf("tencent")==-1)//所有不是腾讯的系统应用7
        {
            if(sysapplistnum == 0)//没有初始化默认应用列表
            {
                syssetting->beginGroup("ukui-menu-sysapplist");
                qDebug()<<"isnottencent"<<str;
                syssetting->setValue(str,0);
                syssetting->sync();
                syssetting->endGroup();
            }

        }/*else{


            //用户隔离/etc/skel/桌面
            QString tmp=QString("%1%2").arg("/usr/share/applications/").arg(str);
            QString appid=getTencentAppid(tmp);
            qDebug()<<"appid"<<tmp<<appid;
            if(reply.value().indexOf(appid)!=-1)
            {
                qDebug()<<"当前用户可见的腾讯应用"<<tmp;
                //ifFileDesktopList.append(QString("%1%2").arg("/usr/share/applications/").arg(str));//tmp
            }else{
                ifFileDesktopList.removeAll(tmp);
            }
        }*/
    }


    filePathList.clear();
    return ifFileDesktopList;
}
//创建应用信息容器
QVector<QStringList> UkuiMenuInterface::createAppInfoVector()
{
    desktopfpVector.clear();
    QVector<QStringList> appInfoVector;
    QVector<QStringList> vector;
    vector.append(QStringList()<<"Android");//0安卓
    vector.append(QStringList()<<"Network");//1网络
    vector.append(QStringList()<<"Messaging");//2社交
    vector.append(QStringList()<<"Audio"<<"Video");//3影音
    vector.append(QStringList()<<"Development");//4开发
    vector.append(QStringList()<<"Graphics");//5图像
    vector.append(QStringList()<<"Game");//6游戏
    vector.append(QStringList()<<"Office"<<"Calculator"<<"Spreadsheet"<<"Presentation"<<"WordProcessor"<<"TextEditor");//7办公
    vector.append(QStringList()<<"Education");//8教育
    vector.append(QStringList()<<"System"<<"Settings"<<"Security");//9系统

    QStringList desktopfpList = getDesktopFilePath();
    for(int i = 0; i < desktopfpList.count(); i++)
    {
        QStringList appInfoList;
        QString desktopfp=desktopfpList.at(i);
        QString name=getAppName(desktopfpList.at(i));
        QString englishName=getAppEnglishName(desktopfpList.at(i));
        QString letter=getAppNameInitial(desktopfpList.at(i));
        QString letters=getAppNameInitials(desktopfpList.at(i));

        desktopfpVector.append(desktopfp);

        appInfoList<<desktopfp<<name<<englishName<<letter<<letters;
        bool is_owned=false;
        for(int j=0;j<vector.size();j++)
        {
            if(matchingAppCategories(desktopfpList.at(i),vector.at(j)))//有对应分类
            {
                is_owned=true;
                appInfoList.append(QString::number(j));
            }
        }
        if(!is_owned)//该应用无对应分类
            appInfoList.append(QString::number(10));

        appInfoVector.append(appInfoList);
    }

    return appInfoVector;
}

//获取tencent应用名
QString UkuiMenuInterface::getTencentAppid(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* Appid=g_key_file_get_locale_string(keyfile,"Desktop Entry","Appid", nullptr, nullptr);
    g_key_file_free(keyfile);
    return QString::fromLocal8Bit(Appid);
}
//获取应用名称
QString UkuiMenuInterface::getAppName(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);

    char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Name", nullptr, nullptr);
    QString namestr=QString::fromLocal8Bit(name);

    g_key_file_free(keyfile);
    return namestr;
}

//获取英应用英文名
QString UkuiMenuInterface::getAppEnglishName(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* name=g_key_file_get_string(keyfile,"Desktop Entry","Name", nullptr);
    QString namestr=QString::fromLocal8Bit(name);
    g_key_file_free(keyfile);
    return namestr;
}

//获取应用分类
QString UkuiMenuInterface::getAppCategories(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* category=g_key_file_get_locale_string(keyfile,"Desktop Entry","Categories", nullptr, nullptr);
    g_key_file_free(keyfile);
    return QString::fromLocal8Bit(category);
}

//获取应用图标
QString UkuiMenuInterface::getAppIcon(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* icon=g_key_file_get_locale_string(keyfile,"Desktop Entry","Icon", nullptr, nullptr);
    g_key_file_free(keyfile);
    return QString::fromLocal8Bit(icon);
}

//获取应用命令
QString UkuiMenuInterface::getAppExec(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* exec=g_key_file_get_locale_string(keyfile,"Desktop Entry","Exec", nullptr, nullptr);
    g_key_file_free(keyfile);
    return QString::fromLocal8Bit(exec);
}

//获取应用注释
QString UkuiMenuInterface::getAppComment(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* comment=g_key_file_get_locale_string(keyfile,"Desktop Entry","Comment", nullptr, nullptr);
    g_key_file_free(keyfile);
    return QString::fromLocal8Bit(comment);

}

//获取应用类型
QString UkuiMenuInterface::getAppType(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);
    char* type=g_key_file_get_locale_string(keyfile,"Desktop Entry","Type", nullptr, nullptr);
    g_key_file_free(keyfile);
    return QString::fromLocal8Bit(type);
}
bool UkuiMenuInterface::cmpApp(QStringList &arg_1, QStringList &arg_2)//更改排序算法，由中英文字母键值从小到大
{
    QLocale local;
    QString language=local.languageToString(local.language());
    if(QString::compare(language,"Chinese")==0)
        local=QLocale(QLocale::Chinese);
    else
        local=QLocale(QLocale::English);
    QCollator collator(local);
    if(collator.compare(arg_1.at(1),arg_2.at(1))<0)
         return true;
    else
         return false;
}
bool UkuiMenuInterface::initAppIni()
{
    QVector<QStringList> appInitVector;
    QVector<QStringList> tencentInitVector;
    QVector<QStringList> customizedVector;
    QVector<QStringList> thirdPartyVector;

    QString tencent_math="/usr/share/applications/tencent-math-precise-practice.desktop";
    QString tencent_chinese="/usr/share/applications/tencent-chinese-precise-practice.desktop";
    QString tencent_english="/usr/share/applications/tencent-english-precise-practice.desktop";
    QVector<QStringList> precise_practiceVector;
    QStringList math;
    QStringList english;
    QStringList chainese;

    setting->beginGroup("application");
    QStringList desktopfnList = setting->allKeys();
    setting->endGroup();
    if(desktopfnList.count() == 0)
    {
        for(int i = 0; i < desktopfpVector.count(); i++)
        {
            //qDebug()<<"appInfoVector"<<appInfoVector.count()<<i;
            QString tmp = appInfoVector.at(i).at(0);

             if(tmp.indexOf("tencent")!=-1)
             {
                 if(tmp==tencent_math )
                 {
                     precise_practiceVector.append(appInfoVector.at(i));
                     math=appInfoVector.at(i);

                     continue;
                 }
                 if(tmp==tencent_chinese )
                 {
                     precise_practiceVector.append(appInfoVector.at(i));

                     chainese=appInfoVector.at(i);
                     continue;
                 }
                 if(tmp==tencent_english )
                 {
                     precise_practiceVector.append(appInfoVector.at(i));
                     english=appInfoVector.at(i);
                     continue;
                 }
                tencentInitVector.append(appInfoVector.at(i));//所有是腾讯的系统应用
             }
             else if(tmp.indexOf("wps")!=-1)
             {
                thirdPartyVector.append(appInfoVector.at(i));
             }
             else if(tmp.indexOf("eye")!=-1)
             {
                 customizedVector.append(appInfoVector.at(i));
             }
             else if(tmp.indexOf("mdm")!=-1)
             {
                 customizedVector.append(appInfoVector.at(i));
             }
             else
             {
                 appInitVector.append(appInfoVector.at(i));
             }
        }
        qSort(appInitVector.begin(),appInitVector.end(),cmpApp);//按中英文字母排序
        qSort(tencentInitVector.begin(),tencentInitVector.end(),cmpApp);//按中英文字母排序
        //腾讯应用的精准类应用处理
        if(precise_practiceVector.contains(english))
        {
             tencentInitVector.insert(0,english);
        }
        if(precise_practiceVector.contains(math) )
        {
             tencentInitVector.insert(0,math);
        }
        if(precise_practiceVector.contains(chainese) )
        {
            tencentInitVector.insert(0,chainese);
        }

        qSort(customizedVector.begin(),customizedVector.end(),cmpApp);//按中英文字母排序
        qSort(thirdPartyVector.begin(),thirdPartyVector.end(),cmpApp);//按中英文字母排序


        setting->beginGroup("application");
        for(int i=0;i<tencentInitVector.count();i++)
        {
            QString str = tencentInitVector.at(i).at(0).section(' ', 0, 0);
            QStringList list = str.split('/');
            str = list[list.size()-1];
            //qDebug()<<str;
            setting->setValue(str,i);
        }
        int a=tencentInitVector.count();

        for(int i=0;i<customizedVector.count();i++)
        {
            QString str = customizedVector.at(i).at(0).section(' ', 0, 0);
            QStringList list = str.split('/');
            str = list[list.size()-1];
            setting->setValue(str,i+a);
        }
        int b=customizedVector.count();

        for(int i=0;i<thirdPartyVector.count();i++)
        {
            QString str = thirdPartyVector.at(i).at(0).section(' ', 0, 0);
            QStringList list = str.split('/');
            str = list[list.size()-1];
            setting->setValue(str,i+a+b);
        }

        int c=thirdPartyVector.count();
        for(int i=0;i<appInitVector.count();i++)//赋值
        {
            QString str = appInitVector.at(i).at(0).section(' ', 0, 0);
            QStringList list = str.split('/');
            str = list[list.size()-1];
            setting->setValue(str,i+a+b+c);
        }
        setting->sync();
        setting->endGroup();
        return 1;
    }
    return 0;

}
QVector<QString> UkuiMenuInterface::getAllApp()
{
    QVector<QString> allAppVector;
    QVector<QString> commonVector;
    QVector<QString> lockVector;
   // QVector<QStringList> appVector;
    allAppVector.clear();
    commonVector.clear();
    lockVector.clear();
    //appVector.clear();
    commonVector=getCommonUseApp();
    lockVector=getLockApp();

    //qSort(appVector.begin(),appVector.end(),cmpApp);//按中英文字母排序
    Q_FOREACH(QString desktopfp, lockVector)//lockapp
        allAppVector.append(desktopfp);

    Q_FOREACH(QString desktopfp, commonVector)
    {
        if(!lockVector.contains(desktopfp))//在上锁配置文件里有没有
        {
            allAppVector.append(desktopfp);
        }
    }

    return allAppVector;//装有所有已经排好序的APP列表
}
QVector<QString> UkuiMenuInterface::getLockApp()
{
    setting->beginGroup("lockapplication");
    QStringList lockdesktopfnList=setting->allKeys();
    for(int i=0;i<lockdesktopfnList.count()-1;i++)//冒泡排序锁住的应用从小到大排列lockdesktopfnList
        for(int j=0;j<lockdesktopfnList.count()-1-i;j++)
        {
            int value_1=setting->value(lockdesktopfnList.at(j)).toInt();
            int value_2=setting->value(lockdesktopfnList.at(j+1)).toInt();
            if(value_1 > value_2)
            {
                QString tmp=lockdesktopfnList.at(j);
                lockdesktopfnList.replace(j,lockdesktopfnList.at(j+1));
                lockdesktopfnList.replace(j+1,tmp);

            }
        }
    setting->endGroup();
    QVector<QString> data;
    Q_FOREACH(QString desktopfn,lockdesktopfnList)
    {
        QString desktopfp;
        if(androidDesktopfnList.contains(desktopfn))//如果锁的应用在安卓列表
            desktopfp=QString(QDir::homePath()+"/.local/share/applications/"+desktopfn);
        else
            desktopfp=QString("/usr/share/applications/"+desktopfn);
//        QFileInfo fileInfo(desktopfp);
//        if(!fileInfo.isFile())//判断是否存在
//            continue;
        data.append(desktopfp);//加入data
    }
     return data;
}
QVector<QString> UkuiMenuInterface::getCommonUseApp()//常用的app
{
    initAppIni();
    //qDebug()<<"getCommonUseApp";
    setting->sync();
    setting->beginGroup("application");
    QStringList desktopfnList=setting->allKeys();
    for(int i=0;i<desktopfnList.count()-1;i++)
        for(int j=0;j<desktopfnList.count()-1-i;j++)//冒泡排序常用的应用从大到小排列desktopfnList
        {
            int value_1=setting->value(desktopfnList.at(j)).toInt();
            int value_2=setting->value(desktopfnList.at(j+1)).toInt();
            if(value_1 > value_2)
            {
                QString tmp=desktopfnList.at(j);
                desktopfnList.replace(j,desktopfnList.at(j+1));
                desktopfnList.replace(j+1,tmp);

            }
        }
    setting->sync();
    setting->endGroup();

    QVector<QString> data;
    //从新读取安装路径应用列表，以获取安卓app
    getDesktopFilePath();
    Q_FOREACH(QString desktopfn,desktopfnList)
    {
        QString desktopfp;
        if (androidDesktopfnList.contains(desktopfn)) {
            desktopfp = QString(QDir::homePath() + "/.local/share/applications/" + desktopfn);
        } else {
            desktopfp = QString("/usr/share/applications/" + desktopfn);
        }

        if (filePathList.contains(desktopfp)) {
            data.append(desktopfp);
        }
    }
    return data;
}

QVector<QStringList> UkuiMenuInterface::getAlphabeticClassification()
{
    QVector<QStringList> data;
    QStringList appnameList;
    appnameList.clear();
    QVector<QStringList> appVector[27];
    int index=0;
    while(index<appInfoVector.size())
    {
        QString appname=appInfoVector.at(index).at(1);
        QString appnamepy=UkuiChineseLetter::getPinyins(appname);
        char c=appnamepy.at(0).toLatin1();
        switch (c) {
        case 'A':
            appVector[0].append(appInfoVector.at(index));
            break;
        case 'B':
            appVector[1].append(appInfoVector.at(index));
            break;
        case 'C':
            appVector[2].append(appInfoVector.at(index));
            break;
        case 'D':
            appVector[3].append(appInfoVector.at(index));
            break;
        case 'E':
            appVector[4].append(appInfoVector.at(index));
            break;
        case 'F':
            appVector[5].append(appInfoVector.at(index));
            break;
        case 'G':
            appVector[6].append(appInfoVector.at(index));
            break;
        case 'H':
            appVector[7].append(appInfoVector.at(index));
            break;
        case 'I':
            appVector[8].append(appInfoVector.at(index));
            break;
        case 'J':
            appVector[9].append(appInfoVector.at(index));
            break;
        case 'K':
            appVector[10].append(appInfoVector.at(index));
            break;
        case 'L':
            appVector[11].append(appInfoVector.at(index));
            break;
        case 'M':
            appVector[12].append(appInfoVector.at(index));
            break;
        case 'N':
            appVector[13].append(appInfoVector.at(index));
            break;
        case 'O':
            appVector[14].append(appInfoVector.at(index));
            break;
        case 'P':
            appVector[15].append(appInfoVector.at(index));
            break;
        case 'Q':
            appVector[16].append(appInfoVector.at(index));
            break;
        case 'R':
            appVector[17].append(appInfoVector.at(index));
            break;
        case 'S':
            appVector[18].append(appInfoVector.at(index));
            break;
        case 'T':
            appVector[19].append(appInfoVector.at(index));
            break;
        case 'U':
            appVector[20].append(appInfoVector.at(index));
            break;
        case 'V':
            appVector[21].append(appInfoVector.at(index));
            break;
        case 'W':
            appVector[22].append(appInfoVector.at(index));
            break;
        case 'X':
            appVector[23].append(appInfoVector.at(index));
            break;
        case 'Y':
            appVector[24].append(appInfoVector.at(index));
            break;
        case 'Z':
            appVector[25].append(appInfoVector.at(index));
            break;
        default:
            appVector[26].append(appInfoVector.at(index));
            break;
        }

        index++;
    }

    for(int i=0;i<26;i++)
    {
        QStringList desktopfpList;
        desktopfpList.clear();
        qSort(appVector[i].begin(),appVector[i].end(),cmpApp);
        for(int j=0;j<appVector[i].size();j++)
            desktopfpList.append(appVector[i].at(j).at(0));
        data.append(desktopfpList);
    }

     QVector<QStringList> otherVector;
     QVector<QStringList> numberVector;
    for(int i=0;i<appVector[26].count();i++)
    {
        QString appname=appVector[26].at(i).at(2);
        QChar c=appname.at(0);
        if(c<48 || (c>57 && c<65) || c>90)
            otherVector.append(appVector[26].at(i));
        else
            numberVector.append(appVector[26].at(i));
    }
    qSort(otherVector.begin(),otherVector.end(),cmpApp);
    qSort(numberVector.begin(),numberVector.end(),cmpApp);
    QStringList otherfpList;
    otherfpList.clear();
    for(int i=0;i<otherVector.size();i++)
        otherfpList.append(otherVector.at(i).at(0));
    QStringList numberfpList;
    numberfpList.clear();
    for(int i=0;i<numberVector.size();i++)
        numberfpList.append(numberVector.at(i).at(0));
    data.append(otherfpList);
    data.append(numberfpList);

    return data;
}

QVector<QStringList> UkuiMenuInterface::getFunctionalClassification()
{
    QVector<QStringList> appVector[11];
    int index=0;
    while(index<appInfoVector.size())
    {
        int count=appInfoVector.at(index).size()-5;
        for(int i=0;i<count;i++)
        {
            int category=appInfoVector.at(index).at(5+i).toInt();
            switch (category) {
            case 0:
                appVector[0].append(appInfoVector.at(index));
                break;
            case 1:
                appVector[1].append(appInfoVector.at(index));
                break;
            case 2:
                appVector[2].append(appInfoVector.at(index));
                break;
            case 3:
                appVector[3].append(appInfoVector.at(index));
                break;
            case 4:
                appVector[4].append(appInfoVector.at(index));
                break;
            case 5:
                appVector[5].append(appInfoVector.at(index));
                break;
            case 6:
                appVector[6].append(appInfoVector.at(index));
                break;
            case 7:
                appVector[7].append(appInfoVector.at(index));
                break;
            case 8:
                appVector[8].append(appInfoVector.at(index));
                break;
            case 9:
                appVector[9].append(appInfoVector.at(index));
                break;
            case 10:
                appVector[10].append(appInfoVector.at(index));
                break;
            default:
                break;
            }
        }
        index++;
    }

    QVector<QStringList> data;
    data.clear();

    for(int i=0;i<11;i++)
    {
        QStringList desktopfpList;
        desktopfpList.clear();
        qSort(appVector[i].begin(),appVector[i].end(),cmpApp);
        for(int j=0;j<appVector[i].size();j++)
            desktopfpList.append(appVector[i].at(j).at(0));
        data.append(desktopfpList);
    }
    return data;
}

bool UkuiMenuInterface::matchingAppCategories(QString desktopfp, QStringList categorylist)
{
    QString category=getAppCategories(desktopfp);
    int index;
    for(index=0;index<categorylist.count();index++)
    {
        if(category.contains(categorylist.at(index),Qt::CaseInsensitive))
            return true;
    }
    if(index==categorylist.count())
        return false;

    return false;
}

void UkuiMenuInterface::getAndroidApp()
{
    androidDesktopfnList.clear();
    QVector<QStringList> androidVector;
    androidVector.clear();
    QString path=QDir::homePath()+"/.local/share/applications/";
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    if(list.size()< 1 ) {
        return;
    }
    int i=0;

    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    do{
        QFileInfo fileInfo = list.at(i);
        if(!fileInfo.isFile())
        {
            i++;
            continue;
        }
        //过滤后缀不是.desktop的文件
        QString filePathStr=fileInfo.filePath();
        if(!filePathStr.endsWith(".desktop"))
        {
            i++;
            continue;
        }

        QByteArray fpbyte=filePathStr.toLocal8Bit();
        char* filepath=fpbyte.data();
        g_key_file_load_from_file(keyfile,filepath,flags,error);
        char* ret_1=g_key_file_get_locale_string(keyfile,"Desktop Entry","Categories", nullptr, nullptr);
        if(ret_1!=nullptr)
        {
            //注释掉的原因是：安卓应用还没有加上 Android 标记
//            QString str=QString::fromLocal8Bit(ret_1);
//            if(!str.contains("Android"))
//            {
//                i++;
//                continue;
//            }
//            else
//            {
                filePathList.append(filePathStr);
                androidDesktopfnList.append(fileInfo.fileName());
//            }
        }

        i++;
    }while(i < list.size());

    g_key_file_free(keyfile);
}

QString UkuiMenuInterface::getAppNameInitials(QString desktopfp)
{
    QString firstLetters;
    QString appname=getAppName(desktopfp);
    QStringList appnamestr=appname.split(" ");
    QString letters;
    Q_FOREACH(QString name,appnamestr)
    {
        letters.clear();
        letters=UkuiChineseLetter::getFirstLettersAll(name);
        if(letters.isEmpty())
            letters=UkuiChineseLetter::getFirstLetter(name);
        firstLetters.append(letters);
    }

    return firstLetters;
}

QString UkuiMenuInterface::getAppNameInitial(QString desktopfp)
{
    return UkuiChineseLetter::getFirstLetter(getAppName(desktopfp));
}

//获取应用拼音
QString UkuiMenuInterface::getAppNamePinyin(QString appname)
{
    return UkuiChineseLetter::getPinyins(appname);
}

//获取指定类型应用列表
QStringList UkuiMenuInterface::getSpecifiedCategoryAppList(QString categorystr)
{
    QByteArray categorybyte=categorystr.toLocal8Bit();
    char* category=categorybyte.data();

    QStringList desktopfpList=getDesktopFilePath();
    QStringList appnameList;
    appnameList.clear();
    for(int index=0;index<desktopfpList.count();index++)
    {
        QString appcategorystr=getAppCategories(desktopfpList.at(index));
        if(!appcategorystr.isEmpty())
        {
            QByteArray appcategorybyte=appcategorystr.toLocal8Bit();
            char* appcategory=appcategorybyte.data();
            int len=static_cast<int>(strlen(appcategory)-6);
            for(int i=0;i<len;i++)
            {
                if(strncmp(appcategory+i,category,strlen(category))==0)
                {
                    QString appname=getAppName(desktopfpList.at(index));
                    if(QString::compare(appname,"访问提示")==0)
                    appnameList.append(appname);
                    break;
                }
            }

        }
    }
    return appnameList;

}

//获取用户图像
QString UkuiMenuInterface::getUserIcon()
{
//    qint64 uid=static_cast<qint64>(getuid());
//    QDBusInterface iface("org.freedesktop.Accounts",
//                         "/org/freedesktop/Accounts",
//                         "org.freedesktop.Accounts",
//                         QDBusConnection::systemBus());
//    QDBusReply<QDBusObjectPath>objPath=iface.call("FindUserById",uid);

//    QDBusInterface useriface("org.freedesktop.Accounts",
//                             objPath.value().path(),
//                             "org.freedesktop.DBus.Properties",
//                             QDBusConnection::systemBus());
//    QDBusReply<QVariant> var=useriface.call("Get","org.freedesktop.Accounts.User","IconFile");
//    QString iconstr=var.value().toString();
//    return iconstr;
    return "";
}

QString UkuiMenuInterface::getUserName()
{
    qint64 uid=static_cast<qint64>(getuid());
    QDBusInterface iface("org.freedesktop.Accounts",
                         "/org/freedesktop/Accounts",
                         "org.freedesktop.Accounts",
                         QDBusConnection::systemBus());
    QDBusReply<QDBusObjectPath> objPath=iface.call("FindUserById",uid);
    QDBusInterface useriface("org.freedesktop.Accounts",
                             objPath.value().path(),
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::systemBus());
    QDBusReply<QVariant> var=useriface.call("Get","org.freedesktop.Accounts.User","UserName");
    QString name=var.value().toString();
    return name;
}
