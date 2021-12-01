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
#include <QStyleHints>

#include "file-info.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-item.h"

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewIndexWidget::IconViewIndexWidget(const IconViewDelegate *delegate, const QStyleOptionViewItem &option, const QModelIndex &index, QWidget *parent) : QWidget(parent)
{
    installEventFilter(parent);

    setMouseTracking(true);

    m_edit_trigger.setInterval(3000);
    m_edit_trigger.setSingleShot(true);

    QTimer::singleShot(750, this, [=]() {
        m_edit_trigger.start();
    });

    //use QTextEdit to show full file name when select
    m_edit = new QTextEdit();

    m_option = option;
    m_index = index;

    m_delegate = delegate;

//    m_delegate->getView()->m_renameTimer->stop();
//    m_delegate->getView()->m_editValid = false;
//    m_delegate->getView()->m_renameTimer->start();

    m_is_dragging = m_delegate->getView()->isDraggingState();

    QSize size = delegate->sizeHint(option, index);
    setMinimumSize(size);

    //extra emblems
    auto proxy_model = static_cast<FileItemProxyFilterSortModel*>(delegate->getView()->model());
    auto item = proxy_model->itemFromIndex(index);
    if (item) {
        m_info = item->info();
    }

    m_delegate->initStyleOption(&m_option, m_index);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    m_option.features.setFlag(QStyleOptionViewItem::WrapText);
#else
    m_option.features |= QStyleOptionViewItem::WrapText;
#endif
    m_option.textElideMode = Qt::ElideNone;

    auto opt = m_option;
    //opt.rect.setHeight(9999);
    opt.rect.moveTo(0, 0);

    //qDebug()<<m_option.rect;
    auto iconExpectedSize = m_delegate->getView()->iconSize();
    QRect iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    auto y_delta = iconExpectedSize.height() - iconRect.height();
    opt.rect.moveTo(opt.rect.x(), opt.rect.y() + y_delta);

    m_option = opt;

    adjustPos();

    auto textSize = IconViewTextHelper::getTextSizeForIndex(opt, index, 2);
    int fixedHeight = 5 + iconExpectedSize.height() + 5 + textSize.height() + 5;

    int y_bottom = option.rect.y() + fixedHeight + 20;
    //qDebug() << "Y:" <<option.rect.y() <<fixedHeight <<m_delegate->getView()->height();
    b_elide_text = false;
    if ( y_bottom > m_delegate->getView()->height() && opt.text.length() > ELIDE_TEXT_LENGTH)
    {
        b_elide_text = true;
        int  charWidth = opt.fontMetrics.averageCharWidth();
        opt.text = opt.fontMetrics.elidedText(opt.text, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
        //recount size
        textSize = IconViewTextHelper::getTextSizeForIndex(opt, index, 2);
        fixedHeight = 5 + iconExpectedSize.height() + 5 + textSize.height() + 5;
    }

    if (fixedHeight >= option.rect.height())
        setFixedHeight(fixedHeight);
    else
        setFixedHeight(option.rect.height());

    m_option.rect.setHeight(fixedHeight - y_delta);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    connect(qApp, &QApplication::fontChanged, this, [=]() {
        m_delegate->getView()->setIndexWidget(m_index, nullptr);
    });
#else
    //FIXME: handle font change.
#endif
}

IconViewIndexWidget::~IconViewIndexWidget()
{
    delete m_edit;
}

bool IconViewIndexWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Resize) {
        adjustPos();
        update();
    }
    return false;
}

