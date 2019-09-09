#include <QPaintEvent>
#include <QPainter>
#include <QPolygon>

#include "icon-view-editor.h"

IconViewEditor::IconViewEditor(QWidget *parent) : QTextEdit(parent)
{

}

void IconViewEditor::paintEvent(QPaintEvent *e)
{
    QTextEdit::paintEvent(e);
    QPainter p(this->viewport());
    QPen pen;
    pen.setWidth(2);
    pen.setColor("red");
    QPolygon polygon = this->rect();
    p.setPen(pen);
    p.drawPolygon(polygon);
}
