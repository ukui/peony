/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "gtk2-style-enhanced-plugin.h"
#include <complementary-style.h>

#include <QIcon>
#include <QTimer>
#include <QApplication>
#include <QTranslator>

#include <glib.h>

#include <QDebug>

using namespace Peony;

void Gtk2StyleEnhancedPlugin::icon_theme_changed_cb(GtkIconTheme *theme, Gtk2StyleEnhancedPlugin *p_this)
{
    gchararray value;
    GtkSettings *settings = gtk_settings_get_default();
    g_object_get(settings, "gtk-icon-theme-name", &value, NULL);
    QString str = QString::fromUtf8(value);
    qDebug()<<str;
    g_free(value);
    if (str != p_this->m_icon_theme_name) {
        p_this->m_icon_theme_name = str;
        QIcon::setThemeName(p_this->m_icon_theme_name);
        qApp->paletteChanged(QPalette());
    }
}

Gtk2StyleEnhancedPlugin::Gtk2StyleEnhancedPlugin(QObject *parent) : QObject(parent)
{
    QTranslator *t = new QTranslator(this);
    qDebug()<<"system().name:"<<QLocale::system().name();
    qDebug()<<"\n\n\n\n\n\n\ntranslate:"<<t->load(":/translations/gtk2-style-enhanced-plugin-extension_"+QLocale::system().name());
    QApplication::installTranslator(t);

    QIcon::setFallbackThemeName("hicolor");
    m_gtk_icon_theme = gtk_icon_theme_get_default();
    m_gtk_settings = gtk_settings_get_default();
    g_signal_connect(m_gtk_icon_theme, "changed", G_CALLBACK(icon_theme_changed_cb), m_gtk_settings);

    m_timer = new QTimer(this);
    m_timer->setInterval(5000);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, [=](){
        //check QT_QPA_PLATFORMTHEME
        QString var = g_getenv("QT_QPA_PLATFORMTHEME");
        if (!var.contains("gtk"))
            return;

        GtkSettings *settings = gtk_settings_get_default();
        if (!GTK_IS_SETTINGS(settings))
            return;

        gchararray value;
        g_object_get(settings, "gtk-icon-theme-name", &value, NULL);
        QString str = QString::fromUtf8(value);
        //qDebug()<<str;
        if (value) {
            g_free(value);
            //qDebug()<<value;
        } else {
            return;
        }

        if (str != m_icon_theme_name) {
            m_icon_theme_name = str;
            QIcon::setThemeName(m_icon_theme_name);
            qApp->paletteChanged(QPalette());
        }
    });
    m_timer->start();
}

Gtk2StyleEnhancedPlugin::~Gtk2StyleEnhancedPlugin()
{
    g_signal_handlers_disconnect_by_func(m_gtk_icon_theme, gpointer(icon_theme_changed_cb), nullptr);
    g_object_unref(m_gtk_icon_theme);
}

QProxyStyle *Gtk2StyleEnhancedPlugin::getStyle()
{
    return ComplementaryStyle::getStyle();
}
