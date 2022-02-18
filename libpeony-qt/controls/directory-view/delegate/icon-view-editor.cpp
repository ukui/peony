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

#include <QPaintEvent>
#include <QPainter>
#include <QPolygon>
#include <QLineEdit>
#include <QAbstractItemView>

#include "icon-view-editor.h"

using namespace Peony;
using namespace DirectoryView;

IconViewEditor::IconViewEditor(QWidget *parent) : QTextEdit(parent)
{
    setAcceptRichText(false);
    //setContextMenuPolicy(Qt::CustomContextMenu);
    m_styled_edit = new QLineEdit;
    setContentsMargins(0, 0, 0, 0);
    setAlignment(Qt::AlignCenter);

//    setStyleSheet("padding: 0px;"
//                  "background-color: white;");

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
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        Q_EMIT returnPressed();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

void IconViewEditor::minimalAdjust()
{
    this->resize(QSize(document()->size().width(), document()->size().height() + 24));
}
