#include "side-bar-personal-item.h"
#include "side-bar-model.h"
#include "file-utils.h"
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
        m_icon_name = "personal";

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
        return;
    }
    m_uri = uri;
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
