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

#include "global-settings.h"
//ukui-interface
#ifdef KYLIN_COMMON
#include <ukuisdk/kylin-com4cxx.h>
#endif

#include <QtConcurrent>

#include <QGSettings>

#include <QApplication>
#include <QPalette>
#include <QScreen>

using namespace Peony;

static GlobalSettings *global_instance = nullptr;

GlobalSettings *GlobalSettings::getInstance()
{
    if (!global_instance) {
        global_instance = new GlobalSettings;
    }
    return global_instance;
}

GlobalSettings::GlobalSettings(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings("org.ukui", "peony-qt-preferences", this);
    if (!m_settings->allKeys().contains(REMOTE_SERVER_REMOTE_IP)) {
        setValue(REMOTE_SERVER_REMOTE_IP, QVariant());
    }

    for (auto key : m_settings->allKeys()) {
        //only REMOTE_SERVER_REMOTE_IP
        m_cache.insert(key, m_settings->value(key));
    }

    m_date_format = tr("yyyy/MM/dd");
    m_time_format = tr("HH:mm:ss");
    if (QGSettings::isSchemaInstalled("org.ukui.control-center.panel.plugins")) {
        m_control_center_plugin = new QGSettings("org.ukui.control-center.panel.plugins", QByteArray(), this);
        connect(m_control_center_plugin, &QGSettings::changed, this, [=](const QString &key) {
            QString value = m_control_center_plugin->get(key).toString();
            if ("hoursystem" == key) {
                m_cache.remove(UKUI_CONTROL_CENTER_PANEL_PLUGIN_TIME);
                m_cache.insert(UKUI_CONTROL_CENTER_PANEL_PLUGIN_TIME, value);
                Q_EMIT this->valueChanged(UKUI_CONTROL_CENTER_PANEL_PLUGIN_TIME);
                setTimeFormat(value);
            }
            else if (key == "date")
            {
                m_cache.remove(UKUI_CONTROL_CENTER_PANEL_PLUGIN_DATE);
                m_cache.insert(UKUI_CONTROL_CENTER_PANEL_PLUGIN_DATE, value);
                Q_EMIT this->valueChanged(UKUI_CONTROL_CENTER_PANEL_PLUGIN_DATE);
                setDateFormat(value);
            }
        });

        QString timeValue = m_control_center_plugin->get("hoursystem").toString();
        QString dateValue = m_control_center_plugin->get("date").toString();
        m_cache.insert(UKUI_CONTROL_CENTER_PANEL_PLUGIN_TIME, timeValue);
        m_cache.insert(UKUI_CONTROL_CENTER_PANEL_PLUGIN_DATE, dateValue);
        setTimeFormat(timeValue);
        setDateFormat(dateValue);
    }

    m_cache.insert(SHOW_TRASH_DIALOG, true);
    m_cache.insert(SHOW_HIDDEN_PREFERENCE, false);
    if (QGSettings::isSchemaInstalled("org.ukui.peony.settings")) {
        m_peony_gsettings = new QGSettings("org.ukui.peony.settings", QByteArray(), this);
        connect(m_peony_gsettings, &QGSettings::changed, this, [=](const QString &key) {
            m_cache.remove(key);
            m_cache.insert(key, m_peony_gsettings->get(key));
            Q_EMIT this->valueChanged(key);
        });

        for (auto key : m_peony_gsettings->keys()) {
            m_cache.remove(key);
            m_cache.insert(key, m_peony_gsettings->get(key));
        }
    }

    getUkuiStyle();
    getDualScreenMode();
    getMachineMode();

    if (m_cache.value(DEFAULT_WINDOW_WIDTH).isNull()
        || m_cache.value(DEFAULT_WINDOW_HEIGHT).isNull()
        || m_cache.value(DEFAULT_SIDEBAR_WIDTH) <= 0)
    {
        QScreen *screen=qApp->primaryScreen();
        QRect geometry = screen->availableGeometry();
        int default_width = geometry.width() * 2/3;
        int default_height =  geometry.height() * 4/5;
        if (default_width < 850)
            default_width = 850;
        if (default_height < 850 *0.618)
            default_height = 850 *0.618;
        setValue(DEFAULT_WINDOW_WIDTH, default_width);
        setValue(DEFAULT_WINDOW_HEIGHT, default_height);
        setValue(DEFAULT_SIDEBAR_WIDTH, 210);
        qDebug() << "default set DEFAULT_SIDEBAR_WIDTH:"<<210;
    }

    if (m_cache.value(DEFAULT_VIEW_ID).isNull()) {
        setValue(DEFAULT_VIEW_ID, "Icon View");
    }

    if (m_cache.value(SORT_ORDER).isNull()){
        setValue(SORT_ORDER, Qt::AscendingOrder);
    }

    if (m_cache.value(SORT_COLUMN).isNull()){
        setValue(SORT_COLUMN, 0);
    }

    if (m_cache.value(DEFAULT_VIEW_ZOOM_LEVEL).isNull()) {
        setValue(DEFAULT_VIEW_ZOOM_LEVEL, 25);
    }

    if (m_cache.value(REMOTE_SERVER_REMOTE_IP).isNull()) {
        setValue(REMOTE_SERVER_REMOTE_IP, QVariant(QList<QString>()));
    }


    if (m_cache.value (SORT_TYPE).isNull()) {
        setValue (SORT_TYPE, 0);
    }

    if (m_cache.value (SORT_ORDER).isNull()) {
        setValue (SORT_ORDER, 0);
    }

}

