#include "desktop-index-widget.h"

#include "desktop-icon-view-delegate.h"
#include "desktop-icon-view.h"

#include <QPainter>
#include <QStyle>
#include <QApplication>

#include <QDebug>

using namespace Peony;

DesktopIndexWidget::DesktopIndexWidget(DesktopIconViewDelegate *delegate,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index,
                                       QWidget *parent) : QWidget(parent)
{
    setContentsMargins(0, 0, 0, 0);
    m_option = option;

    m_index = index;
    m_delegate = delegate;

    updateItem();
}

void DesktopIndexWidget::paintEvent(QPaintEvent *e)
{
    //qDebug()<<"paint";

    Q_UNUSED(e)
    QPainter p(this);
    auto bgColor = m_option.palette.highlight().color();
    bgColor.setAlpha(255*0.7);
    p.fillRect(this->rect(), bgColor);

    auto view = m_delegate->getView();
    auto font = view->getViewItemFont(&m_option);

    auto textOpt = m_option;
    textOpt.font = font;
    textOpt.icon = QIcon();
    auto black = QColor(Qt::black);
    black.setAlpha(255*0.8);
    textOpt.palette.setColor(QPalette::Text, black);
    textOpt.rect.moveTo(textOpt.rect.topLeft() + QPoint(1, 1));
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &textOpt, &p, m_delegate->getView());

    auto opt = m_option;
    opt.font = font;
    auto text = opt.text;
    auto white = QColor(Qt::white);
    opt.palette.setColor(QPalette::Text, white);
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, &p, m_delegate->getView());

    bgColor.setAlpha(255*0.8);
    p.setPen(bgColor);
    p.drawRect(this->rect().adjusted(0, 0, -1, -1));
}

void DesktopIndexWidget::updateItem()
{
    auto view = m_delegate->getView();
    m_option = view->viewOptions();
    m_delegate->initStyleOption(&m_option, m_index);

    QSize size = m_delegate->sizeHint(m_option, m_index);
    auto visualRect = m_delegate->getView()->visualRect(m_index);
    move(visualRect.topLeft());
    setFixedWidth(size.width());
    setMinimumHeight(size.height());

    m_option.text = m_index.data().toString();
    //qDebug()<<m_option.text;
    m_option.features.setFlag(QStyleOptionViewItem::WrapText);
    m_option.textElideMode = Qt::ElideNone;

    m_option.rect.setWidth(this->size().width());
    m_option.rect.setHeight(9999);
    m_option.rect.moveTo(0, 0);

    //qDebug()<<m_option.rect;
    auto font = view->getViewItemFont(&m_option);
    auto opt = m_option;
    opt.font = font;
    QRect iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, m_delegate->getView());
    QRect textRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemText, &opt, m_delegate->getView());
    qDebug()<<m_option.text<<textRect;
    QRect rect = QRect(0, 0, textRect.width(), textRect.height() + iconRect.height() + 20);
    m_option.rect.setHeight(rect.height());
    qDebug()<<m_option.rect;

    setFixedHeight(rect.height() > size.height()? rect.height(): size.height());
}
