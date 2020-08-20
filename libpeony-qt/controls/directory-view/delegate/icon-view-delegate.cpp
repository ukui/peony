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

#include "icon-view-delegate.h"
#include "icon-view.h"
#include "file-item-proxy-filter-sort-model.h"
#include "file-item.h"
#include "file-info.h"

#include "file-operation-manager.h"
#include "file-rename-operation.h"

#include <QDebug>
#include <QLabel>

#include <QPainter>
#include <QPalette>

#include <QTextCursor>
#include <QGraphicsTextItem>
#include <QFont>

#include <QStyle>
#include <QApplication>
#include <QPainter>

#include "icon-view-editor.h"
#include "icon-view-index-widget.h"

#include <QPushButton>

#include "clipboard-utils.h"

#include <QTextLayout>
#include <QFileInfo>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewDelegate::IconViewDelegate(QObject *parent) : QStyledItemDelegate (parent)
{
    m_styled_button = new QPushButton;
}

IconViewDelegate::~IconViewDelegate()
{
    m_styled_button->deleteLater();
}

QSize IconViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    auto view = qobject_cast<IconView*>(this->parent());
    auto iconSize = view->iconSize();
    auto font = qApp->font();
    auto fm = QFontMetrics(font);
    int width = iconSize.width() + 41;
    int height = iconSize.height() + fm.ascent()*2 + 20;
    return QSize(width, height);
    /*
    qDebug()<<option;
    qDebug()<<option.font;
    qDebug()<<option.icon;
    qDebug()<<option.text;
    qDebug()<<option.widget;
    qDebug()<<option.decorationSize;
    qDebug()<<QStyledItemDelegate::sizeHint(option, index);
    */
}

void IconViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //FIXME: how to deal with word wrap correctly?
    painter->save();

    bool isDragging = false;
    auto view = qobject_cast<IconView*>(this->parent());
    if (view->state() == IconView::DraggingState) {
        isDragging = true;
        if (view->selectionModel()->selection().contains(index)) {
            painter->setOpacity(0.8);
        }
    }

    //default painter
    //QStyledItemDelegate::paint(painter, option, index);
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    //qDebug()<<option.widget->style();
    //qDebug()<<option.widget;
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    auto style = QApplication::style();

    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, nullptr);

    if (ClipboardUtils::getClipedFilesParentUri() == view->getDirectoryUri()) {
        if (ClipboardUtils::isClipboardFilesBeCut()) {
            auto clipedUris = ClipboardUtils::getClipboardFilesUris();
            if (clipedUris.contains(index.data(FileItemModel::UriRole).toString())) {
                painter->setOpacity(0.5);
                qDebug()<<"cut item"<<index.data();
            }
        }
    }

    auto iconSizeExpected = view->iconSize();
    auto iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    int y_delta = iconSizeExpected.height() - iconRect.height();
    opt.rect.setY(opt.rect.y() + y_delta);

    auto text = opt.text;
    opt.text = nullptr;
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
    opt.text = text;
    //auto textSize = IconViewTextHelper::getTextSizeForIndex(opt, index, 2, 2);
    painter->save();
    painter->translate(opt.rect.topLeft());
    painter->translate(0, iconRect.size().height() + 5);
    IconViewTextHelper::paintText(painter,
                                  opt,
                                  index,
                                  9999,
                                  2,
                                  2);

    painter->restore();

    //get file info from index
    auto model = static_cast<FileItemProxyFilterSortModel*>(view->model());
    auto item = model->itemFromIndex(index);
    //NOTE: item might be deleted when painting, because we might start a
    //location change during the painting.
    if (!item) {
        return;
    }
    auto info = item->info();
    auto rect = view->visualRect(index);

    bool useIndexWidget = false;
    if (view->selectedIndexes().count() == 1 && view->selectedIndexes().first() == index) {
        useIndexWidget = true;
        if (view->indexWidget(index)) {
        } else if (! view->isDraggingState()) {
            IconViewIndexWidget *indexWidget = new IconViewIndexWidget(this, option, index, getView());
            view->setIndexWidget(index, indexWidget);
        }
    }

    // draw color symbols
    if (!isDragging || !view->selectedIndexes().contains(index)) {
        auto colors = info->getColors();
        int offset = 0;
        for (auto color : colors) {
            painter->save();
            painter->setRenderHint(QPainter::Antialiasing);
            painter->translate(option.rect.topLeft());
            painter->translate(2, 2);
            painter->setPen(opt.palette.highlightedText().color());
            painter->setBrush(color);
            painter->drawEllipse(QRectF(offset, 0, 10, 10));
            painter->restore();
            offset += 10;
        }
    }

    //paint symbolic link emblems
    if (info->isSymbolLink()) {
        QIcon icon = QIcon::fromTheme("emblem-symbolic-link");
        //qDebug()<<info->symbolicIconName();
        icon.paint(painter, rect.x() + rect.width() - 30, rect.y() + 10, 20, 20, Qt::AlignCenter);
    }

    //paint access emblems

    //NOTE: we can not query the file attribute in smb:///(samba) and network:///.
    if (info->uri().startsWith("file:")) {
        if (!info->canRead()) {
            QIcon icon = QIcon::fromTheme("emblem-unreadable");
            icon.paint(painter, rect.x() + 10, rect.y() + 10, 20, 20);
        } else if (!info->canWrite() && !info->canExecute()) {
            QIcon icon = QIcon::fromTheme("emblem-readonly");
            icon.paint(painter, rect.x() + 10, rect.y() + 10, 20, 20);
        }
        painter->restore();
        return;
    }

    painter->restore();

    //single selection, we have to repaint the emblems.

}

