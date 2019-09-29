#ifndef SIDEBARDELEGATE_H
#define SIDEBARDELEGATE_H

#include <QStyledItemDelegate>
#include "peony-core_global.h"

namespace Peony {

class PEONYCORESHARED_EXPORT SideBarDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SideBarDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /*
    //edit
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    */
};

}

#endif // SIDEBARDELEGATE_H
