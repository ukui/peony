/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "list-view-style.h"

#include <QStyleOption>
#include <QPainterPath>
#include <QPainter>
#include <QStyleOptionViewItem>

using namespace Peony;
using namespace Peony::DirectoryView;

static ListViewStyle *global_instance = nullptr;

ListViewStyle::ListViewStyle(QObject *parent) : QProxyStyle()
{

}

ListViewStyle *ListViewStyle::getStyle()
{
    if (!global_instance)
        global_instance = new ListViewStyle;
    return global_instance;
}

void ListViewStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case PE_Frame: {
        painter->save();
        bool isActive = option->state & State_Active;
        bool isEnable = option->state & State_Enabled;
        auto baseColor = option->palette.color(isEnable? (isActive? QPalette::Active: QPalette::Inactive): QPalette::Disabled, QPalette::Window);
        painter->fillRect(widget->rect(), baseColor);
        painter->restore();
        return;
    }
    case PE_FrameWindow: {
        return;
    }
    case PE_IndicatorBranch: {
        const QStyleOptionViewItem *tmp = qstyleoption_cast<const QStyleOptionViewItem *>(option);
        QStyleOptionViewItem opt = *tmp;
        if (!opt.state.testFlag(QStyle::State_Selected)) {
            if (opt.state & QStyle::State_Sunken) {
                opt.palette.setColor(QPalette::Highlight, opt.palette.button().color());
            }
            if (opt.state & QStyle::State_MouseOver) {
                opt.palette.setColor(QPalette::Highlight, opt.palette.mid().color());
            }
        }
        return QProxyStyle::drawPrimitive(element, &opt, painter, widget);
    }
    default:
        return QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}
