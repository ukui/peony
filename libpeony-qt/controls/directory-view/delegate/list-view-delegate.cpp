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
#include "clipboard-utils.h"

#include "file-info.h"

#include <QTimer>
#include <QPushButton>

#include <QPainter>

#include <QKeyEvent>
#include <QItemDelegate>
#include <file-label-model.h>

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

void ListViewDelegate::initIndexOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    return initStyleOption(option, index);
}

void ListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::Alignment(Qt::AlignLeft|Qt::AlignVCenter);

    auto view = qobject_cast<DirectoryView::ListView *>(parent());
    auto info = FileInfo::fromUri(index.data(Qt::UserRole).toString());
    auto colors = info->getColors();
    if (index.column() == 0 && colors.count() >0) {
        if (!view->isDragging() || !view->selectionModel()->selectedIndexes().contains(index)) {
            int xoffset = 5;
            int yoffset = 0;
            int index = 0;
            const int MAX_LABEL_NUM = 3;
            int startIndex = (colors.count() > MAX_LABEL_NUM ? colors.count() - MAX_LABEL_NUM : 0);
            int num = colors.count() - startIndex + 1;

            //set color label on center, fix bug#40609
            auto iconSize = view->iconSize();
            auto labelSize = iconSize.height()/3;
            if (labelSize > 10)
                labelSize = 10;
            if (labelSize <6)
                labelSize = 6;

            yoffset = (opt.rect.height()-labelSize*num/2)/2;
            if(yoffset < 2)
            {
                yoffset = 2;
            }
            for (int i = startIndex; i < colors.count(); ++i, ++index) {
                auto color = colors.at(i);
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->translate(0, opt.rect.topLeft().y());
                painter->translate(2, 0);
                painter->setPen(opt.palette.highlightedText().color());
                painter->setBrush(color);
                painter->drawEllipse(QRectF(xoffset, yoffset, labelSize, labelSize));
                painter->restore();
                yoffset += labelSize/2;
            }
        }
    }

    if (ClipboardUtils::isClipboardHasFiles() &&
        FileUtils::isSamePath(ClipboardUtils::getClipedFilesParentUri(), view->getDirectoryUri())) {
        if (ClipboardUtils::isPeonyFilesBeCut() && ClipboardUtils::isClipboardFilesBeCut()) {
            auto clipedUris = ClipboardUtils::getClipboardFilesUris();
            if (clipedUris.contains(FileUtils::urlEncode(index.data(Qt::UserRole).toString()))) {
                painter->setOpacity(0.5);
                qDebug()<<"cut item in list view"<<index.data();
            }
            else
                painter->setOpacity(1.0);
        }
    }

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    //add link and read only icon support
    if (index.column() == 0) {
        auto rect = view->visualRect(index);
        auto iconSize = view->iconSize();
        auto size = iconSize.width()/2;
        bool isSymbolicLink = info->isSymbolLink();
        auto loc_x = rect.x() + iconSize.width() - size/2;
        auto loc_y =rect.y();
        //paint symbolic link emblems
        if (isSymbolicLink) {
            QIcon icon = QIcon::fromTheme("emblem-symbolic-link");
            //qDebug()<<info->symbolicIconName();
            icon.paint(painter, loc_x, loc_y, size, size);
            //painter->restore();
        }

        //paint access emblems
        //NOTE: we can not query the file attribute in smb:///(samba) and network:///.
        loc_x = rect.x();
        if (info->uri().startsWith("file:")) {
            if (!info->canRead()) {
                QIcon icon = QIcon::fromTheme("emblem-unreadable");
                icon.paint(painter, loc_x, loc_y, size, size);
            } else if (!info->canWrite() && !info->canExecute()) {
                QIcon icon = QIcon::fromTheme("emblem-readonly");
                icon.paint(painter, loc_x, loc_y, size, size);
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

    connect(edit, &TextEdit::finishEditRequest, this, [=]() {
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
    bool isDir = FileUtils::getFileIsFolder(index.data(Qt::UserRole).toString());
    bool isDesktopFile = index.data(Qt::UserRole).toString().endsWith(".desktop");
    bool isSoftLink = FileUtils::getFileIsSymbolicLink(index.data(Qt::UserRole).toString());
    if (!isDesktopFile && !isSoftLink && !isDir && edit->toPlainText().contains(".") && !edit->toPlainText().startsWith(".")) {
        int n = 1;
        if(index.data(Qt::DisplayRole).toString().contains(".tar.")) //ex xxx.tar.gz xxx.tar.bz2
            n = 2;
        while(n){
            cursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, 1);
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
            --n;
        }
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

    //process special name . or .. or only space
    if (text == "." || text == ".." || text.trimmed() == "")
        return;

    if (! index.isValid())
        return;

    auto view = qobject_cast<DirectoryView::ListView *>(parent());
    auto oldName = index.data(Qt::DisplayRole).toString();
    if (text == oldName)
    {
        //create new file, should select the file or folder
        auto flags = QItemSelectionModel::Select|QItemSelectionModel::Rows;
        view->selectionModel()->select(index, flags);
        view->setFocus();
        return;
    }

    auto fileOpMgr = FileOperationManager::getInstance();
    auto renameOp = new FileRenameOperation(index.data(FileItemModel::UriRole).toString(), text);

    connect(renameOp, &FileRenameOperation::operationFinished, view, [=](){
        auto info = renameOp->getOperationInfo().get();
        auto uri = info->target();
        QTimer::singleShot(100, view, [=](){
            view->setSelections(QStringList()<<uri);
            //after rename will nor sort immediately, comment to fix bug#60482
            //view->scrollToSelection(uri);
            view->setFocus();
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

}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
        Q_EMIT finishEditRequest();
        return;
    }
    return QTextEdit::keyPressEvent(e);
}
