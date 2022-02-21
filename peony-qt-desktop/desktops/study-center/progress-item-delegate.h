/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef FULLITEMDELEGATE_H
#define FULLITEMDELEGATE_H
#include <QStyledItemDelegate>
//#include "src/Style/style.h"
#include <QPainter>
#include <QSettings>
#include <QDir>
#include <QToolTip>
#include <QHelpEvent>
#include "common.h"
//#include "src/Interface/ukuimenuinterface.h"
//#include "src/Style/style.h"
namespace Peony {

class FullItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FullItemDelegate(QObject* parent,int module);
    ~FullItemDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    int module=0;
    QIcon ensureLoadIcon(const QString &iconName) const;
//    int m_iProvcessValue=0;
    //QSettings* setting=nullptr;
    //UkuiMenuInterface* pUkuiMenuInterface=nullptr;
   // QProgressBar* m_progressBar=nullptr;

protected:
//    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index);
};

}


#endif // FULLITEMDELEGATE_H
