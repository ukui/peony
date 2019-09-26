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
    m_current_uri = uri;
    auto viewManager = DirectoryViewFactoryManager::getInstance();
    auto defaultList = viewManager->getFactoryNames();

    for (auto id : defaultList) {
        if (viewManager->getFactory(id)->supportUri(m_current_uri)) {
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

QVariant ViewFactoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto viewManager = DirectoryViewFactoryManager::getInstance();
    auto factory = viewManager->getFactory(viewManager->getFactoryNames().at(index.row()));

    if (role == Qt::DecorationRole) {
        return factory->viewIcon();
    }
    return QVariant();
}
