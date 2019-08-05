#ifndef FILEITEMPROXYFILTERSORTMODEL_H
#define FILEITEMPROXYFILTERSORTMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "peony-core_global.h"

namespace Peony {

class FileItem;
class FileItemModel;

class PEONYCORESHARED_EXPORT FileItemProxyFilterSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FileItemProxyFilterSortModel(QObject *parent = nullptr);
    void setSourceModel(QAbstractItemModel *model) override;
    void setShowHidden(bool showHidden);

    FileItem *itemFromIndex(const QModelIndex &proxyIndex);
    QModelIndex getSourceIndex(const QModelIndex &proxyIndex);

public Q_SLOTS:
    void update();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    bool m_show_hidden = false;
};

}

#endif // FILEITEMPROXYFILTERSORTMODEL_H
