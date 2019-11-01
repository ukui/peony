#include "desktop-icon-view-delegate.h"
#include "desktop-icon-view.h"

#include "icon-view-editor.h"

#include "file-operation-manager.h"
#include "file-rename-operation.h"

#include <QPushButton>
#include <QWidget>
#include <QPainter>
#include <QDebug>

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
        if (view->selectionModel()->selection().contains(index)) {
            painter->setOpacity(0.8);
        }
    } else if (opt.state.testFlag(QStyle::State_Selected)) {
        if (view->indexWidget(index)) {
            opt.text = nullptr;
        }
    }

    auto font = opt.font;
    switch (view->zoomLevel()) {
    case DesktopIconView::Small:
        font.setPointSizeF(font.pointSizeF() * 0.8);
        break;
    case DesktopIconView::Large:
        font.setPointSizeF(font.pointSizeF() * 1.2);
        break;
    case DesktopIconView::Huge:
        font.setPointSizeF(font.pointSizeF() * 1.4);
        break;
    default:
        break;
    }

    //paint text shadow
    auto opt2 = opt;
    opt2.font = font;
    opt2.icon = QIcon();
    QColor black = Qt::black;
    black.setAlpha(200);
    opt2.palette.setColor(QPalette::Text, black);
    black.setAlpha(225);
    opt2.palette.setColor(QPalette::HighlightedText, black);
    opt2.rect.moveTo(opt2.rect.topLeft() + QPoint(1, 1));
    style->drawControl(QStyle::CE_ItemViewItem, &opt2, painter, opt.widget);

    //paint item
    auto color = QColor(230, 230, 230);
    opt.palette.setColor(QPalette::Text, color);
    color.setRgb(240, 240, 240);
    opt.palette.setColor(QPalette::HighlightedText, color);
    opt.font = font;

    if (!view->indexWidget(index)) {
        painter->setClipRect(opt.rect);
        painter->save();
        if (opt.state.testFlag(QStyle::State_MouseOver) && !opt.state.testFlag(QStyle::State_Selected)) {
            QColor color = m_styled_button->palette().highlight().color();
            color.setAlpha(255*0.3);//half transparent
            painter->fillRect(opt.rect, color);
            color.setAlpha(255*0.5);
            painter->setPen(color.darker(100));
            painter->drawRect(opt.rect.adjusted(0, 0, -1, -1));
        }
        if (opt.state.testFlag(QStyle::State_Selected)) {
            QColor color = m_styled_button->palette().highlight().color();
            color.setAlpha(255*0.7);//half transparent
            painter->fillRect(opt.rect, color);
            color.setAlpha(255*0.8);
            painter->setPen(color);
            painter->drawRect(opt.rect.adjusted(0, 0, -1, -1));
        }
        painter->restore();
    }

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

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
        return QSize(60, 60);
    case DesktopIconView::Normal:
        return QSize(90, 90);
    case DesktopIconView::Large:
        return QSize(105, 118);
    case DesktopIconView::Huge:
        return QSize(120, 140);
    default:
        return QSize(90, 90);
    }
}

QWidget *DesktopIconViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto edit = new IconViewEditor(parent);
    auto font = option.font;
    auto view = qobject_cast<Peony::DesktopIconView*>(this->parent());
    switch (view->zoomLevel()) {
    case DesktopIconView::Small:
        font.setPointSizeF(font.pointSizeF() * 0.8);
        break;
    case DesktopIconView::Large:
        font.setPointSizeF(font.pointSizeF() * 1.2);
        break;
    case DesktopIconView::Huge:
        font.setPointSizeF(font.pointSizeF() * 1.4);
        break;
    default:
        break;
    }

    edit->setFont(font);

    edit->setContentsMargins(0, 0, 0, 0);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setMinimumSize(sizeHint(option, index).width(), 54);

    edit->setText(index.data(Qt::DisplayRole).toString());
    edit->setAlignment(Qt::AlignCenter);
    //NOTE: if we directly call this method, there will be
    //nothing happen. add a very short delay will ensure that
    //the edit be resized.
    QTimer::singleShot(1, [=](){
        edit->minimalAdjust();
    });

    connect(edit, &IconViewEditor::returnPressed, [=](){
        this->setModelData(edit, nullptr, index);
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
        cursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, 2);
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

    edit->resize(edit->document()->size().width(), edit->document()->size().height() + 10);
}

void DesktopIconViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    IconViewEditor *edit = qobject_cast<IconViewEditor*>(editor);
    if (!edit)
        return;
    auto newName = edit->toPlainText();
    if (!newName.isNull()) {
        if (newName != index.data(Qt::DisplayRole).toString()) {
            auto fileOpMgr = FileOperationManager::getInstance();
            auto renameOp = new FileRenameOperation(index.data(Qt::UserRole).toString(), newName);
            fileOpMgr->startOperation(renameOp, true);
        }
    }
}

DesktopIconView *DesktopIconViewDelegate::getView() const
{
    auto view = qobject_cast<Peony::DesktopIconView*>(parent());
    return view;
}
