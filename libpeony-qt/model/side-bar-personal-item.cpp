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

#include "side-bar-personal-item.h"
#include "side-bar-model.h"
#include "file-utils.h"
#include "file-watcher.h"

#include <QStandardPaths>

using namespace Peony;

SideBarPersonalItem::SideBarPersonalItem(QString uri,
        SideBarPersonalItem *parentItem,
        SideBarModel *model,
        QObject *parent) : SideBarAbstractItem (model, parent)
{
    m_parent = parentItem;
    m_is_root_child = parentItem == nullptr;
    if (m_is_root_child) {
        QString homeUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        m_uri = homeUri;
        m_display_name = tr("Personal");
        //m_icon_name = "emblem-personal";
        //top dir don't show icon
        m_icon_name = "";

        QString documentUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        SideBarPersonalItem *documentItem = new SideBarPersonalItem(documentUri,
                this,
                m_model);
        m_children->append(documentItem);

        QString pictureUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        SideBarPersonalItem *pictureItem = new SideBarPersonalItem(pictureUri,
                this,
                m_model);
        m_children->append(pictureItem);

        QString mediaUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        SideBarPersonalItem *mediaItem = new SideBarPersonalItem(mediaUri,
                this,
                m_model);
        m_children->append(mediaItem);

        QString downloadUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        SideBarPersonalItem *downloadItem = new SideBarPersonalItem(downloadUri,
                this,
                m_model);
        m_children->append(downloadItem);

        QString musicUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
        SideBarPersonalItem *musicItem = new SideBarPersonalItem(musicUri,
                this,
                m_model);
        m_children->append(musicItem);

        m_model->insertRows(0, 5, firstColumnIndex());

        //! \todo Add monitor dir del or create
//        this->initWatcher();
//        m_watcher->startMonitor();
//        connect(m_watcher.get(), &FileWatcher::fileDeleted, this, [=](const QString& uri) {
//            GFile* file = g_file_new_for_uri(uri.toLatin1().constData());
//            QString path = g_file_get_path(file);
//            qDebug() << path;
//            QString _uri = "file://" + path;
//            if (_uri == documentUri) {
//                m_model->removeRow(m_children->indexOf(documentItem), this->firstColumnIndex());
//            }
//            else if (_uri == pictureUri) {
//                m_model->removeRow(m_children->indexOf(pictureItem), this->firstColumnIndex());
//            }
//            else if (_uri == mediaUri) {
//                m_model->removeRow(m_children->indexOf(mediaItem), mediaItem->lastColumnIndex());
//            }
//            else if (_uri == downloadUri) {
//                m_model->removeRow(m_children->indexOf(downloadItem), this->firstColumnIndex());
//            }
//            else if (_uri == musicUri) {
//                m_model->removeRow(m_children->indexOf(musicItem), this->firstColumnIndex());
//            }

//            g_object_unref(file);
//        });

        return;
    }
    m_uri = uri;
    //FIXME: replace BLOCKING api in ui thread.
    m_display_name = FileUtils::getFileDisplayName(uri);
    m_icon_name = FileUtils::getFileIconName(uri);
}

QModelIndex SideBarPersonalItem::firstColumnIndex()
{
    return m_model->firstCloumnIndex(this);
}

QModelIndex SideBarPersonalItem::lastColumnIndex()
{
    return m_model->lastCloumnIndex(this);
}

void SideBarPersonalItem::initWatcher()
{
    if (!m_watcher) {
        m_watcher = std::make_shared<FileWatcher>(m_uri);
    }
}
