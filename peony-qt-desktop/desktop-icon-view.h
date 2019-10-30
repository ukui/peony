#ifndef DESKTOPICONVIEW_H
#define DESKTOPICONVIEW_H

#include <QListView>
#include "directory-view-plugin-iface.h"

#include <QStandardPaths>

class QLabel;

namespace Peony {

class DesktopIconView : public QListView, public DirectoryViewIface
{
    friend class DesktopIconViewDelegate;
    Q_OBJECT
public:
    enum ZoomLevel {
        Small, //icon: 36x36; grid: 56x64; hover rect: 40x56; font: system*0.8
        Normal, //icon: 48x48; grid: 64x72; hover rect = 56x64; font: system
        Large, //icon: 64x64; grid: 115x135; hover rect = 105x118; font: system*1.2
        Huge, //icon: 96x96; grid: 130x150; hover rect = 120x140; font: system*1.4
        Invalid
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
    void setDeafultZoomLevel(ZoomLevel level);
    ZoomLevel zoomLevel();
    void zoomIn();
    void zoomOut();

private:
    ZoomLevel m_zoom_level = Invalid;
};

}

#endif // DESKTOPICONVIEW_H
