/*
 * Peony-Qt
 *
 * Copyright (C) 2021, KylinSoft Co., Ltd.
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
 */

#ifndef STUDYLISTVIEW_H
#define STUDYLISTVIEW_H

#include <QObject>
#include <QListView>
#include <QWidget>

#include "../../tablet/data/tablet-app-manager.h"
#include "progress-item-delegate.h"

class QMouseEvent;
class QStandardItemModel;
namespace Peony {

class StudyListView : public QListView
{
    Q_OBJECT
public:
    explicit StudyListView(QWidget *parent = nullptr);
    ~StudyListView(){};

    void setData(QList<TabletAppEntity*> tabletAppList);
private:
    QStandardItemModel *m_animation = nullptr;
    //QStandardItemModel* listmodel=nullptr;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    int horizontalOffset() const override;

private:
    QStandardItemModel* m_listmodel=nullptr;
    FullItemDelegate* m_delegate = nullptr;
    bool m_iconClicked = false;
    QPoint m_pressedPos;
};

}

#endif // STUDYLISTVIEW_H
