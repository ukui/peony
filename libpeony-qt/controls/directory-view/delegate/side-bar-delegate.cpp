#include "side-bar-delegate.h"
#include "side-bar.h"

#include <QPainter>

#include <QDebug>

using namespace Peony;

SideBarDelegate::SideBarDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void SideBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //return QStyledItemDelegate::paint(painter, option, index);

    //FIXME: maybe i should use qss "show-decoration-selected" instead

    if (index.column() != 0 || !index.isValid())
        return QStyledItemDelegate::paint(painter, option, index);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    auto sideBarView = qobject_cast<SideBar*>(parent());

    auto visualRect = sideBarView->visualRect(index);
    visualRect.setX(0);
    painter->fillRect(visualRect, opt.widget->palette().brush(QPalette::Base));

    int x = opt.rect.x();
    opt.rect.setX(0);
    sideBarView->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, sideBarView);

    opt.rect.setX(x + 7);

    auto iconRect = QRect(opt.rect.topLeft() - QPoint(opt.rect.height(), 0), QSize(opt.rect.height(), opt.rect.height()));
    iconRect.adjust(8, 8, -8, -8);
    iconRect.moveTo(iconRect.topLeft() + QPoint(6, 0));
    if (sideBarView->model()->hasChildren(index)) {
        if (sideBarView->isExpanded(index)) {
            auto icon = QIcon::fromTheme("gtk-go-down");
            icon.paint(painter, iconRect, Qt::AlignCenter);
        } else {
            auto icon = QIcon::fromTheme("gtk-go-forward-ltr");
            icon.paint(painter, iconRect);
        }
    }

    sideBarView->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, sideBarView);

}

QSize SideBarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() != 0)
        return QStyledItemDelegate::sizeHint(option, index);

    auto size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(28);
    size.setWidth(size.width() + 10);
    return size;
}
