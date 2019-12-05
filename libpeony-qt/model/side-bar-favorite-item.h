#ifndef SIDEBARFAVORITEITEM_H
#define SIDEBARFAVORITEITEM_H

#include "peony-core_global.h"

#include "side-bar-abstract-item.h"

namespace Peony {

class PEONYCORESHARED_EXPORT SideBarFavoriteItem : public SideBarAbstractItem
{
    Q_OBJECT
public:
    explicit SideBarFavoriteItem(QString uri,
                                 SideBarFavoriteItem *parentItem,
                                 SideBarModel *model,
                                 QObject *parent = nullptr);
    Type type() override;

    QString uri() override;
    QString displayName() override;
    QString iconName() override;
    bool hasChildren() override;

    bool isRemoveable() override {return false;}
    bool isEjectable() override {return false;}
    bool isMountable() override {return false;}

    QModelIndex firstColumnIndex() override;
    QModelIndex lastColumnIndex() override;

    SideBarAbstractItem *parent() override {return m_parent;}

public Q_SLOTS:
    void eject() override {}
    void unmount() override {}
    void format() override {}

    void onUpdated() override {}

    void findChildren() override {}
    void findChildrenAsync() override {}
    void clearChildren() override {}

private:
    void syncBookMark();

    SideBarFavoriteItem *m_parent = nullptr;

    bool m_is_root_child = false;
    QString m_uri = nullptr;
    QString m_display_name = nullptr;
    QString m_icon_name = nullptr;
};

}

#endif // SIDEBARFAVORITEITEM_H
