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

#include <QTextEdit>

using namespace Peony;
using namespace Peony::DirectoryView;

IconViewDelegate::IconViewDelegate(QObject *parent) : QStyledItemDelegate (parent)
{

}

QSize IconViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    qDebug()<<option;
    qDebug()<<option.font;
    qDebug()<<option.icon;
    qDebug()<<option.text;
    qDebug()<<option.widget;
    qDebug()<<option.decorationSize;
    qDebug()<<QStyledItemDelegate::sizeHint(option, index);
    return QSize(105, 118);

    return QStyledItemDelegate::sizeHint(option, index);
}

void IconViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //default painter
    QStyledItemDelegate::paint(painter, option, index);

    //get file info from index
    auto view = qobject_cast<IconView*>(this->parent());

    auto model = static_cast<FileItemProxyFilterSortModel*>(view->model());
    auto item = model->itemFromIndex(index);
    //NOTE: item might be deleted when painting, because we might start a
    //location change during the painting.
    if (!item) {
        return;
    }
    auto info = item->info();
    auto rect = view->visualRect(index);
    qDebug()<<option.decorationPosition;
    qDebug()<<option.decorationAlignment;
    qDebug()<<option.displayAlignment;

    //paint symbolic link emblems
    if (info->isSymbolLink()) {
        QIcon icon = QIcon::fromTheme("emblem-symbolic-link");
        qDebug()<<info->symbolicIconName();
        icon.paint(painter, rect.x() + rect.width() - 20, rect.y(), 20, 20, Qt::AlignCenter);
    }

    //paint access emblems
    if (!info->canRead()) {
        QIcon icon = QIcon::fromTheme("emblem-unreadable");
        icon.paint(painter, rect.x(), rect.y(), 20, 20);
    } else if (!info->canWrite() && !info->canExecute()){
        QIcon icon = QIcon::fromTheme("emblem-readonly");
        icon.paint(painter, rect.x(), rect.y(), 20, 20);
    }

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
    return new QTextEdit(parent);
}

void IconViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QTextEdit *edit = qobject_cast<QTextEdit*>(editor);
    edit->setText(index.data(Qt::DisplayRole).toString());
}

void IconViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void IconViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QTextEdit *edit = qobject_cast<QTextEdit*>(editor);
    auto newName = edit->toPlainText();
    if (!newName.isNull()) {
        if (newName != index.data(Qt::DisplayRole).toString()) {
            auto fileOpMgr = FileOperationManager::getInstance();
            auto renameOp = new FileRenameOperation(index.data(FileItemModel::UriRole).toString(), newName);
            fileOpMgr->startOperation(renameOp, true);
        }
    }
}
