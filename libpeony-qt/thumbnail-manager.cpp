/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019-2020, Tianjin KYLIN Information Technology Co., Ltd.
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
 *          Burgess Chang <brs@sdf.org>
 *
 */

#include "thumbnail-manager.h"

#include "file-info-manager.h"

#include "file-watcher.h"
#include "file-utils.h"

#include "thumbnail/pdf-thumbnail.h"
#include "thumbnail/video-thumbnail.h"
#include "thumbnail/office-thumbnail.h"
#include "thumbnail/image-pdf-thumbnail.h"
#include "generic-thumbnailer.h"
#include "thumbnail-job.h"

#include "global-settings.h"

#include <QtConcurrent>
#include <QIcon>
#include <QUrl>

#include <QThreadPool>
#include <QSemaphore>

#include <gio/gdesktopappinfo.h>

using namespace Peony;

static ThumbnailManager *global_instance = nullptr;
static bool m_tril_exist = false;

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
    GlobalSettings::getInstance();

    m_thumbnail_thread_pool = new QThreadPool(this);
    m_thumbnail_thread_pool->setMaxThreadCount(1);

    m_semaphore = new QSemaphore(1);

    findAtril();
}

ThumbnailManager::~ThumbnailManager()
{
    delete m_semaphore;
}

ThumbnailManager *ThumbnailManager::getInstance()
{
    if (!global_instance)
        global_instance = new ThumbnailManager;
    return global_instance;
}

void ThumbnailManager::syncThumbnailPreferences()
{
    GlobalSettings::getInstance()->forceSync(FORBID_THUMBNAIL_IN_VIEW);
}

void ThumbnailManager::insertOrUpdateThumbnail(const QString &uri, const QIcon &icon)
{
    m_semaphore->acquire();
    m_hash.remove(uri);
    m_hash.insert(uri, icon);
    m_semaphore->release();
}

void ThumbnailManager::setForbidThumbnailInView(bool forbid)
{
    GlobalSettings::getInstance()->setValue(FORBID_THUMBNAIL_IN_VIEW, forbid);
}

void ThumbnailManager::createVideFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    QIcon thumbnail;

    VideoThumbnail videoThumbnail(uri);
    thumbnail = videoThumbnail.generateThumbnail();
    if (!thumbnail.isNull()) {
        insertOrUpdateThumbnail(uri, thumbnail);
        if (watcher) {
            watcher->fileChanged(uri);
        }
    }

    return;
}

void ThumbnailManager::createImagePdfFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    QIcon thumbnail;

    ImagePdfThumbnail officeThumbnail(uri);
    thumbnail = officeThumbnail.generateThumbnail();;
    if (!thumbnail.isNull()) {
        insertOrUpdateThumbnail(uri, thumbnail);
        if (watcher) {
            watcher->fileChanged(uri);
        }
    }

    return;
}

void ThumbnailManager::createPdfFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    QIcon thumbnail;
    QUrl url = uri;

    if (!uri.startsWith("file:///")) {
        url = FileUtils::getTargetUri(uri);
        //qDebug()<<url;
    }

    PdfThumbnail pdfThumbnail(url.path());
    QPixmap pix = pdfThumbnail.generateThumbnail();

    thumbnail = GenericThumbnailer::generateThumbnail(pix, true);
    if (!thumbnail.isNull()) {
        insertOrUpdateThumbnail(uri, thumbnail);
        if (watcher) {
            watcher->fileChanged(uri);
        }
    }

    return;
}
void ThumbnailManager::createImageFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    QUrl url = uri;

    if (!uri.startsWith("file:///")) {
        url = FileUtils::getTargetUri(uri);
        //qDebug()<<url;
    }

    QIcon thumbnail = GenericThumbnailer::generateThumbnail(url.path(), true);
    if (!thumbnail.isNull()) {
        insertOrUpdateThumbnail(uri, thumbnail);
        if (watcher) {
            watcher->fileChanged(uri);
        }
    }

    //qApp->processEvents();
    return;
}

void ThumbnailManager::createOfficeFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    QIcon thumbnail;

    OfficeThumbnail officeThumbnail(uri);
    thumbnail = officeThumbnail.generateThumbnail();;
    if (!thumbnail.isNull()) {
        insertOrUpdateThumbnail(uri, thumbnail);
        if (watcher) {
            watcher->fileChanged(uri);
        }
    }

    return;
}

