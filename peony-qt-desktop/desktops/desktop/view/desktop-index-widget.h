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

#ifndef DESKTOPINDEXWIDGET_H
#define DESKTOPINDEXWIDGET_H

#include <QWidget>
#include <QStyleOptionViewItem>
#include <QModelIndex>

class QTextEdit;

namespace Peony {

class DesktopIconViewDelegate;

class DesktopIndexWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DesktopIndexWidget(DesktopIconViewDelegate *delegate,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index,
                                QWidget *parent = nullptr);

    ~DesktopIndexWidget();

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void updateItem();

private:
    QStyleOptionViewItem m_option;
    QModelIndex m_index;
    const DesktopIconViewDelegate *m_delegate;
    QFont m_current_font;

    QRect m_text_rect;

    bool b_elide_text = false;
    const int ELIDE_TEXT_LENGTH = 40;
};

}

#endif // DESKTOPINDEXWIDGET_H
