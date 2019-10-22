#ifndef VIEWFACTORYSORTFILTERMODEL_H
#define VIEWFACTORYSORTFILTERMODEL_H

#include <QObject>
#include "peony-core_global.h"

#include <QSortFilterProxyModel>

namespace Peony {

class ViewFactorySortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ViewFactorySortFilterModel(QObject *parent = nullptr);

    const QModelIndex getIndexFromViewId(const QString &viewId);
    const QString getHighestPriorityViewId(int zoom_level_hint);
    const QStringList supportViewIds();

public Q_SLOTS:
    void setDirectoryUri(const QString &uri);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

}

#endif // VIEWFACTORYSORTFILTERMODEL_H
