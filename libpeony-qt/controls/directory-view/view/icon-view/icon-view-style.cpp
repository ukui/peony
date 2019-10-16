#include "icon-view-style.h"

#include <QStyleOptionRubberBand>
#include <QPainter>

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

static IconViewStyle *global_instance = nullptr;

IconViewStyle::IconViewStyle(QStyle *style) : QProxyStyle(style)
{
    qDebug()<<"icon view style";
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

void IconViewStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::CE_RubberBand) {
        //qDebug()<<"draw control rubber band";
        if (qstyleoption_cast<const QStyleOptionRubberBand *>(option)) {
            auto rect = option->rect;
            QColor highlight = option->palette.color(QPalette::Active, QPalette::Highlight);
            painter->save();
            QColor penColor = highlight;
            penColor.setAlpha(180);
            painter->setPen(penColor);
            QColor dimHighlight(qMin(highlight.red()/2 + 110, 255),
                                qMin(highlight.green()/2 + 110, 255),
                                qMin(highlight.blue()/2 + 110, 255));
            dimHighlight.setAlpha(widget && widget->isTopLevel() ? 255 : 80);
            QLinearGradient gradient(rect.topLeft(), QPoint(rect.bottomLeft().x(), rect.bottomLeft().y()));
            gradient.setColorAt(0, dimHighlight.lighter(120));
            gradient.setColorAt(1, dimHighlight);
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->translate(0.5, 0.5);
            painter->setBrush(dimHighlight);
            painter->drawRoundedRect(option->rect.adjusted(0, 0, -1, -1), 1, 1);
            QColor innerLine = Qt::white;
            innerLine.setAlpha(40);
            painter->setPen(innerLine);
            painter->drawRoundedRect(option->rect.adjusted(1, 1, -2, -2), 1, 1);
            painter->restore();
        }
        return;
    }
    QProxyStyle::drawControl(element, option, painter, widget);
}

void IconViewStyle::drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const
{
    qDebug()<<"drawItemPixmap";
    QProxyStyle::drawItemPixmap(painter, rect, alignment, pixmap);
}

void IconViewStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const
{
    qDebug()<<"drawItemText";
    QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
}