void ThumbnailManager::createDesktopFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    QIcon thumbnail;
    QUrl url = uri;

    if (!uri.startsWith("file:///")) {
        url = FileUtils::getTargetUri(uri);
        //qDebug()<<url;
    }

//    auto _desktop_file = g_desktop_app_info_new_from_filename(url.path().toUtf8().constData());
//    if (!_desktop_file) {
//        return;
//    }

//    auto _icon_string = g_desktop_app_info_get_string(_desktop_file, "Icon");
    //! \note add for mdm
    //! mdm禁用应用会把可执行文件的属性改为不可执行，g_desktop_app_info_new_from_filename会
    //! 认为这个desktop文件不是快捷方式，导致图标变为默认图标
    if (!uri.endsWith(".desktop") || !QFile(url.path()).exists())
        return;
    QSettings desktop_file(url.path(), QSettings::IniFormat);
    desktop_file.beginGroup("Desktop Entry");
    QString _icon_string = desktop_file.value("Icon").toString();

    thumbnail = QIcon::fromTheme(_icon_string);
    QString string = _icon_string;

    //fix desktop file set customer icon issue, link to bug#77638
    auto info = FileInfo::fromUri(uri);
    if (! info->customIcon().isEmpty()){
        thumbnail = GenericThumbnailer::generateThumbnail(info->customIcon(), true);
    }

    if (thumbnail.isNull()) {
        if (string.startsWith("/")) {
            thumbnail = GenericThumbnailer::generateThumbnail(_icon_string, true);
        } else if (string.contains(".")) {
            // try getting themed icon with image suffix.
            string.chop(string.count() - string.lastIndexOf("."));
            thumbnail = QIcon::fromTheme(string);
        }
    }

    //add special path search /use/share/pixmaps
    if (thumbnail.isNull())
    {
        QString path = QString("/usr/share/pixmaps/%1.%2").arg(_icon_string).arg("png");
        QString path_svg = QString("/usr/share/pixmaps/%1.%2").arg(_icon_string).arg("svg");
        //qDebug() << "createDesktopFileThumbnail path:" <<path;
        if(QFile::exists(path)){
            thumbnail=QIcon(path);
        }
        else if(QFile::exists(path_svg)){
            thumbnail=QIcon(path_svg);
        }
        else{
            //search /usr/share/icons/hicolor/scalable/apps
            //fix installed app desktop icon not loaded in time issue
            path_svg = QString("/usr/share/icons/hicolor/scalable/apps/%1.%2").arg(_icon_string).arg("svg");
            if(QFile::exists(path_svg))
            {
               thumbnail=QIcon(path_svg);
            }
        }

        //still not find icon, default find 64*64 png
        //link to bug#69429, after install app not show icon issue
        if (thumbnail.isNull())
        {
            path = QString("/usr/share/icons/hicolor/64x64/apps/%1.%2").arg(_icon_string).arg("png");
            if(QFile::exists(path)){
                thumbnail=QIcon(path);
            }
        }
    }

//    g_free(_icon_string);
//    g_object_unref(_desktop_file);

    if (!thumbnail.isNull()) {
        insertOrUpdateThumbnail(uri, thumbnail);
        if (watcher) {
            watcher->fileChanged(uri);
        }
    }

    return;
}

//is system has atril software
void ThumbnailManager::findAtril()
{
    QtConcurrent::run([](){
        GList *infos = g_app_info_get_all();
        GList *l = infos;
        while (l && ! m_tril_exist) {
            const char *cmd = g_app_info_get_executable(static_cast<GAppInfo*>(l->data));
            QString tmp = cmd;
            if (tmp.contains("atril")) {
                m_tril_exist = true;
            }
            l = l->next;
        }
        g_list_free_full(infos, g_object_unref);
    });
}

