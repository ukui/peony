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

#ifndef GTK2STYLEENHANCEDPLUGIN_H
#define GTK2STYLEENHANCEDPLUGIN_H

#include "gtk2-style-enhanced-plugin_global.h"
#include <style-plugin-iface.h>

#include <QObject>
#include <QtPlugin>

#include <gtk/gtk.h>

namespace Peony {

class GTK2STYLEENHANCEDPLUGINSHARED_EXPORT Gtk2StyleEnhancedPlugin : public QObject, public StylePluginIface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID StylePluginIface_iid)
    Q_INTERFACES(Peony::StylePluginIface)
public:
    explicit Gtk2StyleEnhancedPlugin(QObject *parent = nullptr);
    ~Gtk2StyleEnhancedPlugin();

    PluginInterface::PluginType pluginType() {return PluginInterface::StylePlugin;}
    const QString name() {return tr("Gtk theme enhanced extension");}
    const QString description() {return tr("Improve the gtk-themed controls' style and painting");}
    const QIcon icon() {return QIcon::fromTheme("utilities-terminal-symbolic");}
    void setEnable(bool enable) {m_enable = enable;}
    bool isEnable() {return m_enable;}

    int defaultPriority() {return 0;}

    QProxyStyle *getStyle();

protected:
    static void icon_theme_changed_cb(GtkIconTheme *theme, Gtk2StyleEnhancedPlugin *p_this);

private:
    bool m_enable;
    GtkIconTheme *m_gtk_icon_theme;
    GtkSettings *m_gtk_settings;
    QString m_icon_theme_name;

    QTimer *m_timer;
};

}

#endif // GTK2STYLEENHANCEDPLUGIN_H
