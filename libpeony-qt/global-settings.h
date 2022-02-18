/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QMutex>

#include "peony-core_global.h"

//顶部菜单 - Top menu
#define RESIDENT_IN_BACKEND         "resident"
#define SHOW_HIDDEN_PREFERENCE      "showHiddenFile"
#define ALLOW_FILE_OP_PARALLEL      "allowFileOpParallel"
#define FORBID_THUMBNAIL_IN_VIEW    "doNotThumbnail"

//视图 - View
#define DEFAULT_VIEW_ID             "defaultViewId"
#define DEFAULT_VIEW_ZOOM_LEVEL     "defaultViewZoomLevel"

//排序类型 - Sort type
#define SORT_COLUMN                 "sortColumn"
#define SORT_TYPE                   "sortType"

//排序顺序 - Sort order
#define SORT_ORDER                  "sortOrder"

//排序偏好 - Sort preference
#define SORT_FOLDER_FIRST           "folderFirst"
#define SORT_CHINESE_FIRST          "chineseFirst"

//窗口设置 - Window setting
#define DEFAULT_WINDOW_WIDTH        "defaultWindowWidth"
#define DEFAULT_WINDOW_HEIGHT       "defaultWindowHeight"

//侧边栏 - Sidebar
#define DEFAULT_SIDEBAR_WIDTH       "defaultSidebarWidth"

//删除提示框 - tooltip
#define SHOW_TRASH_DIALOG           "showTrashDialog"

//状态栏 - Zoom Slider
#define ZOOM_SLIDER_VISIBLE         "zoomSliderVisible"

//多选模式 - Multiple selection
#define MULTI_SELECT                "multiSelectCheckboxEnabled"

//桌面配置 - Desktop setting
#define LAST_DESKTOP_SORT_ORDER     "lastDesktopSortOrder"
#define TEMPLATES_DIR               "templatesDir"
#define DEFAULT_DESKTOP_ZOOM_LEVEL  "defaultDesktopZoomLevel"

//收藏的服务器IP - favorite ip
#define REMOTE_SERVER_REMOTE_IP     "remote-server/favorite-ip"

//gsettings
#define SIDEBAR_BG_OPACITY          "sidebar-bg-opacity"
#define TABLET_MODE                 "tablet-mode"

#define FONT_SETTINGS               "org.ukui.style"

//difference between Community version and Commercial version
#define COMMERCIAL_VERSION          false


// control center
#define UKUI_CONTROL_CENTER_PANEL_PLUGIN            "org.ukui.control-center.panel.plugins"                 // schema
#define UKUI_CONTROL_CENTER_PANEL_PLUGIN_TIME       "org.ukui.control-center.panel.plugins.time"            // time format key, value is '12' or '24'
#define UKUI_CONTROL_CENTER_PANEL_PLUGIN_DATE       "org.ukui.control-center.panel.plugins.date"            // date format key, value is cn or en

//intel 个性化设置，透明度...
//schema
#define UKUI_CONTROL_CENTER_PERSONALISE             "org.ukui.control-center.personalise"
//keys
#define PERSONALISE_EFFECT                          "effect"
#define PERSONALISE_TRANSPARENCY                    "transparency"

#define SETTINGS_DAEMON_SCHEMA_XRANDR        "org.ukui.SettingsDaemon.plugins.xrandr"
#define DUAL_SCREEN_MODE                     "xrandrMirrorMode"
#define DUAL_SCREEN_EXPAND_MODE              "expand"
#define DUAL_SCREEN_MIRROR_MODE              "mirror"

class QGSettings;

namespace Peony {

/*!
 * \brief The GlobalSettings class
 * \details
 * this class provide a global settings for peony-qt.
 * it most be used to save and load preferences of peony-qt, such as default view,
 * sort type, etc.
 *
 * you can also save another kind of datas using by extensions. such as enable properties.
 * this class instance is shared in both peony-qt and its plugins.
 */
class PEONYCORESHARED_EXPORT GlobalSettings : public QObject
{
    Q_OBJECT
public:
    static GlobalSettings *getInstance();
    const QVariant getValue(const QString &key);
    bool isExist(const QString &key);
    QString getProjectName();

Q_SIGNALS:
    void valueChanged(const QString &key);
    void signal_updateRemoteServer(const QString& server, bool add);

public Q_SLOTS:
    void setValue(const QString &key, const QVariant &value);
    void reset(const QString &key);
    void resetAll();
    void setTimeFormat(const QString &value);
    void setDateFormat(const QString &value);
    QString getSystemTimeFormat();

    /*!
     * \brief 通过GSetting保存设置
     * \param key
     * \param value
     */
    void setGSettingValue(const QString &key, const QVariant &value);

    /*!
     * \brief forceSync
     * \param key
     * \details
     * use force sync to ensure the settings key-value and cached map's data
     * keep same. this may be used in out progress, such as peony-qt-desktop.
     */
    void forceSync(const QString &key = nullptr);
    void slot_updateRemoteServer(const QString& server, bool add);

private:
    explicit GlobalSettings(QObject *parent = nullptr);
    ~GlobalSettings();

    void getUkuiStyle();
    void getMachineMode();
    void getDualScreenMode();

    QSettings*                  m_settings;
    QMap<QString, QVariant>     m_cache;

    QGSettings*                 m_gsettings = nullptr;
    QGSettings*                 m_control_center_plugin = nullptr;
    QGSettings*                 m_peony_gsettings  = nullptr;
    QGSettings *m_gsettings_tablet_mode = nullptr;
    QGSettings *m_gsettings_dual_screen_mode = nullptr;
    QMutex                      m_mutex;

    QString                     m_date_format = "";
    QString                     m_time_format = "";
    QString                     m_system_time_format  = "";
};

}

#endif // GLOBALSETTINGS_H
