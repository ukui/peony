#ifndef DESKTOPITEMPROXYMODEL_H
#define DESKTOPITEMPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace Peony {

class DesktopItemProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum SortType {
        FileName,
        FileType,
        FileSize,
        ModifiedDate,
        Other
    };
    explicit DesktopItemProxyModel(QObject *parent = nullptr);

    void setSortType(int type) {m_sort_type = type;}
    int getSortType() {return m_sort_type;}

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

private:
    int m_sort_type = Other;
};

}

#endif // DESKTOPITEMPROXYMODEL_H
