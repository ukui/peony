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

#ifndef COMPUTERPROPERTIESPAGE_H
#define COMPUTERPROPERTIESPAGE_H

#include <QWidget>

#include "properties-window-tab-page-plugin-save-iface.h"
#include "peony-core_global.h"

class QFormLayout;

namespace Peony {

class PEONYCORESHARED_EXPORT ComputerPropertiesPage : public PropertiesWindowTabPagePluginSaveIface
{
    Q_OBJECT
public:
    explicit ComputerPropertiesPage(const QString &uri, QWidget *parent = nullptr);

protected:
    void addSeparator();

private:
    QString m_uri;
    QFormLayout *m_layout;

    // PropertiesWindowTabPagePluginSaveIface interface
public:
    void saveAllChange();
};

}

#endif // COMPUTERPROPERTIESPAGE_H
