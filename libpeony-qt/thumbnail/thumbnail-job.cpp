/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "thumbnail-job.h"

#include "thumbnail-manager.h"

#include "file-watcher.h"

#include <QApplication>
#include <QDebug>

static int runCount = 0;
static int endCount = 0;

Peony::ThumbnailJob::ThumbnailJob(const QString &uri, const std::shared_ptr<Peony::FileWatcher> watcher, QObject *parent):
    QObject(parent), QRunnable()
{
    m_uri = uri;
    m_watcher = watcher;
    if (watcher) {
        if (watcher->parent()) {
            setParent(watcher->parent());
        }
    }
}

Peony::ThumbnailJob::~ThumbnailJob()
{
    endCount++;
    //qDebug()<<"job end or cancelled. current end"<<endCount<<"current start request:"<<runCount;
}

void Peony::ThumbnailJob::run()
{
    if (!parent())
        return;

    // if all window closed, should not do a thumbnail job.
    if (qApp->topLevelWindows().count() == 0) {
        return;
    }

    runCount++;

    qDebug()<<"job start, current end:"<<endCount<<"current start request:"<<runCount;

    setParent(nullptr);
    auto strongPtr = m_watcher.lock();
    if (strongPtr.get())
        ThumbnailManager::getInstance()->createThumbnailInternal(m_uri, strongPtr);
}
