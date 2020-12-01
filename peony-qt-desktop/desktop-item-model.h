/*
 * Peony-Qt
 *
 * Copyright (C) 2020, KylinSoft Co., Ltd.
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

namespace Peony {

class FileEnumerator;
class FileInfo;
class FileWatcher;

class DesktopItemModel : public QAbstractListModel
{
    friend class DesktopIconView;
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

    void fileCreated(const QString &uri);

public Q_SLOTS:
    void refresh();

protected Q_SLOTS:
    void onEnumerateFinished();

private:
    FileEnumerator *m_enumerator;
    QList<std::shared_ptr<FileInfo>> m_files;
    std::shared_ptr<FileWatcher> m_trash_watcher;
    std::shared_ptr<FileWatcher> m_desktop_watcher;
    std::shared_ptr<FileWatcher> m_thumbnail_watcher; //just handle the thumbnail created.

    std::shared_ptr<FileWatcher> m_system_app_watcher;
    std::shared_ptr<FileWatcher> m_andriod_app_watcher;

    QQueue<QString> m_info_query_queue;
    QQueue<QString> m_new_file_info_query_queue;

    QString m_last_deleted_item_uri;
    QStringList m_items_need_relayout;
};

}

#endif // DESKTOPITEMMODEL_H
