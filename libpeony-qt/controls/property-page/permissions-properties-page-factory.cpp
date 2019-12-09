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

#include "permissions-properties-page-factory.h"
#include "permissions-properties-page.h"

#include <QLabel>

using namespace Peony;

PermissionsPropertiesPageFactory *global_instance = nullptr;

PermissionsPropertiesPageFactory::PermissionsPropertiesPageFactory(QObject *parent) : QObject(parent)
{

}

bool PermissionsPropertiesPageFactory::supportUris(const QStringList &uris)
{
    return uris.count() == 1;
}

QWidget *PermissionsPropertiesPageFactory::createTabPage(const QStringList &uris)
{
    return new PermissionsPropertiesPage(uris);
}

void PermissionsPropertiesPageFactory::closeFactory()
{
    this->deleteLater();
}

PermissionsPropertiesPageFactory *PermissionsPropertiesPageFactory::getInstance()
{
    if (!global_instance)
        global_instance = new PermissionsPropertiesPageFactory;
    return global_instance;
}
