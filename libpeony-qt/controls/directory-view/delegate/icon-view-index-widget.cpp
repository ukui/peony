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
#include <QTextDocument>
#include <QScrollBar>

#include <QMouseEvent>

#include "file-info.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-item.h"

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewIndexWidget::IconViewIndexWidget(const IconViewDelegate *delegate, const QStyleOptionViewItem &option, const QModelIndex &index, QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);

    m_edit_trigger.setInterval(3000);
    m_edit_trigger.setSingleShot(true);
#if QT_VERSION_CHECK(5, 12, 0)
    QTimer::singleShot(750, this, [=](){
        m_edit_trigger.start();
    });
#else
    QTimer::singleShot(750, [=](){
        m_edit_trigger.start();
    });
#endif
    //use QTextEdit to show full file name when select
    m_edit = new QTextEdit();

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
    auto y_delta = iconExpectedSize.height() - iconRect.height();
    opt.rect.moveTo(opt.rect.x(), opt.rect.y() + y_delta);

    m_option = opt;
}

IconViewIndexWidget::~IconViewIndexWidget()
{
    delete m_edit;
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
    //qDebug()<<this->size() << m_delegate->getView()->iconSize();

    auto opt = m_option;
    p.fillRect(opt.rect, m_delegate->selectedBrush());

    auto tmp = opt.text;
    opt.text = nullptr;
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, &p, opt.widget);
    opt.text = std::move(tmp);

    m_edit->document()->setPlainText(opt.text);
    m_edit->document()->setDefaultFont(opt.font);
    m_edit->document()->setTextWidth(this->size().width());
    m_edit->setAlignment(Qt::AlignTop|Qt::AlignHCenter);

    //auto textRectF = QRectF(0, m_delegate->getView()->iconSize().height(), this->width(), this->height());
    p.save();
    p.translate(0, m_delegate->getView()->iconSize().height() + 9);
    //m_edit->document()->drawContents(&p);
    QTextOption textOption(Qt::AlignTop|Qt::AlignHCenter);
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    p.setFont(opt.font);
    p.setPen(opt.palette.highlightedText().color());
    p.drawText(QRect(0, 5, this->width(), 9999), opt.text, textOption);
    p.restore();

    m_edit->adjustSize();
    if (this->height() != int(m_edit->document()->size().height()) + m_delegate->getView()->iconSize().height() + 10)
        setFixedHeight(int(m_edit->document()->size().height()) + m_delegate->getView()->iconSize().height() + 10);

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

void IconViewIndexWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (m_edit_trigger.isActive()) {
            m_delegate->getView()->setIndexWidget(m_index, nullptr);
            m_delegate->getView()->edit(m_index);
            return;
        }
    }
    QWidget::mousePressEvent(e);
}
