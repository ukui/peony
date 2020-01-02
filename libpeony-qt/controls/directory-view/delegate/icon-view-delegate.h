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

#ifndef ICONVIEWDELEGATE_H
#define ICONVIEWDELEGATE_H

#include <QStyledItemDelegate>

class QPushButton;

namespace Peony {

namespace DirectoryView {

class IconView;
class IconViewIndexWidget;

class IconViewDelegate : public QStyledItemDelegate
{
    friend class IconViewIndexWidget;

    Q_OBJECT
public:
    explicit IconViewDelegate(QObject *parent = nullptr);
    ~IconViewDelegate() override;
    IconView *getView() const;

    const QBrush selectedBrush() const;

public Q_SLOTS:
    void setCutFiles(const QModelIndexList &indexes);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    //edit
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    void setIndexWidget(const QModelIndex &index, QWidget *widget) const;

private:
    QModelIndexList m_cut_indexes;

    QModelIndex m_index_widget_index;
    QWidget *m_index_widget;

    QPushButton *m_styled_button;
};

}

}

#endif // ICONVIEWDELEGATE_H
