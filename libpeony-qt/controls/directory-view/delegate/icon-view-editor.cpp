#include <QPaintEvent>
#include <QPainter>
#include <QPolygon>

#include "icon-view-editor.h"

IconViewEditor::IconViewEditor(QWidget *parent) : QTextEdit(parent)
{
    setContentsMargins(0, 0, 0, 0);

    setStyleSheet("padding: 0px;"
                  "background-color: white;");
}

void IconViewEditor::paintEvent(QPaintEvent *e)
{

    QPainter p(this->viewport());
    p.fillRect(this->viewport()->rect(), this->palette().base());
    QPen pen;
    pen.setWidth(2);
    pen.setColor("black");
    QPolygon polygon = this->viewport()->rect();
    p.setPen(pen);
    p.drawPolygon(polygon);
    QTextEdit::paintEvent(e);
}
