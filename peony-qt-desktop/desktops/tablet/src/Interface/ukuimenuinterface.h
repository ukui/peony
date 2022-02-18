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

#ifndef UKUIMENUINTERFACE_H
#define UKUIMENUINTERFACE_H
#include <QtCore>
#include <sys/types.h>
#include <unistd.h>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusObjectPath>
#include <QSettings>
#include <QSqlDatabase> //sql驱动基础
#include <QSqlQuery>//sql查询相关
#include <QSqlError>//sql输出错误
class UkuiMenuInterface
{
private:
    QStringList filePathList;
//    QStringList androidDesktopfnList;
protected:
    void recursiveSearchFile(const QString& _filePath);//遍历/usr/share/applications/文件夹
    QStringList getSpecifiedCategoryAppList(QString categorystr);//获取指定类型应用列表

public:
    UkuiMenuInterface();
    ~UkuiMenuInterface();
    QVector<QStringList> createAppInfoVector();//创建应用信息容器
    static QVector<QStringList> appInfoVector;
    static QVector<QString> desktopfpVector;
    static QVector<QStringList> alphabeticVector;
    static QVector<QStringList> functionalVector;
    static QVector<QString> allAppVector;
    static QStringList androidDesktopfnList;

    /**
     * 获取系统应用名称
     * @param desktopfp 为应用.desktop文件所在路径
     * @return 返回应用名称
     */

    QStringList getInstalledAppList();//获取已安装应用列表
    QString getTencentAppid(QString desktopfp);//获取TencentAppid
    QString getAppName(QString desktopfp);//获取应用名
    QString getAppEnglishName(QString desktopfp);//获取英语英文名
    QString getAppIcon(QString desktopfp);//获取应用图像
    QString getAppCategories(QString desktopfp);//获取应用分类
    QString getAppExec(QString desktopfp);//获取应用命令
    QString getAppType(QString desktopfp);//获取应用类型
    QString getAppComment(QString desktopfp);//获取应用注释
    QStringList getDesktopFilePath();//获取系统deskyop文件路径


    QVector<QString> getLockApp();
    bool initAppIni();

    QVector<QStringList> getAlphabeticClassification();//获取字母分类
    QVector<QStringList> getFunctionalClassification();//获取功能分类
    QVector<QString> getCommonUseApp();//获取常用App
    QVector<QString> getAllApp();
    void getAndroidApp();//获取安卓应用
    static bool cmpApp(QStringList &arg_1,QStringList &arg_2);
    bool matchingAppCategories(QString desktopfp,QStringList categorylist);//匹配应用Categories
    QString getAppNameInitials(QString desktopfp);//获取应用名所有首字母
    QString getAppNameInitial(QString desktopfp);//获取应用名所有首字母
    QString getAppNamePinyin(QString appname);//获取应用名拼音

    //获取用户图像
    QString getUserIcon();
    //获取用户姓名
    QString getUserName();


    /**/
    QSettings* setting=nullptr;
    QSettings *syssetting=nullptr;

};

#endif // UKUIMENUINTERFACE_H
