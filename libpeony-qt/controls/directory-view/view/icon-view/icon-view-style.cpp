/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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

#include "icon-view-style.h"

#include <QStyleOptionRubberBand>
#include <QPainter>

#include <QApplication>

#include <QDebug>

#include <QListView>
#include <QListWidget>
#include <QApplication>

#include <QPainterPath>

using namespace Peony;
using namespace Peony::DirectoryView;

static IconViewStyle *global_instance = nullptr;

IconViewStyle::IconViewStyle(QStyle *style) : QProxyStyle(style)
{
    //qDebug()<<"icon view style";
}

IconViewStyle *IconViewStyle::getStyle()
{
    if (!global_instance)
        global_instance = new IconViewStyle;
    return global_instance;
}

void IconViewStyle::release()
{
    if (global_instance)
        global_instance->deleteLater();
}

void IconViewStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == PE_Frame) {
        return;
    }
    //FIXME: Modify the icon style, only click on the text to respond, click on the icon to not respond
    switch (element) {
    case PE_PanelItemViewItem: {
        bool isIconView = false;
        auto opt = qstyleoption_cast<const QStyleOptionViewItem *>(option);
        if (!opt)
            return;
        if (opt) {
            isIconView = (opt->decorationPosition & QStyleOptionViewItem::Top);
        }
        bool isHover = (option->state & State_MouseOver) && (option->state & ~State_Selected);
        bool isSelected = option->state & State_Selected;
        bool enable = option->state & State_Enabled;
        QColor color = option->palette.color(enable? QPalette::Active: QPalette::Disabled,
                                  QPalette::Highlight);

        color.setAlpha(0);
        if (isHover && !isSelected) {
            int h = color.hsvHue();
            //int s = color.hsvSaturation();
            auto base = option->palette.base().color();
            int v = color.value();
            color.setHsv(h, base.lightness(), v, 64);
        }

        if (isSelected) {
            color.setAlpha(255);
        }
        if ((qobject_cast<const QListView *>(widget) || qobject_cast<const QListWidget *>(widget))
                && (opt->decorationPosition != QStyleOptionViewItem::Top)) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(Qt::transparent);
            painter->setBrush(color);
            painter->drawRoundedRect(option->rect , 4, 4);
            painter->restore();
            return;
        }

        if (!isIconView)
        {
            painter->fillRect(option->rect, color);
        }
        else {
            if(isSelected)
            {
                QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, option, widget);
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(Qt::transparent);
                const qreal radius = 8;
                QRect rect(option->rect.left(),iconRect.top(),option->rect.width(), iconRect.height()+10);

                QPainterPath iconPath;
                iconPath.moveTo(rect.topRight() - QPointF(radius, 0));
                iconPath.lineTo(rect.topLeft() + QPointF(radius, 0));
                iconPath.quadTo(rect.topLeft(), rect.topLeft() + QPointF(0, radius));
                iconPath.lineTo(rect.bottomLeft() );
                iconPath.lineTo(rect.bottomRight());
                iconPath.lineTo(rect.topRight() + QPointF(0, radius));
                iconPath.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));
                QColor iconColor = option->palette.color(QPalette::Button);
                iconColor.setAlpha(255);
                painter->setBrush(iconColor);
                painter->drawPath(iconPath);
                painter->restore();

                painter->save();
                QPainterPath textPath;
                QRect textRect(option->rect.left(),rect.bottom(),option->rect.width(), option->rect.height() - rect.height());
                textPath.moveTo(textRect.topRight() );
                textPath.lineTo(textRect.topLeft() );
                textPath.lineTo(textRect.bottomLeft() +  QPointF(0, -radius));
                textPath.quadTo(textRect.bottomLeft(), textRect.bottomLeft() + QPointF(radius, 0));
                textPath.lineTo(textRect.bottomRight()+  QPointF(-radius, 0));
                textPath.quadTo(textRect.bottomRight(), textRect.bottomRight() + QPointF(0, -radius));
                textPath.lineTo(textRect.topRight());

                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(Qt::transparent);
                painter->setBrush(color);
                painter->drawPath(textPath);
                painter->restore();
                QIcon::Mode mode = QIcon::Normal;
                if (!(option->state & QStyle::State_Enabled))
                    mode = QIcon::Disabled;
                else if (option->state & QStyle::State_Selected)
                    mode = QIcon::Selected;
                QIcon::State state = option->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
                //vopt->icon.paint(p, iconRect, vopt->decorationAlignment, mode, state);
//                auto pixmap = option->icon.pixmap(option->decorationSize,
//                                                mode,
//                                                state);
//                QStyle::drawItemPixmap(painter, iconRect, option->decorationAlignment,pixmap );

            }
            else
            {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(Qt::transparent);
                painter->setBrush(color);
                painter->drawRoundedRect(option->rect, 8, 8);
                painter->restore();
            }
        }
        return;
    }
    default:
        break;
    }
    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void IconViewStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_RubberBand) {
        if (qApp->devicePixelRatio() != 1.0) {
            const QStyleOptionRubberBand *tmp = qstyleoption_cast<const QStyleOptionRubberBand *>(option);
            QStyleOptionRubberBand opt = *tmp;
            if (opt.rect.width() >= 2 && opt.rect.height() >= 2) {
                opt.rect.adjust(1, 1, -1, -1);
            } else {
                opt.rect = QRect();
            }
            return QProxyStyle::drawControl(element, &opt, painter, widget);
        }
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

void IconViewStyle::drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const
{
    qreal scale = pixmap.devicePixelRatio();
    QRect aligned = alignedRect(QApplication::layoutDirection(), QFlag(alignment), pixmap.size() / scale, rect);
    QRect inter = aligned.intersected(rect);

    QPixmap target = pixmap;

    auto device = painter->device();

    painter->drawPixmap(inter.x(), inter.y(), target, inter.x() - aligned.x(), inter.y() - aligned.y(), inter.width() * scale, inter.height() *scale);
}

void IconViewStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    //qDebug()<<"drawItemText";
    QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
}
