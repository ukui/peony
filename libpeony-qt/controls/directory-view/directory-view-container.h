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

#ifndef DIRECTORYVIEWCONTAINER_H
#define DIRECTORYVIEWCONTAINER_H

#include "peony-core_global.h"
#include <QWidget>
#include <QStack>

#include "file-item-model.h"

class QVBoxLayout;

namespace Peony {

class FileItemModel;
class FileItemProxyFilterSortModel;

class DirectoryViewProxyIface;
class DirectoryViewWidget;

/*!
 * \brief The DirectoryViewContainer class
 * \details
 * This class define a set of higher level operation for the directory view,
 * such as history functions and view switch.
 * In peony-qt, tab-window and view-type-switch are reserved from peony.
 * This class is mainly to archive these higher ui logic.
 *
 * A directory view can not swith it type itself because it was created
 * from its own factory, and it aslo could and should not contain more details
 * implement above the view it self. Those should be implement by a wrapper class
 * in higher level.
 * You can interpret it as the middleware of window and directory view.
 */
class DirectoryViewContainer : public QWidget
{
    Q_OBJECT
public:
    explicit DirectoryViewContainer(QWidget *parent = nullptr);
    ~DirectoryViewContainer();

    const QString getCurrentUri();
    const QStringList getCurrentSelections();

    const QStringList getAllFileUris();

    const QStringList getBackList();
    const QStringList getForwardList();

    bool canGoBack();
    bool canGoForward();
    bool canCdUp();

    FileItemModel::ColumnType getSortType();
    Qt::SortOrder getSortOrder();

    //DirectoryViewProxyIface *getProxy() {return m_proxy;}
    DirectoryViewWidget *getView() {return m_view;}

Q_SIGNALS:
    void viewTypeChanged();
    void directoryChanged();
    void selectionChanged();
    void viewDoubleClicked(const QString &uri);
    void updateWindowLocationRequest(const QString &uri, bool addHistory, bool forceUpdate = false);

    void menuRequest(const QPoint &pos);

public Q_SLOTS:
    void goToUri(const QString &uri, bool addHistory, bool forceUpdate = false);
    void switchViewType(const QString &viewId);

    void goBack();
    void goForward();
    void cdUp();

    void refresh();
    void stopLoading();

    void tryJump(int index);
    void clearHistory() {m_back_list.clear(); m_forward_list.clear();}

    void setSortType(FileItemModel::ColumnType type);
    void setSortOrder(Qt::SortOrder order);

    void setSortFilter(int FileTypeIndex, int FileMTimeIndex, int FileSizeIndex);
    void setShowHidden(bool showHidden = false);

    void onViewDoubleClicked(const QString &uri);

protected:
    /*!
     * \brief bindNewProxy
     * \param proxy
     * \deprecated
     * This method is deprecated and useless. It should be removed.
     */
    void bindNewProxy(DirectoryViewProxyIface *proxy);

private:
    QString m_current_uri;

    DirectoryViewProxyIface *m_proxy = nullptr;
    DirectoryViewWidget *m_view = nullptr;

    QStringList m_back_list;
    QStringList m_forward_list;

    QVBoxLayout *m_layout;

    FileItemModel *m_model;
    FileItemProxyFilterSortModel *m_proxy_model;
};

}

#endif // DIRECTORYVIEWCONTAINER_H
