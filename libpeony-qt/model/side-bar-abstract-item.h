#ifndef SIDEBARABSTRACTITEM_H
#define SIDEBARABSTRACTITEM_H

#include <QObject>
#include <QVector>

#include "peony-core_global.h"

namespace Peony {

class SideBarModel;

class PEONYCORESHARED_EXPORT SideBarAbstractItem : public QObject
{
    friend class SideBarModel;
    Q_OBJECT
public:
    enum Type {
        FavoriteItem,
        PersonalItem,
        FileSystemItem
    };

    explicit SideBarAbstractItem(SideBarModel* model, QObject *parent = nullptr);
    virtual ~SideBarAbstractItem();

    virtual Type type() = 0;

    virtual QString uri() = 0;
    virtual QString displayName() = 0;
    virtual QString iconName() = 0;
    virtual bool hasChildren() = 0;
    virtual bool isRemoveable() = 0;
    virtual bool isEjectable() = 0;
    virtual bool isMountable() = 0;

    virtual bool isMounted() {return false;}

    virtual QModelIndex firstColumnIndex() = 0;
    virtual QModelIndex lastColumnIndex() = 0;

    virtual SideBarAbstractItem *parent() = 0;

protected:
    QVector<SideBarAbstractItem*> *m_children = nullptr;
    SideBarModel *m_model = nullptr;

Q_SIGNALS:
    void findChildrenFinished();
    void updated();

public Q_SLOTS:
    virtual void onUpdated() = 0;

    virtual void eject() = 0;
    virtual void unmount() = 0;
    virtual void format() = 0;

    virtual void findChildren() = 0;
    virtual void findChildrenAsync() = 0;
    virtual void clearChildren();
};

}

#endif // SIDEBARABSTRACTITEM_H
