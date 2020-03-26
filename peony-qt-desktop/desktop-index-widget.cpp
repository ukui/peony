/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "icon-view-delegate.h"
#include "desktop-index-widget.h"

#include "desktop-icon-view-delegate.h"
#include "desktop-icon-view.h"

#include <QPainter>
#include <QStyle>
#include <QApplication>

#include <QTextEdit>
#include <QTextOption>

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

    m_current_font = QApplication::font();

    updateItem();
}

DesktopIndexWidget::~DesktopIndexWidget()
{

}

void DesktopIndexWidget::paintEvent(QPaintEvent *e)
{
    //qDebug()<<"paint";
    auto visualRect = m_delegate->getView()->visualRect(m_index);
    move(visualRect.topLeft());

    Q_UNUSED(e)
    QPainter p(this);
    auto bgColor = m_option.palette.highlight().color();
    bgColor.setAlpha(255*0.7);
    p.fillRect(this->rect(), bgColor);

    auto view = m_delegate->getView();
    //auto font = view->getViewItemFont(&m_option);

    auto opt = m_option;
    auto iconSizeExcepted = m_delegate->getView()->iconSize();
    auto iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    int y_delta = iconSizeExcepted.height() - iconRect.height();
    opt.rect.moveTo(opt.rect.x(), opt.rect.y() + y_delta);

    //setFixedHeight(opt.rect.height() + y_delta);

    // draw icon
    opt.text = nullptr;
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, &p, m_delegate->getView());

    p.save();
    p.translate(0, 5 + m_delegate->getView()->iconSize().height() + 5);

    // draw text shadow
    p.save();
    p.translate(1, 1);
    QColor shadow = Qt::black;
    shadow.setAlpha(127);
    p.setPen(shadow);
    Peony::DirectoryView::IconViewTextHelper::paintText(&p,
                                                        m_option,
                                                        m_index,
                                                        9999,
                                                        2);
//    QFontMetrics fm(m_current_font);

//    style()->drawItemText(&p,
//                          m_text_rect,
//                          Qt::AlignTop|Qt::TextWrapAnywhere|Qt::AlignHCenter,
//                          this->palette(),
//                          true,
//                          m_option.text,
//                          QPalette::Shadow);
    p.restore();

    // draw text
    p.setPen(m_option.palette.highlightedText().color());
    Peony::DirectoryView::IconViewTextHelper::paintText(&p,
                                                        m_option,
                                                        m_index,
                                                        9999,
                                                        2);
//    style()->drawItemText(&p,
//                          m_text_rect,
//                          Qt::AlignTop|Qt::TextWrapAnywhere|Qt::AlignHCenter,
//                          this->palette(),
//                          true,
//                          m_option.text,
//                          QPalette::HighlightedText);
    p.restore();

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
    setFixedWidth(visualRect.width());

    m_option.rect.setWidth(visualRect.width());

    int rawHeight = size.height();
    auto textSize = Peony::DirectoryView::IconViewTextHelper::getTextSizeForIndex(m_option, m_index, 2);
    int fixedHeight = 5 + m_delegate->getView()->iconSize().height() + 5 + textSize.height() + 5;
    if (fixedHeight < rawHeight)
        fixedHeight = rawHeight;

    m_option.text = m_index.data().toString();
    //qDebug()<<m_option.text;
    m_option.features.setFlag(QStyleOptionViewItem::WrapText);
    m_option.textElideMode = Qt::ElideNone;

    //m_option.rect.setHeight(9999);
    m_option.rect.moveTo(0, 0);

    //qDebug()<<m_option.rect;
    //auto font = view->getViewItemFont(&m_option);

    auto rawTextRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemText, &m_option, m_option.widget);
    auto iconSizeExcepted = m_delegate->getView()->iconSize();

    auto iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &m_option, m_option.widget);

    auto y_delta = iconSizeExcepted.height() - iconRect.height();

    rawTextRect.setTop(iconRect.bottom() + y_delta + 5);
    rawTextRect.setHeight(9999);

    QFontMetrics fm(m_current_font);
    auto textRect = QApplication::style()->itemTextRect(fm,
                                                        rawTextRect,
                                                        Qt::AlignTop|Qt::AlignHCenter|Qt::TextWrapAnywhere,
                                                        true,
                                                        m_option.text);

    m_text_rect = textRect;

    //m_option.font = font;
    auto opt = m_option;

    qDebug()<<textRect;
//    setFixedHeight(textRect.bottom() + 10);
    setFixedHeight(fixedHeight);
}
