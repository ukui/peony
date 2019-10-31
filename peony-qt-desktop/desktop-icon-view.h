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
        Small, //icon: 24x24; grid: 64x64; hover rect: 60x60; font: system*0.8
        Normal, //icon: 48x48; grid: 96x96; hover rect = 90x90; font: system
        Large, //icon: 64x64; grid: 115x135; hover rect = 105x118; font: system*1.2
        Huge, //icon: 96x96; grid: 140x170; hover rect = 120x140; font: system*1.4
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

    QRect visualRect(const QModelIndex &index) const {
        auto rect = QListView::visualRect(index);
        QPoint p(10, 5);
        switch (m_zoom_level) {
        case Small:
            p *= 0.8;
            break;
        case Large:
            p *= 1.2;
            break;
        case Huge:
            p *= 1.4;
            break;
        default:
            break;
        }
        rect.moveTo(rect.topLeft() + p);
        return rect;
    }

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
