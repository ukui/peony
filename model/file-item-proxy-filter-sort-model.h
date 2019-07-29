#ifndef FILEITEMPROXYFILTERSORTMODEL_H
#define FILEITEMPROXYFILTERSORTMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include "file-item-model.h"
#include "file-item.h"

namespace Peony {

class FileItemProxyFilterSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FileItemProxyFilterSortModel(QSortFilterProxyModel *parent = nullptr);
    void update() { invalidateFilter(); }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};

}

#endif // FILEITEMPROXYFILTERSORTMODEL_H
