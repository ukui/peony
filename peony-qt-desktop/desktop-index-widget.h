#ifndef DESKTOPINDEXWIDGET_H
#define DESKTOPINDEXWIDGET_H

#include <QWidget>
#include <QStyleOptionViewItem>
#include <QModelIndex>

namespace Peony {

class DesktopIconViewDelegate;

class DesktopIndexWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DesktopIndexWidget(DesktopIconViewDelegate *delegate,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index,
                                QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e);

    void updateItem();

private:
    QStyleOptionViewItem m_option;
    QModelIndex m_index;
    const DesktopIconViewDelegate *m_delegate;
    QFont m_current_font;
};

}

#endif // DESKTOPINDEXWIDGET_H