GlobalSettings::~GlobalSettings()
{

}

void GlobalSettings::getUkuiStyle()
{
    m_cache.insert(SIDEBAR_BG_OPACITY, 50);
    if (getProjectName() == V10_SP1_EDU) {
        if (QGSettings::isSchemaInstalled(UKUI_CONTROL_CENTER_PERSONALISE)) {
            m_gsettings = new QGSettings(UKUI_CONTROL_CENTER_PERSONALISE);
            connect(m_gsettings, &QGSettings::changed, [this](const QString &key) {
                if (key == PERSONALISE_EFFECT) {
                    qreal opacity = 100.0;
                    if (m_gsettings->get(PERSONALISE_EFFECT).toBool()) {
                        opacity *= m_gsettings->get(PERSONALISE_TRANSPARENCY).toReal();
                    }
                    m_cache.remove(SIDEBAR_BG_OPACITY);
                    m_cache.insert(SIDEBAR_BG_OPACITY, opacity);
                }
            });
            qreal opacity = 100.0;
            if (m_gsettings->get(PERSONALISE_EFFECT).toBool()) {
                opacity *= m_gsettings->get(PERSONALISE_TRANSPARENCY).toReal();
            }
            m_cache.remove(SIDEBAR_BG_OPACITY);
            m_cache.insert(SIDEBAR_BG_OPACITY, opacity);
        }
    } else {
        if (QGSettings::isSchemaInstalled("org.ukui.style")) {
            m_gsettings = new QGSettings("org.ukui.style", QByteArray(), this);
            connect(m_gsettings, &QGSettings::changed, this, [=](const QString &key) {
                if (key == "peonySideBarTransparency") {
                    m_cache.remove(SIDEBAR_BG_OPACITY);
                    m_cache.insert(SIDEBAR_BG_OPACITY, m_gsettings->get(key).toString());
                    qApp->paletteChanged(qApp->palette());
                }
            });
            m_cache.remove(SIDEBAR_BG_OPACITY);
            m_cache.insert(SIDEBAR_BG_OPACITY, m_gsettings->get("peonySideBarTransparency").toString());
        }
    }
}

void GlobalSettings::getMachineMode()
{
    m_cache.insert(TABLET_MODE, "false");
    if (QGSettings::isSchemaInstalled("org.ukui.SettingsDaemon.plugins.tablet-mode")) {
        m_gsettings_tablet_mode = new QGSettings("org.ukui.SettingsDaemon.plugins.tablet-mode", QByteArray(), this);
        m_cache.remove(TABLET_MODE);
        m_cache.insert(TABLET_MODE, m_gsettings_tablet_mode->get("tablet-mode").toString());
        connect(m_gsettings_tablet_mode, &QGSettings::changed, this, [=](const QString &key) {
            if (key == "tabletMode") {
                m_cache.remove(TABLET_MODE);
                m_cache.insert(TABLET_MODE, m_gsettings_tablet_mode->get(key).toString());
                qApp->paletteChanged(qApp->palette());
            }
        });
    }
}

