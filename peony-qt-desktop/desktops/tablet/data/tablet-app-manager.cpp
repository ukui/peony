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
#include "tablet-app-manager.h"
#include "desktop-global-settings.h"

#include <QDir>
#include <QLocale>
#include <QDebug>
#include <QStandardPaths>
#include <glib/gerror.h>
#include <QCollator>
#include <QTextCodec>
#include <QDBusInterface>

using namespace Peony;

static TabletAppManager *g_appManager = nullptr;

//系统程序路径，安卓路径，第三方程序路径
QStringList TabletAppManager::g_appPathList = QStringList();

TabletAppManager *TabletAppManager::getInstance(QObject *parent)
{
    if (!g_appManager) {
        g_appManager = new TabletAppManager(parent);
    }

    return g_appManager;
}

TabletAppManager::TabletAppManager(QObject *parent) : QObject(parent)
{
    TabletAppManager::initSystemAppPath();

    this->initSettings();
    this->initWatcher();

    //从文件系统加载数据,检测是否存在应用安装或卸载
    this->directoryChangedSlot();
}

void TabletAppManager::initSystemAppPath()
{
    //清除应用列表
    TabletAppManager::g_appPathList.clear();
    //系统应用路径
    TabletAppManager::g_appPathList.append("/usr/share/applications/");

    //安卓和某些第三方程序路径
//    QString homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
//    QString androidAppPath = homePath + "/.local/share/applications/";
//
//    TabletAppManager::g_appPathList.append(androidAppPath);
}

void TabletAppManager::initSettings()
{
//    QString homePath = QStandardPaths::StandardLocation(QStandardPaths::HomeLocation);
    QString homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();

    QString appNameSettingPath   = homePath + "/.config/ukui/desktop-tablet-app.ini";
    QString systemAppSettingPath = homePath + "/.config/ukui/desktop-tablet-system-app.ini";
    QString disabledSettingPath  = homePath + "/.cache/ukui-menu/ukui-menu.ini";
//    QString systemAppSettingPath = homePath + "/usr/bin/desktop-tablet-system-app.ini";

    //显示在平板桌面的app
    m_appNameSetting = new QSettings(appNameSettingPath, QSettings::IniFormat, this);
    m_appNameSetting->setIniCodec(QTextCodec::codecForName("utf-8"));
    //系统预置的app
    m_systemAppSetting = new QSettings(systemAppSettingPath, QSettings::IniFormat, this);
    m_systemAppSetting->setIniCodec(QTextCodec::codecForName("utf-8"));
    //被禁用的app列表
    m_disabledAppSetting = new QSettings(disabledSettingPath, QSettings::IniFormat, this);
    m_disabledAppSetting->setIniCodec(QTextCodec::codecForName("utf-8"));
}

void TabletAppManager::initWatcher()
{
    QDBusConnection::systemBus().connect(QString(), QString("/com/ukui/desktop/software"),
                                         "com.ukui.desktop.software", "send_to_client",
                                         this, SLOT(directoryChangedSlot(QString)));

    m_appPathWatcher = new QFileSystemWatcher(this);

    for (const QString &path : TabletAppManager::g_appPathList) {
        m_appPathWatcher->addPath(path);
    }

    connect(m_appPathWatcher, &QFileSystemWatcher::directoryChanged, this, &TabletAppManager::directoryChangedSlot);
}

void TabletAppManager::directoryChangedSlot(QString)
{
    //TODO 2021-0822 在目录发生变化时，只加载发生变化的目录
    this->fillAppPathList();

    //提前填充map
    this->loadAppData();

    this->updateAppNameSetting();

    //从新加载应用列表
    this->updateTabletAppList();
}

void TabletAppManager::fillAppPathList()
{
    m_appPathList.clear();

    //读取应用数据
    for (const QString &path : TabletAppManager::g_appPathList) {
        this->searchAppFromPath(path);
    }

    //删除重复的项目
    m_appPathList.removeDuplicates();

    //排除不需要显示的应用
    this->excludeAppFromList();

    //添加必须显示的应用
    this->addAppToList();

    //对应用列表进行过滤
    this->filterAppList();
}

