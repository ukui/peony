/*
 * Peony-Qt
 *
 * Copyright (C) 2020, Tianjin KYLIN Information Technology Co., Ltd.
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

#include "file-label-box.h"
#include "file-label-model.h"

#include "label-box-delegate.h"

#include <QMenu>

#include <QColorDialog>
#include <QMouseEvent>

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QMap>

#include <QStyleOptionViewItem>

#include <QApplication>
#include <QDebug>

static LabelBoxStyle *global_instance = nullptr;

static QMap<QString, QIcon> color_icons;

FileLabelBox::FileLabelBox(QWidget *parent) : QListView(parent)
{
    setItemDelegate(new LabelBoxDelegate(this));
    setStyle(LabelBoxStyle::getStyle());
    viewport()->setStyle(LabelBoxStyle::getStyle());
    viewport()->setAutoFillBackground(true);
    viewport()->setBackgroundRole(QPalette::AlternateBase);
    setModel(FileLabelModel::getGlobalModel());

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QWidget::customContextMenuRequested, this, [=](const QPoint &pos) {
        auto index = indexAt(pos);
        //bool labelRemovable = false;
        QMenu menu;
        if (index.isValid()) {
            auto item = FileLabelModel::getGlobalModel()->itemFormIndex(index);
            int id = item->id();
//            if (id > TOTAL_DEFAULT_COLOR)
//                labelRemovable = true;

            menu.addAction(tr("Rename"), [=]() {
                //FIXME: edit
                edit(index);
            });

            menu.addAction(tr("Edit Color"), [=]() {
                QColorDialog d;
                if (d.exec()) {
                    auto color = d.selectedColor();
                    FileLabelModel::getGlobalModel()->setLabelColor(item->id(), color);
                }
            });

            auto a = menu.addAction(tr("Delete"), [=]() {
                FileLabelModel::getGlobalModel()->removeLabel(id);
            });
            //a->setEnabled(labelRemovable);
        } else {
            menu.addAction(tr("Create New Label"), [=]() {
                QColorDialog dialog;
                if (dialog.exec()) {
                    auto color = dialog.selectedColor();
                    auto name = color.name();
                    FileLabelModel::getGlobalModel()->addLabel(name, color);
                }
            });
        }
        menu.exec(QCursor::pos());
    });
}

QSize FileLabelBox::sizeHint() const
{
    auto w = this->topLevelWidget()->width();
    auto size = QListView::sizeHint();
    size.setWidth(w/5);
    return size;
}

void FileLabelBox::mousePressEvent(QMouseEvent *e)
{
    QModelIndex index = indexAt(e->pos());
    //qDebug() << "mousePressEvent:"<<e->pos() <<index.isValid() <<index.data() <<e->type();
    if (!index.isValid() && e->button() == Qt::LeftButton)
    {
        this->clearSelection();
        Q_EMIT leftClickOnBlank();
    }
    else {
        QListView::mousePressEvent(e);
    }
}

void FileLabelBox::paintEvent(QPaintEvent *e)
{
    QListView::paintEvent(e);
}

//LabelBoxStyle
LabelBoxStyle *LabelBoxStyle::getStyle()
{
    if (!global_instance) {
        global_instance = new LabelBoxStyle;
    }
    return global_instance;
}

void LabelBoxStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_Frame)
        return;

    return QApplication::style()->drawPrimitive(element, option, painter, widget);
}

void LabelBoxStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_ItemViewItem) {
        if (auto tmp = qstyleoption_cast<const QStyleOptionViewItem*>(option)) {
            QStyleOptionViewItem opt = *tmp;
            auto color = qvariant_cast<QColor>(opt.index.data(Qt::DecorationRole));
            auto icon = color_icons.value(color.name());
            if (icon.isNull()) {
                QPixmap pic(QSize(12, 12));
                pic.fill(Qt::transparent);
                QPainter p(&pic);
                p.setRenderHint(QPainter::Antialiasing);
                p.setPen(QPen(Qt::gray, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                p.setBrush(color);
                p.drawEllipse(QRect(0, 0, 12, 12));
                p.end();
                icon.addPixmap(pic);
                color_icons.insert(color.name(), icon);
            }
            opt.icon = icon;
            return QApplication::style()->drawControl(element, &opt, painter, widget);
        }

    }
    return QApplication::style()->drawControl(element, option, painter, widget);
}

QSize LabelBoxStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    if (type == CT_ItemViewItem) {
        QSize size = QApplication::style()->sizeFromContents(type, option, size, widget);
        size += QSize(0, 8);
        return size;
    }
    return QApplication::style()->sizeFromContents(type, option, size, widget);
}
