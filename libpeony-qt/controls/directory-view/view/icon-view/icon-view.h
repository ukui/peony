#ifndef ICONVIEW_H
#define ICONVIEW_H

#include "peony-core_global.h"
#include "directory-view-plugin-iface.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"
#include <QListView>

namespace Peony {

namespace DirectoryView {

class PEONYCORESHARED_EXPORT IconView : public QListView, public DirectoryViewIface
{
    Q_OBJECT
public:

    explicit IconView(QWidget *parent = nullptr);
    ~IconView() override;

    DirectoryViewProxyIface *getProxy() override;

    //location
    const QString getDirectoryUri() override;

    //selections
    QStringList getSelections() override;

    //zoom
    bool canZoomIn() override;
    bool canZoomOut() override;

public Q_SLOTS:
    //location
    void open(const QStringList &uris, bool newWindow) override;
    void setDirectoryUri(const QString &uri) override;
    void beginLocationChange() override;
    void stopLocationChange() override;
    void close() override;

    //selections
    void setSelections(const QStringList &uris) override;
    void invertSelections() override;
    void scrollToSelection(const QString &uri) override;

    //clipboard
    void setCutFiles(const QStringList &uris) override;

    //zoom
    void zoomIn() override;
    void zoomOut() override;

protected:
    void changeZoomLevel();

private:
    DirectoryViewProxyIface *m_proxy = nullptr;

    FileItemModel *m_model = nullptr;
    FileItemProxyFilterSortModel *m_sort_filter_proxy_model = nullptr;

    ZoomLevel m_zoom_level = Normal;
    QString m_current_uri = nullptr;
};

}

}

#endif // ICONVIEW_H
