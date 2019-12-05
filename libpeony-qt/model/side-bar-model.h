#ifndef SIDEBARMODEL_H
#define SIDEBARMODEL_H

#include <QAbstractItemModel>
#include <QVector>

#include "peony-core_global.h"

namespace Peony {

class SideBarAbstractItem;

/*!
 * \brief The SideBarModel class
 * \todo
 * Add dnd support and custom favorite items support.
 */
class PEONYCORESHARED_EXPORT SideBarModel : public QAbstractItemModel
{
    friend class SideBarAbstractItem;
    Q_OBJECT

public:
    explicit SideBarModel(QObject *parent = nullptr);
    ~SideBarModel() override;

    QModelIndex firstCloumnIndex(SideBarAbstractItem *item);
    QModelIndex lastCloumnIndex(SideBarAbstractItem *item);

    SideBarAbstractItem *itemFromIndex(const QModelIndex &index);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

Q_SIGNALS:
    void indexUpdated(const QModelIndex &index);

protected:
    QVector<SideBarAbstractItem*> *m_root_children = nullptr;

    void onIndexUpdated(const QModelIndex &index);

protected:
    QStringList m_bookmark_uris;
};

}

#endif // SIDEBARMODEL_H
