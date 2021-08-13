#ifndef GROUPLISTDELEGATE_H
#define GROUPLISTDELEGATE_H
#include <QStyledItemDelegate>
#include "src/Style/style.h"
#include <QPainter>
#include <QSettings>
#include "src/Interface/ukuimenuinterface.h"
#include "src/Style/style.h"


class GroupListDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    GroupListDelegate(QObject* parent);
    ~GroupListDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    UkuiMenuInterface* pUkuiMenuInterface=nullptr;
    QSettings *setting=nullptr;

};

#endif // GROUPLISTDELEGATE_H
