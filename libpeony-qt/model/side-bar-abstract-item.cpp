#include "side-bar-abstract-item.h"

using namespace Peony;

SideBarAbstractItem::SideBarAbstractItem(SideBarModel *model, QObject *parent) : QObject(parent)
{
    m_model = model;
}
