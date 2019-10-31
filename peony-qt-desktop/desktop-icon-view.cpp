#include "desktop-icon-view.h"

#include "icon-view-style.h"
#include "desktop-icon-view-delegate.h"

using namespace Peony;

DesktopIconView::DesktopIconView(QWidget *parent) : QListView(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setAttribute(Qt::WA_TranslucentBackground);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //fix rubberband style.
    setStyle(DirectoryView::IconViewStyle::getStyle());

    setItemDelegate(new DesktopIconViewDelegate(this));

    setSelectionMode(QListView::ExtendedSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setViewMode(QListView::IconMode);
    setMovement(QListView::Snap);
    setFlow(QListView::TopToBottom);
    setResizeMode(QListView::Adjust);
    setWordWrap(true);

    setDragDropMode(QListView::DragDrop);

    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QListView::ExtendedSelection);

    auto zoomLevel = this->zoomLevel();
    setDeafultZoomLevel(zoomLevel);
}

DesktopIconView::~DesktopIconView()
{

}

const QStringList DesktopIconView::getSelections()
{

}

const QStringList DesktopIconView::getAllFileUris()
{

}

void DesktopIconView::setSelections(const QStringList &uris)
{

}

void DesktopIconView::invertSelections()
{

}

void DesktopIconView::scrollToSelection(const QString &uri)
{

}

int DesktopIconView::getSortType()
{
    //FIXME:
    return 0;
}

void DesktopIconView::setSortType(int sortType)
{

}

int DesktopIconView::getSortOrder()
{
    //FIXME:
    return Qt::AscendingOrder;
}

void DesktopIconView::setSortOrder(int sortOrder)
{

}

void DesktopIconView::editUri(const QString &uri)
{

}

void DesktopIconView::editUris(const QStringList uris)
{

}

void DesktopIconView::setCutFiles(const QStringList &uris)
{

}

void DesktopIconView::closeView()
{
    deleteLater();
}

void DesktopIconView::zoomIn()
{
    switch (m_zoom_level) {
    case Huge:
        setDeafultZoomLevel(Large);
        break;
    case Large:
        setDeafultZoomLevel(Normal);
        break;
    case Normal:
        setDeafultZoomLevel(Small);
        break;
    default:
        break;
    }
}

void DesktopIconView::zoomOut()
{
    switch (m_zoom_level) {
    case Small:
        setDeafultZoomLevel(Normal);
        break;
    case Normal:
        setDeafultZoomLevel(Large);
        break;
    case Large:
        setDeafultZoomLevel(Huge);
        break;
    default:
        break;
    }
}

/*
Small, //icon: 32x32; grid: 56x64; hover rect: 40x56; font: system*0.8
Normal, //icon: 48x48; grid: 64x72; hover rect = 56x64; font: system
Large, //icon: 64x64; grid: 115x135; hover rect = 105x118; font: system*1.2
Huge //icon: 96x96; grid: 130x150; hover rect = 120x140; font: system*1.4
*/
void DesktopIconView::setDeafultZoomLevel(ZoomLevel level)
{
    m_zoom_level = level;
    switch (level) {
    case Small:
        setIconSize(QSize(24, 24));
        setGridSize(QSize(64, 64));
        break;
    case Large:
        setIconSize(QSize(64, 64));
        setGridSize(QSize(115, 135));
        break;
    case Huge:
        setIconSize(QSize(96, 96));
        setGridSize(QSize(140, 170));
        break;
    default:
        //Normal
        setIconSize(QSize(48, 48));
        setGridSize(QSize(96, 96));
        break;
    }
}

DesktopIconView::ZoomLevel DesktopIconView::zoomLevel()
{
    //FIXME:
    return m_zoom_level != Invalid? m_zoom_level: Normal;
}
