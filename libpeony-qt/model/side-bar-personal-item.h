#ifndef SIDEBARPERSONALITEM_H
#define SIDEBARPERSONALITEM_H

#include "peony-core_global.h"
#include "side-bar-abstract-item.h"

namespace Peony {

class SideBarModel;
class PEONYCORESHARED_EXPORT SideBarPersonalItem : public SideBarAbstractItem
{
    Q_OBJECT
public:
    explicit SideBarPersonalItem(QString uri,
                                 SideBarPersonalItem *parentItem,
                                 SideBarModel *model,
                                 QObject *parent = nullptr);

    Type type() override {return SideBarAbstractItem::PersonalItem;}

    QString uri() override {return m_uri;}
    QString displayName() override {return m_display_name;}
    QString iconName() override {return m_icon_name;}
    bool hasChildren() override {return m_is_root_child;}

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
    SideBarPersonalItem *m_parent = nullptr;

    bool m_is_root_child = false;
    QString m_uri = nullptr;
    QString m_display_name = nullptr;
    QString m_icon_name = nullptr;
};

}

#endif // SIDEBARPERSONALITEM_H
