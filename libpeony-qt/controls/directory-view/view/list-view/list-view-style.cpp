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
#include <QTextLayout>

using namespace Peony;
using namespace Peony::DirectoryView;

static ListViewStyle *global_instance = nullptr;

//文本绘制布局
static QSizeF viewItemTextLayout(QTextLayout &textLayout, int lineWidth, int maxHeight = -1, int *lastVisibleLine = nullptr)
{
    if (lastVisibleLine)
        *lastVisibleLine = -1;
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    int i = 0;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        // we assume that the height of the next line is the same as the current one
        if (maxHeight > 0 && lastVisibleLine && height + line.height() > maxHeight) {
            const QTextLine nextLine = textLayout.createLine();
            *lastVisibleLine = nextLine.isValid() ? i : -1;
            break;
        }
        ++i;
    }
    textLayout.endLayout();
    return QSizeF(widthUsed, height);
}

//文本缩略
QString calculateElidedText(const QString &text, const QTextOption &textOption,
                            const QFont &font, const QRect &textRect, const Qt::Alignment valign,
                            Qt::TextElideMode textElideMode, int flags,
                            bool lastVisibleLineShouldBeElided, QPointF *paintStartPosition)
{
    QTextLayout textLayout(text, font);
    textLayout.setTextOption(textOption);

    // In AlignVCenter mode when more than one line is displayed and the height only allows
    // some of the lines it makes no sense to display those. From a users perspective it makes
    // more sense to see the start of the text instead something inbetween.
    const bool vAlignmentOptimization = paintStartPosition && valign.testFlag(Qt::AlignVCenter);

    int lastVisibleLine = -1;
    viewItemTextLayout(textLayout, textRect.width(), vAlignmentOptimization ? textRect.height() : -1, &lastVisibleLine);

    const QRectF boundingRect = textLayout.boundingRect();
    // don't care about LTR/RTL here, only need the height
    const QRect layoutRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, valign,
                                                 boundingRect.size().toSize(), textRect);

    if (paintStartPosition)
        *paintStartPosition = QPointF(textRect.x(), layoutRect.top());

    QString ret;
    qreal height = 0;
    const int lineCount = textLayout.lineCount();
    for (int i = 0; i < lineCount; ++i) {
        const QTextLine line = textLayout.lineAt(i);
        height += line.height();

        // above visible rect
        if (height + layoutRect.top() <= textRect.top()) {
            if (paintStartPosition)
                paintStartPosition->ry() += line.height();
            continue;
        }

        const int start = line.textStart();
        const int length = line.textLength();
        const bool drawElided = line.naturalTextWidth() > textRect.width();
        bool elideLastVisibleLine = lastVisibleLine == i;
        if (!drawElided && i + 1 < lineCount && lastVisibleLineShouldBeElided) {
            const QTextLine nextLine = textLayout.lineAt(i + 1);
            const int nextHeight = height + nextLine.height() / 2;
            // elide when less than the next half line is visible
            if (nextHeight + layoutRect.top() > textRect.height() + textRect.top())
                elideLastVisibleLine = true;
        }

        QString text = textLayout.text().mid(start, length);
        if (drawElided || elideLastVisibleLine) {
            if (elideLastVisibleLine) {
                if (text.endsWith(QChar::LineSeparator))
                    text.chop(1);
                text += QChar(0x2026);
            }
            const QFontMetrics engine( font);
            ret += engine.elidedText(text, textElideMode, textRect.width(), flags);

            // no newline for the last line (last visible or real)
            // sometimes drawElided is true but no eliding is done so the text ends
            // with QChar::LineSeparator - don't add another one. This happened with
            // arabic text in the testcase for QTBUG-72805
            if (i < lineCount - 1 &&
                    !ret.endsWith(QChar::LineSeparator))
                ret += QChar::LineSeparator;
        } else {
            ret += text;
        }

        // below visible text, can stop
        if ((height + layoutRect.top() >= textRect.bottom()) ||
                (lastVisibleLine >= 0 && lastVisibleLine == i))
            break;
    }
    return ret;
}

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
    case QStyle::PE_IndicatorItemViewItemDrop: {
        if (option->rect.height() <= 1) {
            // 不显示中间线条，规避#85608，另外偶现错位的问题暂时没有定位，有可能是因为重写了updateGeometries导致的
            return;
        }
        painter->setRenderHint(QPainter::Antialiasing, true);/* 反锯齿 */
        /* 按设计要求，边框颜色为调色板highlight值，圆角为6px */
        QColor color = option->palette.color(QPalette::Highlight);
        painter->setPen(color);
        painter->drawRoundedRect(option->rect, 6, 6);
        return;
    }
    default:
        return QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}

void ListViewStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case CE_ItemViewItem: {
        auto p = painter;
        auto opt = option;
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            p->save();
            if (p->clipPath().isEmpty())
                p->setClipRect(opt->rect);

            QRect checkRect = proxy()->subElementRect(SE_ItemViewItemCheckIndicator, vopt, widget);
            QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt, widget);
            QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt, widget);

            //绘制背景
            proxy()->drawPrimitive(PE_PanelItemViewItem, opt, p, widget);

            // draw the check mark
            if (vopt->features & QStyleOptionViewItem::HasCheckIndicator) {
                QStyleOptionViewItem option(*vopt);
                option.rect = checkRect;
                option.state = option.state & ~QStyle::State_HasFocus;

                switch (vopt->checkState) {
                case Qt::Unchecked:
                    option.state |= QStyle::State_Off;
                    break;
                case Qt::PartiallyChecked:
                    option.state |= QStyle::State_NoChange;
                    break;
                case Qt::Checked:
                    option.state |= QStyle::State_On;
                    break;
                default:
                    break;
                }
                proxy()->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &option, p, widget);
            }

            //获取列表视图图标
            QIcon::Mode mode = QIcon::Normal;
            if (!(vopt->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;

            QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
            auto pixmap = vopt->icon.pixmap(vopt->decorationSize,
                                            mode,
                                            state);
            //绘制列表视图图标
            QStyle::drawItemPixmap(painter, iconRect, vopt->decorationAlignment, pixmap);

            //绘制列表视图文本，设置文本颜色
            if (!vopt->text.isEmpty()) {
                QPalette::ColorGroup cg = vopt->state & QStyle::State_Enabled
                        ? QPalette::Normal : QPalette::Disabled;
                if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
                    cg = QPalette::Inactive;

                if (vopt->state & QStyle::State_Selected) {
                    p->setPen(vopt->palette.color(cg, QPalette::HighlightedText));
                } else {
                    p->setPen(vopt->palette.color(cg, QPalette::Text));
                }
                if (vopt->state & QStyle::State_Editing) {
                    p->setPen(vopt->palette.color(cg, QPalette::Text));
                    p->drawRect(textRect.adjusted(0, 0, -1, -1));
                }
                //绘制列表视图文本
                viewItemDrawText(p, vopt, textRect);
            }

            p->restore();
            return;
        }
        break;
    }
    default:
        break;
    }
    return QProxyStyle::drawControl(element, option, painter, widget);
}
//绘制列表视图文本
void ListViewStyle::viewItemDrawText(QPainter *p, const QStyleOptionViewItem *option, const QRect &rect) const
{
    const QWidget *widget = option->widget;
    const int textMargin = proxy()->pixelMetric(QStyle::PM_FocusFrameHMargin, 0, widget) + 1;

    QRect textRect = rect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding
    const bool wrapText = option->features & QStyleOptionViewItem::WrapText;
    QTextOption textOption;
    textOption.setWrapMode(wrapText ? QTextOption::WordWrap : QTextOption::ManualWrap);
    textOption.setTextDirection(option->direction);
    textOption.setAlignment(QStyle::visualAlignment(option->direction, option->displayAlignment));

    QPointF paintPosition;
    const QString newText = calculateElidedText(option->text, textOption,
                                                option->font, textRect, option->displayAlignment,
                                                option->textElideMode, 0,
                                                true, &paintPosition);

    QTextLayout textLayout(newText, option->font);
    textLayout.setTextOption(textOption);
    viewItemTextLayout(textLayout, textRect.width());
    textLayout.draw(p, paintPosition);
}
