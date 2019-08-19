#ifndef SIDEBARPROXYFILTERSORTMODEL_H
#define SIDEBARPROXYFILTERSORTMODEL_H

#include "peony-core_global.h"
#include <QSortFilterProxyModel>

namespace Peony {

class SideBarAbstractItem;

class PEONYCORESHARED_EXPORT SideBarProxyFilterSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SideBarProxyFilterSortModel(QObject *parent = nullptr);
    SideBarAbstractItem *itemFromIndex(const QModelIndex &proxy_index);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

}

#endif // SIDEBARPROXYFILTERSORTMODEL_H
