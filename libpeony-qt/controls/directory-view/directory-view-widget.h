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

#ifndef DIRECTORYVIEWWIDGET_H
#define DIRECTORYVIEWWIDGET_H

#include <QWidget>

namespace Peony {

class FileItemModel;
class FileItemProxyFilterSortModel;
class FileItemModel;
class FileItemProxyFilterSortModel;

/*!
 * \brief The DirectoryViewWidget class
 *
 * \details
 * This class is an instantiable interface.
 *
 * DirectoryViewWidget define a set of signals and slots for extensionable directory view
 * implement. If you want to custom a directory view, you should derive this class,
 * override it slots, and emit the signals at the appropriate time.
 * \see IconView2, ListView2
 *
 * DirectoryViewWidget is a return value of DirectoryViewPluginIface2::create().
 * If you want to make a directory view plugin, you should also create a libraries project and
 * derive both DirectoryViewPluginIface2 and DirectoryViewWidget.
 */
class DirectoryViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DirectoryViewWidget(QWidget *parent = nullptr);
    virtual ~DirectoryViewWidget() {}

    const virtual QString viewId() {return tr("Directory View");}

    //location
    const virtual QString getDirectoryUri() {return nullptr;}

    //selections
    const virtual QStringList getSelections() {return QStringList();}

    //children
    const virtual QStringList getAllFileUris() {return QStringList();}

    virtual int getSortType() {return 0;}
    virtual Qt::SortOrder getSortOrder() {return Qt::AscendingOrder;}

Q_SIGNALS:
    //loaction
    //FIXME: support open in new TAB?
    void viewDoubleClicked(const QString &uri);
    void viewDirectoryChanged();
    void viewSelectionChanged();
    void sortTypeChanged(int type);
    void sortOrderChanged(Qt::SortOrder order);

    //menu
    void menuRequest(const QPoint &pos);

    //window
    void updateWindowLocationRequest(const QString &uri);

public Q_SLOTS:
    virtual void bindModel(FileItemModel *model, FileItemProxyFilterSortModel *proxyModel) {}

    //location
    //virtual void open(const QStringList &uris, bool newWindow) {}
    virtual void setDirectoryUri(const QString &uri) {}
    virtual void beginLocationChange() {}
    virtual void stopLocationChange() {}

    virtual void closeDirectoryView() {}

    //selections
    virtual void setSelections(const QStringList &uris) {}
    virtual void invertSelections() {}
    virtual void scrollToSelection(const QString &uri) {}

    //clipboard
    //cut items should be drawn differently.
    virtual void setCutFiles(const QStringList &uris) {}

    virtual void setSortType(int sortType) {}
    virtual void setSortOrder(int sortOrder) {}

    virtual void editUri(const QString &uri) {}
    virtual void editUris(const QStringList uris) {}

    virtual void repaintView() {}
};

}

#endif // DIRECTORYVIEWWIDGET_H
