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

#include "basic-properties-page-factory.h"
#include "basic-properties-page.h"

using namespace Peony;

static BasicPropertiesPageFactory *global_instance = nullptr;

BasicPropertiesPageFactory *BasicPropertiesPageFactory::getInstance()
{
    if (!global_instance)
        global_instance = new BasicPropertiesPageFactory;
    return global_instance;
}

BasicPropertiesPageFactory::BasicPropertiesPageFactory(QObject *parent) : QObject(parent)
{

}

BasicPropertiesPageFactory::~BasicPropertiesPageFactory()
{

}

bool BasicPropertiesPageFactory::supportUris(const QStringList &uri)
{
    //FIXME:
    if (uri.first().contains("computer:///") || uri.first().contains("network:///") || uri.first().contains("trash:///"))
        return false;
    return true;
}

void BasicPropertiesPageFactory::closeFactory()
{
    deleteLater();
}

QWidget *BasicPropertiesPageFactory::createTabPage(const QStringList &uris)
{
    BasicPropertiesPage *p = new BasicPropertiesPage(uris);
    return p;
}
