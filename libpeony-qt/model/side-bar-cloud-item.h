#ifndef SIEDBARCLOUDITEM_H
#define SIEDBARCLOUDITEM_H

#include "peony-core_global.h"
#include "side-bar-abstract-item.h"

namespace Peony {

class SideBarModel;
class PEONYCORESHARED_EXPORT SideBarCloudItem : public SideBarAbstractItem
{
    Q_OBJECT
public:
    explicit SideBarCloudItem(QString uri,
                              SideBarCloudItem *parentItem,
                              SideBarModel *model,
                              QObject *parent = nullptr);

    Type type() override {
        return SideBarAbstractItem::PersonalItem;
    }

    QString uri() override {
        return m_uri;
    }
    QString displayName() override {
        return m_display_name;
    }
    QString iconName() override {
        return m_icon_name;
    }
    bool hasChildren() override {
        return m_is_root_child;
    }

    bool isRemoveable() override {
        return false;
    }
    bool isEjectable() override {
        return false;
    }
    bool isMountable() override {
        return false;
    }

    QModelIndex firstColumnIndex() override;
    QModelIndex lastColumnIndex() override;

    SideBarAbstractItem *parent() override {
        return m_parent;
    }

public Q_SLOTS:
    void eject(GMountUnmountFlags ejectFlag) override {}
    void unmount() override {}
    void format() override {}

    void onUpdated() override {}

    void findChildren() override {}
    void findChildrenAsync() override {}
    void clearChildren() override {}

private:
    SideBarCloudItem *m_parent = nullptr;

    bool m_is_root_child = false;
    QString m_uri = nullptr;
    QString m_display_name = nullptr;
    QString m_icon_name = nullptr;
};

}
#endif // SIEDBARCLOUDITEM_H
