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
    //set default allow parallel
    if (! m_settings->allKeys().contains(ALLOW_FILE_OP_PARALLEL)) {
        qDebug() << "default ALLOW_FILE_OP_PARALLEL:true";
        setValue(ALLOW_FILE_OP_PARALLEL, true);
    }
    //if local languege is chinese, set chinese first as deafult
    if (QLocale::system().name().contains("zh") && !m_settings->allKeys().contains(SORT_CHINESE_FIRST))
        setValue(SORT_CHINESE_FIRST, true);
    for (auto key : m_settings->allKeys()) {
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

        /* hotfix bug#101227:解决兼容升级后对应设置项恢复gsetting默认值问题。判断字段（INIT_FOR_FIRST_TIME）不存在，则为首次初始化，反之不是 */
        if(!isExist(INIT_FOR_FIRST_TIME)){
            setValue(INIT_FOR_FIRST_TIME, false);
            /* /usr/share/glib-2.0/schemas/org.ukui.peony.settings.gschema.xml文件首次初始化时，
             * SHOW_HIDDEN_PREFERENCE字段为 "org.ukui/peony-qt-preferences" 文件中"show-hidden"的值*/
            if(isExist("show-hidden")){
                bool value = getValue("show-hidden").toBool();
                m_cache.insert(SHOW_HIDDEN_PREFERENCE, value);
                setGSettingValue(SHOW_HIDDEN_PREFERENCE, value);
            }
        }

        connect(m_peony_gsettings, &QGSettings::changed, this, [=](const QString &key) {
            if (key == "showTrashDialog") {
                m_cache.remove(SHOW_TRASH_DIALOG);
                m_cache.insert(SHOW_TRASH_DIALOG, m_peony_gsettings->get(key).toBool());
            } else if (SHOW_HIDDEN_PREFERENCE == key) {
                if (m_cache.value(key) != m_peony_gsettings->get(key).toBool())
                {
                    m_cache.remove(key);
                    m_cache.insert(key, m_peony_gsettings->get(key).toBool());
                }
                /* Solve the problem: When opening multiple document management, check "Show hidden files" in one document management,
                 *  but the other document management does not take effect in real time.modified by 2021/06/15  */
                Q_EMIT this->valueChanged(key);
            }
        });
        m_cache.remove(SHOW_TRASH_DIALOG);
        m_cache.insert(SHOW_TRASH_DIALOG, m_peony_gsettings->get(SHOW_TRASH_DIALOG).toBool());

        m_cache.remove(SHOW_HIDDEN_PREFERENCE);
        m_cache.insert(SHOW_HIDDEN_PREFERENCE, m_peony_gsettings->get(SHOW_HIDDEN_PREFERENCE).toBool());
    }

    m_cache.insert(SIDEBAR_BG_OPACITY, 100);
    if (QGSettings::isSchemaInstalled(PERSONAL_EFFECT_SCHEMA)) {
        m_gsettings = new QGSettings(PERSONAL_EFFECT_SCHEMA, QByteArray(), this);

        connect(m_gsettings, &QGSettings::changed, this, [=](const QString &key) {
            if (key == PERSONAL_EFFECT_TRANSPARENCY || key == PERSONAL_EFFECT_ENABLE) {
                qreal opacity = m_gsettings->get(PERSONAL_EFFECT_TRANSPARENCY).toReal() * 100;
                m_cache.remove(SIDEBAR_BG_OPACITY);
                m_cache.insert(SIDEBAR_BG_OPACITY, opacity);
            }
        });

        if (m_gsettings->get(PERSONAL_EFFECT_ENABLE).toBool()) {
            qreal opacity = m_gsettings->get(PERSONAL_EFFECT_TRANSPARENCY).toReal() * 100;
            m_cache.remove(SIDEBAR_BG_OPACITY);
            m_cache.insert(SIDEBAR_BG_OPACITY, opacity);
        }
    }

    if (m_cache.value(DEFAULT_WINDOW_SIZE).isNull() || m_cache.value(DEFAULT_SIDEBAR_WIDTH) <= 0) {
        QScreen *screen=qApp->primaryScreen();
        QRect geometry = screen->availableGeometry();
        int default_width = geometry.width() * 2/3;
        int default_height =  geometry.height() * 4/5;
        if (default_width < 850)
            default_width = 850;
        if (default_height < 850 *0.618)
            default_height = 850 *0.618;
        setValue(DEFAULT_WINDOW_SIZE, QSize(default_width, default_height));
        setValue(DEFAULT_SIDEBAR_WIDTH, 210);
        qDebug() << "deafult set DEFAULT_SIDEBAR_WIDTH:"<<210;
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
}

GlobalSettings::~GlobalSettings()
{

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

    m_cache.remove(key);
    m_cache.insert(key, value);
    QtConcurrent::run([=]() {
        if (m_mutex.tryLock(1000)) {
            m_settings->setValue(key, value);
            m_settings->sync();
            m_mutex.unlock();
        }
    });
}

void GlobalSettings::forceSync(const QString &key)
{
    m_settings->sync();
    if (key.isNull()) {
        m_cache.clear();
        for (auto key : m_settings->allKeys()) {
            m_cache.insert(key, m_settings->value(key));
        }
    } else {
        m_cache.remove(key);
        m_cache.insert(key, m_settings->value(key));
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