void TabletAppManager::insertNewAppName(const QString &appName)
{
    //m_appNameSetting->beginGroup("application");
    quint32 maxValue = 0;
    for (const QString &key : m_appNameSetting->allKeys()) {
        if (m_appNameSetting->value(key).toInt() > maxValue) {
            maxValue = m_appNameSetting->value(key).toInt();
        }
    }
    m_appNameSetting->setValue(appName, (maxValue + 1));
}

void TabletAppManager::updateAppNameSetting()
{
    m_mutex.lock();

    QStringList InstalledAppList;
    QStringList UninstalledAppList;

    for (const QString &appPath : m_appPathList) {
        InstalledAppList.append(appPath.split("/").last());
    }

    for (const QString &appName : m_appNameMap.keys()) {
        if (InstalledAppList.contains(appName)) {
            //去掉未改变的应用
            InstalledAppList.removeOne(appName);
        } else {
            //被卸载的应用
            UninstalledAppList.append(appName);
        }
    }

    qDebug() << "[TabletAppManager::updateAppList] InstalledAppCount" << InstalledAppList.count();
    qDebug() << "[TabletAppManager::updateAppList] UninstalledAppCount" << UninstalledAppList.count();

    m_appNameSetting->beginGroup("application");

    for (const QString &appName : InstalledAppList) {
        this->insertNewAppName(appName);
    }
    m_appNameSetting->sync();

    for (const QString &appName : UninstalledAppList) {
        m_appNameSetting->remove(appName);
    }
    m_appNameSetting->sync();
    m_appNameSetting->endGroup();

    //存在新安装应用和卸载应用操作后才刷新
    if (InstalledAppList.count() > 0 && UninstalledAppList.count() > 0) {
        //更新文件后刷新map
        this->loadAppData();
    }

    m_mutex.unlock();
}

void TabletAppManager::loadAppData()
{
    m_appNameMap.clear();

    m_appNameSetting->beginGroup("application");
    QStringList appList = m_appNameSetting->allKeys();

    if (appList.count()  == 0) {
        m_appNameSetting->endGroup();
        //第一次启动时进行初始化
        appList = this->initAppListSetting();
    } else {
        m_appNameSetting->endGroup();
    }

    m_appNameSetting->beginGroup("application");
    for (const QString &appName : appList) {
        m_appNameMap.insert(appName, m_appNameSetting->value(appName).toInt());
    }
    m_appNameSetting->endGroup();
}

//更改排序算法，由中英文字母键值从小到大
bool sortFunction(QString &arg_1, QString &arg_2)
{
    QLocale local;
    QString language = local.languageToString(local.language());

    if (QString::compare(language, "Chinese") == 0) {
        local = QLocale(QLocale::Chinese);
    } else {
        local = QLocale(QLocale::English);
    }

    QCollator collator(local);
    if (collator.compare(arg_1, arg_2) < 0) {
        return true;
    } else {
        return false;
    }
}

