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
    explicit SideBarAbstractItem(SideBarModel* model, QObject *parent = nullptr);
    virtual ~SideBarAbstractItem() = 0;
    virtual QString uri() = 0;
    virtual QString displayName() = 0;
    virtual QString iconName() = 0;
    virtual bool hasChildren() = 0;
    virtual bool isRemoveable() = 0;
    virtual bool isEjectable() = 0;
    virtual bool isMountable() = 0;

protected:
    QVector<SideBarAbstractItem*> *m_children = nullptr;
    SideBarModel *m_model = nullptr;

Q_SIGNALS:
    void updated();

public Q_SLOTS:
    virtual void onUpdated() = 0;

    virtual void edject() = 0;
    virtual void unmount() = 0;
    virtual void format() = 0;
};

}

#endif // SIDEBARABSTRACTITEM_H
