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

#include "icon-view-editor.h"
#include "icon-view-index-widget.h"

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewDelegate::IconViewDelegate(QObject *parent) : QStyledItemDelegate (parent)
{

}

QSize IconViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /*
    qDebug()<<option;
    qDebug()<<option.font;
    qDebug()<<option.icon;
    qDebug()<<option.text;
    qDebug()<<option.widget;
    qDebug()<<option.decorationSize;
    qDebug()<<QStyledItemDelegate::sizeHint(option, index);
    */
    return QSize(105, 118);

    return QStyledItemDelegate::sizeHint(option, index);
}

void IconViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto view = qobject_cast<IconView*>(this->parent());

    bool useIndexWidget = false;
    if (view->selectedIndexes().count() == 1 && view->selectedIndexes().first() == index) {
        useIndexWidget = true;
        if (view->indexWidget(index)) {
            return;
        } else {
            IconViewIndexWidget *indexWidget = new IconViewIndexWidget(this, option, index);
            view->setIndexWidget(index, indexWidget);
            return;
        }
    }

    //default painter
    //QStyledItemDelegate::paint(painter, option, index);
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    //qDebug()<<option.widget->style();
    qDebug()<<option.widget;
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    auto style = option.widget->style();

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    //QIcon icon = static_cast<QIcon>(index.data(Qt::DecorationRole).toByteArray());
    //painter->fillRect(option.rect, option.palette.highlight());
    //icon.paint(painter, option.rect.x(), option.rect.y(), option.rect.width(), option.rect.width());

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
    /*
    qDebug()<<option.decorationPosition;
    qDebug()<<option.decorationAlignment;
    qDebug()<<option.displayAlignment;
    */

    painter->save();
    //paint symbolic link emblems
    if (info->isSymbolLink()) {
        QIcon icon = QIcon::fromTheme("emblem-symbolic-link");
        //qDebug()<<info->symbolicIconName();
        icon.paint(painter, rect.x() + rect.width() - 30, rect.y() + 10, 20, 20, Qt::AlignCenter);
    }

    //paint access emblems

    //NOTE: we can not query the file attribute in smb:///(samba) and network:///.
    if (info->uri().startsWith("smb:") || info->uri().startsWith("network:")) {
        painter->restore();
        return;
    }

    if (!info->canRead()) {
        QIcon icon = QIcon::fromTheme("emblem-unreadable");
        icon.paint(painter, rect.x() + 10, rect.y() + 10, 20, 20);
    } else if (!info->canWrite() && !info->canExecute()){
        QIcon icon = QIcon::fromTheme("emblem-readonly");
        icon.paint(painter, rect.x() + 10, rect.y() + 10, 20, 20);
    }
    painter->restore();

    /*
    qDebug()<<view->palette().currentColorGroup();
    auto color = view->palette().color(view->palette().currentColorGroup(), QPalette::Highlight);
    painter->fillRect(0, 0, 20, 20, color);
    */
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
    edit->setMinimumHeight(54);

    edit->connect(edit, &IconViewEditor::textChanged, [=](){
        //edit->adjustSize();
        updateEditorGeometry(edit, option, index);
    });
    return edit;
}

void IconViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    IconViewEditor *edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;

    edit->setText(index.data(Qt::DisplayRole).toString());
    edit->setAlignment(Qt::AlignCenter);
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

void IconViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    auto edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;

    QGraphicsTextItem item;
    item.setFont(edit->currentFont());
    item.setTextWidth(50);
    item.setPlainText(edit->toPlainText());
    //qDebug()<<item.boundingRect();
    //qDebug()<<item.textWidth();
    item.adjustSize();
    //qDebug()<<item.textWidth();
    //qDebug()<<item.boundingRect();
    edit->resize(edit->width(), static_cast<int>(item.boundingRect().height()));
}

void IconViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    IconViewEditor *edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;
    auto newName = edit->toPlainText();
    if (!newName.isNull()) {
        if (newName != index.data(Qt::DisplayRole).toString()) {
            auto fileOpMgr = FileOperationManager::getInstance();
            auto renameOp = new FileRenameOperation(index.data(FileItemModel::UriRole).toString(), newName);
            fileOpMgr->startOperation(renameOp, true);
        }
    }
}

void IconViewDelegate::setIndexWidget(const QModelIndex &index, QWidget *widget) const
{
    auto view = qobject_cast<IconView*>(this->parent());
    view->setIndexWidget(index, widget);
}
