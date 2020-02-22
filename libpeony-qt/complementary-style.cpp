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

#include "complementary-style.h"

#include "tool-bar.h"
#include "location-bar.h"

#include <QMenu>
#include <QPushButton>
#include <QStyleOptionComplex>

#include <QDebug>
#include <qdrawutil.h>

using namespace Peony;

static ComplementaryStyle *global_instance = nullptr;

ComplementaryStyle::ComplementaryStyle(QStyle *parent) : QProxyStyle(parent)
{
    m_styled_menu = new QMenu;
    m_styled_button = new QPushButton;
}

ComplementaryStyle::~ComplementaryStyle()
{
    m_styled_menu->deleteLater();
    m_styled_button->deleteLater();
}

ComplementaryStyle *ComplementaryStyle::getStyle()
{
    if (!global_instance)
        global_instance = new ComplementaryStyle;
    return global_instance;
}

int ComplementaryStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch (hint) {
    case QStyle::SH_Menu_Scrollable: {
        return 1;
    }
    default:
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
}

void ComplementaryStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (widget->inherits("QMenu")) {
        return QProxyStyle::drawPrimitive(element, option, painter, widget);
    }

    switch (element) {
    case QStyle::PE_IndicatorToolBarSeparator: {
        //do not draw toolbar separator
        return;
    }
    case PE_IndicatorArrowDown: {
        ToolBar *toolBar = qobject_cast<ToolBar*>(widget->parentWidget());
        LocationBar *locationBar = qobject_cast<LocationBar*>(widget->parentWidget());
        if (toolBar) {
            auto rect = option->rect;
            rect.setY((rect.top()+rect.bottom())/2 - 8);
            rect.setWidth(16);
            rect.setHeight(16);
            if (option->state & State_Sunken) {
                rect.adjust(1, 1, 1, 1);
            }
            QIcon arrowDown = QIcon::fromTheme("pan-down-symbolic");
            arrowDown.paint(painter, rect.adjusted(-3, 0, -3, 0));
            return;
        } else if (locationBar) {
            auto rect = option->rect;
            rect.setY((rect.top()+rect.bottom())/2 - 8);
            rect.setX((rect.left()+rect.right())/2 - 8);
            rect.setWidth(16);
            rect.setHeight(16);
            if (option->state & State_Sunken) {
                QIcon arrowDown = QIcon::fromTheme("pan-down-symbolic");
                arrowDown.paint(painter, rect.adjusted(1, 1, 1, 1));
            } else {
                QIcon arrowDown = QIcon::fromTheme("pan-end-symbolic");
                arrowDown.paint(painter, rect.adjusted(1, 1, 1, 1));
            }
            return;
        }
        break;
    }
    //draw custome QLineEdit
    case PE_FrameLineEdit:
    case PE_PanelLineEdit:
    {
        if (const QStyleOptionFrame *toolbutton
                = qstyleoption_cast<const QStyleOptionFrame *>(option))
        if (toolbutton)
        {
            drawPrimitive(PE_FrameLineEdit, toolbutton, painter, widget);
        }
        break;
    }
    default:
        break;
    }
    return QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void ComplementaryStyle::drawComplexControl(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget) const
{
    switch (cc) {
    case QStyle::CC_ToolButton: {
        if (const QStyleOptionToolButton *toolbutton
                = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {

            //Peony::ToolBar has different style with other toolbar.
            ToolBar *parentWidget = qobject_cast<ToolBar*>(widget->parentWidget());

            QRect button, menuarea;
            button = proxy()->subControlRect(cc, toolbutton, SC_ToolButton, widget);
            menuarea = proxy()->subControlRect(cc, toolbutton, SC_ToolButtonMenu, widget);

            State bflags = toolbutton->state & ~State_Sunken;

            if (bflags & State_AutoRaise) {
                if (!(bflags & State_MouseOver) || !(bflags & State_Enabled)) {
                    bflags &= ~State_Raised;
                }
            }
            State mflags = bflags;
            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_Sunken;
                mflags |= State_Sunken;
            }

            QStyleOption tool = *toolbutton;
            if (toolbutton->subControls & SC_ToolButton) {
                if (bflags & (State_Sunken | State_On | State_Raised)) {
                    tool.rect = button;
                    tool.state = bflags;
                    if (parentWidget) {
                        if (toolbutton->features.testFlag(QStyleOptionToolButton::Menu)) {
                            tool.rect = QRect(button.topLeft(), QSize(button.width() + menuarea.width(), button.height()));
                        }
                        proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);
                    } else {
                        proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);
                    }
                } else {
                    if (parentWidget) {
                        tool.rect = button;
                        tool.state = bflags;
                        if (toolbutton->features.testFlag(QStyleOptionToolButton::HasMenu)) {
                            tool.rect = QRect(button.topLeft(), QSize(button.width() + menuarea.width(), button.height()));
                        }

                        qDrawShadePanel(p,
                                        tool.rect.adjusted(1, 1, -1, -1),
                                        m_styled_menu->palette().button().color(),
                                        false,
                                        0,
                                        &m_styled_menu->palette().brush(QPalette::Normal, QPalette::Button));
                    }
                }
            }

            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect fr;
                fr.QStyleOption::operator=(*toolbutton);
                fr.rect.adjust(3, 3, -3, -3);
                if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)
                    fr.rect.adjust(0, 0, -proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator,
                                   toolbutton, widget), 0);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fr, p, widget);
            }
            QStyleOptionToolButton label = *toolbutton;
            label.state = bflags;
            int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, opt, widget);
            label.rect = button.adjusted(fw, fw, -fw, -fw);
            label.palette.setColor(QPalette::Window, m_styled_menu->palette().window().color());

            proxy()->drawControl(CE_ToolButtonLabel, &label, p, widget);

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.rect = menuarea;//.adjusted(1, 1, -1, -1);
                tool.state = mflags;
                tool.palette.setColor(QPalette::Button, m_styled_button->palette().highlight().color());
                //FIXME: paint indicator button background correctly.
                if (mflags & (State_Sunken | State_On | State_Raised)) {
                    if (parentWidget) {
                        //do nothing
                    } else {
                        proxy()->drawPrimitive(PE_PanelButtonCommand, &tool, p, widget);
                    }

                }
                drawPrimitive(PE_IndicatorArrowDown, &tool, p, widget);
            } else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, widget);
                QRect ir = toolbutton->rect;
                QStyleOptionToolButton newBtn = *toolbutton;
                newBtn.rect = QRect(ir.right() + 5 - mbi, ir.y() + ir.height() - mbi + 4, mbi - 6, mbi - 6);
                newBtn.rect = visualRect(toolbutton->direction, button, newBtn.rect);
                drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, widget);
            }
        }
        break;
    }
    case QStyle::CC_ScrollBar:
    {
       //migrate changes from style-example-qt5
        /// 我们参考qt的官方文档和源码，可以知道滚动条需要绘制的子控件有8个
        /// QStyle::SC_ScrollBarAddLine     一般的向下或者向右箭头 Scroll bar add line (i.e., down/right arrow); see also QScrollBar.
        /// QStyle::SC_ScrollBarSubLine     向下或向左箭头 Scroll bar sub line (i.e., up/left arrow).
        /// QStyle::SC_ScrollBarAddPage     slider一侧的区域 Scroll bar add page (i.e., page down).
        /// QStyle::SC_ScrollBarSubPage     slider另一侧的区域 Scroll bar sub page (i.e., page up).
        /// QStyle::SC_ScrollBarFirst       置顶按钮，某些style没有 Scroll bar first line (i.e., home).
        /// QStyle::SC_ScrollBarLast        至底按钮 Scroll bar last line (i.e., end).
        /// QStyle::SC_ScrollBarSlider      滚动条slider Scroll bar slider handle.
        /// QStyle::SC_ScrollBarGroove      滚动槽 Special sub-control which contains the area in which the slider handle may move.
        /// 其实有一些子控件不一定要绘制，比如SC_ScrollBarFirst和SC_ScrollBarLast，有一些style中默认是没有的（比如fusion）
        /// 还有SC_ScrollBarGroove也未必被绘制或者被遮蔽，
        /// SC_ScrollBarAddLine，SC_ScrollBarSubLine，
        /// SC_ScrollBarAddPage，SC_ScrollBarSubPage和SC_ScrollBarSlider是一个
        /// 滚动条最为常见的组成部分

        /// 我们需要获取scrollbar的详细信息，通过qstyleoption_cast可以得到
        /// 对应的option，通过拷贝构造函数得到一份备份用于绘制子控件
        /// 我们一般不用在意option是怎么得到的，大部分的Qt控件都能够提供了option的init方法
        const QStyleOptionSlider option = *qstyleoption_cast<const QStyleOptionSlider*>(opt);
        QStyleOption tmp = option;

        /// 绘制两端指示器
        /// 我们可以通过subControlRect获取对应子控件的rect
        auto subLineRect = proxy()->subControlRect(CC_ScrollBar, opt, SC_ScrollBarSubLine, widget);
        tmp.rect = subLineRect;
        drawControl(CE_ScrollBarSubLine, &tmp, p, widget);

        auto addLineRect = proxy()->subControlRect(CC_ScrollBar, opt, SC_ScrollBarAddLine, widget);
        tmp.rect = addLineRect;
        drawControl(CE_ScrollBarAddLine, &tmp, p, widget);

        /// 绘制slider两侧的翻页控件，一般来说可以认为是背景
        /// 其实我们一般没有必要在两端各绘制一次背景，除非我们希望两端的背景根据状态有所区别
        /// 不论如何，Qt还是提供了这两个枚举，这里尊重一下规范
        auto subPageRect = proxy()->subControlRect(CC_ScrollBar, opt, SC_ScrollBarSubPage, widget);
        tmp.rect = subPageRect;
        drawControl(CE_ScrollBarSubPage, &tmp, p, widget);

        auto addPageRect = proxy()->subControlRect(CC_ScrollBar, opt, SC_ScrollBarAddPage, widget);
        tmp.rect = addPageRect;
        drawControl(CE_ScrollBarAddPage, &tmp, p, widget);

        /// 绘制滚动条slider
        ///
        /// complex control与一般控件不同之处之一在于它可能拥有不统一的子控件状态，
        /// 以滚动条为例，我们在把鼠标移动到滚动条上时，slider未必属于被hover的状态，
        /// 这是通过activeSubControls来进行判断的
        ///
        /// 我们可以通过activeSubControls和state赋予不同subcontrol更详细的状态，
        /// 比如如果scrollbar本身是被悬停但是并未悬停在slider上，
        /// 那么我们的tmp opt也可以将state的State_MouseOver设置为false，
        /// 然后再绘制这个SC
        bool hover = false;
        bool pressed = false;
        if (opt->activeSubControls.testFlag(SC_ScrollBarSlider)) {
            /// 鼠标是否悬停在slider上
            if (opt->state.testFlag(State_MouseOver)) {
                hover = true;
                /// 鼠标是否按住slider
                if (opt->state.testFlag(State_Sunken)) {
                    pressed = true;
                }
            }
        }
        //p->save();
        /// 根据slider的state不同，drawControl可以做不同的绘制，
        /// 简单的处理也可以放在这里进行

        tmp.state.setFlag(State_MouseOver, hover);
        tmp.state.setFlag(State_Sunken, pressed);
        auto sliderRect = subControlRect(CC_ScrollBar, opt, SC_ScrollBarSlider, widget);
        tmp.rect = sliderRect;
        drawControl(CE_ScrollBarSlider, &tmp, p, widget);
        //p->restore();

        return;
    }
    default:
        return QProxyStyle::drawComplexControl(cc, opt, p, widget);
    }
}
