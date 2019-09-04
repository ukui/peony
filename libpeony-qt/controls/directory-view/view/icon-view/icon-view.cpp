#include "icon-view.h"
#include "standard-view-proxy.h"

using namespace Peony;
using namespace Peony::DirectoryView;

IconView::IconView(QWidget *parent) : QListView(parent)
{
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Snap);
    setWordWrap(true);

    m_proxy = new StandardViewProxy(this, parent);

    switch (m_zoom_level) {
    case Normal: {
        setGridSize(QSize(64, 72));
        setIconSize(QSize(48, 48));
        break;
    }
    default:
        break;
    }
}

IconView::~IconView()
{

}

DirectoryViewProxyIface *IconView::getProxy()
{
    return m_proxy;
}

//zoom
bool IconView::canZoomIn()
{
    return m_zoom_level != Small;
}

void IconView::zoomIn()
{
    if (!canZoomIn())
        return;

    m_zoom_level = ZoomLevel(m_zoom_level - 1);
}

bool IconView::canZoomOut()
{
    return m_zoom_level != Huge;
}

void IconView::zoomOut()
{
    if (!canZoomOut())
        return;

    m_zoom_level = ZoomLevel(m_zoom_level + 1);
    changeZoomLevel();
}

void IconView::changeZoomLevel()
{
    switch (m_zoom_level) {
    case Small: {
        setIconSize(QSize(36, 36));
        setGridSize(QSize(48, 64));
        break;
    }
    case Normal: {
        setIconSize(QSize(48, 48));
        setGridSize(QSize(64, 80));
        break;
    }
    case Big: {
        setIconSize(QSize(64, 64));
        setGridSize(QSize(80, 102));
        break;
    }
    case Huge: {
        setIconSize(QSize(96, 96));
        setGridSize(QSize(102, 144));
        break;
    }
    default:
        m_zoom_level = Normal;
        changeZoomLevel();
        break;
    }
}

//selection
//FIXME: implement the selection functions.
void IconView::setSelections(const QStringList &uris)
{

}

QStringList IconView::getSelections()
{
    return QStringList();
}

void IconView::invertSelections()
{

}

void IconView::scrollToSelection(const QString &uri)
{

}

//location
//FIXME: implement location functions.
void IconView::setDirectoryUri(const QString &uri)
{

}

const QString IconView::getDirectoryUri()
{
    return nullptr;
}

void IconView::beginLocationChange()
{

}

void IconView::stopLocationChange()
{

}

//other
void IconView::open(const QStringList &uris, bool newWindow)
{

}

void IconView::close()
{

}
