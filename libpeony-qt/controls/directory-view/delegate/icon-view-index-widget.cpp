#include "icon-view-index-widget.h"
#include "icon-view-delegate.h"
#include "icon-view.h"

#include <QPainter>
#include <QPaintEvent>
#include <QLabel>

#include <QApplication>
#include <QStyle>

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
    m_option.textElideMode = Qt::ElideNone;
    m_option.widget = nullptr;

    m_index = index;

    //TODO: should i create a new view (with a unlimte grid height)
    //and delegate for indexWidget item's calculation?
    //For now i couldn't use this delegate to calculate the real text
    //height.
    delegate->initStyleOption(&m_option, index);

    m_delegate = delegate;

    QSize size = delegate->sizeHint(option, index);
    setMinimumSize(size);

    auto opt = m_option;
    qDebug()<<opt;

    //extra emblems
    auto proxy_model = static_cast<FileItemProxyFilterSortModel*>(delegate->getView()->model());
    auto item = proxy_model->itemFromIndex(index);
    if (item) {
        m_info = item->info();
    }
}

void IconViewIndexWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter p(this);
    //p.fillRect(0, 0, 999, 999, Qt::red);

    m_option.rect.setWidth(size().width());
    m_option.rect.setHeight(9999);
    m_option.rect.moveTo(0, 0);

    //qDebug()<<m_option.rect;
    QRect iconRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &m_option, m_delegate->getView());
    QRect textRect = QApplication::style()->subElementRect(QStyle::SE_ItemViewItemText, &m_option, m_delegate->getView());
    QRect rect = QRect(0, 0, textRect.width(), textRect.height() + iconRect.height() + 20);
    m_option.rect.size() = rect.size();
    resize(rect.size());
    IconView *view = m_delegate->getView();
    view->backgroundRole();

    qDebug()<<m_option.backgroundBrush;
    qDebug()<<this->size();

    QRect tmp(0, iconRect.height(), textRect.width(), textRect.height());
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &m_option, &p, m_delegate->getView());

    //extra emblems
    if (!m_info) {
        return;
    }
    auto info = m_info;
    //paint symbolic link emblems
    if (info->isSymbolLink()) {
        QIcon icon = QIcon::fromTheme("emblem-symbolic-link");
        //qDebug()<<info->symbolicIconName();
        icon.paint(&p, this->width() - 30, 10, 20, 20, Qt::AlignCenter);
    }

    //paint access emblems
    //NOTE: we can not query the file attribute in smb:///(samba) and network:///.
    if (info->uri().startsWith("smb:") || info->uri().startsWith("network:")) {
        return;
    }

    if (!info->canRead()) {
        QIcon icon = QIcon::fromTheme("emblem-unreadable");
        icon.paint(&p, rect.x() + 10, rect.y() + 10, 20, 20);
    } else if (!info->canWrite() && !info->canExecute()){
        QIcon icon = QIcon::fromTheme("emblem-readonly");
        icon.paint(&p, rect.x() + 10, rect.y() + 10, 20, 20);
    }
}
