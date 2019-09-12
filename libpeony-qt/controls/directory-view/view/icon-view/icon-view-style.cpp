#include "icon-view-style.h"

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewStyle::IconViewStyle(QStyle *style) : QProxyStyle(style)
{
    qDebug()<<"icon view style";
}

void IconViewStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    qDebug()<<"draw control";
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
