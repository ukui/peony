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
        UriRole = Qt::UserRole
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
