#include "desktop-icon-view.h"

using namespace Peony;

DesktopIconView::DesktopIconView(QWidget *parent) : QListView(parent)
{

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

}

void DesktopIconView::zoomOut()
{

}

void DesktopIconView::setDeafultZoomLevel(ZoomLevel level)
{

}

DesktopIconView::ZoomLevel DesktopIconView::zoomLevel()
{
    //FIXME:
    return Normal;
}
