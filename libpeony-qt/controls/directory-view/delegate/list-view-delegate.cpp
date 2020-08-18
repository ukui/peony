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

#include "list-view-delegate.h"
#include "file-operation-manager.h"
#include "file-rename-operation.h"
#include "file-item-model.h"

#include "list-view.h"

#include "file-info.h"

#include <QTimer>
#include <QPushButton>

#include <QPainter>

#include <QKeyEvent>

#include <QApplication>

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

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    if (index.column() == 0) {
        auto view = qobject_cast<DirectoryView::ListView *>(parent());
        if (!view->isDragging() || !view->selectionModel()->selectedIndexes().contains(index)) {
            auto info = FileInfo::fromUri(index.data(Qt::UserRole).toString());
            auto colors = info->getColors();
            int offset = 0;
            for (auto color : colors) {
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->translate(0, opt.rect.topLeft().y());
                painter->translate(2, 2);
                painter->setPen(opt.palette.highlightedText().color());
                painter->setBrush(color);
                painter->drawEllipse(QRectF(offset, 0, 10, 10));
                painter->restore();
                offset += 10;
            }
        }
    }
}

QWidget *ListViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    TextEdit *edit = new TextEdit(parent);
    edit->setAcceptRichText(false);
    edit->setContextMenuPolicy(Qt::CustomContextMenu);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setWordWrapMode(QTextOption::NoWrap);

    QTimer::singleShot(1, [=]() {
        this->updateEditorGeometry(edit, option, index);
    });

    connect(edit, &TextEdit::textChanged, [=]() {
        updateEditorGeometry(edit, option, index);
    });

    connect(edit, &TextEdit::finishEditRequest, [=]() {
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
        cursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, 1);
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
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
    //process special name . or .. or only space
    if (text == "." || text == ".." || text.trimmed() == "")
        return;

    auto view = qobject_cast<DirectoryView::ListView *>(parent());

    auto fileOpMgr = FileOperationManager::getInstance();
    auto renameOp = new FileRenameOperation(index.data(FileItemModel::UriRole).toString(), text);

    connect(renameOp, &FileRenameOperation::operationFinished, view, [=](){
        auto info = renameOp->getOperationInfo().get();
        auto uri = info->target();
        QTimer::singleShot(100, view, [=](){
            view->setSelections(QStringList()<<uri);
            view->scrollToSelection(uri);
        });
    }, Qt::BlockingQueuedConnection);

    fileOpMgr->startOperation(renameOp, true);
}

//TextEdit
TextEdit::TextEdit(QWidget *parent) : QTextEdit (parent)
{

}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        Q_EMIT finishEditRequest();
        return;
    }
    return QTextEdit::keyPressEvent(e);
}
