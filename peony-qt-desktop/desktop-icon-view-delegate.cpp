/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#include "desktop-icon-view-delegate.h"
#include "desktop-icon-view.h"

#include "icon-view-editor.h"

#include "file-operation-manager.h"
#include "file-rename-operation.h"

#include "icon-view-delegate.h"

#include <QPushButton>
#include <QWidget>
#include <QPainter>
#include <QFileInfo>
#include <file-info-job.h>

#include <QApplication>

#include <QDebug>
#include <file-info.h>

using namespace Peony;
using namespace Peony::DirectoryView;

DesktopIconViewDelegate::DesktopIconViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_styled_button = new QPushButton;
}

DesktopIconViewDelegate::~DesktopIconViewDelegate()
{
    m_styled_button->deleteLater();
}

void DesktopIconViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    auto view = qobject_cast<Peony::DesktopIconView*>(parent());

    auto style = option.widget->style();

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (view->state() == DesktopIconView::DraggingState) {
        if (auto widget = view->indexWidget(index)) {
            view->setIndexWidget(index, nullptr);
        }

        if (view->selectionModel()->selection().contains(index)) {
            painter->setOpacity(0.8);
        }
    } else if (opt.state.testFlag(QStyle::State_Selected)) {
        if (view->indexWidget(index)) {
            opt.text = nullptr;
        }
    }

    //paint background
    if (!view->indexWidget(index)) {
        //painter->setClipRect(opt.rect);
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        if (opt.state.testFlag(QStyle::State_MouseOver) && !opt.state.testFlag(QStyle::State_Selected)) {
            QColor color = m_styled_button->palette().highlight().color();
            color.setAlpha(255*0.3);//half transparent
            //painter->fillRect(opt.rect, color);
            color.setAlpha(255);
            painter->setPen(color.darker(100));
            painter->setBrush(color);
            painter->drawRoundedRect(opt.rect.adjusted(1, 1, -1, -1), 6, 6);
        }
        if (opt.state.testFlag(QStyle::State_Selected)) {
            QColor color = m_styled_button->palette().highlight().color();
            color.setAlpha(255*0.7);//half transparent
            //painter->fillRect(opt.rect, color);
            color.setAlpha(255*0.8);
            painter->setPen(color);
            painter->setBrush(color);
            painter->drawRoundedRect(opt.rect.adjusted(1, 1, -1, -1), 6, 6);
        }
        painter->restore();
    }

    auto iconSizeExpected = view->iconSize();
    auto iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, opt.widget);
    int y_delta = iconSizeExpected.height() - iconRect.height();
    opt.rect.translate(0, y_delta);

    int maxTextHight = opt.rect.height() - iconSizeExpected.height() - 10;
    if (maxTextHight < 0) {
        maxTextHight = 0;
    }

    //paint icon item
    auto color = QColor(230, 230, 230);
    opt.palette.setColor(QPalette::Text, color);
    color.setRgb(240, 240, 240);
    opt.palette.setColor(QPalette::HighlightedText, color);

    auto text = opt.text;
    opt.text = nullptr;

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    opt.text = text;

    painter->save();
    //painter->translate(visualRect.topLeft());
    painter->translate(opt.rect.topLeft());
    painter->translate(0, -y_delta);

    //paint text shadow
    painter->save();
    painter->translate(1, 1 + iconSizeExpected.height() + 10);
    //painter->setFont(opt.font);
    QColor shadow = Qt::black;
    shadow.setAlpha(127);
    painter->setPen(shadow);
    IconViewTextHelper::paintText(painter,
                                  opt,
                                  index,
                                  maxTextHight,
                                  0,
                                  2,
                                  false);
    painter->restore();

    //paint text
    painter->save();
    painter->translate(0, 0 + iconSizeExpected.height() + 10);
    //painter->setFont(opt.font);
    painter->setPen(opt.palette.highlightedText().color());
    IconViewTextHelper::paintText(painter,
                                  opt,
                                  index,
                                  maxTextHight,
                                  0,
                                  2,
                                  false);
    painter->restore();

    painter->restore();

    //paint link icon and locker icon
    FileInfo* file = FileInfo::fromUri(index.data(Qt::UserRole).toString(), true).get();
    if ((index.data(Qt::UserRole).toString() != "computer:///") && (index.data(Qt::UserRole).toString() != "trash:///")) {
        QSize lockerIconSize = QSize(16, 16);
        int offset = 8;
        switch (view->zoomLevel()) {
        case DesktopIconView::Small: {
            lockerIconSize = QSize(8, 8);
            offset = 10;
            break;
        }
        case DesktopIconView::Normal: {
            break;
        }
        case DesktopIconView::Large: {
            offset = 4;
            lockerIconSize = QSize(24, 24);
            break;
        }
        case DesktopIconView::Huge: {
            offset = 2;
            lockerIconSize = QSize(32, 32);
            break;
        }
        default: {
            break;
        }
        }
        auto topRight = opt.rect.topRight();
        topRight.setX(topRight.x() - opt.rect.width() + 10);
        topRight.setY(topRight.y() + 10);
        auto linkRect = QRect(topRight, lockerIconSize);

        if (! file->canRead())
        {
            QIcon symbolicLinkIcon = QIcon::fromTheme("emblem-unreadable");
            symbolicLinkIcon.paint(painter, linkRect, Qt::AlignCenter);
        }
        else if(! file->canWrite() && ! file->canExecute())
        {
            QIcon symbolicLinkIcon = QIcon::fromTheme("emblem-readonly");
            symbolicLinkIcon.paint(painter, linkRect, Qt::AlignCenter);
        }
    }

    if (index.data(Qt::UserRole + 1).toBool()) {
        QSize symbolicIconSize = QSize(16, 16);
        int offset = 8;
        switch (view->zoomLevel()) {
        case DesktopIconView::Small: {
            symbolicIconSize = QSize(8, 8);
            offset = 10;
            break;
        }
        case DesktopIconView::Normal: {
            break;
        }
        case DesktopIconView::Large: {
            offset = 4;
            symbolicIconSize = QSize(24, 24);
            break;
        }
        case DesktopIconView::Huge: {
            offset = 2;
            symbolicIconSize = QSize(32, 32);
            break;
        }
        default: {
            break;
        }
        }
        auto topRight = opt.rect.topRight();
        topRight.setX(topRight.x() - offset - symbolicIconSize.width());
        topRight.setY(topRight.y() + offset);
        auto linkRect = QRect(topRight, symbolicIconSize);
        QIcon symbolicLinkIcon = QIcon::fromTheme("emblem-symbolic-link");
        symbolicLinkIcon.paint(painter, linkRect, Qt::AlignCenter);
    }

    /*
    qDebug()<<index.data();
    qDebug()<<opt.rect;
    qDebug()<<view->visualRect(index);
    qDebug()<<view->rectForIndex(index);
    qDebug()<<style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);
    */
    painter->restore();
    //return QStyledItemDelegate::paint(painter, option, index);
}