void IconViewIndexWidget::paintEvent(QPaintEvent *e)
{
    IconView *view = m_delegate->getView();
    auto expectedRect = view->visualRect(m_index);
    auto rectPos = this->mapToParent(QPoint(0, 0));
    if (expectedRect.topLeft() != rectPos) {
        adjustPos();
        update();
        return;
    }

    QWidget::paintEvent(e);
    QPainter p(this);
    //p.fillRect(0, 0, 999, 999, Qt::red);

    //adjustPos();

    //qDebug()<<m_option.backgroundBrush;
    //qDebug()<<this->size() << m_delegate->getView()->iconSize();

    auto opt = m_option;
    auto rawRect = m_option.rect;
    opt.rect = this->rect();
    //p.fillRect(opt.rect, m_delegate->selectedBrush());
    auto rawDecoSize = opt.decorationSize;
    opt.decorationSize = m_delegate->getView()->iconSize();
    //FIXME: Modify the icon style, only click on the text to respond, click on the icon to not respond
    view->style()->drawPrimitive(QStyle::PE_PanelItemViewItem,
                                         &opt,
                                         &p,
                                         nullptr);
    opt.decorationSize = rawDecoSize;

    opt.rect = rawRect;
    auto tmp = opt.text;
    opt.text = nullptr;
    //FIXME: Modify the icon style, only click on the text to respond, click on the icon to not respond
    view->style()->drawControl(QStyle::CE_ItemViewItem, &opt, &p, opt.widget);
    if (b_elide_text)
    {
        int  charWidth = opt.fontMetrics.averageCharWidth();
        tmp = opt.fontMetrics.elidedText(tmp, Qt::ElideRight, ELIDE_TEXT_LENGTH * charWidth);
    }
    opt.text = std::move(tmp);

    //auto textRectF = QRectF(0, m_delegate->getView()->iconSize().height(), this->width(), this->height());
    p.save();

    p.setPen(opt.palette.highlightedText().color());
    p.translate(0, m_delegate->getView()->iconSize().height() + 5);
    IconViewTextHelper::paintText(&p, opt, m_index, 9999, 2, 4);

//    p.translate(-1, m_delegate->getView()->iconSize().height() + 13);
//    //m_edit->document()->drawContents(&p);
//    QTextOption textOption(Qt::AlignTop|Qt::AlignHCenter);
//    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
//    p.setFont(opt.font);
//    p.setPen(opt.palette.highlightedText().color());
//    p.drawText(QRect(1, 0, this->width() - 1, 9999), opt.text, textOption);
    p.restore();

    //extra emblems
    if (!m_info.lock()) {
        return;
    }
    auto info = m_info.lock();

    // draw color symbols
    auto colors = info->getColors();
    int offset = 0;
    const int MAX_LABEL_NUM = 3;
    int startIndex = (colors.count() > MAX_LABEL_NUM ? colors.count() - MAX_LABEL_NUM : 0);
    for (int i = startIndex; i < colors.count(); ++i) {
        auto color = colors.at(i);
        p.save();
        p.setRenderHint(QPainter::Antialiasing);
        p.translate(2, 2);
        p.setPen(opt.palette.highlightedText().color());
        p.setBrush(color);
        p.drawEllipse(QRectF(offset, 0, 10, 10));
        p.restore();
        offset += 10/2;
    }

    //paint symbolic link emblems
    if (info->isSymbolLink()) {
        QIcon icon = QIcon::fromTheme("emblem-symbolic-link");
        //qDebug()<< "symbolic:" << info->symbolicIconName();
        icon.paint(&p, this->width() - 30, 10, 20, 20, Qt::AlignCenter);
    }
    if(view->m_multi_select)
    {
        QIcon icon = QIcon(":/icons/icon-selected.png");
        icon.paint(&p, this->width() - 20, 4, 16, 16, Qt::AlignCenter);
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
    } else if (!info->canWrite() && !info->canExecute()) {
        QIcon icon = QIcon::fromTheme("emblem-readonly");
        icon.paint(&p, rect.x() + 10, rect.y() + 10, 20, 20);
    }
}

void IconViewIndexWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        IconView *view = m_delegate->getView();

        if (view->isDraggingState() || m_is_dragging) {
            view->m_renameTimer->stop();
            view->m_editValid = false;
            return QWidget::mousePressEvent(e);
        }
        //FIXME: Modify the icon style, only click on the text to respond, click on the icon to not respond
        QRect rect =  m_option.rect;
        QSize iconExpectedSize = m_delegate->getView()->iconSize();
        rect.setY(iconExpectedSize.height());
        rect.setHeight(m_option.rect.height()-iconExpectedSize.height());
        if(!rect.contains(e->pos()))
        {
            view->m_editValid = false;
            view->m_renameTimer->start();
            return ;
        }

        view->m_editValid = true;
        if (view->m_renameTimer->isActive()) {
            if (view->m_renameTimer->remainingTime() < 3000 - qApp->styleHints()->mouseDoubleClickInterval() && view->m_renameTimer->remainingTime() > 0) {
                view->slotRename();
            } else {
                view->m_editValid = false;
                view->m_renameTimer->stop();
            }
        } else {
            view->m_editValid = false;
            view->m_renameTimer->start();
        }
        e->ignore();
//        if (m_edit_trigger.isActive()) {
//            qDebug()<<"IconViewIndexWidget::mousePressEvent: edit"<<e->type();
//            m_delegate->getView()->setIndexWidget(m_index, nullptr);
//            m_delegate->getView()->edit(m_index);
//            return;
//        }
    }
//    QWidget::mousePressEvent(e);
}

void IconViewIndexWidget::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
    //comment to fix click selected file name unselect file, when file has long name
    //m_is_dragging = true;
}

void IconViewIndexWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
    //m_is_dragging = false;
}

void IconViewIndexWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_delegate->getView()->activated(m_index);
    return;
}

void IconViewIndexWidget::adjustPos()
{
    IconView *view = m_delegate->getView();
    if (m_index.model() != view->model())
        return;

    if (!view->selectionModel()->selectedIndexes().contains(m_index)) {
        this->close();
        return;
    }

    auto visualRect = view->visualRect(m_index);
    if (this->mapToParent(QPoint()) != visualRect.topLeft())
        this->move(visualRect.topLeft());
}
