/*
 * Peony-Qt
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 *
 */

#ifndef DESKTOPITEMMODEL_H
#define DESKTOPITEMMODEL_H

#include <QAbstractListModel>
#include <QQueue>
#include <memory>

#include <QMutex>

namespace Peony {

class FileEnumerator;
class FileInfo;
class FileWatcher;

class DesktopItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        UriRole = Qt::UserRole,
        IsLinkRole = Qt::UserRole + 1
    };
    Q_ENUM(Role)

    explicit DesktopItemModel(QObject *parent = nullptr);
    ~DesktopItemModel() override;

    const QModelIndex indexFromUri(const QString &uri);
    const QString indexUri(const QModelIndex &index);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QMimeData *mimeData(const QModelIndexList& indexes) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;

    Qt::DropActions supportedDropActions() const override;

Q_SIGNALS:
    void requestLayoutNewItem(const QString &uri);
    void requestClearIndexWidget();
    void requestUpdateItemPositions(const QString &uri = nullptr);
    void refreshed();

public Q_SLOTS:
    void refresh();

protected Q_SLOTS:
    void onEnumerateFinished();

private:
    FileEnumerator *m_enumerator;
    QList<std::shared_ptr<FileInfo>> m_files;
    std::shared_ptr<FileWatcher> m_trash_watcher;
    std::shared_ptr<FileWatcher> m_desktop_watcher;

    QQueue<QString> m_info_query_queue;

    QMutex m_mutex;
};

}

#endif // DESKTOPITEMMODEL_H
