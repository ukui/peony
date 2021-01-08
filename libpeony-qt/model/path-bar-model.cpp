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

#include "path-bar-model.h"
#include "file-enumerator.h"
#include "file-info.h"
#include "file-utils.h"
#include "file-info-job.h"

#include <QUrl>

using namespace Peony;

PathBarModel::PathBarModel(QObject *parent) : QStringListModel (parent)
{
    m_enumerator = new FileEnumerator(this);
    m_enumerator->setEnumerateWithInfoJob();

    connect(m_enumerator, &FileEnumerator::enumerateFinished, this, [=](bool successed){
        if (successed) {
            m_childrens = m_enumerator->getChildren();

            QStringList list;
            for (auto info : m_childrens) {
                if (!(info->isDir() || info->isVolume()))
                    continue;

                if (info.get()->displayName().startsWith("."))
                    continue;

                QUrl url = info.get()->uri();
                list<<url.toDisplayString();
                m_uri_display_name_hash.insert(info.get()->uri(), info.get()->displayName());
            }

            beginResetModel();
            setStringList(list);
            sort(0);
            endResetModel();

            Q_EMIT updated();
        } else {
            beginResetModel();
            setStringList(QStringList());
            endResetModel();
        }
    });
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
    }

    m_current_uri = uri;

    m_enumerator->cancel();
    m_uri_display_name_hash.clear();

    m_info = FileInfo::fromUri(uri);
    auto infoJob = new FileInfoJob(m_info);
    infoJob->setAutoDelete();
    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=](bool successed){
        if (successed) {
            m_enumerator->cancel();
            m_enumerator->setEnumerateDirectory(m_info.get()->uri());
            m_enumerator->enumerateAsync();
        }
    });
    infoJob->queryAsync();

    return;
    //FIXME: replace BLOCKING api in ui thread.
    if (!force) {
        if (uri.contains("////"))
            return;

        if (m_current_uri == uri)
            return;

        auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
        //FIXME: replace BLOCKING api in ui thread.
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

    //FIXME: replace BLOCKING api in ui thread.
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
        //FIXME: replace BLOCKING api in ui thread.
        QString display_name = FileUtils::getFileDisplayName(info->uri());
        if (display_name.startsWith("."))
            continue;

        QUrl url = info->uri();
        l<<url.toDisplayString();
        m_uri_display_name_hash.insert(info->uri(), display_name);
    }
    setStringList(l);
    sort(0);
    endResetModel();
    Q_EMIT updated();
}

QString PathBarModel::findDisplayName(const QString &uri)
{
    QUrl url = uri;
    if (m_uri_display_name_hash.find(url.toDisplayString())->isNull()) {
        //FIXME: replace BLOCKING api in ui thread.
        return FileUtils::getFileDisplayName(uri);
    } else {
        return m_uri_display_name_hash.value(url.toDisplayString());
    }
}
