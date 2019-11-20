#include "list-view.h"
#include "file-item-model.h"
#include "file-item-proxy-filter-sort-model.h"

#include "list-view-delegate.h"

#include "file-item.h"

#include <QHeaderView>

#include <QVBoxLayout>

#include <QDebug>

using namespace Peony;
using namespace Peony::DirectoryView;

ListView::ListView(QWidget *parent) : QTreeView(parent)
{
    setItemDelegate(new ListViewDelegate(this));

    header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    header()->setSectionsMovable(true);
    //header()->setStretchLastSection(false);

    setExpandsOnDoubleClick(false);
    setSortingEnabled(true);

    setEditTriggers(QTreeView::NoEditTriggers);
    setDragEnabled(true);
    setDragDropMode(QTreeView::DropOnly);
    setSelectionMode(QTreeView::ExtendedSelection);

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void ListView::bindModel(FileItemModel *sourceModel, FileItemProxyFilterSortModel *proxyModel)
{
    if (!sourceModel || !proxyModel)
        return;
    m_model = sourceModel;
    m_proxy_model = proxyModel;
    m_proxy_model->setSourceModel(m_model);
    setModel(proxyModel);

    //edit trigger
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, [=](const QItemSelection &selection, const QItemSelection &deselection){
        qDebug()<<"selection changed";
        auto currentSelections = selection.indexes();

        for (auto index : deselection.indexes()) {
            this->setIndexWidget(index, nullptr);
        }

        //Q_EMIT m_proxy->viewSelectionChanged();
        //rename trigger
    });
}

void ListView::setProxy(DirectoryViewProxyIface *proxy)
{
    m_proxy = proxy;

    if (!proxy)
        return;

    m_proxy = proxy;
    if (!m_proxy) {
        return;
    }

    connect(m_model, &FileItemModel::updated, this, &ListView::resort);

    connect(m_model, &FileItemModel::findChildrenFinished,
            this, &ListView::reportViewDirectoryChanged);

    connect(this, &ListView::doubleClicked, [=](const QModelIndex &index){
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
        //rename trigger
    });

    //menu
    connect(this, &ListView::customContextMenuRequested, [=](const QPoint &pos){
        qDebug()<<"menu request";
        if (!indexAt(pos).isValid())
            this->clearSelection();

        auto index = indexAt(pos);
        if (index.column() != 0) {
            auto visualRect = this->visualRect(index);
            auto sizeHint = this->itemDelegate()->sizeHint(viewOptions(), index);
            auto validRect = QRect(visualRect.topLeft(), sizeHint);
            if (!validRect.contains(pos))
                this->clearSelection();
        }

        //NOTE: we have to ensure that we have cleared the
        //selection if menu request at blank pos.
        QTimer::singleShot(1, [=](){
            Q_EMIT this->getProxy()->menuRequest(QCursor::pos());
        });
    });
}

void ListView::resort()
{
    m_proxy_model->sort(getSortType(), Qt::SortOrder(getSortOrder()));
}

void ListView::reportViewDirectoryChanged()
{
    Q_EMIT m_proxy->viewDirectoryChanged();
}

DirectoryViewProxyIface *ListView::getProxy()
{
    return m_proxy;
}

const QString ListView::getDirectoryUri()
{
    if (!m_model)
        return nullptr;
    return m_model->getRootUri();
}

void ListView::setDirectoryUri(const QString &uri)
{
    m_current_uri = uri;
}

const QStringList ListView::getSelections()
{
    QStringList uris;
    QModelIndexList selections = selectedIndexes();
    for (auto index : selections) {
        if (index.column() == 0)
            uris<<index.data(FileItemModel::UriRole).toString();
    }
    return uris;
}

void ListView::setSelections(const QStringList &uris)
{
    clearSelection();
    for (auto uri: uris) {
        const QModelIndex index = m_proxy_model->indexFromUri(uri);
        if (index.isValid()) {
            auto flags = QItemSelectionModel::Select|QItemSelectionModel::Rows;
            selectionModel()->select(index, flags);
        }
    }
}

const QStringList ListView::getAllFileUris()
{
    return m_proxy_model->getAllFileUris();
}

void ListView::open(const QStringList &uris, bool newWindow)
{
    return;
}

void ListView::beginLocationChange()
{
    m_model->setRootUri(m_current_uri);
}

void ListView::stopLocationChange()
{
    m_model->cancelFindChildren();
}

void ListView::closeView()
{
    this->deleteLater();
}

void ListView::invertSelections()
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    const QItemSelection currentSelection = selectionModel->selection();
    this->selectAll();
    selectionModel->select(currentSelection, QItemSelectionModel::Deselect);
}

void ListView::scrollToSelection(const QString &uri)
{
    auto index = m_proxy_model->indexFromUri(uri);
    scrollTo(index);
}

void ListView::setCutFiles(const QStringList &uris)
{
    return;
}

int ListView::getSortType()
{
    int type = m_proxy_model->sortColumn();
    return type<0? 0: type;
}

void ListView::setSortType(int sortType)
{
    m_proxy_model->sort(sortType, Qt::SortOrder(getSortOrder()));
}

int ListView::getSortOrder()
{
    return m_proxy_model->sortOrder();
}

void ListView::setSortOrder(int sortOrder)
{
    m_proxy_model->sort(getSortType(), Qt::SortOrder(sortOrder));
}

void ListView::editUri(const QString &uri)
{
    setIndexWidget(m_proxy_model->indexFromUri(uri), nullptr);
    edit(m_proxy_model->indexFromUri(uri));
}

void ListView::editUris(const QStringList uris)
{
    //FIXME:
    //implement batch rename.
}

//List View 2
ListView2::ListView2(QWidget *parent) : DirectoryViewWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    m_view = new ListView(this);
    layout->addWidget(m_view);

    setLayout(layout);
}

ListView2::~ListView2()
{

}

void ListView2::bindModel(FileItemModel *model, FileItemProxyFilterSortModel *proxyModel)
{
    disconnect(m_model);
    disconnect(m_proxy_model);
    m_model = model;
    m_proxy_model = proxyModel;

    m_view->bindModel(model, proxyModel);
    connect(model, &FileItemModel::findChildrenFinished, this, &DirectoryViewWidget::viewDirectoryChanged);
    connect(m_model, &FileItemModel::updated, m_view, &ListView::resort);

    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &DirectoryViewWidget::viewSelectionChanged);

    connect(m_view, &ListView::doubleClicked, this, [=](const QModelIndex &index){
        qDebug()<<index.data(Qt::UserRole).toString();
        Q_EMIT this->viewDoubleClicked(index.data(Qt::UserRole).toString());
    });

    //menu
    connect(m_view, &ListView::customContextMenuRequested, this, [=](const QPoint &pos){
        qDebug()<<"menu request";
        if (!m_view->indexAt(pos).isValid())
            m_view->clearSelection();

        auto index = m_view->indexAt(pos);
        if (index.column() != 0) {
            auto visualRect = m_view->visualRect(index);
            auto sizeHint = m_view->itemDelegate()->sizeHint(m_view->viewOptions(), index);
            auto validRect = QRect(visualRect.topLeft(), sizeHint);
            if (!validRect.contains(pos))
                m_view->clearSelection();
        }

        //NOTE: we have to ensure that we have cleared the
        //selection if menu request at blank pos.
        QTimer::singleShot(1, [=](){
            Q_EMIT this->menuRequest(QCursor::pos());
        });
    });
}