void ThumbnailManager::createThumbnailInternal(const QString &uri, std::shared_ptr<FileWatcher> watcher, bool force)
{
    auto settings = GlobalSettings::getInstance();
    if (settings->isExist(FORBID_THUMBNAIL_IN_VIEW)) {
        bool do_not_thumbnail = settings->getValue(FORBID_THUMBNAIL_IN_VIEW).toBool();
        if (do_not_thumbnail && !force) {
            qDebug()<<"setting is not thumbnail";
            return;
        }
    }

    //NOTE: we should do createThumbnail() after we have queried the file's info.
    auto info = FileInfo::fromUri(uri);
    //qDebug()<<"file uri:"<< uri << " mime type:" << info->mimeType();
    //qDebug()<<"file path:" << info->filePath();
    //qDebug()<<"file modify time:" << info->modifiedTime();

    if (!info->mimeType().isEmpty()) {
        if (!info->customIcon().isEmpty()) {
            auto icon = GenericThumbnailer::generateThumbnail(info->customIcon());
            if (!icon.isNull()) {
                insertOrUpdateThumbnail(uri, icon);
                if (watcher) {
                    watcher->fileChanged(uri);
                }
            }
        }
        else if (info->isImagePdfFile())
        {
             qDebug() <<"isImagePdfFile m_tril_exist:" <<m_tril_exist;
             if (m_tril_exist)
             {
                 createImagePdfFileThumbnail(uri, watcher);
             }
        }
        else if (info->isImageFile()) {
            createImageFileThumbnail(uri, watcher);
        }
        else if (info->mimeType().contains("pdf")) {
            createPdfFileThumbnail(uri, watcher);
        }
        else if(info->isVideoFile()) {
            createVideFileThumbnail(uri, watcher);
        }
        else if (info->isOfficeFile()) {
            createOfficeFileThumbnail(uri, watcher);
        }
        else if (info->isDesktopFile()) {
            createDesktopFileThumbnail(uri, watcher);
        }
        else {
            //qDebug()<<"the file type: " << info->mimeType();
            //qDebug()<<"the mime type can not generate thumbnail.";
        }
    }
}

void ThumbnailManager::createThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher, bool force)
{
    qDebug() <<"createThumbnail:" <<force<<uri;
    auto thumbnail = tryGetThumbnail(uri);
    if (!thumbnail.isNull()) {
        if (!force) {
            watcher->thumbnailUpdated(uri);
            watcher->fileChanged(uri);
            qDebug() <<"createThumbnail return:" <<uri;
            return;
        }
    }

    // check if need thumbnail
    bool needThumbnail = false;

    auto info = FileInfo::fromUri(uri);

    if (!info->customIcon().isEmpty() && info->customIcon().startsWith("/"))
        needThumbnail = true;

    if (!info->mimeType().isEmpty()) {
        if (info->isImageFile()) {
            needThumbnail = true;
        }
        else if (info->mimeType().contains("pdf")) {
            needThumbnail = true;
        }
        else if(info->isVideoFile()) {
            needThumbnail = true;
        }
        else if (info->isOfficeFile()) {
            needThumbnail = true;
        }
        else if (info->uri().endsWith(".desktop")) {
            if (thumbnail.isNull())
            {
                needThumbnail = false;
                updateDesktopFileThumbnail(uri, watcher);
            }
            else
                needThumbnail = true;
        }
    }

    if (!needThumbnail)
        return;

    auto thumbnailJob = new ThumbnailJob(uri, watcher, this);
    m_thumbnail_thread_pool->start(thumbnailJob);
    qDebug() <<"createThumbnail thumbnailJob start:" <<uri;
}

void ThumbnailManager::updateDesktopFileThumbnail(const QString &uri, std::shared_ptr<FileWatcher> watcher)
{
    auto info = FileInfo::fromUri(uri);
    if (info->uri().endsWith(".desktop")) {
        //qDebug()<<"is desktop file"<<uri;
        //get desktop file icon.
        //async
        //qDebug()<<"desktop file"<<uri;
        QtConcurrent::run([=]() {
            createDesktopFileThumbnail(uri, watcher);
        });
    } else {
        releaseThumbnail(uri);
        if (watcher) {
            watcher->thumbnailUpdated(uri);
        }
    }
}

void ThumbnailManager::clearThumbnail()
{
    m_semaphore->acquire();
    if (!m_hash.isEmpty()) {
        m_hash.clear();
    }
    m_semaphore->release();
}

void ThumbnailManager::releaseThumbnail(const QString &uri)
{
    m_semaphore->acquire();
    m_hash.remove(uri);
    m_semaphore->release();
}

void ThumbnailManager::releaseThumbnail(const QStringList &uris)
{
    m_semaphore->acquire();
    for (auto uri : uris) {
        m_hash.remove(uri);
    }
    m_semaphore->release();
}

const QIcon ThumbnailManager::tryGetThumbnail(const QString &uri)
{
    m_semaphore->acquire();
    auto icon = m_hash.value(uri);
    m_semaphore->release();
    return icon;
}
