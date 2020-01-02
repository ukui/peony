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

#ifndef STANDARDVIEWPROXY_H
#define STANDARDVIEWPROXY_H

#include <QStack>
#include "directory-view-plugin-iface.h"

namespace Peony {

namespace DirectoryView {

/*!
 * \brief The StandardViewProxy class
 * \details
 * this class implement a part of methods of DirectoryViewProxyIface.
 * This class is the peony-qt's icon view and list view proxy interface's
 * implement.
 * \note
 * If you want to write a directory view plugin for peony-qt,
 * you can use this proxy class as your plugin-view's proxy. It
 * is recommend, otherwise you should spend a while to re-write a new
 * proxy by yourself.
 *
 * \deprecated
 * This class is deprecated and useless. It should be removed from codes.
 */
class StandardViewProxy : public DirectoryViewProxyIface
{
    Q_OBJECT
public:
    explicit StandardViewProxy(QObject *parent = nullptr);
    explicit StandardViewProxy(DirectoryViewIface *view, QObject *parent = nullptr);
    ~StandardViewProxy() override;

    void switchView(DirectoryViewIface *view) override;

    DirectoryViewIface *getView() override {return m_view;}

    //location
    const QString getDirectoryUri() override;

    //selections
    const QStringList getSelections() override;

    //children
    const QStringList getAllFileUris() override;

public Q_SLOTS:
    //location
    void open(const QStringList &uris, bool newWindow) override;
    void setDirectoryUri(const QString &uri) override;
    void beginLocationChange() override;
    void stopLocationChange() override;

    void closeProxy() override;

    //selections
    void setSelections(const QStringList &uris) override;
    void invertSelections() override;
    void scrollToSelection(const QString &uri) override;

    //clipboard
    void setCutFiles(const QStringList &uris) override;

    int getSortType() override {return m_view->getSortType();}
    void setSortType(int sortType) override {m_view->setSortType(sortType);}

    int getSortOrder() override {return m_view->getSortOrder();}
    void setSortOrder(int sortOrder) override {m_view->setSortOrder(sortOrder);}

    void editUri(const QString &uri) override;
    void editUris(const QStringList uris) override;
private:
    DirectoryViewIface *m_view = nullptr;
    QString m_viewId;
};

}

}

#endif // STANDARDVIEWPROXY_H
