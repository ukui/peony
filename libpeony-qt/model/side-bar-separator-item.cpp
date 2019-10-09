#include "side-bar-separator-item.h"
#include "side-bar-model.h"

using namespace Peony;

SideBarSeparatorItem::SideBarSeparatorItem(Details type,
                                           SideBarAbstractItem *parentItem,
                                           SideBarModel *model,
                                           QObject *parent) : SideBarAbstractItem(model, parent)
{
    m_type = type;
    m_parent = parentItem;
}

QModelIndex SideBarSeparatorItem::firstColumnIndex()
{
    return m_model->firstCloumnIndex(this);
}

QModelIndex SideBarSeparatorItem::lastColumnIndex()
{
    return m_model->lastCloumnIndex(this);
}
