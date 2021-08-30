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
//        const qreal radius = 4;
//        QPainterPath path;
//        path.moveTo(rect.topRight() - QPointF(radius, 0));
//        path.lineTo(rect.topLeft() + QPointF(radius, 0));
//        path.quadTo(rect.topLeft(), rect.topLeft() + QPointF(0, radius));
//        path.lineTo(rect.bottomLeft() + QPointF(0, -radius));
//        path.quadTo(rect.bottomLeft(), rect.bottomLeft() + QPointF(radius, 0));
//        path.lineTo(rect.bottomRight() - QPointF(radius, 0));
//        path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
//        path.lineTo(rect.topRight() + QPointF(0, radius));
//        path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

//        painter->setRenderHint(QPainter::Antialiasing);
//        if(option.state & QStyle::State_MouseOver)
//        {
//            painter->setPen(QPen(Qt::NoPen));
//            QColor color;
//            color.setNamedColor("white");
//            painter->setBrush(QBrush(color));

//            painter->setOpacity(0.14);
//            painter->drawPath(path);
//        }

        painter->setOpacity(1);
//        QIcon icon=index.data(Qt::DecorationRole).value<QIcon>();
        TABLETAPP tabletApp = index.data(Qt::DisplayRole).value<TABLETAPP>();
        QString iconstr= tabletApp.appIcon;
        iconstr.remove(".png");
        iconstr.remove(".svg");
        QIcon icon=QIcon::fromTheme(iconstr);
        if(icon.isNull())
            icon=QIcon::fromTheme(QString("application-x-desktop"));
        QString appname= tabletApp.appName;

//        QFont font;
        QRect iconRect;
//        font.setPixelSize(Style::AppListFontSize);
        iconRect=QRect(rect.x(),rect.y()+5, Style::AppListItemSizeWidth, Style::AppListItemSizeHeight);
//        painter->setFont(font);
        icon.paint(painter,iconRect);

        painter->setPen(QPen(Qt::black));
        QRect textRect;

        textRect=QRect(rect.x(),
                       iconRect.height()+5,
                       iconRect.width(),
                       rect.height()-iconRect.height()-10);
        QFontMetrics fm=painter->fontMetrics();
        QString appnameElidedText=fm.elidedText(appname,Qt::ElideRight,rect.width(),Qt::TextShowMnemonic);
        painter->drawText(textRect,Qt::AlignLeft,appnameElidedText.toLocal8Bit().data());

//        QRect progressRect;
//        progressRect=QRect(textRect.x(), textRect.height(), rect.width() - iconRect.width()-5, rect.height()/2);
//        painter->drawRect(progressRect);
//        painter->fillRect(progressRect,QColor(Qt::blue));
//        if(option.state & QStyle::State_MouseOver)
//        {
////            painter->setPen(QPen(Qt::NoPen));
////            QColor color;
////            color.setNamedColor(QString::fromLocal8Bit(AppBtnHover));
////            painter->setBrush(QBrush(color));

////            painter->setOpacity(0.14);
////            painter->drawPath(path);

//            if(fm.boundingRect(appname).width()>rect.width())
//            {
//                QToolTip::showText(QCursor::pos(),appname);
//            }
////                QToolTip::showText(QPoint(QCursor::pos().x(),option.rect.bottom()),appname);

//        }
//        else {
//            QToolTip::hideText();
//        }
        painter->restore();

    }
}

QSize FullItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(128, 128);
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
