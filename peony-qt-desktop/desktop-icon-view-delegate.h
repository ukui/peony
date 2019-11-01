#ifndef DESKTOPICONVIEWDELEGATE_H
#define DESKTOPICONVIEWDELEGATE_H

#include <QStyledItemDelegate>

class QPushButton;

namespace Peony {

class DesktopIconView;

class DesktopIconViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DesktopIconViewDelegate(QObject *parent = nullptr);
    ~DesktopIconViewDelegate() override;

    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override {return QStyledItemDelegate::initStyleOption(option, index);}
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    DesktopIconView *getView() const;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    //edit
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    QPushButton *m_styled_button;
};

}

#endif // DESKTOPICONVIEWDELEGATE_H
