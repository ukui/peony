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
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

//
// Created by hxf on 2021/8/20.
//

#ifndef PEONY_TABLET_APP_MANAGER_H
#define PEONY_TABLET_APP_MANAGER_H

#define STUDY_CENTER_MATH          "math"
#define STUDY_CENTER_ENGLISH       "english"
#define STUDY_CENTER_CHINESE       "chinese"
#define STUDY_CENTER_OTHER         "other"
#define STUDY_CENTER_SYNCHRONIZED  "synchronized"
#define STUDY_CENTER_STUDENT_GUARD "studentGuard"

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QGSettings>
#include <QStringList>
#include <glib.h>
#include <QFileSystemWatcher>

namespace Peony {

class TabletAppEntity : public QObject
{
Q_OBJECT
public:
    explicit TabletAppEntity(QObject *parent = nullptr) {}

    TabletAppEntity(const TabletAppEntity &entity, QObject *parent = nullptr)
    {
        copyFields(entity);
    }

    explicit TabletAppEntity(const TabletAppEntity *entity, QObject *parent = nullptr)
    {
        copyFields(*entity);
    }

    inline TabletAppEntity &operator=(const TabletAppEntity &entity)
    {
        copyFields(entity);
        return *this;
    }

    inline TabletAppEntity &operator=(const TabletAppEntity *entity)
    {
        copyFields(*entity);
        return *this;
    }

private:
    void copyFields(const TabletAppEntity &entity) {
        this->serialNumber = entity.serialNumber;
        this->desktopName  = entity.desktopName;
        this->appName      = entity.appName;
        this->appIcon      = entity.appIcon;
        this->execCommand  = entity.execCommand;
        this->iTime        = entity.iTime;
    }

public:
    long int iTime = 0;
    quint32 serialNumber = 0; //排序序号
    QString desktopName;  //desktop文件名
    QString appName;      //应用名称
    QString appIcon;      //应用图标路径
    QString execCommand;  //应用的执行路径

};
/**
 * @brief 平板模式APP数据的加载，desktop文件解析和执行应用。
 */

class TabletAppManager : public QObject
{
    Q_OBJECT
public:
    static TabletAppManager *getInstance(QObject *parent = nullptr);

    QString getAppName(const QString &appPath);

    QString getAppIcon(const QString &appPath);

    QString getAppExecCommand(const QString &appPath);

    bool appIsDisabled(TabletAppEntity *appEntity);

    bool appIsDisabled(const QString &execCommand);

    bool execApp(TabletAppEntity *appEntity);

    bool execApp(const QString &appPath);

    /**
     * @brief 桌面图标拖动时，进行排序
     * @param appEntity
     * @return
     */
    bool updateAppEntitySerialNumber(TabletAppEntity *appEntity);

    const QList<TabletAppEntity*> &getTabletAppEntityList();

    /**
     * @brief 封装的学习中心数据
     *   TabletAppManager *appManager = TabletAppManager::getInstance(this);
     *   QMap<QString, QList<TabletAppEntity *>> map = appManager->studyCenterData();
     *   for (QString typeName : map.keys()) {
     *       qDebug() << "==studyCenter=" << typeName << map.value(typeName).count();
     *       for(TabletAppEntity *entity : map.value(typeName)) {
     *           qDebug() << "==" << entity->desktopName << entity->appName;
     *       }
     *   }
     * @return
     */
    QMap<QString, QList<TabletAppEntity*>> getStudyCenterData();

Q_SIGNALS:
    void appEntityListChanged(const QList<TabletAppEntity*> &appEntityList);

private:
    GKeyFile *getKeyFile(const QString &appPath);

    //系统app路径集合
    static QStringList g_appPathList;

    //加载系统app路径
    static void initSystemAppPath();

private Q_SLOTS:
    void directoryChangedSlot(QString string = QString());

private:
    explicit TabletAppManager(QObject *parent = nullptr);

    void initWatcher();

    void updateAppNameSetting();

    /**
     * @brief 像文件中写入新安装的app,并设置序号为最大值加一
     * @param appName
     */
    void insertNewAppName(const QString &appName);

    /**
     * @brief 加载配置文件
     */
    void initSettings();

    /**
     * @brief 第一次启动时，将应用集合存放到文件中
     * 将应用程序按照来源进行排序。
     * tencent应用排序在前
     */
    QStringList initAppListSetting();

    void loadAppData();

    /**
     * @brief 填充app 路径数据到list中
     */
    void fillAppPathList();

    /**
     * @brief 初始化时从指定的路径加载desktop文件
     * @param targetPath
     */
    void searchAppFromPath(const QString &targetPath);

    /**
     * @brief 从读取到的app列表中排除不需要显示的app
     */
    void excludeAppFromList();

    /**
     * @brief 将必须要显示的app添加到app列表
     */
    void addAppToList();

    /**
     * @brief 从app列表中过滤无效的应用
     */
    void filterAppList();

    void updateTabletAppList();

    void clearTabletAppList();

    void sortAppEntityList();

    bool launchApp(const QString &execCommand);

private:
    QMutex m_mutex;
    QFileSystemWatcher* m_appPathWatcher;
    //禁用app列表
    QSettings *m_disabledAppSetting = nullptr;

    //显示在平板桌面的app
    QSettings *m_appNameSetting   = nullptr;
    //系统预置的app
    QSettings *m_systemAppSetting = nullptr;

    //.desktop文件
    QStringList m_appPathList;
    //桌面显示的图标对应的 map
    QMap<QString, quint32> m_appNameMap;

    QList<TabletAppEntity*> m_appEntityList;

    QMap<QString, QList<TabletAppEntity *>> m_studyCenterDataMap;
};


}

#endif //PEONY_TABLET_APP_MANAGER_H
