#ifndef PATHCOMPLETER_H
#define PATHCOMPLETER_H

#include "peony-core_global.h"
#include <QCompleter>

namespace Peony {

class PEONYCORESHARED_EXPORT PathCompleter : public QCompleter
{
    Q_OBJECT
public:
    explicit PathCompleter(QObject *parent = nullptr);
    explicit PathCompleter(QAbstractItemModel *model, QObject *parent = nullptr);

protected:
    QStringList splitPath(const QString &path) const override;
    QString pathFromIndex(const QModelIndex &index) const override;
};

}

#endif // PATHCOMPLETER_H