QSize DesktopIconViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto view = qobject_cast<Peony::DesktopIconView*>(parent());
    auto zoomLevel = view->zoomLevel();
    switch (zoomLevel) {
    case DesktopIconView::Small:
        return QSize(60, 70);
    case DesktopIconView::Normal:
        return QSize(90, 100);
    case DesktopIconView::Large:
        return QSize(105, 128);
    case DesktopIconView::Huge:
        return QSize(120, 150);
    default:
        return QSize(90, 100);
    }
}

QWidget *DesktopIconViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto edit = new IconViewEditor(parent);
    auto font = option.font;
    auto view = qobject_cast<Peony::DesktopIconView*>(this->parent());
//    switch (view->zoomLevel()) {
//    case DesktopIconView::Small:
//        font.setPixelSize(int(font.pixelSize() * 0.8));
//        break;
//    case DesktopIconView::Large:
//        font.setPixelSize(int(font.pixelSize() * 1.2));
//        break;
//    case DesktopIconView::Huge:
//        font.setPixelSize(int(font.pixelSize() * 1.4));
//        break;
//    default:
//        break;
//    }

    edit->setFont(font);

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

    return edit;
}

void DesktopIconViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    IconViewEditor *edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;

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

void DesktopIconViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

void DesktopIconViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
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
        auto renameOp = new FileRenameOperation(index.data(Qt::UserRole).toString(), newName);
        getView()->setRenaming(true);
        fileOpMgr->startOperation(renameOp, true);
    }
}

DesktopIconView *DesktopIconViewDelegate::getView() const
{
    auto view = qobject_cast<Peony::DesktopIconView*>(parent());
    return view;
}
