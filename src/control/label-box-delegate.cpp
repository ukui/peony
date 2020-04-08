#include "label-box-delegate.h"

#include <QStyleOptionViewItem>
#include <QDebug>

LabelBoxDelegate::LabelBoxDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    qDebug() << "LabelBoxDelegate enter";
}

void LabelBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qDebug() << "LabelBoxDelegate paint";
    auto opt = option;
    opt.decorationSize = QSize();
    QStyledItemDelegate::paint(painter, opt, index);
}
