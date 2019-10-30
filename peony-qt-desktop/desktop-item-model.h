#ifndef DESKTOPITEMMODEL_H
#define DESKTOPITEMMODEL_H

#include <QAbstractListModel>
#include <memory>

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

public Q_SLOTS:
    void refresh();

protected Q_SLOTS:
    void onEnumerateFinished();

private:
    FileEnumerator *m_enumerator;
    QList<std::shared_ptr<FileInfo>> m_files;
    FileWatcher *m_trash_watcher;
    FileWatcher *m_desktop_watcher;
};

}

#endif // DESKTOPITEMMODEL_H
