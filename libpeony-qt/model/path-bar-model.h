#ifndef PATHBARMODEL_H
#define PATHBARMODEL_H

#include "peony-core_global.h"
#include <QStringListModel>
#include <QHash>

namespace Peony {

class PEONYCORESHARED_EXPORT PathBarModel : public QStringListModel
{
    Q_OBJECT
public:
    explicit PathBarModel(QObject *parent = nullptr);
    QString findDisplayName(const QString &uri);
    QString currentDirUri() {return m_current_uri;}

Q_SIGNALS:
    void updated();

public Q_SLOTS:
    void setRootPath(const QString &path);
    void setRootUri(const QString &uri);

private:
    QString m_current_uri = nullptr;
    QHash<QString, QString> m_uri_display_name_hash;
};

}

#endif // PATHBARMODEL_H
