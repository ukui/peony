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

#include "side-bar-favorite-item.h"
#include "side-bar-model.h"
#include "file-utils.h"

#include "file-info.h"
#include "file-info-job.h"

#include "bookmark-manager.h"

#include <QStandardPaths>
#include <QUrl>

using namespace Peony;

bool kydroidInstall = "kydroid:///";
QString kydroidPath = "";

SideBarFavoriteItem::SideBarFavoriteItem(QString uri,
        SideBarFavoriteItem *parentItem,
        SideBarModel *model,
        QObject *parent) : SideBarAbstractItem (model, parent)
{
    m_parent = parentItem;
    m_is_root_child = m_parent == nullptr;
    if (m_is_root_child) {
        m_uri = "favorite:///";
        m_display_name = tr("Favorite");
        //m_icon_name = "emblem-favorite";
        //top dir don't show icon
        m_icon_name = "";

        SideBarFavoriteItem *recentItem = new SideBarFavoriteItem("recent:///", this, m_model);
        QString desktopUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        SideBarFavoriteItem *desktopItem = new SideBarFavoriteItem(desktopUri, this, m_model);
        SideBarFavoriteItem *trashItem = new SideBarFavoriteItem("trash:///", this, m_model);
        m_children->append(recentItem);
        m_children->append(desktopItem);
        m_children->append(trashItem);

        if (FileUtils::isFileExsit("file:///data/usershare")) {
            m_children->append(new SideBarFavoriteItem("file:///data/usershare", this, m_model));
        }

        // check kydroid is install
        if (FileUtils::isFileExsit("file:///var/lib/kydroid") || FileUtils::isFileExsit("file:///var/lib/kmre")) {
            GVfs* vfs = g_vfs_get_default();
            if (vfs) {
                const gchar* const* schemas = g_vfs_get_supported_uri_schemes (vfs);
                if (schemas) {
                    int i = 0;
                    for (; schemas[i] != NULL; ++i) {
                        if (0 == strcmp(schemas[i], "kydroid")) {
                            kydroidInstall = true;
                            kydroidPath = "kydroid:///";
                            break;
                        }
                        else if(0 == strcmp(schemas[i], "kmre"))
                        {
                            kydroidInstall = true;
                            kydroidPath = "kmre:///";
                            break;
                        }
                    }
                }
            }
            if (kydroidInstall)
                m_children->append(new SideBarFavoriteItem(kydroidPath, this, m_model));
        }

        m_model->insertRows(0, m_children->count(), firstColumnIndex());
        //TODO: support custom bookmarks.
        auto bookmark = BookMarkManager::getInstance();
        if (bookmark->isLoaded()) {
            syncBookMark();
        } else {
            connect(bookmark, &BookMarkManager::urisLoaded, this, [=]() {
                syncBookMark();
                disconnect(bookmark, &BookMarkManager::urisLoaded, this, nullptr);
            });
        }
        return;
    }
    m_uri = uri;
    //FIXME: replace BLOCKING api in ui thread.
    m_display_name = FileUtils::getFileDisplayName(uri);
    m_icon_name = FileUtils::getFileIconName(uri);

    m_info = FileInfo::fromUri(uri);
    auto infoJob = new FileInfoJob(m_info);
    infoJob->setAutoDelete();
    connect(infoJob, &FileInfoJob::queryAsyncFinished, this, [=](){
        Q_EMIT this->queryInfoFinished();
    });
    infoJob->queryAsync();
}

SideBarAbstractItem::Type SideBarFavoriteItem::type() {
    return SideBarAbstractItem::FavoriteItem;
}

QString SideBarFavoriteItem::uri()
{
    return m_uri;
}

QString SideBarFavoriteItem::displayName()
{
    if (!m_info)
        return m_display_name;
    return m_info.get()->displayName();
}

QString SideBarFavoriteItem::iconName()
{
    if (!m_info)
        return m_icon_name;
    return m_info.get()->iconName();
}

bool SideBarFavoriteItem::hasChildren()
{
    return m_is_root_child;
}

QModelIndex SideBarFavoriteItem::firstColumnIndex()
{
    //TODO: bind with model
    return m_model->firstColumnIndex(this);
}

QModelIndex SideBarFavoriteItem::lastColumnIndex()
{
    //TODO: bind with model
    return m_model->lastColumnIndex(this);
}

void SideBarFavoriteItem::syncBookMark()
{
    qDebug()<<"sync book mark=================="<<this->displayName();
    auto bookmark = BookMarkManager::getInstance();
    auto uris = bookmark->getCurrentUris();
    for (auto uri : uris) {
        auto item = new SideBarFavoriteItem(uri, this, m_model);
        *m_children<<item;
        m_model->insertRows(m_children->count() - 1, 1, this->firstColumnIndex());
    }
    connect(bookmark, &BookMarkManager::bookMarkAdded, this, [=](const QString &uri, bool successed) {
        if (successed) {
            auto item = new SideBarFavoriteItem(uri, this, m_model);
            *m_children<<item;
            m_model->insertRows(m_children->count() - 1, 1, this->firstColumnIndex());
        }
    });
    connect(bookmark, &BookMarkManager::bookMarkRemoved, this, [=](const QString &uri, bool successed) {
        if (successed) {
            for (auto item : *m_children) {
                if (item->uri() == uri) {
                    m_model->removeRow(m_children->indexOf(item), this->firstColumnIndex());
                    m_children->removeOne(item);
                }
            }
        }
    });
}