QStringList TabletAppManager::initAppListSetting()
{
    QStringList tencentApps;
    QStringList tencentPrecisePracticeApps;
    QStringList thirdPartyApps;
    QStringList customizedApps;
    QStringList otherApps;

    for (const QString &appPath : m_appPathList) {
        QString appName = appPath.split("/").last();
        //腾讯应用
        if (appName.contains("tencent")) {
            if (appName.contains("precise-practice")) {
                tencentPrecisePracticeApps.append(appName);
            } else {
                tencentApps.append(appName);
            }

        } else if (appName.contains("wps")) {
            thirdPartyApps.append(appName);

        } else if (appName.contains("eye")) {
            customizedApps.append(appName);

        } else if (appName.contains("mdm")) {
            customizedApps.append(appName);

        } else {
            otherApps.append(appName);
        }
    }

    //排序
    std::sort(tencentApps.begin(), tencentApps.end(), sortFunction);
    std::sort(tencentPrecisePracticeApps.begin(), tencentPrecisePracticeApps.end(), sortFunction);
    std::sort(thirdPartyApps.begin(), thirdPartyApps.end(), sortFunction);
    std::sort(customizedApps.begin(), customizedApps.end(), sortFunction);
    std::sort(otherApps.begin(), otherApps.end(), sortFunction);

    //写入文件
    m_appNameSetting->beginGroup("application");
    //精准系列
    for (int i = 0; i < tencentPrecisePracticeApps.count(); ++i) {
        m_appNameSetting->setValue(tencentPrecisePracticeApps.at(i), i);
    }
    //腾讯应用
    quint32 count_a = tencentPrecisePracticeApps.count();
    for (int i = 0; i < tencentApps.count(); ++i) {
        m_appNameSetting->setValue(tencentApps.at(i), i + count_a);
    }
    //定制应用
    quint32 count_b = tencentApps.count() + count_a;
    for (int i = 0; i < customizedApps.count(); ++i) {
        m_appNameSetting->setValue(customizedApps.at(i), i + count_b);
    }
    //第三方应用
    quint32 count_c = customizedApps.count() + count_a + count_b;
    for (int i = 0; i < thirdPartyApps.count(); ++i) {
        m_appNameSetting->setValue(thirdPartyApps.at(i), i + count_c);
    }
    //其他应用
    quint32 count_d = thirdPartyApps.count() + count_a + count_b + count_c;
    for (int i = 0; i < otherApps.count(); ++i) {
        m_appNameSetting->setValue(otherApps.at(i), i + count_d);
    }

    m_appNameSetting->sync();
    QStringList appList = m_appNameSetting->allKeys();
    m_appNameSetting->endGroup();

    return appList;
}

void TabletAppManager::searchAppFromPath(const QString &targetPath)
{
    GError **error = nullptr;
    GKeyFileFlags flags = G_KEY_FILE_NONE;
    GKeyFile *keyfile = g_key_file_new();

    QDir dir(targetPath);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    list.removeAll(QFileInfo("/usr/share/applications/screensavers"));

    qDebug() << "[TabletAppManager::searchAppFromPath] path:" << targetPath << "应用数量:" << list.count();

    if (list.size() < 1) {
        return;
    }
    int i = 0;

    //递归算法的核心部分
    do {
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool isDir = fileInfo.isDir();
        if (isDir) {
            searchAppFromPath(fileInfo.filePath());
        } else {
            //过滤后缀不是.desktop的文件
            QString filePathStr = fileInfo.filePath();
            if (!filePathStr.endsWith(".desktop")) {
                i++;
                continue;
            }

            QByteArray fpbyte = filePathStr.toLocal8Bit();
            char *filepath = fpbyte.data();
            g_key_file_load_from_file(keyfile, filepath, flags, error);
            char *ret_1 = g_key_file_get_locale_string(keyfile, "Desktop Entry", "NoDisplay", nullptr, nullptr);
            if (ret_1 != nullptr) {
                QString str = QString::fromLocal8Bit(ret_1);
                if (str.contains("true")) {
                    i++;
                    continue;
                }
            }

            char *ret_2 = g_key_file_get_locale_string(keyfile, "Desktop Entry", "NotShowIn", nullptr, nullptr);
            if (ret_2 != nullptr) {
                QString str = QString::fromLocal8Bit(ret_2);
                if (str.contains("UKUI")) {
                    i++;
                    continue;
                }
            }

            //过滤LXQt、KDE
            char *ret = g_key_file_get_locale_string(keyfile, "Desktop Entry", "OnlyShowIn", nullptr, nullptr);
            if (ret != nullptr) {
                QString str = QString::fromLocal8Bit(ret);
                if (str.contains("LXQt") || str.contains("KDE")) {
                    i++;
                    continue;
                }
            }
            //过滤中英文名为空的情况
            QLocale cn;
            QString language = cn.languageToString(cn.language());
            if (QString::compare(language, "Chinese") == 0) {
                char *nameCh = g_key_file_get_string(keyfile, "Desktop Entry", "Name[zh_CN]", nullptr);
                char *nameEn = g_key_file_get_string(keyfile, "Desktop Entry", "Name", nullptr);

                if (QString::fromLocal8Bit(nameCh).isEmpty() && QString::fromLocal8Bit(nameEn).isEmpty()) {
                    i++;
                    continue;
                }
            } else {
                char *name = g_key_file_get_string(keyfile, "Desktop Entry", "Name", nullptr);
                if (QString::fromLocal8Bit(name).isEmpty()) {
                    i++;
                    continue;
                }
            }

            m_appPathList.append(filePathStr);
        }

        i++;
    } while (i < list.size());

    g_key_file_free(keyfile);
}

