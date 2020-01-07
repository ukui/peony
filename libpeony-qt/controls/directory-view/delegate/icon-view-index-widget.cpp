/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "icon-view-index-widget.h"
#include "icon-view-delegate.h"
#include "icon-view.h"

#include <QPainter>
#include <QPaintEvent>
#include <QLabel>

#include <QApplication>
#include <QStyle>
#include <QTextEdit>
#include <QScrollBar>

#include "file-info.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-item.h"

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewIndexWidget::IconViewIndexWidget(const IconViewDelegate *delegate, const QStyleOptionViewItem &option, const QModelIndex &index, QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);

    m_option = option;
    m_index = index;

    m_delegate = delegate;

    QSize size = delegate->sizeHint(option, index);
    setMinimumSize(size);

    //extra emblems
    auto proxy_model = static_cast<FileItemProxyFilterSortModel*>(delegate->getView()->model());
    auto item = proxy_model->itemFromIndex(index);
    if (item) {
        m_info = item->info();
    }

    m_delegate->initStyleOption(&m_option, m_index);
    m_option.features.setFlag(QStyleOptionViewItem::WrapText);
    m_option.textElideMode = Qt::ElideNone;

    auto opt = m_option;
    opt.rect.setHeight(9999);
    opt.rect.moveTo(0, 0);

    //qDebug()<<m_option.rect;
    auto iconExpectedSize = m_delegate->getView()->iconSize();
    QRect iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    QRect textRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);
    auto y_delta = iconExpectedSize.height() - iconRect.height();
    opt.rect.moveTo(opt.rect.x(), opt.rect.y() + y_delta);
    setFixedHeight(iconExpectedSize.height() + textRect.height() + 20 + opt.text.size()/12 * 12);

    m_option = opt;
}

void IconViewIndexWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter p(this);
    //p.fillRect(0, 0, 999, 999, Qt::red);

    IconView *view = m_delegate->getView();
    auto visualRect = view->visualRect(m_index);
    this->move(visualRect.topLeft());

    //qDebug()<<m_option.backgroundBrush;
    //qDebug()<<this->size();

    auto opt = m_option;
    p.fillRect(opt.rect, m_delegate->selectedBrush());

    //add \n in long file name to show complete name when choose
    const int CharCount = 13;
    QString temp = opt.text.left(CharCount);
    int count = 1;
    while (opt.text.size() > CharCount * count)
    {
        temp.append("\n");
        if (opt.text.size() > CharCount* (count + 1))
           temp += opt.text.mid(CharCount * count +1, CharCount);
        else {
            temp += opt.text.mid(CharCount * count +1, -1);
        }
        count++;
    }
    opt.text = temp;
    //qDebug()<< "size:" <<opt.text.size() << "temp:" << temp << count;

    visualRect.size().setHeight(this->size().height() + count * 10);
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, &p, opt.widget);
    //qDebug() << "visualRect:" << visualRect << "text:" << text_opt.text;

    //extra emblems
    if (!m_info.lock()) {
        return;
    }
    auto info = m_info.lock();
    //paint symbolic link emblems
    if (info->isSymbolLink()) {
        QIcon icon = QIcon::fromTheme("emblem-symbolic-link");
        //qDebug()<< "symbolic:" << info->symbolicIconName();
        icon.paint(&p, this->width() - 30, 10, 20, 20, Qt::AlignCenter);
    }

    //paint access emblems
    //NOTE: we can not query the file attribute in smb:///(samba) and network:///.
    if (!info->uri().startsWith("file:")) {
        return;
    }

    auto rect = this->rect();
    if (!info->canRead()) {
        QIcon icon = QIcon::fromTheme("emblem-unreadable");
        icon.paint(&p, rect.x() + 10, rect.y() + 10, 20, 20);
    } else if (!info->canWrite() && !info->canExecute()){
        QIcon icon = QIcon::fromTheme("emblem-readonly");
        icon.paint(&p, rect.x() + 10, rect.y() + 10, 20, 20);
    }
}
