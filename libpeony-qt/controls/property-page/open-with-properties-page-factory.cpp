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
#include "open-with-properties-page-factory.h"
#include "open-with-properties-page.h"
#include "file-info.h"
#include "file-info-job.h"

using namespace Peony;

static OpenWithPropertiesPageFactory *global_instance = nullptr;

OpenWithPropertiesPageFactory *OpenWithPropertiesPageFactory::getInstance()
{
    if (global_instance)
        return global_instance;

    return new OpenWithPropertiesPageFactory;
}

OpenWithPropertiesPageFactory::OpenWithPropertiesPageFactory(QObject *parent) : QObject(parent)
{

}

OpenWithPropertiesPageFactory::~OpenWithPropertiesPageFactory()
{

}

bool OpenWithPropertiesPageFactory::supportUris(const QStringList &uris)
{
    //FIX:正确设置需要打开的文件
    if (uris.count() != 1)
        return false;

    auto fileInfo = FileInfo::fromUri(uris.first());
    FileInfoJob *job = new FileInfoJob(fileInfo);
    job->setAutoDelete(true);
    job->querySync();

    qDebug() << "file is :::" << fileInfo.get()->isDir() << fileInfo.get()->isDesktopFile();

    if (fileInfo.get()->displayName().endsWith(".desktop"))
        return false;

    if (fileInfo.get()->isDir() || fileInfo.get()->isDesktopFile() || fileInfo.get()->isVolume() || fileInfo.get()->isVirtual())
        return false;

    if (uris.first().contains("computer:///") || uris.first().contains("recent:///") || uris.first().contains("trash:///"))
        return false;

    return true;
}

PropertiesWindowTabIface *OpenWithPropertiesPageFactory::createTabPage(const QStringList &uris)
{
    return new OpenWithPropertiesPage(uris.first());
}

void OpenWithPropertiesPageFactory::closeFactory()
{
    this->deleteLater();
}