void TabletAppManager::excludeAppFromList()
{
    m_appPathList.removeAll("/usr/share/applications/software-properties-livepatch.desktop");
    m_appPathList.removeAll("/usr/share/applications/mate-color-select.desktop");
    m_appPathList.removeAll("/usr/share/applications/blueman-adapters.desktop");
    m_appPathList.removeAll("/usr/share/applications/blueman-manager.desktop");
    m_appPathList.removeAll("/usr/share/applications/mate-user-guide.desktop");
    m_appPathList.removeAll("/usr/share/applications/nm-connection-editor.desktop");
    m_appPathList.removeAll("/usr/share/applications/debian-uxterm.desktop");
    m_appPathList.removeAll("/usr/share/applications/debian-xterm.desktop");
    m_appPathList.removeAll("/usr/share/applications/im-config.desktop");
    m_appPathList.removeAll("/usr/share/applications/fcitx.desktop");
    m_appPathList.removeAll("/usr/share/applications/fcitx-configtool.desktop");
    m_appPathList.removeAll("/usr/share/applications/onboard-settings.desktop");
    m_appPathList.removeAll("/usr/share/applications/info.desktop");
    m_appPathList.removeAll("/usr/share/applications/ukui-power-preferences.desktop");
    m_appPathList.removeAll("/usr/share/applications/ukui-power-statistics.desktop");
    m_appPathList.removeAll("/usr/share/applications/software-properties-drivers.desktop");
    m_appPathList.removeAll("/usr/share/applications/software-properties-gtk.desktop");
    m_appPathList.removeAll("/usr/share/applications/gnome-session-properties.desktop");
    m_appPathList.removeAll("/usr/share/applications/org.gnome.font-viewer.desktop");
    m_appPathList.removeAll("/usr/share/applications/xdiagnose.desktop");
    m_appPathList.removeAll("/usr/share/applications/gnome-language-selector.desktop");
    m_appPathList.removeAll("/usr/share/applications/mate-notification-properties.desktop");
    m_appPathList.removeAll("/usr/share/applications/transmission-gtk.desktop");
    m_appPathList.removeAll("/usr/share/applications/mpv.desktop");
    m_appPathList.removeAll("/usr/share/applications/system-config-printer.desktop");
    m_appPathList.removeAll("/usr/share/applications/org.gnome.DejaDup.desktop");
    m_appPathList.removeAll("/usr/share/applications/yelp.desktop");
    m_appPathList.removeAll("/usr/share/applications/peony-computer.desktop");
    m_appPathList.removeAll("/usr/share/applications/peony-home.desktop");
    m_appPathList.removeAll("/usr/share/applications/peony-trash.desktop");
    //m_appPathList.removeAll("/usr/share/applications/peony.desktop");
    //*过滤*//
    m_appPathList.removeAll("/usr/share/applications/recoll-searchgui.desktop");
    m_appPathList.removeAll("/usr/share/applications/ukui-about.desktop");
    m_appPathList.removeAll("/usr/share/applications/org.gnome.dfeet.desktop");
    m_appPathList.removeAll("/usr/share/applications/ukui-feedback.desktop");
    m_appPathList.removeAll("/usr/share/applications/users.desktop");
//    m_appPathList.removeAll("/usr/share/applications/mate-terminal.desktop");
    m_appPathList.removeAll("/usr/share/applications/vim.desktop");
    m_appPathList.removeAll("/usr/share/applications/mpv.desktop");

    m_appPathList.removeAll("/usr/share/applications/engrampa.desktop");
    m_appPathList.removeAll("/usr/share/applications/hp-document.desktop");
//    m_appPathList.removeAll("/usr/share/applications/kylin-user-guide.desktop");

    m_appPathList.removeAll("/usr/share/applications/wps-office-prometheus.desktop");
//    m_appPathList.removeAll("/usr/share/applications/indicator-china-weather.desktop");    //禁用麒麟天气

    //v10
    m_appPathList.removeAll("/usr/share/applications/mate-about.desktop");
    m_appPathList.removeAll("/usr/share/applications/time.desktop");
    m_appPathList.removeAll("/usr/share/applications/network.desktop");
    m_appPathList.removeAll("/usr/share/applications/shares.desktop");
    m_appPathList.removeAll("/usr/share/applications/mate-power-statistics.desktop");
    m_appPathList.removeAll("/usr/share/applications/display-im6.desktop");
    m_appPathList.removeAll("/usr/share/applications/display-im6.q16.desktop");
    m_appPathList.removeAll("/usr/share/applications/openjdk-8-policytool.desktop");
    m_appPathList.removeAll("/usr/share/applications/kylin-io-monitor.desktop");
    m_appPathList.removeAll("/usr/share/applications/wps-office-uninstall.desktop");
    m_appPathList.removeAll("/usr/share/applications/wps-office-misc.desktop");
}

