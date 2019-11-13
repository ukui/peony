#include <QPaintEvent>
#include <QPainter>
#include <QPolygon>
#include <QLineEdit>

#include "icon-view-editor.h"

using namespace Peony;
using namespace DirectoryView;

IconViewEditor::IconViewEditor(QWidget *parent) : QTextEdit(parent)
{
    m_styled_edit = new QLineEdit;
    setContentsMargins(0, 0, 0, 0);
    setAlignment(Qt::AlignCenter);

    setStyleSheet("padding: 0px;"
                  "background-color: white;");

    connect(this, &IconViewEditor::textChanged, this, &IconViewEditor::minimalAdjust);
}

IconViewEditor::~IconViewEditor()
{
    m_styled_edit->deleteLater();
}

void IconViewEditor::paintEvent(QPaintEvent *e)
{
    QPainter p(this->viewport());
    p.fillRect(this->viewport()->rect(), m_styled_edit->palette().base());
    QPen pen;
    pen.setWidth(2);
    pen.setColor(this->palette().highlight().color());
    QPolygon polygon = this->viewport()->rect();
    p.setPen(pen);
    p.drawPolygon(polygon);
    QTextEdit::paintEvent(e);
}

void IconViewEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return) {
        Q_EMIT returnPressed();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

void IconViewEditor::minimalAdjust()
{
    this->resize(QSize(document()->size().width(), document()->size().height() + 10));
}
