#include "side-bar-favorite-item.h"
#include "side-bar-model.h"
#include "file-utils.h"

#include <QStandardPaths>
#include <QUrl>

using namespace Peony;

SideBarFavoriteItem::SideBarFavoriteItem(QString uri,
                                         SideBarFavoriteItem *parentItem,
                                         SideBarModel *model,
                                         QObject *parent) : SideBarAbstractItem (model, parent)
{
    m_parent = parentItem;
    m_is_root_child = m_parent == nullptr;
    if (m_is_root_child) {
        m_display_name = tr("Favorite");
        m_icon_name = "favorite";

        SideBarFavoriteItem *recentItem = new SideBarFavoriteItem("recent:///", this, m_model);
        QString desktopUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        SideBarFavoriteItem *desktopItem = new SideBarFavoriteItem(desktopUri, this, m_model);
        SideBarFavoriteItem *trashItem = new SideBarFavoriteItem("trash:///", this, m_model);
        m_children->append(recentItem);
        m_children->append(desktopItem);
        m_children->append(trashItem);
        m_model->insertRows(0, m_children->count(), firstColumnIndex());
        //TODO: support custom bookmarks.
        return;
    }
    m_uri = uri;
    m_display_name = FileUtils::getFileDisplayName(uri);
    m_icon_name = FileUtils::getFileIconName(uri);
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
    return m_display_name;
}

QString SideBarFavoriteItem::iconName()
{
    return m_icon_name;
}

bool SideBarFavoriteItem::hasChildren()
{
    return m_is_root_child;
}

QModelIndex SideBarFavoriteItem::firstColumnIndex()
{
    //TODO: bind with model
    return m_model->firstCloumnIndex(this);
}

QModelIndex SideBarFavoriteItem::lastColumnIndex()
{
    //TODO: bind with model
    return m_model->firstCloumnIndex(this);
}
