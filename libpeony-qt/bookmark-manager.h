#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <QObject>
#include <QSettings>
#include <QMutex>

#include "peony-core_global.h"

namespace Peony {

class PEONYCORESHARED_EXPORT BookMarkManager : public QObject
{
    Q_OBJECT
public:
    static BookMarkManager *getInstance();

    const QStringList getCurrentUris() {return m_uris;}
    bool isLoaded() {return m_is_loaded;}

Q_SIGNALS:
    void urisLoaded();
    void bookMarkAdded(const QString &uri, bool successed);
    void bookMarkRemoved(const QString &uri, bool successed);

public Q_SLOTS:
    void addBookMark(const QString &uri);
    void removeBookMark(const QString &uri);

private:
    explicit BookMarkManager(QObject *parent = nullptr);
    ~BookMarkManager();

    QStringList m_uris;
    QSettings *m_book_mark = nullptr;
    bool m_is_loaded = false;
    QMutex m_mutex;
};

}

#endif // BOOKMARKMANAGER_H
