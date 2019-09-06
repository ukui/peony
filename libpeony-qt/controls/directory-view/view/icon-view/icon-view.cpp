#include "icon-view.h"
#include "standard-view-proxy.h"
#include "file-item.h"

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconView::IconView(QWidget *parent) : QListView(parent)
{
    setSelectionMode(QListView::ExtendedSelection);
    setEditTriggers(QListView::NoEditTriggers);
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setMovement(QListView::Snap);
    setWordWrap(true);

    m_model = new FileItemModel(this);
    m_sort_filter_proxy_model = new FileItemProxyFilterSortModel(m_model);
    m_sort_filter_proxy_model->setSourceModel(m_model);

    setModel(m_sort_filter_proxy_model);

    m_proxy = new StandardViewProxy(this, this);

    switch (m_zoom_level) {
    case Normal: {
        setGridSize(QSize(64, 72));
        setIconSize(QSize(48, 48));
        break;
    }
    default:
        break;
    }

    connect(m_model, &FileItemModel::findChildrenFinished,
            m_proxy, &DirectoryViewProxyIface::viewDirectoryChanged);
    connect(this, &IconView::selectionChanged, [=](){
        Q_EMIT m_proxy->viewSelectionChanged();
    });
    connect(this, &IconView::doubleClicked, [=](const QModelIndex &index){
        qDebug()<<"double click"<<index.data(FileItemModel::UriRole);
        Q_EMIT m_proxy->viewDoubleClicked(index.data(FileItemModel::UriRole).toString());
    });
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
    clearSelection();
    for (auto uri: uris) {
        const QModelIndex index = m_sort_filter_proxy_model->indexFromUri(uri);
        if (index.isValid()) {
            selectionModel()->select(index, QItemSelectionModel::Select);
        }
    }
}

QStringList IconView::getSelections()
{
    QStringList uris;
    QModelIndexList selections = selectedIndexes();
    for (auto index : selections) {
        auto item = m_sort_filter_proxy_model->itemFromIndex(index);
        uris<<item->uri();
    }
    return uris;
}

void IconView::invertSelections()
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    const QItemSelection currentSelection = selectionModel->selection();
    this->selectAll();
    selectionModel->select(currentSelection, QItemSelectionModel::Deselect);
}

void IconView::scrollToSelection(const QString &uri)
{
    auto index = m_sort_filter_proxy_model->indexFromUri(uri);
    scrollTo(index);
}

//clipboard
void IconView::setCutFiles(const QStringList &uris)
{
    //let delegate and model know how to deal with cut files.
}

//location
//FIXME: implement location functions.
void IconView::setDirectoryUri(const QString &uri)
{
    m_current_uri = uri;
}

const QString IconView::getDirectoryUri()
{
    return m_current_uri;
}

void IconView::beginLocationChange()
{
    m_model->setRootUri(m_current_uri);
}

void IconView::stopLocationChange()
{
    m_model->cancelFindChildren();
}

//other
void IconView::open(const QStringList &uris, bool newWindow)
{

}

void IconView::close()
{

}
