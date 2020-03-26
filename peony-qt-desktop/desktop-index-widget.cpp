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

#include <QMouseEvent>

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

    // draw text shadow
    p.save();
    p.translate(1, 1);
    QFontMetrics fm(m_current_font);

    style()->drawItemText(&p,
                          m_text_rect,
                          Qt::AlignTop|Qt::TextWrapAnywhere|Qt::AlignHCenter,
                          this->palette(),
                          true,
                          m_option.text,
                          QPalette::Shadow);
    p.restore();

    // draw text
    style()->drawItemText(&p,
                          m_text_rect,
                          Qt::AlignTop|Qt::TextWrapAnywhere|Qt::AlignHCenter,
                          this->palette(),
                          true,
                          m_option.text,
                          QPalette::HighlightedText);
    p.restore();

    bgColor.setAlpha(255*0.8);
    p.setPen(bgColor);
    p.drawRect(this->rect().adjusted(0, 0, -1, -1));
}

void DesktopIndexWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        auto *view = m_delegate->getView();
        view->m_real_do_edit = true;
        if (view->m_edit_trigger_timer.isActive()) {
            if (view->m_edit_trigger_timer.remainingTime() < 2250 && view->m_edit_trigger_timer.remainingTime() > 0) {
                QTimer::singleShot(300, view, [=](){
                    if (view->m_real_do_edit) {
                        view->setIndexWidget(m_index, nullptr);
                        view->edit(m_index);
                    }
                });
            } else {
                view->m_real_do_edit = false;
                view->m_edit_trigger_timer.stop();
            }
        } else {
            view->m_real_do_edit = false;
            view->m_edit_trigger_timer.start();
        }
        event->accept();
        return;
//        if (m_edit_trigger.isActive()) {
//            qDebug()<<"IconViewIndexWidget::mousePressEvent: edit"<<e->type();
//            m_delegate->getView()->setIndexWidget(m_index, nullptr);
//            m_delegate->getView()->edit(m_index);
//            return;
//        }
    }
    QWidget::mousePressEvent(event);
}

void DesktopIndexWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_delegate->getView()->doubleClicked(m_index);
    return;
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
    setFixedHeight(textRect.bottom() + 10);
}
