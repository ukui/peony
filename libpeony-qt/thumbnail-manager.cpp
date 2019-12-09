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

#include "thumbnail-manager.h"

#include "file-info-manager.h"

#include "file-watcher.h"
#include "file-utils.h"

#include "thumbnail/pdf-thumbnail.h"

#include <QtConcurrent>
#include <QIcon>
#include <QUrl>

#include <gio/gdesktopappinfo.h>

using namespace Peony;

static ThumbnailManager *global_instance = nullptr;

/*!
 * \brief ThumbnailManager::ThumbnailManager
 * \param parent
 * \bug
 * thumbnail will do i/o on the file. if we write on a pictrue and save
 * save it, the image editor might report a modified error due to we hold
 * the file in peony-qt.
 *
 * this bug is not critical, but i have to consider current thumbnailer
 * might be a bad desgin.
 */
ThumbnailManager::ThumbnailManager(QObject *parent) : QObject(parent)
{

}

ThumbnailManager *ThumbnailManager::getInstance()
{
    if (!global_instance)
        global_instance = new ThumbnailManager;
    return global_instance;
}

void ThumbnailManager::createThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    //qDebug()<<"create thumbnail"<<uri;
    //NOTE: we should do createThumbnail() after we have queried the file's info.
    auto info = FileInfo::fromUri(uri);
    if (!info->mimeType().isEmpty()) {
        if (info->mimeType().startsWith("image/")) {
            QtConcurrent::run([=]() {
                QUrl url = uri;
                qDebug()<<url;
                if (!info->uri().startsWith("file:///")) {
                    url = FileUtils::getTargetUri(info->uri());
                    qDebug()<<url;
                }
                QIcon thumbnail;
                thumbnail.addFile(url.path());
                if (!thumbnail.isNull()) {
                    //add lock
                    //m_mutex.lock();
                    m_hash.remove(uri);
                    m_hash.insert(uri, thumbnail);
                    auto info = FileInfo::fromUri(uri);
                    //Q_EMIT info->updated();
                    if (watcher) {
                        watcher->fileChanged(uri);
                    }
                    //info->setThumbnail(thumbnail);
                    //m_mutex.unlock();
                }
            });
        } else if (info->mimeType().contains("pdf")) {
            QtConcurrent::run([=]() {
                QUrl url = uri;
                qDebug()<<url;
                if (!info->uri().startsWith("file:///")) {
                    url = FileUtils::getTargetUri(info->uri());
                    qDebug()<<url;
                }
                PdfThumbnail pdfThumbnail(info->uri());
                QIcon thumbnail;
                thumbnail.addPixmap(pdfThumbnail.generateThumbnail());
                //thumbnail.addFile(url.path());
                if (!thumbnail.isNull()) {
                    //add lock
                    //m_mutex.lock();
                    m_hash.remove(uri);
                    m_hash.insert(uri, thumbnail);
                    auto info = FileInfo::fromUri(uri);
                    //Q_EMIT info->updated();
                    if (watcher) {
                        watcher->fileChanged(uri);
                    }
                    //info->setThumbnail(thumbnail);
                    //m_mutex.unlock();
                }
            });
        } else if (info->isDesktopFile()) {
            qDebug()<<"is desktop file"<<uri;
            //get desktop file icon.
            //async
            qDebug()<<"desktop file"<<uri;
            QtConcurrent::run([=]() {
                QIcon thumbnail;
                QUrl url = uri;
                qDebug()<<url;
                if (!info->uri().startsWith("file:///")) {
                    url = FileUtils::getTargetUri(info->uri());
                    qDebug()<<url;
                }

                auto _desktop_file = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
                auto _icon_string = g_desktop_app_info_get_string(_desktop_file, "Icon");
                thumbnail = QIcon::fromTheme(_icon_string);
                qDebug()<<_icon_string;
                QString string = _icon_string;
                if (thumbnail.isNull() && string.startsWith("/")) {
                    qDebug()<<"add file";
                    thumbnail.addFile(_icon_string);
                }
                g_free(_icon_string);
                g_object_unref(_desktop_file);

                if (!thumbnail.isNull()) {
                    //add lock
                    //m_mutex.lock();
                    m_hash.remove(uri);
                    m_hash.insert(uri, thumbnail);
                    auto info = FileInfo::fromUri(uri);
                    //Q_EMIT info->updated();
                    if (watcher) {
                        watcher->fileChanged(uri);
                    }
                    //info->setThumbnail(thumbnail);
                    //m_mutex.unlock();
                }
            });
        }
    }
}

void ThumbnailManager::updateDesktopFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    auto info = FileInfo::fromUri(uri);
    if (info->isDesktopFile() && info->canExecute()) {
        qDebug()<<"is desktop file"<<uri;
        //get desktop file icon.
        //async
        qDebug()<<"desktop file"<<uri;
        QtConcurrent::run([=]() {
            QIcon thumbnail;
            QUrl url = uri;
            qDebug()<<url;
            if (!info->uri().startsWith("file:///")) {
                url = FileUtils::getTargetUri(info->uri());
                qDebug()<<url;
            }

            auto _desktop_file = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
            auto _icon_string = g_desktop_app_info_get_string(_desktop_file, "Icon");
            thumbnail = QIcon::fromTheme(_icon_string);
            qDebug()<<_icon_string;
            QString string = _icon_string;
            if (thumbnail.isNull() && string.startsWith("/")) {
                qDebug()<<"add file";
                thumbnail.addFile(_icon_string);
            }
            g_free(_icon_string);
            g_object_unref(_desktop_file);

            if (!thumbnail.isNull()) {
                //add lock
                //m_mutex.lock();
                m_hash.remove(uri);
                m_hash.insert(uri, thumbnail);
                auto info = FileInfo::fromUri(uri);
                //Q_EMIT info->updated();
                if (watcher) {
                    watcher->thumbnailUpdated(uri);
                }
                //info->setThumbnail(thumbnail);
                //m_mutex.unlock();
            }
        });
    } else {
        releaseThumbnail(uri);
        if (watcher) {
            watcher->thumbnailUpdated(uri);
        }
    }
}

void ThumbnailManager::releaseThumbnail(const QString &uri)
{
    //m_mutex.lock();
    m_hash.remove(uri);
    //m_mutex.unlock();
}

const QIcon ThumbnailManager::tryGetThumbnail(const QString &uri)
{
    //m_mutex.lock();
    auto icon = m_hash.value(uri);
    //m_mutex.unlock();
    return icon;
}
