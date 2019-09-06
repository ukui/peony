#ifndef ICONVIEWDELEGATE_H
#define ICONVIEWDELEGATE_H

#include <QStyledItemDelegate>

namespace Peony {

namespace DirectoryView {

class IconViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit IconViewDelegate(QObject *parent = nullptr);

public Q_SLOTS:
    void setCutFiles(const QModelIndexList &indexes);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QModelIndexList m_cut_indexes;
};

}

}

#endif // ICONVIEWDELEGATE_H
