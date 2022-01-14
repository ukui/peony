/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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

#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QTreeView>
#include "directory-view-plugin-iface.h"
#include "peony-core_global.h"

#include "directory-view-widget.h"

#include <QTimer>

namespace Peony {

class FileItemModel;
class FileItemProxyFilterSortModel;

namespace DirectoryView {

/*!
 * \brief The ListView class
 * \todo
 * improve extend selection actions.
 */
class PEONYCORESHARED_EXPORT ListView : public QTreeView, public DirectoryViewIface
{
    friend class ListView2;
    Q_OBJECT
public:
    explicit ListView(QWidget *parent = nullptr);

    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;

    const QString viewId() override {
        return tr("List View");
    }

    bool isDragging();

    void bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel) override;
    void setProxy(DirectoryViewProxyIface *proxy) override;

    /*!
     * \brief getProxy
     * \return
     * \deprecated
     */
    DirectoryViewProxyIface *getProxy() override;

    //location
    const QString getDirectoryUri() override;

    //selections
    const QStringList getSelections() override;

    //children
    const QStringList getAllFileUris() override;

    QRect visualRect(const QModelIndex &index) const override;

    bool getDelegateEditFlag();

Q_SIGNALS:
    void zoomLevelChangedRequest(bool zoomIn);

public Q_SLOTS:
    //location
    void open(const QStringList &uris, bool newWindow) override;
    void setDirectoryUri(const QString &uri) override;
    void beginLocationChange() override;
    void stopLocationChange() override;
    void closeView() override;

    //selections
    void setSelections(const QStringList &uris) override;
    void invertSelections() override;
    void scrollToSelection(const QString &uri) override;

    //clipboard
    void setCutFiles(const QStringList &uris) override;

    int getSortType() override;
    void setSortType(int sortType) override;

    int getSortOrder() override;
    void setSortOrder(int sortOrder) override;

    void editUri(const QString &uri) override;
    void editUris(const QStringList uris) override;

    void keyboardSearch(const QString &key) override;

    void resort();
    void reportViewDirectoryChanged();
    void adjustColumnsSize();

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

    void resizeEvent(QResizeEvent *e) override;

    void updateGeometries() override;
    void reUpdateScrollBar();

    void wheelEvent (QWheelEvent *e) override;

    void focusInEvent(QFocusEvent *e) override;

    void startDrag(Qt::DropActions flags) override;

private Q_SLOTS:
    void slotRename();

private:
    FileItemModel *m_model = nullptr;
    FileItemProxyFilterSortModel *m_proxy_model = nullptr;

    QTimer* m_renameTimer;
    bool  m_editValid;
    bool  m_ctrl_key_pressed;
    bool  m_delegate_editing = false;

    QRubberBand *m_rubberBand;
    QPoint m_lastPressedLogicPoint;
    QRect m_logicRect;
    bool m_isLeftButtonPressed = false;

    QModelIndex m_last_index;

    DirectoryViewProxyIface *m_proxy = nullptr;

    QString m_current_uri;

    QSize m_last_size;

    const int BOTTOM_STATUS_MARGIN = 200;
};

//ListView2
class ListView2 : public DirectoryViewWidget
{
    Q_OBJECT
    //internal plugin
public:
    explicit ListView2(QWidget *parent = nullptr);
    ~ListView2();

    const QString viewId() {
        return "List View";
    }

    //location
    const QString getDirectoryUri() {
        return m_view->getDirectoryUri();
    }

    //selections
    const QStringList getSelections() {
        return m_view->getSelections();
    }

    //children
    const QStringList getAllFileUris() {
        return m_view->getAllFileUris();
    }

    int getSortType() {
        return m_view->getSortType();
    }
    Qt::SortOrder getSortOrder() {
        return Qt::SortOrder(m_view->getSortOrder());
    }

    int currentZoomLevel() {
        return m_zoom_level;
    }
    int minimumZoomLevel() {
        return 0;
    }
    int maximumZoomLevel() {
        return 20;
    }

    bool supportZoom() {
        return true;
    }

public Q_SLOTS:
    void bindModel(FileItemModel *model, FileItemProxyFilterSortModel *proxyModel);

    //location
    //void open(const QStringList &uris, bool newWindow);
    void setDirectoryUri(const QString &uri) {
        m_need_resize_header = true;
        m_view->setDirectoryUri(uri);
    }
    void beginLocationChange() {
        m_view->beginLocationChange();
    }
    void stopLocationChange() {
        m_view->stopLocationChange();
    }

    void closeDirectoryView() {
        m_view->closeView();
    }

    //selections
    void setSelections(const QStringList &uris) {
        m_view->setSelections(uris);
    }
    void invertSelections() {
        m_view->invertSelections();
    }
    void scrollToSelection(const QString &uri) {
        m_view->scrollToSelection(uri);
    }

    //clipboard
    //cut items should be drawn differently.
    void setCutFiles(const QStringList &uris) {
        m_view->setCutFiles(uris);
    }

    void setSortType(int sortType) {
        m_view->setSortType(sortType);
    }
    void setSortOrder(int sortOrder) {
        m_view->setSortOrder(sortOrder);
    }

    void editUri(const QString &uri) {
        m_view->editUri(uri);
    }
    void editUris(const QStringList uris) {
        m_view->editUris(uris);
    }

    void setCurrentZoomLevel(int zoomLevel);

    void clearIndexWidget();

    void repaintView();

private:
    ListView *m_view = nullptr;
    FileItemModel *m_model = nullptr;
    FileItemProxyFilterSortModel *m_proxy_model = nullptr;

    int m_zoom_level = 20;
    bool m_need_resize_header;
};

}

}

#endif // LISTVIEW_H