void TabletAppManager::addAppToList()
{
    /*加入的*/
    m_appPathList.append("/usr/share/applications/mate-calc.desktop");
}

void TabletAppManager::filterAppList()
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
    /*新的应用列表*/
    qDebug() << "[TabletAppManager::filterAppList] 从系统中获取的应用进行删除后数量:" << m_appPathList.count();
    for (const QString &appPath : m_appPathList)//过滤 得到真实存在的应用
    {
        QFileInfo fileInfo(appPath);
        if (!fileInfo.isFile()) {
            m_appPathList.removeOne(appPath);
        }
    }

    /*得到系统默认应用*/
    //判断当前是否已经得到了默认应用
    m_systemAppSetting->beginGroup("desktop-tablet-system-app");
    int sysAppListNum = m_systemAppSetting->allKeys().count();
    m_systemAppSetting->sync();
    m_systemAppSetting->endGroup();

    for (const QString &appPath : m_appPathList) {
        QString appName = appPath.split("/").last();

        //所有不是腾讯的系统应用
        if (!appName.contains("tencent")) {
            //没有初始化默认应用列表
            if (sysAppListNum == 0) {
                m_systemAppSetting->beginGroup("desktop-tablet-system-app");
                qDebug() << "不是腾讯应用" << appName;
                m_systemAppSetting->setValue(appName, 0);
                m_systemAppSetting->sync();
                m_systemAppSetting->endGroup();
            }
        } /*else {
            //用户隔离/etc/skel/桌面
            QString tmp = QString("%1%2").arg("/usr/share/applications/").arg(str);
            QString appid = getTencentAppid(tmp);
            qDebug() << "appid" << tmp << appid;
            if (reply.value().indexOf(appid) != -1) {
                qDebug() << "当前用户可见的腾讯应用" << tmp;
                //newAppPathList.append(QString("%1%2").arg("/usr/share/applications/").arg(str));//tmp
            } else {
                newAppPathList.removeAll(tmp);
            }
        }*/
    }
}

GKeyFile *TabletAppManager::getKeyFile(const QString &filePath)
{
    GError **error = nullptr;
    GKeyFileFlags flags = G_KEY_FILE_NONE;
    GKeyFile *keyFile = g_key_file_new();

    char *path = filePath.toLocal8Bit().data();

    g_key_file_load_from_file(keyFile, path, flags, error);

    return keyFile;
}

QString TabletAppManager::getAppName(const QString &appPath)
{
    GKeyFile *keyFile = getKeyFile(appPath);

    char *name = g_key_file_get_locale_string(keyFile, "Desktop Entry", "Name", nullptr, nullptr);

    g_key_file_free(keyFile);

    return QString::fromLocal8Bit(name);
}

