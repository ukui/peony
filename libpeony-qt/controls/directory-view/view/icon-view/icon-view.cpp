#include "icon-view.h"
#include "standard-view-proxy.h"
#include "file-item.h"

#include "icon-view-delegate.h"
#include "icon-view-style.h"

#include <QMouseEvent>

#include <QDragEnterEvent>
#include <QMimeData>
#include <QDragMoveEvent>
#include <QDropEvent>

#include <QPainter>
#include <QPaintEvent>

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

IconView::IconView(QWidget *parent) : QListView(parent)
{
    IconViewStyle *style = new IconViewStyle();
    setStyle(style);

    IconViewDelegate *delegate = new IconViewDelegate(this);
    setItemDelegate(delegate);

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

    setGridSize(QSize(120, 135));
    setIconSize(QSize(64, 64));

    connect(m_model, &FileItemModel::updated, [=](){
        m_sort_filter_proxy_model->sort(FileItemModel::FileName);
    });

    connect(m_model, &FileItemModel::findChildrenFinished,
            m_proxy, &DirectoryViewProxyIface::viewDirectoryChanged);

    connect(this, &IconView::doubleClicked, [=](const QModelIndex &index){
        qDebug()<<"double click"<<index.data(FileItemModel::UriRole);
        Q_EMIT m_proxy->viewDoubleClicked(index.data(FileItemModel::UriRole).toString());
    });

    //edit trigger
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection){
        qDebug()<<"selection changed";
        auto currentSelections = selection.indexes();

        for (auto index : deselection.indexes()) {
            this->setIndexWidget(index, nullptr);
        }

        Q_EMIT m_proxy->viewSelectionChanged();
        if (currentSelections.count() == 1) {
            m_last_index = currentSelections.first();
            this->resetEditTriggerTimer();
        } else {
            m_last_index = QModelIndex();
        }

    });
}

IconView::~IconView()
{

}

DirectoryViewProxyIface *IconView::getProxy()
{
    return m_proxy;
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

void IconView::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->setDropAction(Qt::MoveAction);
        e->accept();
    }
}

void IconView::dragMoveEvent(QDragMoveEvent *e)
{
    if (this == e->source()) {
        return QListView::dragMoveEvent(e);
    }
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void IconView::dropEvent(QDropEvent *e)
{
    m_last_index = QModelIndex();
    m_edit_trigger_timer.stop();
    qDebug()<<"dropEvent";
    if (e->source() == this) {
        if (indexAt(e->pos()).isValid()) {
            return QListView::dropEvent(e);
        }
        else {
            return;
        }
    }
    e->setDropAction(Qt::MoveAction);
    auto proxy_index = indexAt(e->pos());
    auto index = m_sort_filter_proxy_model->mapToSource(proxy_index);
    m_model->dropMimeData(e->mimeData(), Qt::MoveAction, 0, 0, index);
}

void IconView::mousePressEvent(QMouseEvent *e)
{
    QListView::mousePressEvent(e);

    qDebug()<<m_edit_trigger_timer.isActive()<<m_edit_trigger_timer.interval();
    if (indexAt(e->pos()) == m_last_index && m_last_index.isValid()) {
        if (m_edit_trigger_timer.isActive()) {
            setIndexWidget(m_last_index, nullptr);
            edit(m_last_index);
        }
    }
}

void IconView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);
    if (!m_edit_trigger_timer.isActive() && indexAt(e->pos()).isValid() && this->selectedIndexes().count() == 1) {
        resetEditTriggerTimer();
    }
}

void IconView::resetEditTriggerTimer()
{
    m_edit_trigger_timer.disconnect();
    m_edit_trigger_timer.stop();
    QTimer::singleShot(750, [&](){
        qDebug()<<"start";
        m_edit_trigger_timer.setSingleShot(true);
        m_edit_trigger_timer.start(1000);
    });
}

void IconView::paintEvent(QPaintEvent *e)
{
    //This will avoid a "Black Edge" when drag and drop resize.
    //FIXME: how to deal with the maximize?
    QPainter p(this->viewport());
    //maybe i should use palette().base(). infact their colors are different.
    p.fillRect(this->geometry(), this->palette().base());

    QListView::paintEvent(e);
}

void IconView::resizeEvent(QResizeEvent *e)
{
    //FIXME: first resize is disfluency.
    //but I have to reset the index widget in view's resize.
    setIndexWidget(m_last_index, nullptr);
    QListView::resizeEvent(e);
}
