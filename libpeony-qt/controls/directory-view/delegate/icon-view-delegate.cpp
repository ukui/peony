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

#include <QStyleOptionViewItem>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewDelegate::IconViewDelegate(QObject *parent) : QStyledItemDelegate (parent)
{
    m_styled_button = new QPushButton;
    m_isStartDrag = false;
}

IconViewDelegate::~IconViewDelegate()
{
    m_styled_button->deleteLater();
}

QSize IconViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //QStyleOptionViewItem opt = option;
    //initStyleOption(&opt, index);

    auto view = qobject_cast<IconView*>(this->parent());
    auto iconSize = view->iconSize();
    auto fm = qApp->fontMetrics();
    int width = iconSize.width() + 41;
    int height = iconSize.height() + fm.ascent()*2 + 20 + 10;
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

    auto rawDecoSize = opt.decorationSize;
    opt.decorationSize = view->iconSize();
    if (qApp->devicePixelRatio() != 1.0) {
        opt.rect.adjust(1, 1, -1, -1);
    }

    if (!opt.state.testFlag(QStyle::State_Selected)) {
        if (opt.state & QStyle::State_Sunken) {
            opt.palette.setColor(QPalette::Highlight, opt.palette.button().color());
        }
        if (opt.state & QStyle::State_MouseOver) {
            opt.palette.setColor(QPalette::Highlight, opt.palette.mid().color());
        }
    }
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, nullptr);
    opt.decorationSize = rawDecoSize;

    bool bCutFile = false;
    if (ClipboardUtils::isClipboardHasFiles() &&
        FileUtils::isSamePath(ClipboardUtils::getClipedFilesParentUri(), view->getDirectoryUri())) {
        if (ClipboardUtils::isPeonyFilesBeCut() && ClipboardUtils::isClipboardFilesBeCut()) {
            auto clipedUris = ClipboardUtils::getClipboardFilesUris();
            if (clipedUris.contains(FileUtils::urlEncode(index.data(FileItemModel::UriRole).toString()))) {
                painter->setOpacity(0.5);
                bCutFile = true;
                qDebug()<<"cut item"<<index.data();
            }
        }
    }
    else
       painter->setOpacity(1.0);

    auto iconSizeExpected = view->iconSize();
    auto iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    int y_delta = iconSizeExpected.height() - iconRect.height();
    opt.rect.setY(opt.rect.y() + y_delta);

    auto text = opt.text;
    opt.text = nullptr;
    auto state = opt.state;
    //bug#99340,修改图标选中状态，会变暗
    if((opt.state & QStyle::State_Enabled) && (opt.state & QStyle::State_Selected) && !m_isStartDrag)
    {
        opt.state &= ~QStyle::State_Selected;
    }
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
    opt.state = state;
    opt.text = text;

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
    if (view->selectedIndexes().count() == 1 && view->selectedIndexes().first() == index && !bCutFile) {
        useIndexWidget = true;
        if (view->indexWidget(index)) {
        } else if (! view->isDraggingState() && view->m_allow_set_index_widget) {
            IconViewIndexWidget *indexWidget = new IconViewIndexWidget(this, option, index, getView());
            connect(getView()->m_model, &FileItemModel::dataChanged, indexWidget, [=](const QModelIndex &topleft, const QModelIndex &bottomRight){
                // if item has been removed and there is no reference for responding info,
                // clear index widgets.
                if (!indexWidget->m_info.lock()) {
                    getView()->clearIndexWidget();
                    return;
                }
                if (topleft.data(Qt::UserRole).toString() == indexWidget->m_info.lock().get()->uri()) {
                    if (getView()->getSelections().count() == 1 && getView()->getSelections().first() == topleft.data(Qt::UserRole).toString()) {
                        auto selections = getView()->getSelections();
                        getView()->clearSelection();
                        getView()->setSelections(selections);
                    }
                }
            });
            view->setIndexWidget(index, indexWidget);
            indexWidget->adjustPos();
        }
    }

    //fix bug#46785, select one file cut has no effect issue
    if (bCutFile && !getView()->getDelegateEditFlag())/* Rename is index is not set to nullptr,link to bug#61119.modified by 2021/06/22 */
        view->setIndexWidget(index, nullptr);

    // draw color symbols
    int iLine = 0;
    int yoffset = 0;
    if (!isDragging || !view->selectedIndexes().contains(index)) {
        auto colors = info->getColors();
        if(0 < colors.count())
        {
            const int MAX_LABEL_NUM = 3;
            int startIndex = (colors.count() > MAX_LABEL_NUM ? colors.count() - MAX_LABEL_NUM : 0);
            int num =  colors.count() - startIndex;
            int xoffset = 0;
            auto lineSpacing = option.fontMetrics.lineSpacing();

            QString text = opt.text;
            QFont font = opt.font;
            QTextLayout textLayout(text, font);

            QTextOption textOpt;
            textOpt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

            textLayout.setTextOption(textOpt);
            textLayout.beginLayout();

            QTextLine line = textLayout.createLine();
            if (!line.isValid())
                return;
            int width = opt.rect.width() - (num+1)*6 - 2*2 - 4;
            line.setLineWidth(width);
            xoffset = (width - line.naturalTextWidth())/2 ;
            if(xoffset < 0)
            {
                xoffset = 2;
            }
            yoffset = (lineSpacing -12 )/2+2;
            for (int i = startIndex; i < colors.count(); ++i) {
                auto color = colors.at(i);
                painter->save();
                painter->setRenderHint(QPainter::Antialiasing);
                painter->translate(opt.rect.topLeft());
                painter->translate(0, iconRect.size().height() + 5);
                painter->setPen(opt.palette.highlightedText().color());
                painter->setBrush(color);
                painter->drawEllipse(QRectF(xoffset, yoffset, 12, 12));
                painter->restore();
                xoffset += 12/2;
            }

            yoffset = 0;
            painter->save();
            painter->translate(opt.rect.topLeft());
            painter->translate(xoffset+10, iconRect.size().height() + 5);
            if (opt.state.testFlag(QStyle::State_Selected))
                painter->setPen(opt.palette.highlightedText().color());
            else
                painter->setPen(opt.palette.text().color());
            line.draw(painter, QPoint(0, yoffset));
            yoffset += lineSpacing;
            opt.text = text.mid(line.textLength());

            textLayout.endLayout();
            painter->restore();
            iLine++;
        }
    }

    if(!opt.text.isEmpty())
    {
        painter->save();
        painter->translate(opt.rect.topLeft());
        painter->translate(0, iconRect.size().height() + 5 + yoffset);

        IconViewTextHelper::paintText(painter,
                                      opt,
                                      index,
                                      9999,
                                      2,
                                      2-iLine);

        painter->restore();
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

void IconViewDelegate::doneWithEditor()
{
    auto editor = qobject_cast<QWidget *>(sender());
    commitData(editor);
    closeEditor(editor, NoHint);
    isEditing(false);
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
    QTimer::singleShot(1, edit, [=]() {
        edit->minimalAdjust();
    });

    connect(edit, &IconViewEditor::returnPressed, this, &IconViewDelegate::doneWithEditor);

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
            //model->sort(getView()->getSortType(), Qt::SortOrder(getView()->getSortOrder()));
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
    if (newName.isNull())
        return;
    //process special name . or .. or only space
    if (newName == "." || newName == ".." || newName.trimmed() == "")
        newName = "";
    //comment new name != suffix check to fix feedback issue
    if (newName.length() >0 && newName != oldName/* && newName != suffix*/) {
        auto fileOpMgr = FileOperationManager::getInstance();
        auto renameOp = new FileRenameOperation(index.data(FileItemModel::UriRole).toString(), newName);

        connect(renameOp, &FileRenameOperation::operationFinished, getView(), [=](){
            auto info = renameOp->getOperationInfo().get();
            auto uri = info->target();
            QTimer::singleShot(100, getView(), [=](){
                getView()->setSelections(QStringList()<<uri);
                getView()->scrollToSelection(uri);
                //set focus to fix bug#54061
                getView()->setFocus();
            });
        }, Qt::BlockingQueuedConnection);

        fileOpMgr->startOperation(renameOp, true);
    }
    else if (newName == oldName)
    {
        //create new file, should select the file or folder
        getView()->selectionModel()->select(index, QItemSelectionModel::Select);
        //set focus to fix bug#54061
        getView()->setFocus();
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

void IconViewTextHelper::paintText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, int textMaxHeight, int horizalMargin, int maxLineCount, bool useSystemPalette, const QColor &customColor)
{
    painter->save();
    painter->translate(horizalMargin, 0);

    if (useSystemPalette) {
        if (option.state.testFlag(QStyle::State_Selected))
            painter->setPen(option.palette.highlightedText().color());
        else
            painter->setPen(option.palette.text().color());
    }

    if (customColor != Qt::transparent) {
        painter->setPen(customColor);
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
void IconViewDelegate::initIndexOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    return initStyleOption(option, index);
}