QString TabletAppManager::getAppIcon(const QString &appPath)
{
    GKeyFile *keyFile = getKeyFile(appPath);

    char *icon = g_key_file_get_locale_string(keyFile, "Desktop Entry", "Icon", nullptr, nullptr);

    g_key_file_free(keyFile);

    return QString::fromLocal8Bit(icon);
}

QString TabletAppManager::getAppExecCommand(const QString &appPath)
{
    GKeyFile *keyFile = getKeyFile(appPath);

    char *exec = g_key_file_get_locale_string(keyFile, "Desktop Entry", "Exec", nullptr, nullptr);

    g_key_file_free(keyFile);

    return QString::fromLocal8Bit(exec);
}

void TabletAppManager::updateTabletAppList()
{
    //TODO 可尝试只刷新部分数据
    this->clearTabletAppList();

    for (const QString &appPath : m_appPathList) {
        QString appName = appPath.split("/").last();
        if (m_appNameMap.keys().contains(appName)) {
            TabletAppEntity *tabletAppEntity = new TabletAppEntity(this);

            tabletAppEntity->serialNumber = m_appNameMap.value(appName);
            tabletAppEntity->desktopName  = appName;
            tabletAppEntity->appName      = this->getAppName(appPath);
            tabletAppEntity->appIcon      = this->getAppIcon(appPath);
            tabletAppEntity->execCommand  = this->getAppExecCommand(appPath);

            m_appEntityList.append(tabletAppEntity);
        }
    }

    this->sortAppEntityList();

    Q_EMIT appEntityListChanged(m_appEntityList);
}

void TabletAppManager::clearTabletAppList()
{
    for (TabletAppEntity *tabletAppEntity : m_appEntityList) {
        m_appEntityList.removeOne(tabletAppEntity);
        tabletAppEntity->deleteLater();
    }

    m_appEntityList.clear();
}

const QList<TabletAppEntity *> &TabletAppManager::getTabletAppEntityList()
{
    return m_appEntityList;
}

bool TabletAppManager::updateAppEntitySerialNumber(TabletAppEntity *appEntity)
{
    if (appEntity == nullptr) {
        return false;
    }

    if (m_appNameMap.contains(appEntity->desktopName)) {
        m_appNameSetting->beginGroup("application");

        m_appNameSetting->setValue(appEntity->desktopName, appEntity->serialNumber);

        m_appNameSetting->sync();
        m_appNameSetting->endGroup();

        m_appNameMap.insert(appEntity->desktopName, appEntity->serialNumber);
        this->sortAppEntityList();
        return true;
    }
    return false;
}

bool sortTabletAppEntity(TabletAppEntity *arg1, TabletAppEntity *arg2)
{
    return arg1->serialNumber < arg2->serialNumber;
}

void TabletAppManager::sortAppEntityList()
{
    if (m_appEntityList.count() > 0) {
        std::sort(m_appEntityList.begin(), m_appEntityList.end(), sortTabletAppEntity);
    }
}

