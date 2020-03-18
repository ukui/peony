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

#include "list-view-delegate.h"
#include "file-operation-utils.h"
#include "file-item-model.h"

#include <QTimer>
#include <QPushButton>

#include <QPainter>

#include <QKeyEvent>

using namespace Peony;

ListViewDelegate::ListViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_styled_button = new QPushButton;
}

ListViewDelegate::~ListViewDelegate()
{
    m_styled_button->deleteLater();
}

void ListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::Alignment(Qt::AlignLeft|Qt::AlignVCenter);

    auto widget = qobject_cast<QWidget*>(parent());
    auto style = widget->style();
    painter->save();
    if (opt.state.testFlag(QStyle::State_MouseOver)) {
        if (!opt.state.testFlag(QStyle::State_Selected)) {
            auto rect = opt.rect;
            QColor color = m_styled_button->palette().highlight().color();
            color.setAlpha(127);//half transparent
            painter->fillRect(rect, color);
        } else {
            auto rect = opt.rect;
            QColor color = m_styled_button->palette().highlight().color();
            painter->fillRect(rect, color);
        }
    }
    painter->restore();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QWidget *ListViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    TextEdit *edit = new TextEdit(parent);
    edit->setContextMenuPolicy(Qt::CustomContextMenu);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setWordWrapMode(QTextOption::NoWrap);

    QTimer::singleShot(1, [=](){
        this->updateEditorGeometry(edit, option, index);
    });

    connect(edit, &TextEdit::textChanged, [=](){
        updateEditorGeometry(edit, option, index);
    });

    connect(edit, &TextEdit::finishEditRequest, [=](){
        setModelData(edit, nullptr, index);
        edit->deleteLater();
    });

    return edit;
}

void ListViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    TextEdit *edit = qobject_cast<TextEdit *>(editor);
    if (!edit)
        return;

    edit->setText(index.data(Qt::DisplayRole).toString());
    auto cursor = edit->textCursor();
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    //qDebug()<<cursor.position();
    if (edit->toPlainText().contains(".") && !edit->toPlainText().startsWith(".")) {
        cursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, 2);
        //qDebug()<<cursor.position();
    }
    //qDebug()<<cursor.anchor();
    edit->setTextCursor(cursor);
}

void ListViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    TextEdit *edit = qobject_cast<TextEdit*>(editor);
    edit->setFixedHeight(editor->height());
    edit->resize(edit->document()->size().width(), -1);
}

void ListViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    TextEdit *edit = qobject_cast<TextEdit*>(editor);
    if (!edit)
        return;

    auto text = edit->toPlainText();
    if (text.isEmpty())
        return;
    if (text == index.data(Qt::DisplayRole).toString())
        return;

    FileOperationUtils::rename(index.data(FileItemModel::UriRole).toString(), text, true);
}

//TextEdit
TextEdit::TextEdit(QWidget *parent) : QTextEdit (parent)
{

}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return) {
        Q_EMIT finishEditRequest();
        return;
    }
    return QTextEdit::keyPressEvent(e);
}