void IconViewDelegate::setCutFiles(const QModelIndexList &indexes)
{
    m_cut_indexes = indexes;
}

QWidget *IconViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    auto edit = new IconViewEditor(parent);
    edit->setContentsMargins(0, 0, 0, 0);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setMinimumSize(sizeHint(option, index).width(), 54);

    edit->setText(index.data(Qt::DisplayRole).toString());
    edit->setAlignment(Qt::AlignCenter);
    //NOTE: if we directly call this method, there will be
    //nothing happen. add a very short delay will ensure that
    //the edit be resized.
    QTimer::singleShot(1, [=]() {
        edit->minimalAdjust();
    });

    connect(edit, &IconViewEditor::returnPressed, [=]() {
        this->setModelData(edit, nullptr, index);
        edit->deleteLater();
    });

    connect(edit, &QWidget::destroyed, this, [=]() {
        // NOTE: resort view after edit closed.
        // it's because if we not, the viewport might
        // not be updated in some cases.
        Q_EMIT isEditing(false);
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
        QTimer::singleShot(100, this, [=]() {
#else
        QTimer::singleShot(100, [=]() {
#endif
            auto model = qobject_cast<QSortFilterProxyModel*>(getView()->model());
            //fix rename file back to default sort order
            //model->sort(-1, Qt::SortOrder(getView()->getSortOrder()));
            model->sort(getView()->getSortType(), Qt::SortOrder(getView()->getSortOrder()));
        });
    });

    return edit;
}

void IconViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    IconViewEditor *edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;

    Q_EMIT isEditing(true);
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

void IconViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    auto edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;

    auto opt = option;
    auto iconExpectedSize = getView()->iconSize();
    //auto iconRect = opt.widget->style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    //auto y_delta = iconExpectedSize.height() - iconRect.height();
    //edit->move(opt.rect.x(), opt.rect.y() + y_delta + 10);
    edit->move(opt.rect.x(), opt.rect.y() + iconExpectedSize.height() + 5);

    edit->resize(edit->document()->size().width(), edit->document()->size().height() + 10);
}

void IconViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    IconViewEditor *edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;
    auto newName = edit->toPlainText();
    auto oldName = index.data(Qt::DisplayRole).toString();
    QFileInfo info(index.data().toUrl().path());
    auto suffix = "." + info.suffix();
    if (newName.isNull())
        return;
    //process special name . or .. or only space
    if (newName == "." || newName == ".." || newName.trimmed() == "")
        newName = "";
    if (newName.length() >0 && newName != oldName && newName != suffix) {
        auto fileOpMgr = FileOperationManager::getInstance();
        auto renameOp = new FileRenameOperation(index.data(FileItemModel::UriRole).toString(), newName);

        connect(renameOp, &FileRenameOperation::operationFinished, getView(), [=](){
            auto info = renameOp->getOperationInfo().get();
            auto uri = info->target();
            QTimer::singleShot(100, getView(), [=](){
                getView()->setSelections(QStringList()<<uri);
                getView()->scrollToSelection(uri);
            });
        }, Qt::BlockingQueuedConnection);

        fileOpMgr->startOperation(renameOp, true);
    }
}

void IconViewDelegate::setIndexWidget(const QModelIndex &index, QWidget *widget) const
{
    auto view = qobject_cast<IconView*>(this->parent());
    view->setIndexWidget(index, widget);
}

IconView *IconViewDelegate::getView() const
{
    return qobject_cast<IconView*>(parent());
}

const QBrush IconViewDelegate::selectedBrush() const
{
    return m_styled_button->palette().highlight();
}

QSize IconViewTextHelper::getTextSizeForIndex(const QStyleOptionViewItem &option, const QModelIndex &index, int horizalMargin, int maxLineCount)
{
    int fixedWidth = option.rect.width() - horizalMargin*2;
    QString text = option.text;
    QFont font = option.font;
    QFontMetrics fontMetrics = option.fontMetrics;
    int lineSpacing = fontMetrics.lineSpacing();
    int textHight = 0;

    QTextLayout textLayout(text, font);

    QTextOption opt;
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    opt.setAlignment(Qt::AlignHCenter);

    textLayout.setTextOption(opt);
    textLayout.beginLayout();

    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(fixedWidth);
        textHight += lineSpacing;
    }

    textLayout.endLayout();
    if (maxLineCount > 0) {
        textHight = qMin(maxLineCount * lineSpacing, textHight);
    }

    return QSize(fixedWidth, textHight);
}

void IconViewTextHelper::paintText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, int textMaxHeight, int horizalMargin, int maxLineCount, bool useSystemPalette)
{
    painter->save();
    painter->translate(horizalMargin, 0);

    if (useSystemPalette) {
        if (option.state.testFlag(QStyle::State_Selected))
            painter->setPen(option.palette.highlightedText().color());
        else
            painter->setPen(option.palette.text().color());
    }

    int lineCount = 0;

    QString text = option.text;
    QFont font = option.font;
    QFontMetrics fontMetrics = option.fontMetrics;
    int lineSpacing = fontMetrics.lineSpacing();

    QTextLayout textLayout(text, font);

    QTextOption opt;
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    opt.setAlignment(Qt::AlignHCenter);

    textLayout.setTextOption(opt);
    textLayout.beginLayout();

    int width = option.rect.width() - 2*horizalMargin;

    int y = 0;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(width);
        int nextLineY = y + lineSpacing;
        lineCount++;

        if (textMaxHeight >= nextLineY + lineSpacing && lineCount != maxLineCount) {
            line.draw(painter, QPoint(0, y));
            y = nextLineY;
        } else {
            QString lastLine = option.text.mid(line.textStart());
            QString elidedLastLine = fontMetrics.elidedText(lastLine, Qt::ElideRight, width);
            auto rect = QRect(horizalMargin, y /*+ fontMetrics.ascent()*/, width, textMaxHeight);
            //opt.setWrapMode(QTextOption::NoWrap);
            opt.setWrapMode(QTextOption::NoWrap);
            painter->drawText(rect, elidedLastLine, opt);
            //painter->drawText(QPoint(0, y + fontMetrics.ascent()), elidedLastLine);
            line = textLayout.createLine();
            break;
        }
    }
    textLayout.endLayout();

    painter->restore();
}
