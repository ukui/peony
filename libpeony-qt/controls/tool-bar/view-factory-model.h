#ifndef VIEWFACTORYMODEL_H
#define VIEWFACTORYMODEL_H

#include <QAbstractListModel>
#include "peony-core_global.h"

namespace Peony {

/*!
 * \brief The ViewFactoryModel class
 * \details
 * This class is used in toolbar's viewCombox.
 *
 * The model changes dynamically as the path it is given.
 * For example, a current location file:/// might have 2 views' data in
 * model, the iconview and the listview. A current location computer:///
 * might have a extra computerview supported.
 *
 * \see Peony::ToolBar, Peony::DirectoryViewPluginIface::supportUri()
 */
class PEONYCORESHARED_EXPORT ViewFactoryModel : public QAbstractListModel
{
    friend class ViewFactorySortFilterModel;
    Q_OBJECT

public:
    explicit ViewFactoryModel(QObject *parent = nullptr);
    void setDirectoryUri(const QString &uri);

    const QModelIndex getIndexFromViewId(const QString &viewId);

    const QString getViewId(int index);
    const QStringList supportViewIds() {return m_support_views_id;}
    const QString getHighestPriorityViewId(int zoom_level_hint);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QString m_current_uri;
    QStringList m_support_views_id;

    QHash<int /*zoom_level*/, QPair<int/*priority*/, QString /*view id*/>> m_factory_hash;
};

}

#endif // VIEWFACTORYMODEL_H
