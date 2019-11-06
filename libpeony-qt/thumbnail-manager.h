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

    bool hasThumbnail(const QString &uri) {return !m_hash.values(uri).isEmpty();}
    void createThumbnail(const QString &uri, FileWatcher *watcher = nullptr);
    /*!
     * \brief releaseThumbnail
     * \param uri
     * \deprecated
     */
    void releaseThumbnail(const QString &uri);
    /*!
     * \brief tryGetThumbnail
     * \param uri
     * \return
     * \deprecated
     */
    const QIcon tryGetThumbnail(const QString &uri);

Q_SIGNALS:

public Q_SLOTS:

private:
    explicit ThumbnailManager(QObject *parent = nullptr);

    /*!
     * \brief m_hash
     * \deprecated
     */
    QHash<QString, QIcon> m_hash;
    QMutex m_mutex;
};

}

#endif // THUMBNAILMANAGER_H
