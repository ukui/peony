#ifndef LABELBOXDELEGATE_H
#define LABELBOXDELEGATE_H

#include <QStyledItemDelegate>

class LabelBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit LabelBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index);

Q_SIGNALS:

};

#endif // LABELBOXDELEGATE_H
