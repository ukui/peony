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

#ifndef DIRECTORYCHANGEDTHREAD_H
#define DIRECTORYCHANGEDTHREAD_H
#include <QThread>
#include <QObject>
#include <QSettings>
#include "src/Interface/ukuimenuinterface.h"
#include <QStandardItemModel>
#include <src/UtilityFunction/fulllistview.h>

class DirectoryChangedThread: public QThread
{
    Q_OBJECT
public:
    DirectoryChangedThread();
    ~DirectoryChangedThread();
    void run();

private:
    UkuiMenuInterface *m_ukuiMenuInterface=nullptr;
    QString m_path;
    QSettings* setting=nullptr;
    FullListView* m_listView=nullptr;
    QStandardItemModel* listmodel=nullptr;

    int index=0;

public Q_SLOTS:
    /**
     * @brief Receive the changed directory
     * @param arg: Desktop file directory
     */
    void recvDirectoryPath(QString arg);

Q_SIGNALS:
    /**
     * @brief Request update signal
     */
    void requestUpdateSignal(QString desktopfp);
    void deleteAppSignal();

};

#endif // DIRECTORYCHANGEDTHREAD_H
