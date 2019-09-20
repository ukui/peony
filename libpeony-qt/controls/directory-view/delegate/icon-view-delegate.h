#ifndef ICONVIEWDELEGATE_H
#define ICONVIEWDELEGATE_H

#include <QStyledItemDelegate>

namespace Peony {

namespace DirectoryView {

class IconViewIndexWidget;

class IconViewDelegate : public QStyledItemDelegate
{
    friend class IconViewIndexWidget;

    Q_OBJECT
public:
    explicit IconViewDelegate(QObject *parent = nullptr);

public Q_SLOTS:
    void setCutFiles(const QModelIndexList &indexes);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    //edit
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void setIndexWidget(const QModelIndex &index, QWidget *widget) const;

private:
    QModelIndexList m_cut_indexes;

    QModelIndex m_index_widget_index;
    QWidget *m_index_widget;
};

}

}

#endif // ICONVIEWDELEGATE_H
