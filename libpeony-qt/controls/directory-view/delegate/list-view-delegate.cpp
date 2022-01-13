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
#include <QPainter>
#include <QItemDelegate>
#include <QHeaderView>

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
    auto view = qobject_cast<DirectoryView::ListView *>(parent());

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::Alignment(Qt::AlignLeft|Qt::AlignVCenter);

    auto rect = view->visualRect(index);

    if (index.column() == 0) {
        if (!view->isDragging() || !view->selectionModel()->selectedIndexes().contains(index)) {
            auto info = FileInfo::fromUri(index.data(Qt::UserRole).toString());
            auto colors = info->getColors();
            //修改标记个数最多为3个，以及标记位置
            const int MAX_LABEL_NUM = 3;
            const int LABEL_SIZE = 12;
            int startIndex = (colors.count() > MAX_LABEL_NUM ? colors.count() - MAX_LABEL_NUM : 0);
            int num =  colors.count() - startIndex;
            auto lineSpacing = option.fontMetrics.lineSpacing();
            int xOffSet = rect.topRight().x() - LABEL_SIZE/2 - 20;
            int yOffSet = rect.height()/2 - LABEL_SIZE/2;
            int width = rect.width();
            if(num > 0){
                //bug#94242 修改标记位置后和名称重叠，设置标记位置的背景颜色
                QRect markRect = opt.rect;
                markRect.setLeft(rect.width() - (num+1)*LABEL_SIZE/2 );
                bool isHover = (opt.state & QStyle::State_MouseOver) && (opt.state & ~QStyle::State_Selected);
                bool isSelected = opt.state & QStyle::State_Selected;
                bool enable = opt.state & QStyle::State_Enabled;
                QColor color = opt.palette.color(enable? QPalette::Active: QPalette::Disabled,
                                                     QPalette::Highlight);

                if (isSelected) {
                    color.setAlpha(255);
                } else if (isHover) {
                    color = opt.palette.color(QPalette::Active, QPalette::BrightText);
                    color.setAlphaF(0.05);
                } else {
                    color.setAlpha(0);
                }

                painter->save();
                painter->fillRect(markRect, color);
                painter->restore();
                width = width - (num+1)*LABEL_SIZE/2 - 20;
            }
            for (int i = startIndex; i < colors.count(); ++i) {
                auto color = colors.at(i);
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->translate(0, opt.rect.topLeft().y());
                painter->translate(2, 2);
                painter->setPen(opt.palette.highlightedText().color());
                painter->setBrush(color);
                painter->drawEllipse(QRectF(xOffSet, yOffSet, LABEL_SIZE, LABEL_SIZE));
                painter->restore();

                xOffSet -= LABEL_SIZE/2;
            }
            //bug#94242 修改标记位置后和名称重叠，设置汉字宽度
            opt.rect.setWidth(width);
        }
    }

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    if (view->isEnableMultiSelect()) {
        int selectBox = 0;
        //get current checkbox positon and draw them.
        selectBox = view->getCurrentCheckboxColumn();
        int selectBoxPosion = view->viewport()->width()+view->viewport()->x()-view->header()->sectionViewportPosition(selectBox)-48;
        if(index.column() == selectBox)
        {
            if(view->selectionModel()->selectedIndexes().contains(index))
            {
                QIcon icon = QIcon(":/icons/icon-selected.png");
                icon.paint(painter, rect.x()+selectBoxPosion, rect.y()+rect.height()/2-8, 16, 16, Qt::AlignCenter);
            }
            else
            {
                QIcon icon = QIcon(":/icons/icon-select.png");
                icon.paint(painter, rect.x()+selectBoxPosion, rect.y()+rect.height()/2-8, 16, 16, Qt::AlignCenter);
            }
        }
    }
}

QWidget *ListViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    TextEdit *edit = new TextEdit(parent);
    edit->setAcceptRichText(false);
    //edit->setContextMenuPolicy(Qt::CustomContextMenu);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setWordWrapMode(QTextOption::NoWrap);

//    QTimer::singleShot(1, parent, [=]() {
//        this->updateEditorGeometry(edit, option, index);
//    });

//    connect(edit, &TextEdit::textChanged, this, [=]() {
//        updateEditorGeometry(edit, option, index);
//    });

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

//void ListViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
//    TextEdit *edit = qobject_cast<TextEdit*>(editor);
//    edit->setFixedHeight(editor->height());
//    edit->resize(edit->document()->size().width(), -1);
//}

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

//not comment this bug to fix bug#93314
QSize ListViewDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    auto view = qobject_cast<DirectoryView::ListView *>(parent());
    int expectedHeight = view->iconSize().height() + 4;
    size.setHeight(qMax(expectedHeight, size.height()));
    return size;
}

//TextEdit
TextEdit::TextEdit(QWidget *parent) : QTextEdit (parent)
{
    this->setContentsMargins(0,0,0,0);
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        Q_EMIT finishEditRequest();
        return;
    }
    return QTextEdit::keyPressEvent(e);
}
