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
    auto font = view->getViewItemFont(&m_option);

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
    p.translate(0, iconSizeExcepted.height() + 15);
    QTextOption textOption(Qt::AlignTop|Qt::AlignHCenter);
    textOption.setWrapMode(QTextOption::WrapAnywhere);

    // draw text shadow
    p.save();
    p.translate(1, 1);
    p.setFont(opt.font);
    p.setPen(QColor(50, 50, 50));
    p.drawText(m_text_rect, m_option.text, textOption);
    p.restore();

    // draw text
    p.setFont(opt.font);
    p.setPen(QColor(245, 245, 245));
    p.drawText(m_text_rect, m_option.text, textOption);
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

    QFontMetrics fm(font);
    auto textRect = QApplication::style()->itemTextRect(fm,
                                                        m_option.rect,
                                                        Qt::AlignTop|Qt::AlignHCenter|Qt::TextWrapAnywhere,
                                                        true,
                                                        m_option.text);

    auto tmpRect = textRect;
    while (textRect.x() < 4) {
        m_option.rect.adjust(1, 0, -1, 0);
        if (m_option.rect.width() > 0) {
            textRect = QApplication::style()->itemTextRect(fm,
                                                           m_option.rect,
                                                           Qt::AlignTop|Qt::AlignHCenter|Qt::TextWrapAnywhere,
                                                           true,
                                                           m_option.text);
        } else {
            m_option.rect.setX(0);
            m_option.rect.setWidth(this->width());
            textRect = tmpRect;
            break;
        }
    }

    textRect.adjust(0, 0, 0, fm.height()/1.5);
    m_text_rect = textRect;

    m_option.font = font;
    auto opt = m_option;
    QRect iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, m_delegate->getView());
    QRect rect = QRect(0, 0, textRect.width(), textRect.height() + iconRect.height() + 20);

    qDebug()<<textRect;
    setFixedHeight(rect.height() > size.height()? rect.height(): size.height());
}
