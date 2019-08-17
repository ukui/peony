#include "side-bar-abstract-item.h"
#include "side-bar-model.h"
#include <QDebug>

using namespace Peony;

SideBarAbstractItem::SideBarAbstractItem(SideBarModel *model, QObject *parent) : QObject(parent)
{
    m_model = model;
    m_children = new QVector<SideBarAbstractItem*>();
}

SideBarAbstractItem::~SideBarAbstractItem()
{
    qDebug()<<"~SideBarAbstractItem";
    for (auto child : *m_children) {
        delete child;
    }
    delete m_children;
}

void SideBarAbstractItem::clearChildren()
{
    m_model->beginRemoveRows(firstColumnIndex(), 0, m_children->count() - 1);
    for (auto child : *m_children) {
        delete child;
    }
    m_children->clear();
    m_model->endRemoveRows();
}
