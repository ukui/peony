#include "view-factory-model.h"
#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"
#include <QPair>

using namespace Peony;

ViewFactoryModel::ViewFactoryModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void ViewFactoryModel::setDirectoryUri(const QString &uri)
{
    beginResetModel();
    m_support_views_id.clear();
    m_factory_hash.clear();
    m_current_uri = uri;
    auto viewManager = DirectoryViewFactoryManager::getInstance();
    auto defaultList = viewManager->getFactoryNames();

    for (auto id : defaultList) {
        if (viewManager->getFactory(id)->supportUri(m_current_uri)) {
            QPair<int, QString> pair(viewManager->getFactory(id)->priority(m_current_uri), id);
            m_factory_hash.insert(viewManager->getFactory(id)->zoom_level_hint(),
                                  pair);

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

const QString ViewFactoryModel::getHighestPriorityViewId(int zoom_level_hint)
{
    auto manager = DirectoryViewFactoryManager::getInstance();
    auto pairs = m_factory_hash.values(zoom_level_hint);
    DirectoryViewPluginIface *factory = nullptr;
    int priority = -9999;
    QString result;
    for (auto pair : pairs) {
        factory = manager->getFactory(pair.second);
        if (factory->priority(m_current_uri) > priority) {
            result = pair.second;
            priority = pair.first;
        }
    }
    return result;
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
