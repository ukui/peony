#ifndef PATHCOMPLETER_H
#define PATHCOMPLETER_H

#include "peony-core_global.h"
#include <QCompleter>

namespace Peony {

/*!
 * \brief The PathCompleter class
 * \details
 * This class is used to complete the path based on gvfs.
 * Note that the peony's file system model is desgining-different from
 * QFileSystemModel, so that the QCompleter should aslo do some special
 * treatments.
 * PathCompleter binds with a derived QStringListModel, the PathModel.
 * This model dynamiclly changes its contents when PathCompleter changed
 * its split string. The most cases of that is the line edit which bind with
 * a completer changed its texts. Completer will trigger the splitPath()
 * method, then the model will decided if its contents need be refereshed.
 * \see PathModel
 */
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
