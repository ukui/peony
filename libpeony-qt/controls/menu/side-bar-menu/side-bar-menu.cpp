#include "side-bar-menu.h"
#include "side-bar-abstract-item.h"

#include "bookmark-manager.h"
#include "properties-window.h"

#include "menu-plugin-manager.h"

#include <QAction>
#include <QModelIndex>

#include <QDebug>

using namespace Peony;

SideBarMenu::SideBarMenu(SideBarAbstractItem *item, SideBar *sideBar, QWidget *parent) : QMenu (parent)
{
    m_uri = item->uri();
    m_item = item;
    m_side_bar = sideBar;

    if (!item) {
        auto action = addAction(QIcon::fromTheme("preview-file"), tr("&Properties"));
        action->setEnabled(false);
        return;
    }

    switch (item->type()) {
    case SideBarAbstractItem::FavoriteItem: {
        constructFavoriteActions();
        break;
    }
    case SideBarAbstractItem::PersonalItem: {
        constructPersonalActions();
        break;
    }
    case SideBarAbstractItem::FileSystemItem: {
        constructFileSystemItemActions();
        break;
    }
    default: {
        auto action = addAction(QIcon::fromTheme("preview-file"), tr("&Properties"));
        action->setEnabled(false);
        break;
    }
    }
}

const QList<QAction *> SideBarMenu::constructFavoriteActions()
{
    QList<QAction *> l;
    l<<addAction(QIcon::fromTheme("window-close-symbolic"), tr("&Delete Symbolic"), [=](){
        BookMarkManager::getInstance()->removeBookMark(m_uri);
    });
    if (!m_item->firstColumnIndex().parent().isValid()) {
        l.last()->setEnabled(false);
    } else if (m_item->firstColumnIndex().row() < 3) {
        l.last()->setEnabled(false);
    }

    l<<addAction(QIcon::fromTheme("preview-file"), tr("&Properties"), [=](){
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });
    if (!m_item->firstColumnIndex().parent().isValid()) {
        l.last()->setEnabled(false);
    }

    return l;
}

const QList<QAction *> SideBarMenu::constructPersonalActions()
{
    QList<QAction *> l;

    l<<addAction(QIcon::fromTheme("preview-file"), tr("&Properties"), [=](){
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });

    return l;
}

const QList<QAction *> SideBarMenu::constructFileSystemItemActions()
{
    QList<QAction *> l;

    if (m_item->isEjectable() || m_item->isMountable() || m_item->isEjectable()) {
        l<<addAction(QIcon::fromTheme("media-eject"), tr("&Unmount"), [=](){
            m_item->unmount();
        });
        l.last()->setEnabled(m_item->isMounted());
    }

    auto mgr = MenuPluginManager::getInstance();
    auto ids = mgr->getPluginIds();
    for (auto id : ids) {
        auto factory = mgr->getPlugin(id);
        //qDebug()<<id;
        auto tmp = factory->menuActions(MenuPluginInterface::SideBar, m_uri, QStringList()<<m_uri);
        addActions(tmp);
        for (auto action : tmp) {
            action->setParent(this);
        }
        l<<tmp;
    }

    l<<addAction(QIcon::fromTheme("preview-file"), tr("&Properties"), [=](){
        PropertiesWindow *w = new PropertiesWindow(QStringList()<<m_uri);
        w->show();
    });

    return l;
}

