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

#include <QMenu>

#include <QColorDialog>

#include <QApplication>
#include <QDebug>

static LabelBoxStyle *global_instance = nullptr;

FileLabelBox::FileLabelBox(QWidget *parent) : QListView(parent)
{
    setStyle(LabelBoxStyle::getStyle());
    viewport()->setStyle(LabelBoxStyle::getStyle());
    setModel(FileLabelModel::getGlobalModel());

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QWidget::customContextMenuRequested, this, [=](const QPoint &pos){
        auto index = indexAt(pos);
        bool labelRemovable = false;
        QMenu menu;
        if (index.isValid()) {
            auto item = FileLabelModel::getGlobalModel()->itemFormIndex(index);
            int id = item->id();
            if (id > 7)
                labelRemovable = true;

            menu.addAction(tr("Rename"), [=](){
                //FIXME: edit
                edit(index);
            });

            menu.addAction(tr("Edit Color"), [=](){
                QColorDialog d;
                if (d.exec()) {
                    auto color = d.selectedColor();
                    item->setColor(color);
                }
            });

            auto a = menu.addAction(tr("Delete"), [=](){
                FileLabelModel::getGlobalModel()->removeLabel(id);
            });
            a->setEnabled(labelRemovable);
        } else {
            menu.addAction(tr("Create New Label"), [=](){
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
    return QApplication::style()->drawPrimitive(element, option, painter, widget);
}
