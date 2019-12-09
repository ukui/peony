/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

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

class PEONYCORESHARED_EXPORT ViewFactoryModel2 : public QAbstractListModel
{
    friend class ViewFactorySortFilterModel2;
    Q_OBJECT

public:
    explicit ViewFactoryModel2(QObject *parent = nullptr);
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