void GlobalSettings::getDualScreenMode()
{
    m_cache.insert(DUAL_SCREEN_MODE, DUAL_SCREEN_EXPAND_MODE);
    if(QGSettings::isSchemaInstalled(SETTINGS_DAEMON_SCHEMA_XRANDR)) {
        m_gsettings_dual_screen_mode = new QGSettings(SETTINGS_DAEMON_SCHEMA_XRANDR, QByteArray(), this);
        m_cache.remove(DUAL_SCREEN_MODE);
        if (m_gsettings_dual_screen_mode->keys().contains(DUAL_SCREEN_MODE)) {
            m_cache.insert(DUAL_SCREEN_MODE, m_gsettings_dual_screen_mode->get(DUAL_SCREEN_MODE).toString());
        }
        connect(m_gsettings_dual_screen_mode, &QGSettings::changed, this, [=](const QString &key){
           if (key == DUAL_SCREEN_MODE) {
               m_cache.remove(DUAL_SCREEN_MODE);
               m_cache.insert(DUAL_SCREEN_MODE, m_gsettings_dual_screen_mode->get(key).toString());
               qApp->paletteChanged(qApp->palette());
           }
        });
    }
}

const QVariant GlobalSettings::getValue(const QString &key)
{
    return m_cache.value(key);
}

bool GlobalSettings::isExist(const QString &key)
{
    return !m_cache.value(key).isNull();
}

void GlobalSettings::reset(const QString &key)
{
    m_cache.remove(key);
    QtConcurrent::run([=]() {
        if (m_mutex.tryLock(1000)) {
            m_settings->remove(key);
            m_settings->sync();
            m_mutex.unlock();
        }
    });
    Q_EMIT this->valueChanged(key);
}

void GlobalSettings::resetAll()
{
    QStringList tmp = m_cache.keys();
    m_cache.clear();
    for (auto key : tmp) {
        Q_EMIT this->valueChanged(key);
    }
    QtConcurrent::run([=]() {
        if (m_mutex.tryLock(1000)) {
            m_settings->clear();
            m_settings->sync();
            m_mutex.unlock();
        }
    });
}

void GlobalSettings::setValue(const QString &key, const QVariant &value)
{
    if (key == REMOTE_SERVER_REMOTE_IP) {
        m_cache.remove(key);
        m_cache.insert(key, value);
        QtConcurrent::run([=]() {
            if (m_mutex.tryLock(1000)) {
                m_settings->setValue(key, value);
                m_settings->sync();
                m_mutex.unlock();
            }
        });
    } else {
        setGSettingValue(key, value);
    }
}

void GlobalSettings::forceSync(const QString &key)
{
    m_settings->sync();
    if (key.isNull()) {
        m_cache.clear();
        for (auto key : m_settings->allKeys()) {
            m_cache.insert(key, m_settings->value(key));
        }

        if (m_peony_gsettings) {
            for (auto key : m_peony_gsettings->keys()) {
                m_cache.insert(key, m_peony_gsettings->get(key));
            }
        }
    } else {
        m_cache.remove(key);
        if (m_settings->allKeys().contains(key)) {
            m_cache.insert(key, m_settings->value(key));
        } else {
            m_cache.insert(key, m_peony_gsettings ? m_peony_gsettings->get(key) : QVariant());
        }
    }
}

void GlobalSettings::slot_updateRemoteServer(const QString& server, bool add)
{
    Q_EMIT signal_updateRemoteServer(server, add);
}

void GlobalSettings::setTimeFormat(const QString &value)
{
    if (value == "12"){
        m_time_format = tr("AP hh:mm:ss");
    }
    else{
        m_time_format = tr("HH:mm:ss");
    }
}

void GlobalSettings::setDateFormat(const QString &value)
{
    if (value == "cn"){
        m_date_format = tr("yyyy/MM/dd");
    }
    else{
        m_date_format = tr("yyyy-MM-dd");
    }
}

QString GlobalSettings::getSystemTimeFormat()
{
    m_system_time_format = m_date_format + " " + m_time_format;
    return m_system_time_format;
}
void GlobalSettings::setGSettingValue(const QString &key, const QVariant &value)
{
    if (!m_peony_gsettings)
        return;

    const QStringList list = m_peony_gsettings->keys();
    if (!list.contains(key))
        return;

    m_peony_gsettings->set(key, value);
    m_cache.remove(key);
    m_cache.insert(key, m_peony_gsettings->get(key));
}

QString GlobalSettings::getProjectName()
{
    return QString::fromStdString(KDKGetPrjCodeName());
}
