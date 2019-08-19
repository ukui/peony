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
    m_model->beginRemoveRows(firstColumnIndex(), 0, m_children->count());
    for (auto child : *m_children) {
        m_children->removeOne(child);
        child->deleteLater();
    }
    m_children->clear();
    m_model->endRemoveRows();
    qDebug()<<"clear children has children"<<m_model->hasChildren(firstColumnIndex());
}
