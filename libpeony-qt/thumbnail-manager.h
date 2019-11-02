#ifndef THUMBNAILMANAGER_H
#define THUMBNAILMANAGER_H

#include <QObject>
#include "peony-core_global.h"
#include "file-info.h"

#include <QHash>
#include <QIcon>
#include <QMutex>

namespace Peony {

class FileWatcher;

class PEONYCORESHARED_EXPORT ThumbnailManager : public QObject
{
    Q_OBJECT
public:
    static ThumbnailManager *getInstance();

    void createThumbnail(const QString &uri, FileWatcher *watcher = nullptr);
    void releaseThumbnail(const QString &uri);
    const QIcon tryGetThumbnail(const QString &uri);

Q_SIGNALS:

public Q_SLOTS:

private:
    explicit ThumbnailManager(QObject *parent = nullptr);

    QHash<QString, QIcon> m_hash;
    QMutex m_mutex;
};

}

#endif // THUMBNAILMANAGER_H
