/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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

#ifndef DESKTOPICONVIEW_H
#define DESKTOPICONVIEW_H

#include <QListView>
#include "directory-view-plugin-iface.h"

#include <QStandardPaths>
#include <QTimer>

class QLabel;

namespace Peony {

class DesktopItemModel;
class DesktopItemProxyModel;

class DesktopIconView : public QListView, public DirectoryViewIface
{
    friend class DesktopIndexWidget;
    friend class DesktopIconViewDelegate;
    Q_OBJECT
public:
    enum ZoomLevel {
        Invalid,
        Small, //icon: 24x24; grid: 64x64; hover rect: 60x60; font: system*0.8
        Normal, //icon: 48x48; grid: 96x96; hover rect = 90x90; font: system
        Large, //icon: 64x64; grid: 115x135; hover rect = 105x118; font: system*1.2
        Huge //icon: 96x96; grid: 140x170; hover rect = 120x140; font: system*1.4

    };
    Q_ENUM(ZoomLevel)

    explicit DesktopIconView(QWidget *parent = nullptr);
    ~DesktopIconView();

    void bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel) {Q_UNUSED(sourceModel) Q_UNUSED(proxyModel)}
    void setProxy(DirectoryViewProxyIface *proxy) {Q_UNUSED(proxy)}

    const QString viewId() {return tr("Desktop Icon View");}

    //location
    const QString getDirectoryUri() {return "file://" + QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);}

    //selections
    const QStringList getSelections();

    //children
    const QStringList getAllFileUris();

    int getSortType();
    int getSortOrder();

    QRect visualRect(const QModelIndex &index) const;
    const QFont getViewItemFont(QStyleOptionViewItem *item);

Q_SIGNALS:
    void zoomLevelChanged(ZoomLevel level);

public Q_SLOTS:
    //location
    void open(const QStringList &uris, bool newWindow) {}
    void setDirectoryUri(const QString &uri) {}
    void beginLocationChange() {}
    void stopLocationChange() {}
    void closeView();

    //selections
    void setSelections(const QStringList &uris);
    void invertSelections();
    void scrollToSelection(const QString &uri);

    //clipboard
    void setCutFiles(const QStringList &uris);

    DirectoryViewProxyIface *getProxy() {return nullptr;}

    void setSortType(int sortType);

    void setSortOrder(int sortOrder);

    //edit
    void editUri(const QString &uri);

    void editUris(const QStringList uris);

    //zoom
    void setDefaultZoomLevel(ZoomLevel level);
    ZoomLevel zoomLevel() const;
    void zoomIn();
    void zoomOut();

    void clearAllIndexWidgets();

    void refresh();

    void saveAllItemPosistionInfos();
    void saveItemPositionInfo(const QString &uri);
    void resetAllItemPositionInfos();
    void resetItemPosistionInfo(const QString &uri);

    void updateItemPosistions(const QString &uri = nullptr);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);

    void wheelEvent(QWheelEvent *e);

    void resetEditTriggerTimer();

private:
    ZoomLevel m_zoom_level = Invalid;

    QModelIndex m_last_index;
    QTimer m_edit_trigger_timer;

    DesktopItemModel *m_model;
    DesktopItemProxyModel *m_proxy_model;
};

}

#endif // DESKTOPICONVIEW_H
