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
 * Authors: Wenfei He <hewenfei@kylinos.cn>
 *
 */

#include "mark-properties-page-factory.h"
#include "mark-properties-page.h"
using namespace Peony;

static MarkPropertiesPageFactory *global_instance = nullptr;

MarkPropertiesPageFactory *MarkPropertiesPageFactory::getInstance()
{
    if (global_instance)
        return global_instance;

    return new MarkPropertiesPageFactory;
}

MarkPropertiesPageFactory::MarkPropertiesPageFactory(QObject *parent) : QObject(parent)
{

}

MarkPropertiesPageFactory::~MarkPropertiesPageFactory()
{

}

bool MarkPropertiesPageFactory::supportUris(const QStringList &uris)
{
    if (uris.count() != 1)
        return false;

    if (uris.first().contains("computer:///") && uris.first().contains("recent:///") && uris.first().contains("trash:///"))
        return false;

    return true;
}

PropertiesWindowTabIface *MarkPropertiesPageFactory::createTabPage(const QStringList &uris)
{
    return new MarkPropertiesPage(uris.first());
}

void MarkPropertiesPageFactory::closeFactory()
{
    this->deleteLater();
}
