#ifndef PATHBARMODEL_H
#define PATHBARMODEL_H

#include "peony-core_global.h"
#include <QStringListModel>
#include <QHash>

namespace Peony {

/*!
 * \brief The PathBarModel class
 * \details
 * PathBarModel is desgin for path completions.
 * This model would cache a directory driect children when the target uri
 * was set.
 * \note
 * A completion is theoretically responsive, so the enumeration of model
 * items should be as fast as possible.
 * It must be fast and lightweight enough to keep the ui-frequency.
 * For now, it performs well at local file system, but not good enough at
 * remote fs.
 */
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
    void setRootPath(const QString &path, bool force = false);
    void setRootUri(const QString &uri, bool force = false);

private:
    QString m_current_uri = nullptr;
    QHash<QString, QString> m_uri_display_name_hash;
};

}

#endif // PATHBARMODEL_H
