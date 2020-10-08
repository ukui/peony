#ifndef RECENTVFSMANAGER_H
#define RECENTVFSMANAGER_H

#include <QObject>
#include <QDomDocument>
#include <peony-core_global.h>

namespace Peony {
class PEONYCORESHARED_EXPORT RecentVFSManager : public QObject
{
    Q_OBJECT
public:
    static RecentVFSManager* getInstance ();
    void insert (QString uri, QString mimetype, QString name, QString exec);

private:
    explicit RecentVFSManager(QObject *parent = nullptr);
    bool read ();
    bool write ();
    bool exists (QString uri);
    bool createNode (QString uri, QString mimetype, QString name, QString exec);

private:
    static RecentVFSManager* m_instance;
    QString m_recent_path;
    QDomDocument m_dom_document;
};
};
#endif // RECENTVFSMANAGER_H
