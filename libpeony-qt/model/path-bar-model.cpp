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

#include "path-bar-model.h"
#include "file-enumerator.h"
#include "file-info.h"
#include "file-utils.h"

using namespace Peony;

PathBarModel::PathBarModel(QObject *parent) : QStringListModel (parent)
{

}

void PathBarModel::setRootPath(const QString &path, bool force)
{
    setRootUri("file://" + path, force);
}

void PathBarModel::setRootUri(const QString &uri, bool force)
{
    if (!force) {
        if (uri.contains("////"))
            return;

        if (m_current_uri == uri)
            return;

        auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
        if (!g_file_query_exists(file.get()->get(), nullptr)) {
            return;
        }
    }

    //do not enumerate a search:/// directory
    if (uri.startsWith("search://"))
        return;

    if (uri.startsWith("trash://"))
        return;

    //qDebug()<<"setUri"<<uri<<"raw"<<m_current_uri;

    beginResetModel();

    m_current_uri = uri;

    FileEnumerator e;
    e.setEnumerateDirectory(uri);
    e.enumerateSync();
    auto infos = e.getChildren();
    if (infos.isEmpty()) {
        endResetModel();
        Q_EMIT updated();
        return;
    }

    m_uri_display_name_hash.clear();
    QStringList l;
    for (auto info : infos) {
        //skip the independent file.
        if (!(info->isDir() || info->isVolume()))
            continue;

        //skip the hidden file.
        QString display_name = FileUtils::getFileDisplayName(info->uri());
        if (display_name.startsWith("."))
            continue;

        //NOTE: uri encode can not support chinese correctly.
        //I have fixed it in FileInfo class constructor.
        l<<info->uri();
        m_uri_display_name_hash.insert(info->uri(), display_name);
    }
    setStringList(l);
    sort(0);
    endResetModel();
    Q_EMIT updated();
}

QString PathBarModel::findDisplayName(const QString &uri)
{
    if (m_uri_display_name_hash.find(uri)->isNull()) {
        return FileUtils::getFileDisplayName(uri);
    } else {
        return m_uri_display_name_hash.value(uri);
    }
}
