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

#ifndef DIRECTORYVIEWPLUGINIFACE2_H
#define DIRECTORYVIEWPLUGINIFACE2_H

#include "plugin-iface.h"

#include <QWidget>

#define DirectoryViewPluginIface2_iid "org.ukui.peony-qt.plugin-iface.DirectoryViewPluginInterface2"

namespace Peony {

class DirectoryViewWidget;

class DirectoryViewPluginIface2 : public PluginInterface
{
public:
    virtual ~DirectoryViewPluginIface2() {}

    virtual QString viewIdentity() = 0;
    virtual QIcon viewIcon() = 0;
    virtual bool supportUri(const QString &uri) = 0;

    virtual int zoom_level_hint() = 0;

    virtual int priority(const QString &directoryUri) = 0;

    //virtual void fillDirectoryView(DirectoryViewWidget *view) = 0;

    virtual DirectoryViewWidget *create() = 0;
};

}

Q_DECLARE_INTERFACE (Peony::DirectoryViewPluginIface2, DirectoryViewPluginIface2_iid)

#endif // DIRECTORYVIEWPLUGINIFACE2_H
