/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#include "progress-item-delegate.h"
#include <QDebug>
#include <QColor>
#include "../../tablet/src/Style/style.h"

using namespace Peony;

FullItemDelegate::FullItemDelegate(QObject *parent, int module):
    QStyledItemDelegate(parent)
{
    this->module=module;  
}

FullItemDelegate::~FullItemDelegate()
{

}

void FullItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.isValid())
    {
        painter->save();
        QStyleOptionViewItem viewOption(option);//用来在视图中画一个item
        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width());
        rect.setHeight(option.rect.height());

        //QPainterPath画圆角矩形
        const qreal radius = 4;
        QPainterPath path;
        path.moveTo(rect.topRight() - QPointF(radius, 0));
        path.lineTo(rect.topLeft() + QPointF(radius, 0));
        path.quadTo(rect.topLeft(), rect.topLeft() + QPointF(0, radius));
        path.lineTo(rect.bottomLeft() + QPointF(0, -radius));
        path.quadTo(rect.bottomLeft(), rect.bottomLeft() + QPointF(radius, 0));
        path.lineTo(rect.bottomRight() - QPointF(radius, 0));
        path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
        path.lineTo(rect.topRight() + QPointF(0, radius));
        path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

        painter->setRenderHint(QPainter::Antialiasing);
        if(option.state & QStyle::State_MouseOver)
        {
            painter->setPen(QPen(Qt::NoPen));
            QColor color;
            color.setNamedColor("black");
            painter->setBrush(QBrush(color));

            painter->setOpacity(0.14);
            painter->drawPath(path);
        }

        painter->setOpacity(1);
        TABLETAPP tabletApp = index.data(Qt::DisplayRole).value<TABLETAPP>();
        QString iconstr= tabletApp.appIcon;
        qDebug("FullItemDelegate::paint : name:%s, icon:%s/n",tabletApp.appName.toLocal8Bit().data(),iconstr.toLocal8Bit().data());

        iconstr.remove(".png");
        iconstr.remove(".svg");
        QIcon icon=QIcon::fromTheme(iconstr);
        if(icon.isNull())
            icon=QIcon::fromTheme(QString("application-x-desktop"));
        QString appname= tabletApp.appName;


        QRect iconRect;
        QRect textRect;
        QPixmap pixmap;
        bool bigIcon = index.data(Qt::UserRole).toBool();

        if(bigIcon)
        {
            iconRect=QRect(rect.x(), rect.y()+Style::topSpace-5, Style::BigIconSize,Style::BigIconSize);
            textRect=QRect(rect.x(),iconRect.bottom()+5, rect.width(),rect.height()-iconRect.height()-10-5);
            pixmap = icon.pixmap((Style::BigIconSize ,Style::BigIconSize),QIcon::Normal,QIcon::On);
            pixmap = pixmap.scaled(Style::BigIconSize ,Style::BigIconSize,Qt::IgnoreAspectRatio);

        }
        else
        {
            iconRect=QRect(rect.x(), rect.y()+Style::topSpace, Style::SmallIconSize,Style::SmallIconSize);
            textRect=QRect(rect.x(),iconRect.bottom()+10, rect.width() ,rect.height()-iconRect.height()-10-Style::topSpace);
            pixmap = icon.pixmap((Style::SmallIconSize,Style::SmallIconSize),QIcon::Normal,QIcon::On);
            pixmap = pixmap.scaled(Style::SmallIconSize ,Style::SmallIconSize,Qt::IgnoreAspectRatio);

        }

        icon = QIcon(pixmap);
        icon.paint(painter,iconRect);      
        painter->restore();
        painter->save();
        painter->setPen(QPen(Qt::black));

        QFontMetrics fm=painter->fontMetrics();
        QString appnameElidedText=fm.elidedText(appname,Qt::ElideRight,textRect.width(),Qt::TextShowMnemonic);
        painter->drawText(textRect,Qt::AlignHCenter,appnameElidedText.toLocal8Bit().data());
        painter->restore();
        painter->save();

        if(option.state & QStyle::State_MouseOver)
        {
            painter->setPen(QPen(Qt::NoPen));
            QColor color;
            color.setNamedColor(QString::fromLocal8Bit(AppBtnHover));
            painter->setBrush(QBrush(color));

            painter->setOpacity(0.14);
            painter->drawPath(path);

            if(fm.boundingRect(appname).width()>rect.width())
            {
                QToolTip::showText(QCursor::pos(),appname);
            }
            QToolTip::showText(QPoint(QCursor::pos().x(),option.rect.bottom()),appname);

        }
        else {
            QToolTip::hideText();
        }
        painter->restore();

    }
}

QSize FullItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(Style::itemWidth,Style::itemHeight);
}

//bool FullItemDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
//{
//    if(event->type()==QEvent::ToolTip)
//    {
//    }
//    else if(event->type()==QEvent::Leave)
//    {
//    }
//    return true;
//}
