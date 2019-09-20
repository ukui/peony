#include "icon-view-index-widget.h"
#include "icon-view-delegate.h"

#include <QPainter>
#include <QPaintEvent>
#include <QLabel>

#include <QApplication>

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewIndexWidget::IconViewIndexWidget(const IconViewDelegate *delegate, const QStyleOptionViewItem &option, const QModelIndex &index, QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);

    m_option = option;
    m_option.widget = nullptr;

    m_index = index;

    //TODO: should i create a new view (with a unlimte grid height)
    //and delegate for indexWidget item's calculation?
    //For now i couldn't use this delegate to calculate the real text
    //height.
    delegate->initStyleOption(&m_option, index);

    m_delegate = delegate;

    QSize size = delegate->sizeHint(option, index);
    setFixedWidth(size.width());
    setFixedHeight(200);

    //this->style()->drawControl(QStyle::CE_ItemViewItem, &opt, &p, opt.widget);
}

void IconViewIndexWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter p(this);
    //p.fillRect(0, 0, 999, 999, Qt::red);

    m_option.rect.setHeight(9999);
    m_option.rect.moveTo(0, 10);

    qDebug()<<m_option.rect;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &m_option, &p, nullptr);
}
