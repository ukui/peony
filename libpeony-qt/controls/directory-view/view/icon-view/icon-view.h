#ifndef ICONVIEW_H
#define ICONVIEW_H

#include "peony-core_global.h"
#include "directory-view-plugin-iface.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"
#include <QListView>
#include <QTimer>

namespace Peony {

namespace DirectoryView {

class IconViewDelegate;

class PEONYCORESHARED_EXPORT IconView : public QListView, public DirectoryViewIface
{
    friend class IconViewDelegate;
    Q_OBJECT
public:

    /*!
     * \brief IconView
     * \param parent
     * \deprecated
     * We should not create a proxy in a view itself. Proxy should be created by tabpage
     * or FMWindow.
     */
    explicit IconView(QWidget *parent = nullptr);
    explicit IconView(DirectoryViewProxyIface *proxy, QWidget *parent = nullptr);
    ~IconView() override;

    const QString viewId() override {return tr("Icon View");}

    void bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel) override;
    void setProxy(DirectoryViewProxyIface *proxy) override;

    /*!
     * \brief setUsePeonyQtDirectoryMenu
     * \param use
     * \deprecated
     */
    void setUsePeonyQtDirectoryMenu(bool use) {m_use_peony_qt_directory_menu = use;}

    DirectoryViewProxyIface *getProxy() override;

    //location
    const QString getDirectoryUri() override;

    //selections
    const QStringList getSelections() override;

    //children
    const QStringList getAllFileUris() override;

    QRect visualRect(const QModelIndex &index) const override;

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

protected:
    void changeZoomLevel();
    void resetEditTriggerTimer();
    void connectDefaultMenuAction();

    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

    void wheelEvent(QWheelEvent *e) override;

protected:
    void rebindProxy();

private:
    QTimer m_edit_trigger_timer;
    QTimer m_repaint_timer;
    QModelIndex m_last_index;

    DirectoryViewProxyIface *m_proxy = nullptr;

    FileItemModel *m_model = nullptr;
    FileItemProxyFilterSortModel *m_sort_filter_proxy_model = nullptr;

    QString m_current_uri = nullptr;

    /*!
     * \brief m_use_peony_qt_directory_menu
     * \deprecated
     */
    bool m_use_peony_qt_directory_menu = false;
};

}

}

#endif // ICONVIEW_H
