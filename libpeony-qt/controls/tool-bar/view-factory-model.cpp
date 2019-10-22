#include "view-factory-model.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"

using namespace Peony;

ViewFactoryModel::ViewFactoryModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void ViewFactoryModel::setDirectoryUri(const QString &uri)
{
    beginResetModel();
    m_support_views_id.clear();
    m_current_uri = uri;
    auto viewManager = DirectoryViewFactoryManager::getInstance();
    auto defaultList = viewManager->getFactoryNames();

    int priority = 0;
    for (auto id : defaultList) {
        if (viewManager->getFactory(id)->supportUri(m_current_uri)) {
            /*!
              \bug
              there is a bug of these code. i have to correct the filter/sort process, or
              use proxy model to replace this model to show the data in the view.
              */
            /*
            int p = viewManager->getFactory(id)->priority(m_current_uri);
            if (p > priority) {
                priority = p;
                m_highest_priority_view_id = id;
            }
            if (!m_support_views_id.isEmpty()) {
                auto largestView = viewManager->getFactory(m_support_views_id.last());
                auto smallestView = viewManager->getFactory(m_support_views_id.first());
                auto currentView = viewManager->getFactory(id);
                if (largestView->zoom_level_hint() < currentView->zoom_level_hint()) {
                    m_support_views_id<<id;
                } else if (smallestView->zoom_level_hint() > currentView->zoom_level_hint()) {
                    m_support_views_id.prepend(id);
                } else {
                    for (auto tmp : m_support_views_id) {
                        auto view = viewManager->getFactory(tmp);
                        if (view->zoom_level_hint() > currentView->zoom_level_hint()) {
                            m_support_views_id.insert(m_support_views_id.indexOf(tmp), id);
                            break;
                        }
                    }
                }
            }
            */
            m_support_views_id<<id;
        }
    }

    endResetModel();
}

const QString ViewFactoryModel::getViewId(int index)
{
    if (index > m_support_views_id.count() - 1 || index < 0) {
        return nullptr;
    }
    return m_support_views_id.at(index);
}

int ViewFactoryModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_support_views_id.count();
}

const QString ViewFactoryModel::getHighestPriorityViewId()
{
    return m_highest_priority_view_id;
}

QVariant ViewFactoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto viewManager = DirectoryViewFactoryManager::getInstance();
    auto factory = viewManager->getFactory(viewManager->getFactoryNames().at(index.row()));

    switch (role) {
    case Qt::DecorationRole:
        return factory->viewIcon();
    case Qt::ToolTipRole:
        return factory->viewIdentity();
    default:
        break;
    }
    return QVariant();
}

const QModelIndex ViewFactoryModel::getIndexFromViewId(const QString &viewId)
{
    if (!m_support_views_id.contains(viewId))
        return QModelIndex();
    return index(m_support_views_id.indexOf(viewId));
}
