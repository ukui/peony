#include "view-factory-sort-filter-model.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include "directory-view-plugin-iface2.h"

#include "view-factory-model.h"

using namespace Peony;

ViewFactorySortFilterModel::ViewFactorySortFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    ViewFactoryModel *model = new ViewFactoryModel(this);
    setSourceModel(model);
}

bool ViewFactorySortFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    ViewFactoryModel *model = static_cast<ViewFactoryModel*>(sourceModel());
    auto index = model->index(sourceRow, 0, sourceParent);
    auto manager = DirectoryViewFactoryManager::getInstance();
    auto viewId = index.data(Qt::ToolTipRole).toString();
    auto factory = manager->getFactory(viewId);
    int zoom_level_hint = factory->zoom_level_hint();
    int priority = factory->priority(model->m_current_uri);

    auto same_zoom_level_factories = model->m_factory_hash.values(zoom_level_hint);
    bool accept = true;
    for (auto pair : same_zoom_level_factories) {
        if (pair.first > priority) {
            accept = false;
            break;
        }
    }
    return accept;
}

bool ViewFactorySortFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    auto manager = DirectoryViewFactoryManager::getInstance();

    auto leftId = left.data(Qt::ToolTipRole).toString();
    auto rightId = right.data(Qt::ToolTipRole).toString();

    auto leftFactory = manager->getFactory(leftId);
    auto rightFactory = manager->getFactory(rightId);

    return leftFactory->zoom_level_hint()<rightFactory->zoom_level_hint();
}

void ViewFactorySortFilterModel::setDirectoryUri(const QString &uri)
{
    ViewFactoryModel *model = static_cast<ViewFactoryModel*>(sourceModel());
    model->setDirectoryUri(uri);
    sort(0);
}

const QModelIndex ViewFactorySortFilterModel::getIndexFromViewId(const QString &viewId)
{
    ViewFactoryModel *model = static_cast<ViewFactoryModel*>(sourceModel());
    auto sourceIndex = model->getIndexFromViewId(viewId);
    return mapFromSource(sourceIndex);
}

const QString ViewFactorySortFilterModel::getHighestPriorityViewId(int zoom_level_hint)
{
    ViewFactoryModel *model = static_cast<ViewFactoryModel*>(sourceModel());
    return model->getHighestPriorityViewId(zoom_level_hint);
}

const QStringList ViewFactorySortFilterModel::supportViewIds()
{
    QStringList l;
    for (int i = 0; i < rowCount(); i++) {
        l<<index(i, 0).data(Qt::ToolTipRole).toString();
    }
    return l;
}

const QIcon ViewFactorySortFilterModel::iconFromViewId(const QString &viewId)
{
    auto manager = DirectoryViewFactoryManager::getInstance();
    auto factory = manager->getFactory(viewId);
    return factory->icon();
}

//Proxy Model 2
ViewFactorySortFilterModel2::ViewFactorySortFilterModel2(QObject *parent) : QSortFilterProxyModel(parent)
{
    ViewFactoryModel2 *model = new ViewFactoryModel2(this);
    setSourceModel(model);
}

bool ViewFactorySortFilterModel2::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    ViewFactoryModel2 *model = static_cast<ViewFactoryModel2*>(sourceModel());
    auto index = model->index(sourceRow, 0, sourceParent);
    auto manager = DirectoryViewFactoryManager2::getInstance();
    auto viewId = index.data(Qt::ToolTipRole).toString();
    auto factory = manager->getFactory(viewId);
    int zoom_level_hint = factory->zoom_level_hint();
    int priority = factory->priority(model->m_current_uri);

    auto same_zoom_level_factories = model->m_factory_hash.values(zoom_level_hint);
    bool accept = true;
    for (auto pair : same_zoom_level_factories) {
        if (pair.first > priority) {
            accept = false;
            break;
        }
    }
    return accept;
}

bool ViewFactorySortFilterModel2::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    auto manager = DirectoryViewFactoryManager2::getInstance();

    auto leftId = left.data(Qt::ToolTipRole).toString();
    auto rightId = right.data(Qt::ToolTipRole).toString();

    auto leftFactory = manager->getFactory(leftId);
    auto rightFactory = manager->getFactory(rightId);

    return leftFactory->zoom_level_hint()<rightFactory->zoom_level_hint();
}

void ViewFactorySortFilterModel2::setDirectoryUri(const QString &uri)
{
    ViewFactoryModel2 *model = static_cast<ViewFactoryModel2*>(sourceModel());
    model->setDirectoryUri(uri);
    sort(0);
}

const QModelIndex ViewFactorySortFilterModel2::getIndexFromViewId(const QString &viewId)
{
    ViewFactoryModel2 *model = static_cast<ViewFactoryModel2*>(sourceModel());
    auto sourceIndex = model->getIndexFromViewId(viewId);
    return mapFromSource(sourceIndex);
}

const QString ViewFactorySortFilterModel2::getHighestPriorityViewId(int zoom_level_hint)
{
    ViewFactoryModel2 *model = static_cast<ViewFactoryModel2*>(sourceModel());
    return model->getHighestPriorityViewId(zoom_level_hint);
}

const QStringList ViewFactorySortFilterModel2::supportViewIds()
{
    QStringList l;
    for (int i = 0; i < rowCount(); i++) {
        l<<index(i, 0).data(Qt::ToolTipRole).toString();
    }
    return l;
}

const QIcon ViewFactorySortFilterModel2::iconFromViewId(const QString &viewId)
{
    auto manager = DirectoryViewFactoryManager2::getInstance();
    auto factory = manager->getFactory(viewId);
    return factory->icon();
}
