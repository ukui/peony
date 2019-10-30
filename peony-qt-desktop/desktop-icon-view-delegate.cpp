#include "desktop-icon-view-delegate.h"
#include "desktop-icon-view.h"

#include <QPushButton>

using namespace Peony;

DesktopIconViewDelegate::DesktopIconViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_styled_button = new QPushButton;
}

DesktopIconViewDelegate::~DesktopIconViewDelegate()
{
    m_styled_button->deleteLater();
}

void DesktopIconViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::paint(painter, option, index);
}

QSize DesktopIconViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto view = qobject_cast<Peony::DesktopIconView*>(parent());
    auto zoomLevel = view->zoomLevel();
    switch (zoomLevel) {
    case DesktopIconView::Small:
        return QSize(60, 60);
    case DesktopIconView::Normal:
        return QSize(90, 90);
    case DesktopIconView::Large:
        return QSize(105, 118);
    case DesktopIconView::Huge:
        return QSize(120, 140);
    }
}
