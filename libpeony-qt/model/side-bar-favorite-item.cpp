#include "side-bar-favorite-item.h"
#include "side-bar-model.h"
#include "file-utils.h"

#include "bookmark-manager.h"

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
        m_icon_name = "emblem-favorite";

        SideBarFavoriteItem *recentItem = new SideBarFavoriteItem("recent:///", this, m_model);
        QString desktopUri = "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        SideBarFavoriteItem *desktopItem = new SideBarFavoriteItem(desktopUri, this, m_model);
        SideBarFavoriteItem *trashItem = new SideBarFavoriteItem("trash:///", this, m_model);
        m_children->append(recentItem);
        m_children->append(desktopItem);
        m_children->append(trashItem);
        m_model->insertRows(0, m_children->count(), firstColumnIndex());
        //TODO: support custom bookmarks.
        auto bookmark = BookMarkManager::getInstance();
        if (bookmark->isLoaded()) {
            syncBookMark();
        } else {
            connect(bookmark, &BookMarkManager::urisLoaded, this, [=](){
                syncBookMark();
                disconnect(bookmark, &BookMarkManager::urisLoaded, this, nullptr);
            });
        }
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
    connect(bookmark, &BookMarkManager::bookMarkAdded, this, [=](const QString &uri, bool successed){
        if (successed) {
            auto item = new SideBarFavoriteItem(uri, this, m_model);
            *m_children<<item;
            m_model->insertRows(m_children->count() - 1, 1, this->firstColumnIndex());
        }
    });
    connect(bookmark, &BookMarkManager::bookMarkRemoved, this, [=](const QString &uri, bool successed){
        if (successed) {
            for (auto item : *m_children) {
                if (item->uri() == uri) {
                    m_model->removeRow(m_children->indexOf(item));
                    m_children->removeOne(item);
                }
            }
        }
    });
}