QMap<QString, QList<TabletAppEntity *>> TabletAppManager::getStudyCenterData()
{
    QMap<QString, QList<TabletAppEntity *>> map;

    //精准练习  Practice
    QList<TabletAppEntity *> mathList;
    QList<TabletAppEntity *> englishList;
    QList<TabletAppEntity *> chineseList;
    QList<TabletAppEntity *> otherList;
    //同步学习  Synchronized
    QList<TabletAppEntity *> synchronizedList;
    //守护中心  Student Guards
    QList<TabletAppEntity *> studentGuardsList;

    for (TabletAppEntity *entity : m_appEntityList) {
        if (entity->desktopName.contains("tencent")) {
            TabletAppEntity *appEntity = new TabletAppEntity(this);
            appEntity->serialNumber = 0;
            appEntity->desktopName  = entity->desktopName;
            appEntity->appName      = entity->appName;
            appEntity->appIcon      = entity->appIcon;
            appEntity->execCommand  = entity->execCommand;

            //TODO 区分精准练习应用与其他应用 20210826
            if (entity->desktopName.contains("tencent-math")) {
                mathList.append(appEntity);

            } else if (entity->desktopName.contains("tencent-english")) {
                englishList.append(appEntity);

            } else if (entity->desktopName.contains("tencent-chinese")) {
                chineseList.append(appEntity);

            } else if ((QString::compare("tencent-course-center.desktop", entity->desktopName) == 0)
                        || (QString::compare("tencent-penguin-academy.desktop", entity->desktopName) == 0))
            {
                synchronizedList.append(appEntity);

            } else {
                otherList.append(appEntity);
            }
        } else {
            //守护中心应用
            if ((QString::compare("eye-protection-center.desktop", entity->desktopName) == 0)
                || (QString::compare("mdm-acpanel.desktop", entity->desktopName) == 0)) {
                TabletAppEntity *appEntity = new TabletAppEntity(this);
                appEntity->serialNumber = 0;
                appEntity->desktopName  = entity->desktopName;
                appEntity->appName      = entity->appName;
                appEntity->appIcon      = entity->appIcon;
                appEntity->execCommand  = entity->execCommand;

                studentGuardsList.append(appEntity);
            }
        }
    }

    map.insert(STUDY_CENTER_MATH, mathList);
    map.insert(STUDY_CENTER_ENGLISH, englishList);
    map.insert(STUDY_CENTER_CHINESE, chineseList);
    map.insert(STUDY_CENTER_OTHER, otherList);
    map.insert(STUDY_CENTER_SYNCHRONIZED, synchronizedList);
    map.insert(STUDY_CENTER_STUDENT_GUARD, studentGuardsList);

    for (QString typeName : m_studyCenterDataMap.keys()) {
        for (TabletAppEntity *entity : m_studyCenterDataMap.value(typeName)) {
            entity->deleteLater();
        }
    }

    m_studyCenterDataMap.clear();

    m_studyCenterDataMap = map;

    return m_studyCenterDataMap;
}

bool TabletAppManager::execApp(TabletAppEntity *appEntity)
{
    return launchApp(appEntity->execCommand);
}

bool TabletAppManager::execApp(const QString &appPath)
{
    return launchApp(getAppExecCommand(appPath));
}

bool TabletAppManager::launchApp(const QString &execCommand)
{
    if (execCommand.isEmpty()) {
        qDebug() << "[TabletAppManager::launchApp] launch app : execCommand is empty";
        return false;
    }

    if (appIsDisabled(execCommand)) {
        qDebug() << "[TabletAppManager::launchApp] app" << execCommand << "is disabled, so launch failed";
        return false;
    }

    QString exe = execCommand;
    QStringList parameters;
    if (exe.indexOf("%") != -1) {
        exe = exe.left(exe.indexOf("%") - 1);
    }

    if (exe.contains(" ")) {
        //排除参数之间多个空格分隔的情况
        parameters = exe.split(QRegExp("\\s+"));
        exe = parameters[0];
        parameters.removeAt(0);
    }

    for (auto begin = parameters.begin(); begin != parameters.end(); ++begin) {
        if (begin->contains("%")) {
            // 命令行最多可包含一个％f，％u，％F或％U字段代码
            if (begin->count() == 2) {
                parameters.removeOne(*begin);
            } else {
                begin->remove(begin->indexOf("%"), 2);
            }
            break;
        }
    }

    QDBusInterface session("org.gnome.SessionManager", "/com/ukui/app", "com.ukui.app");
    session.call("app_open", exe, parameters);

    return true;
}

bool TabletAppManager::appIsDisabled(const QString &execCommand)
{
    if (execCommand.isEmpty()) {
        qDebug() << "[TabletAppManager::appIsDisabled] execCommand is empty";
        return true;
    }

    m_disabledAppSetting->sync();
    m_disabledAppSetting->beginGroup("application");
    bool isExist = m_disabledAppSetting->contains(execCommand);
    bool notDisable = true;
    if (isExist) {
        notDisable = m_disabledAppSetting->value(execCommand).toBool();
    }
    m_disabledAppSetting->endGroup();

    if (isExist && !notDisable) {
        return true;
    }

    return false;
}

bool TabletAppManager::appIsDisabled(TabletAppEntity *appEntity)
{
    return appIsDisabled(appEntity->execCommand);
}

